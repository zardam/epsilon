#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <escher/text_input_helpers.h>

#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

extern "C" {
#include "py/nlr.h"
#include "py/lexer.h"
}
#include <python/port/port.h>

static inline size_t min(size_t a, size_t b) {
  return (a>b ? b : a);
}

TextArea::Text::Text(char * buffer, size_t bufferSize) :
  m_buffer(buffer),
  m_bufferSize(bufferSize)
{
}

void TextArea::Text::setText(char * buffer, size_t bufferSize) {
  m_buffer = buffer;
  m_bufferSize = bufferSize;
}

TextArea::Text::Line::Line(const char * text) :
  m_text(text),
  m_length(0)
{
  if (m_text != nullptr) {
    while (*text != 0 && *text != '\n') {
      text++;
    }
    m_length = text-m_text;
  }
}

bool TextArea::Text::Line::contains(const char * c) const {
  return (c >= m_text) && (c < m_text + m_length);
}

TextArea::Text::LineIterator & TextArea::Text::LineIterator::operator++() {
  const char * last = m_line.text() + m_line.length();
  m_line = Line(*last == 0 ? nullptr : last+1);
  return *this;
}

size_t TextArea::Text::indexAtPosition(Position p) const {
  assert(m_buffer != nullptr);
  if (p.line() < 0) {
    return 0;
  }
  int y = 0;
  const char * endOfLastLine = nullptr;
  for (Line l : *this) {
    if (p.line() == y) {
      size_t x = p.column() < 0 ? 0 : p.column();
      x = min(x, l.length());
      return l.text() - m_buffer + x;
    }
    endOfLastLine = l.text() + l.length();
    y++;
  }
  assert(endOfLastLine != nullptr && endOfLastLine >= m_buffer);
  return endOfLastLine - m_buffer;
}

TextArea::Text::Position TextArea::Text::positionAtIndex(size_t index) const {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize);
  const char * target = m_buffer + index;
  size_t y = 0;
  for (Line l : *this) {
    if (l.text() <= target && l.text() + l.length() >= target) {
      size_t x = target - l.text();
      return Position(x, y);
    }
    y++;
  }
  assert(false);
  return Position(0, 0);
}

void TextArea::Text::insertChar(char c, size_t index) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize-1);
  char previous = c;
  for (size_t i=index; i<m_bufferSize; i++) {
    char inserted = previous;
    previous = m_buffer[i];
    m_buffer[i] = inserted;
    if (inserted == 0) {
      break;
    }
  }
}

char TextArea::Text::removeChar(size_t index) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize-1);
  char deletedChar = m_buffer[index];
  for (size_t i=index; i<m_bufferSize; i++) {
    m_buffer[i] = m_buffer[i+1];
    if (m_buffer[i] == 0) {
      break;
    }
  }
  return deletedChar;
}

int TextArea::Text::removeRemainingLine(size_t index, int direction) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize);
  int jump = index;
  while (m_buffer[jump] != '\n' && m_buffer[jump] != 0 && jump >= 0) {
    jump += direction;
  }
  int delta = direction > 0 ? jump - index : index - jump;
  if (delta == 0) {
    return 0;
  }
  /* We stop at m_bufferSize-1 because:
   * - if direction > 0: jump >= k+1 so we will reach the 0 before m_bufferSize-1
   * - if direction < 0: k+1 will reach m_bufferSize. */
  for (size_t k = index; k < m_bufferSize-1; k++) {
    if (direction > 0) {
      m_buffer[k] = m_buffer[jump++];
    } else {
      m_buffer[++jump] = m_buffer[k+1];
    }
    if (m_buffer[k] == 0 || m_buffer[k+1] == 0) {
      return delta;
    }
  }
  assert(false);
  return 0;
}

TextArea::Text::Position TextArea::Text::span() const {
  assert(m_buffer != nullptr);
  size_t width = 0;
  size_t height = 0;
  for (Line l : *this) {
    if (l.length() > width) {
      width = l.length();
    }
    height++;
  }
  return Position(width, height);
}

/* TextArea::ContentView */

TextArea::ContentView::ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  TextInput::ContentView(fontSize, textColor, backgroundColor),
  m_text(textBuffer, textBufferSize)
{
}

KDSize TextArea::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize charSize = KDText::charSize(m_fontSize);
  Text::Position span = m_text.span();
  return KDSize(
    /* We take into account the space required to draw a cursor at the end of
     * line by adding charSize.width() to the width. */
    charSize.width() * (span.column()+1),
    charSize.height() * span.line()
  );
}

// This list must be sorted according to strcmp
const char builtins[][20] = { "abs", "all", "any", "ascii", "bin", "bool", "bytearray",
  "bytes", "callable", "chr", "classmethod", "compile", "complex", "copyright",
  "credits", "delattr", "dict", "dir", "divmod", "enumerate", "eval", "exec",
  "exit", "filter", "float", "format", "frozenset", "getattr", "globals", "hasattr",
  "hash", "help", "hex", "id", "input", "int", "isinstance", "issubclass", "iter", "len",
  "license", "list", "locals", "map", "max", "memoryview", "min", "next", "object",
  "oct", "open", "ord", "pow", "print", "property", "quit", "range", "repr", "reversed",
  "round", "set", "setattr", "slice", "sorted", "staticmethod", "str", "sum", "super",
  "tuple", "type", "vars", "zip"};

bool isBuiltin(const char * name) {
  return bsearch(name, builtins, sizeof(builtins) / sizeof(*builtins), sizeof(*builtins), (int(*) (const void*,const void*)) strcmp);
}

constexpr KDColor KDColorString = KDColor::RGB24(0x00AA00);
constexpr KDColor KDColorClassOrFunction = KDColor::RGB24(0x0000FF);
constexpr KDColor KDColorBuiltin = KDColor::RGB24(0x900090);
constexpr KDColor KDColorKeyord = KDColor::RGB24(0xFF7700);
constexpr KDColor KDColorComment = KDColor::RGB24(0xDD0000);

KDColor determineColor(mp_token_kind_t prev_tok_kind, mp_token_kind_t tok_kind, bool isBuiltin) {
  switch(tok_kind) {
    case MP_TOKEN_STRING:
    case MP_TOKEN_LONELY_STRING_OPEN:
      return KDColorString;

    case MP_TOKEN_NAME:
      if(prev_tok_kind == MP_TOKEN_KW_DEF || prev_tok_kind == MP_TOKEN_KW_CLASS) {
        return KDColorClassOrFunction;
      } else if(isBuiltin) {
        return KDColorBuiltin;
      } else {
        return KDColorBlack;
      }

    case MP_TOKEN_KW_FALSE:
    case MP_TOKEN_KW_NONE:
    case MP_TOKEN_KW_TRUE:
    case MP_TOKEN_KW___DEBUG__:
    case MP_TOKEN_KW_AND:
    case MP_TOKEN_KW_AS:
    case MP_TOKEN_KW_ASSERT:
    #if MICROPY_PY_ASYNC_AWAIT
    case MP_TOKEN_KW_ASYNC:
    case MP_TOKEN_KW_AWAIT:
    #endif
    case MP_TOKEN_KW_BREAK:
    case MP_TOKEN_KW_CLASS:
    case MP_TOKEN_KW_CONTINUE:
    case MP_TOKEN_KW_DEF:
    case MP_TOKEN_KW_DEL:
    case MP_TOKEN_KW_ELIF:
    case MP_TOKEN_KW_ELSE:
    case MP_TOKEN_KW_EXCEPT:
    case MP_TOKEN_KW_FINALLY:
    case MP_TOKEN_KW_FOR:
    case MP_TOKEN_KW_FROM:
    case MP_TOKEN_KW_GLOBAL:
    case MP_TOKEN_KW_IF:
    case MP_TOKEN_KW_IMPORT:
    case MP_TOKEN_KW_IN:
    case MP_TOKEN_KW_IS:
    case MP_TOKEN_KW_LAMBDA:
    case MP_TOKEN_KW_NONLOCAL:
    case MP_TOKEN_KW_NOT:
    case MP_TOKEN_KW_OR:
    case MP_TOKEN_KW_PASS:
    case MP_TOKEN_KW_RAISE:
    case MP_TOKEN_KW_RETURN:
    case MP_TOKEN_KW_TRY:
    case MP_TOKEN_KW_WHILE:
    case MP_TOKEN_KW_WITH:
    case MP_TOKEN_KW_YIELD:
      return KDColorKeyord;

    default:
      return KDColorBlack;
  }
}

int tokenLength(mp_lexer_t * lex, const char * text) {
  switch(lex->tok_kind) {
    case MP_TOKEN_OP_PLUS:
    case MP_TOKEN_OP_MINUS:
    case MP_TOKEN_OP_STAR:
    case MP_TOKEN_OP_SLASH:
    case MP_TOKEN_OP_PERCENT:
    case MP_TOKEN_OP_LESS:
    case MP_TOKEN_OP_MORE:
    case MP_TOKEN_OP_AMPERSAND:
    case MP_TOKEN_OP_PIPE:
    case MP_TOKEN_OP_CARET:
    case MP_TOKEN_OP_TILDE:
    case MP_TOKEN_OP_LESS_EQUAL:
    case MP_TOKEN_OP_MORE_EQUAL:
    case MP_TOKEN_OP_NOT_EQUAL:
    case MP_TOKEN_DEL_PAREN_OPEN:
    case MP_TOKEN_DEL_PAREN_CLOSE:
    case MP_TOKEN_DEL_BRACKET_OPEN:
    case MP_TOKEN_DEL_BRACKET_CLOSE:
    case MP_TOKEN_DEL_BRACE_OPEN:
    case MP_TOKEN_DEL_BRACE_CLOSE:
    case MP_TOKEN_DEL_COMMA:
    case MP_TOKEN_DEL_COLON:
    case MP_TOKEN_DEL_PERIOD:
    case MP_TOKEN_DEL_SEMICOLON:
    case MP_TOKEN_DEL_AT:
    case MP_TOKEN_DEL_EQUAL:
    case MP_TOKEN_DEL_PLUS_EQUAL:
    case MP_TOKEN_DEL_MINUS_EQUAL:
    case MP_TOKEN_DEL_STAR_EQUAL:
    case MP_TOKEN_DEL_SLASH_EQUAL:
    case MP_TOKEN_DEL_PERCENT_EQUAL:
    case MP_TOKEN_DEL_AMPERSAND_EQUAL:
    case MP_TOKEN_DEL_PIPE_EQUAL:
    case MP_TOKEN_DEL_CARET_EQUAL:
    case MP_TOKEN_DEL_MINUS_MORE:
      return 1;
    case MP_TOKEN_OP_DBL_STAR:
    case MP_TOKEN_OP_DBL_SLASH:
    case MP_TOKEN_OP_DBL_LESS:
    case MP_TOKEN_OP_DBL_MORE:
    case MP_TOKEN_OP_DBL_EQUAL:
    case MP_TOKEN_DEL_DBL_SLASH_EQUAL:
    case MP_TOKEN_DEL_DBL_MORE_EQUAL:
    case MP_TOKEN_DEL_DBL_LESS_EQUAL:
    case MP_TOKEN_DEL_DBL_STAR_EQUAL:
      return 2;
    case MP_TOKEN_STRING:
      if(lex->vstr.len > 0 && text[0] == text[1]) {
        return lex->vstr.len + 6;
      } else {
        return lex->vstr.len + 2;
      }
    case MP_TOKEN_LONELY_STRING_OPEN:
      if(lex->vstr.len > 0 && text[0] == text[1]) {
        return lex->vstr.len + 2;
      } else {
        return lex->vstr.len + 1;
      }
    default:
      return lex->vstr.len;
  }
}

void TextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize charSize = KDText::charSize(m_fontSize);

  Text::Position topLeft(
       rect.x()/charSize.width(),
       rect.y()/charSize.height()
     );
   Text::Position bottomRight(
     rect.right()/charSize.width() + 1,
     rect.bottom()/charSize.height() + 1
   );

  char m_pythonHeap[4096];
  MicroPython::init(m_pythonHeap, m_pythonHeap + 4096);

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t * lex = mp_lexer_new_from_str_len(0, text(), strlen(text()), false);

    int cur_line = 1;
    int cur_col = 1;
    int cur_len = 0;
    bool cur_builtin = false;
    mp_token_kind_t prev_tok_kind = MP_TOKEN_END;
    mp_token_kind_t tok_kind = MP_TOKEN_END;

    do {
      int beginIndex = m_text.indexAtPosition(TextArea::Text::Position(cur_col-1, cur_line-1));
      TextArea::Text::Position middlePosition = m_text.positionAtIndex(beginIndex + cur_len);
      TextArea::Text::Position endPosition = TextArea::Text::Position(lex->tok_column-1, lex->tok_line-1);
      int endIndex = m_text.indexAtPosition(endPosition);

      // Draws the current token.
      if(cur_line-1 <= bottomRight.line() && middlePosition.line() >= topLeft.line()) {
        ctx->drawString(
          m_text.text() + beginIndex,
          KDPoint(cur_col*charSize.width()-charSize.width(), cur_line*charSize.height()-charSize.height()),
          m_fontSize,
          determineColor(prev_tok_kind, tok_kind, cur_builtin),
          KDColorWhite,
          cur_len
        );
      }

      // Draws what is between the current token and the next token to be displayed (comment or whitespace)
      if(middlePosition.line() <= bottomRight.line() &&  endPosition.line() >= topLeft.line()) {
        ctx->drawString(
           m_text.text() + beginIndex + cur_len,
           KDPoint(middlePosition.column()*charSize.width(), middlePosition.line()*charSize.height()),
           m_fontSize,
           KDColorComment,
           KDColorWhite,
           endIndex - beginIndex - cur_len
         );
      }

      // Stores the "next" current token to be displayed
      cur_line = lex->tok_line;
      cur_col = lex->tok_column;
      cur_len = tokenLength(lex, m_text.text() + endIndex);
      cur_builtin = lex->tok_kind == MP_TOKEN_NAME && isBuiltin(lex->vstr.buf);
      prev_tok_kind = tok_kind;
      tok_kind = lex->tok_kind;

      mp_lexer_to_next(lex);
    } while(tok_kind != MP_TOKEN_END);

    // Clears the lines under the last line of the script
    cur_line--;
    while(cur_line < bottomRight.line()) {
      ctx->drawString(
         "\n",
         KDPoint(0, cur_line*charSize.height()),
         m_fontSize,
         KDColorBlack,
         KDColorWhite,
         1
       );
       cur_line++;
    }

    mp_lexer_free(lex);
    nlr_pop();
  }
  MicroPython::deinit();
}

void TextArea::TextArea::ContentView::setText(char * textBuffer, size_t textBufferSize) {
  m_text.setText(textBuffer, textBufferSize);
  m_cursorIndex = 0;
}

const char * TextArea::TextArea::ContentView::text() const {
  return m_text.text();
}

bool TextArea::TextArea::ContentView::insertTextAtLocation(const char * text, int location) {
  int textSize = strlen(text);
  if (m_text.textLength() + textSize >= m_text.bufferSize() || textSize == 0) {
    return false;
  }
  bool lineBreak = false;
  int currentLocation = location;
  while (*text != 0) {
    lineBreak |= *text == '\n';
    m_text.insertChar(*text++, currentLocation++);
  }
  reloadRectFromCursorPosition(currentLocation-1, lineBreak);
  return true;
}

bool TextArea::TextArea::ContentView::removeChar() {
  if (cursorLocation() <= 0) {
    return false;
  }
  bool lineBreak = false;
  assert(m_cursorIndex > 0);
  lineBreak = m_text.removeChar(--m_cursorIndex) == '\n';
  layoutSubviews(); // Reposition the cursor
  reloadRectFromCursorPosition(cursorLocation(), lineBreak);
  return true;
}

bool TextArea::ContentView::removeEndOfLine() {
  int removedLine = m_text.removeRemainingLine(cursorLocation(), 1);
  if (removedLine > 0) {
    layoutSubviews();
    reloadRectFromCursorPosition(cursorLocation(), false);
    return true;
  }
  return false;
}

bool TextArea::ContentView::removeStartOfLine() {
  if (cursorLocation() <= 0) {
    return false;
  }
  int removedLine = m_text.removeRemainingLine(cursorLocation()-1, -1);
  if (removedLine > 0) {
    assert((int)m_cursorIndex >= removedLine);
    setCursorLocation(cursorLocation()-removedLine);
    reloadRectFromCursorPosition(cursorLocation(), false);
    return true;
  }
  return false;
}

KDRect TextArea::TextArea::ContentView::characterFrameAtIndex(size_t index) const {
  KDSize charSize = KDText::charSize(m_fontSize);
  Text::Position p = m_text.positionAtIndex(index);
  return KDRect(
    p.column() * charSize.width(),
    p.line() * charSize.height(),
    charSize.width(),
    charSize.height()
  );
}

void TextArea::TextArea::ContentView::moveCursorGeo(int deltaX, int deltaY) {
  Text::Position p = m_text.positionAtIndex(m_cursorIndex);
  setCursorLocation(m_text.indexAtPosition(Text::Position(p.column() + deltaX, p.line() + deltaY)));
}

/* TextArea */

TextArea::TextArea(Responder * parentResponder, char * textBuffer,
    size_t textBufferSize, TextAreaDelegate * delegate,
    KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  TextInput(parentResponder, &m_contentView),
  m_contentView(textBuffer, textBufferSize, fontSize, textColor, backgroundColor),
  m_delegate(delegate)
{
  assert(textBufferSize < INT_MAX/2);
}

bool TextArea::handleEventWithText(const char * text, bool indentation) {
  int nextCursorLocation = cursorLocation();
  if ((indentation && insertTextWithIndentation(text, cursorLocation())) || insertTextAtLocation(text, cursorLocation())) {
    nextCursorLocation += TextInputHelpers::CursorIndexInCommand(text);
  }
  setCursorLocation(nextCursorLocation);
  return true;
}

bool TextArea::handleEvent(Ion::Events::Event event) {
  if (m_delegate != nullptr && m_delegate->textAreaDidReceiveEvent(this, event)) {
    return true;
  } else if (Responder::handleEvent(event)) {
    // The only event Responder handles is 'Toolbox' displaying.
    return true;
  } else if (event == Ion::Events::Left) {
    return setCursorLocation(cursorLocation()-1);
  } else if (event == Ion::Events::Right) {
    return setCursorLocation(cursorLocation()+1);
  } else if (event == Ion::Events::Up) {
    m_contentView.moveCursorGeo(0, -1);
  } else if (event == Ion::Events::Down) {
    m_contentView.moveCursorGeo(0, 1);
  } else if (event == Ion::Events::ShiftLeft) {
    m_contentView.moveCursorGeo(-INT_MAX/2, 0);
  } else if (event == Ion::Events::ShiftRight) {
    m_contentView.moveCursorGeo(INT_MAX/2, 0);
  } else if (event == Ion::Events::Backspace) {
    return removeChar();
  } else if (event.hasText()) {
    return handleEventWithText(event.text());
  } else if (event == Ion::Events::EXE) {
    return handleEventWithText("\n");
  } else if (event == Ion::Events::Clear) {
    if (!m_contentView.removeEndOfLine()) {
      m_contentView.removeStartOfLine();
    }
  } else if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::sharedClipboard()->storedText());
  } else {
    return false;
  }
  scrollToCursor();
  return true;
}

void TextArea::setText(char * textBuffer, size_t textBufferSize) {
  m_contentView.setText(textBuffer, textBufferSize);
  m_contentView.moveCursorGeo(0, 0);
}

bool TextArea::insertTextWithIndentation(const char * textBuffer, int location) {
  int indentation = indentationBeforeCursor();
  char spaceString[indentation+1]; // WOUHOU
  for (int i = 0; i < indentation; i++) {
    spaceString[i] = ' ';
  }
  spaceString[indentation] = 0;
  int spaceStringSize = strlen(spaceString);
  int textSize = strlen(textBuffer);
  int totalIndentationSize = 0;
  for (size_t i = 0; i < strlen(textBuffer); i++) {
    if (textBuffer[i] == '\n') {
      totalIndentationSize+=spaceStringSize;
    }
  }
  if (m_contentView.getText()->textLength() + textSize + totalIndentationSize >= m_contentView.getText()->bufferSize() || textSize == 0) {
    return false;
  }
  int currentLocation = location;
  for (size_t i = 0; i < strlen(textBuffer); i++) {
    const char charString[] = {textBuffer[i], 0};
    insertTextAtLocation(charString, currentLocation++);
    if (textBuffer[i] == '\n') {
      insertTextAtLocation(spaceString, currentLocation);
      currentLocation += strlen(spaceString);
    }
  }
  return true;
}

int TextArea::indentationBeforeCursor() const {
  int charIndex = cursorLocation()-1;
  int indentationSize = 0;
  while (charIndex >= 0 && m_contentView.text()[charIndex] != '\n') {
    if (m_contentView.text()[charIndex] == ' ') {
      indentationSize++;
    } else {
      indentationSize = 0;
    }
    charIndex--;
  }
  return indentationSize;
}
