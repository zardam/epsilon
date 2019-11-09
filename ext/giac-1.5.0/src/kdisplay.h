// -*- mode:C++ ; compile-command: "g++ -I.. -g -c Equation.cc" -*-
#ifndef _KDISPLAY_H
#define _KDISPLAY_H
#include "config.h"
#include "giacPCH.h"
#ifdef KHICAS
#include "misc.h"

extern  const int LCD_WIDTH_PX;
extern   const int LCD_HEIGHT_PX;
#define STATUS_AREA_PX 0 // 24
#define GIAC_HISTORY_MAX_TAILLE 32
#define GIAC_HISTORY_SIZE 2

extern "C" {
#include "k_csdk.h"
}

  int select_item(const char ** ptr,const char * title);
  const char * gettext(const char * s) ;

#ifndef NO_NAMESPACE_XCAS
namespace xcas {
#endif // ndef NO_NAMESPACE_XCAS
  int giac_filebrowser(char * filename,const char * extension,const char * title);
  void draw_rectangle(int x,int y,int w,int h,int c);
  void draw_line(int x0,int y0,int x1,int y1,int c);
  void draw_circle(int xc,int yc,int r,int color,bool q1=true,bool q2=true,bool q3=true,bool q4=true);
  void draw_filled_circle(int xc,int yc,int r,int color,bool left=true,bool right=true);
  void draw_polygon(std::vector< std::vector<int> > & v1,int color);
  void draw_filled_polygon(std::vector< vector<int> > &L,int xmin,int xmax,int ymin,int ymax,int color);
  void draw_arc(int xc,int yc,int rx,int ry,int color,double theta1, double theta2);
  void draw_filled_arc(int x,int y,int rx,int ry,int theta1_deg,int theta2_deg,int color,int xmin,int xmax,int ymin,int ymax,bool segment);
			   
  bool textedit(char * s,int bufsize,const giac::context * contextptr);
  // maximum "size" of symbolics displayed in an Equation (pretty print)
  extern unsigned max_prettyprint_equation;
  // matrix select
  bool eqw_select(const giac::gen & eq,int l,int c,bool select,giac::gen & value);
  void Equation_select(giac::gen & eql,bool select);
  int eqw_select_down(giac::gen & g);
  int eqw_select_up(giac::gen & g);

  giac::gen Equation_compute_size(const giac::gen & g,const giac::attributs & a,int windowhsize,const giac::context * contextptr);
  giac::eqwdata Equation_total_size(const giac::gen & g);  
  // Equation_translate(giac::gen & g,int deltax,int deltay);
  void Equation_vertical_adjust(int hp,int yp,int & h,int & y);
  bool Equation_find_vector_pos(giac::const_iterateur it,giac::const_iterateur itend,int & i,int &nrows);
  bool Equation_adjust_xy(giac::gen & g,int & xleft,int & ytop,int & xright,int & ybottom,giac::gen * & gsel,giac::gen * & gselparent,int &gselpos,std::vector<int> * gotosel=0);
  // select and set value from eqwdata in eql
  bool do_select(giac::gen & eql,bool select,giac::gen & value);

  class Equation {
    int _x,_y;
  public:
    giac::gen data,undodata; // of type eqwdata or undef if empty
    giac::attributs attr;
    const giac::context * contextptr;
    int x() const { return _x;}
    int y() const { return _y;}
    Equation(int x_, int y_, const giac::gen & g,const giac::context *);
  };

  void display(Equation &eq ,int x,int y,const giac::context *);
  // replace selection in eq by tmp
  void replace_selection(Equation & eq,const giac::gen & tmp,giac::gen * gsel,const std::vector<int> * gotoptr,const giac::context *);
  int eqw_select_leftright(xcas::Equation & g,bool left,int exchange,const giac::context *);

  class Graph2d{
  public:
    double window_xmin,window_xmax,window_ymin,window_ymax,
      x_scale,y_scale,x_tick,y_tick;
    int display_mode,show_axes,show_names,labelsize;
    giac::gen g;
    const giac::context * contextptr;
    bool findij(const giac::gen & e0,double x_scale,double y_scale,double & i0,double & j0,const giac::context * ) const;
    void update();
    void zoomx(double d,bool round=false);
    void zoomy(double d,bool round=false);
    void zoom(double);
    void left(double d);
    void right(double d);
    void up(double d);
    void down(double d);
    void autoscale(bool fullview=false);
    void orthonormalize();
    void draw();
    Graph2d(const giac::gen & g_,const giac::context * );
  };

  struct Turtle {
    void draw();
#ifdef TURTLETAB
    giac::logo_turtle * turtleptr;
#else
    std::vector<giac::logo_turtle> * turtleptr;
#endif
    int turtlex,turtley; // Turtle translate
    double turtlezoom; // Zoom factor for turtle screen
    int maillage; // 0 (none), 1 (square), 2 (triangle), bit3 used for printing
  };
  
  void displaygraph(const giac::gen & ge, const giac::context * contextptr);
  void displaylogo();
  giac::gen eqw(const giac::gen & ge,bool editable,const giac::context * contextptr);
  typedef short int color_t;
  typedef struct
  {
    std::string s;
    color_t color=giac::_BLACK;
    short int newLine=0; // if 1, new line will be drawn before the text
    short int spaceAtEnd=0;
    short int lineSpacing=0;
    short int minimini=0;
    int nlines=1;
  } textElement;

#define TEXTAREATYPE_NORMAL 0
#define TEXTAREATYPE_INSTANT_RETURN 1
  typedef struct
  {
    int x=0;
    int y=0;
    int line=0,undoline=0;
    int pos=0,undopos=0;
    int clipline,undoclipline;
    int clippos,undoclippos;
    int width=LCD_WIDTH_PX;
    int lineHeight=17;
    std::vector<textElement> elements,undoelements;
    const char* title = NULL;
    std::string filename;
    int scrollbar=1;
    bool allowEXE=false; //whether to allow EXE to exit the screen
    bool allowF1=false; //whether to allow F1 to exit the screen
    bool OKparse=true;
    bool editable=false;
    bool changed=false;
    int python=0;
    int type=TEXTAREATYPE_NORMAL;
  } textArea;

#define TEXTAREA_RETURN_EXIT 0
#define TEXTAREA_RETURN_EXE 1
#define TEXTAREA_RETURN_F1 2
  int doTextArea(textArea* text,const giac::context * contextptr); //returns 0 when user EXITs, 1 when allowEXE is true and user presses EXE, 2 when allowF1 is true and user presses F1.
  std::string merge_area(const std::vector<textElement> & v);
  void save_script(const char * filename,const std::string & s);
  void add(textArea *edptr,const std::string & s);

  extern textArea * edptr;
  std::string get_searchitem(std::string & replace);
  int check_leave(textArea * text);
  void reload_edptr(const char * filename,textArea *edptr,const giac::context *);
  void print(int &X,int&Y,const char * buf,int color,bool revert,bool fake,bool minimini);

#if 1
#define MAX_FILENAME_SIZE 270
  void save_console_state_smem(const char * filename,const giac::context *);
  bool load_console_state_smem(const char * filename,const giac::context *);

  struct DISPBOX {
    int     left;
    int     top;
    int     right;
    int     bottom;
    unsigned char mode;
  } ;


  enum CONSOLE_RETURN_VAL {
			   CONSOLE_NEW_LINE_SET = 1,
			   CONSOLE_SUCCEEDED = 0,
			   CONSOLE_MEM_ERR = -1,
			   CONSOLE_ARG_ERR = -2,
			   CONSOLE_NO_EVENT = -3
  };

  enum CONSOLE_CURSOR_DIRECTION{
				CURSOR_UP,
				CURSOR_DOWN,
				CURSOR_LEFT,
				CURSOR_RIGHT,
				CURSOR_SHIFT_LEFT,
				CURSOR_SHIFT_RIGHT,
				CURSOR_ALPHA_UP,
				CURSOR_ALPHA_DOWN,
  };

  enum CONSOLE_LINE_TYPE{
			 LINE_TYPE_INPUT=0,
			 LINE_TYPE_OUTPUT=1
  };

  enum CONSOLE_CASE{
		    LOWER_CASE,
		    UPPER_CASE
  };

  enum CONSOLE_SCREEN_SPEC {
			    _LINE_MAX = 48,
			    LINE_DISP_MAX = 11,
			    COL_DISP_MAX = 30,//32
			    EDIT_LINE_MAX = 2048
  };
  
  struct console_line {
    char *str;
    short int readonly;
    short int type;
    int start_col;
    int disp_len;
  };

  struct FMenu{
    char* name;
    char** str;
    unsigned char count;
  };

  struct location{
    int x;
    int y;
  };

#define MAX_FMENU_ITEMS 8
#define FMENU_TITLE_LENGHT 4

#define is_wchar(c) ((c == 0x7F) || (c == 0xF7) || (c == 0xF9) || (c == 0xE5) || (c == 0xE6) || (c == 0xE7))
#define printf(s) Console_Output((const char *)s);

  int Console_DelStr(char *str, int end_pos, int n);
  int Console_InsStr(char *dest, const char *src, int disp_pos);
  int Console_GetActualPos(const char *str, int disp_pos);
  int Console_GetDispLen(const char *str);
  int Console_MoveCursor(int direction);
  int Console_Input(const char *str);
  int Console_Output(const char *str);
  void Console_Clear_EditLine();
  int Console_NewLine(int pre_line_type, int pre_line_readonly);
  int Console_Backspace(void);
  int Console_GetKey(const giac::context *);
  int Console_Init(void);
  int Console_Disp(int redraw_mode=1);
  int Console_FMenu(int key,const giac::context *);
  extern char menu_f1[8],menu_f2[8],menu_f3[8],menu_f4[8],menu_f5[8],menu_f6[8];
  const char * console_menu(int key,char* cfg,int active_app);
  void Console_FMenu_Init(void);
  const char * Console_Draw_FMenu(int key, struct FMenu* menu,char * cfg_,int active_app);
  char *Console_Make_Entry(const char* str);
  char *Console_GetLine(const giac::context *);
  char* Console_GetEditLine();
  void dConsolePut(const char *);
  void dConsolePutChar(const char );
  void dConsoleRedraw(void);
  extern int dconsole_mode;
  extern int console_changed; // 1 if something new in history
  extern char session_filename[MAX_FILENAME_SIZE+1];
  const char * input_matrix(bool list,const giac::context *);
  void warn_python(int python,bool autochange=false);
  // void draw_menu(int editor); // 0 console, 1 editor
  int get_set_session_setting(int value);
  void menu_setup(const giac::context *);
  int console_main(const giac::context *);
#endif
  int periodic_table(const char * & name,const char * & symbol,char * protons,char * nucleons,char * mass,char * electroneg);


#ifndef NO_NAMESPACE_XCAS
} // namespace xcas
#endif // ndef NO_NAMESPACE_XCAS

/* ************************************************************
**************************************************************
***********************************************************  */


#ifndef NO_NAMESPACE_XCAS
namespace giac {
#endif // ndef NO_NAMESPACE_XCAS
#define TEXT_MODE_NORMAL 0
#define TEXT_MODE_INVERT 1
#define MENUITEM_NORMAL 0
#define MENUITEM_CHECKBOX 1
#define MENUITEM_SEPARATOR 2
#define MENUITEM_VALUE_NONE 0
#define MENUITEM_VALUE_CHECKED 1
  typedef struct
  {
    char* text; // text to be shown on screen. mandatory, must be a valid pointer to a string.
    int type=MENUITEM_NORMAL; // type of the menu item. use MENUITEM_* to set this
    int value=MENUITEM_VALUE_NONE; // value of the menu item. For example, if type is MENUITEM_CHECKBOX and the checkbox is checked, the value of this var will be MENUITEM_VALUE_CHECKED
    int color=giac::_BLACK; // color of the menu item (use TEXT_COLOR_* to define)
    // The following two settings require the menu type to be set to MENUTYPE_MULTISELECT
    int isfolder=0; // for file browsers, this will signal the item is a folder
    int isselected=0; // for file browsers and other multi-select screens, this will show an arrow before the item
    int icon=-1; //for file browsers, to show a file icon. -1 shows no icon (default)
    int token; // for syntax help on keywords not in the catalog
  } MenuItem;

  typedef struct
  {
    unsigned short data[0x12*0x18];
  } MenuItemIcon;

#define MENUTYPE_NORMAL 0
#define MENUTYPE_MULTISELECT 1
#define MENUTYPE_INSTANT_RETURN 2 // this type of menu insantly returns even if user hasn't selected an option (allows for e.g. redrawing the GUI behind it). if user hasn't exited or selected an option, menu will return MENU_RETURN_INSTANT
#define MENUTYPE_NO_KEY_HANDLING 3 //this type of menu doesn't handle any keys, only draws.
#define MENUTYPE_FKEYS 4 // returns GetKey value of a Fkey when one is pressed
  typedef struct {
    char* statusText = NULL; // text to be shown on the status bar, may be empty
    char* title = NULL; // title to be shown on the first line if not null
    char* subtitle = NULL;
    int titleColor=giac::_BLUE; //color of the title
    char* nodatamsg; // message to show when there are no menu items to display
    int startX=1; //X where to start drawing the menu. NOTE this is not absolute pixel coordinates but rather character coordinates
    int startY=0; //Y where to start drawing the menu. NOTE this is not absolute pixel coordinates but rather character coordinates
    int width=22; // NOTE this is not absolute pixel coordinates but rather character coordinates
    int height=12; // NOTE this is not absolute pixel coordinates but rather character coordinates
    int scrollbar=1; // 1 to show scrollbar, 0 to not show it.
    int scrollout=0; // whether the scrollbar goes out of the menu area (1) or it overlaps some of the menu area (0)
    int numitems; // number of items in menu
    int type=MENUTYPE_NORMAL; // set to MENUTYPE_* .
    int selection=1; // currently selected item. starts counting at 1
    int scroll=0; // current scrolling position
    int fkeypage=0; // for MULTISELECT menu if it should allow file selecting and show the fkey label
    int numselitems=0; // number of selected items
    int returnOnInfiniteScrolling=0; //whether the menu should return when user reaches the last item and presses the down key (or the first item and presses the up key)
    int darken=0; // for dark theme on homeGUI menus
    int miniMiniTitle=0; // if true, title will be drawn in minimini. for calendar week view
    int pBaRtR=0; //preserve Background And Return To Redraw. Rarely used
    MenuItem* items; // items in menu
  } Menu;

#define MENU_RETURN_EXIT 0
#define MENU_RETURN_SELECTION 1
#define MENU_RETURN_INSTANT 2
#define MENU_RETURN_SCROLLING 3 //for returnOnInfiniteScrolling

  typedef struct {
    const char* name;
    const char* insert;
    const char* desc;
    const char * example;
    const char * example2;
    int category;
  } catalogFunc;

  giac::gen select_var(const giac::context * contextptr);
  int showCatalog(char* insertText,int preselect,int menupos,const giac::context * contextptr);
  int doMenu(Menu* menu, MenuItemIcon* icontable=NULL);
  void reset_alpha();
  // category=0 for CATALOG, 1 for OPTN
  // returns 0 on exit, 1 on success
  int doCatalogMenu(char* insertText, const char* title, int category,const giac::context * contextptr);
  extern const char shortcuts_string[];
  extern const char apropos_string[];
  void init_locale();

  gen turtle_state(const giac::context * contextptr);
  int inputline(const char * msg1,const char * msg2,std::string & s,bool numeric,int ypos=65,const giac::context *contextptr=0);
  bool inputdouble(const char * msg1,double & d,const giac::context *contextptr);
  bool do_confirm(const char * s);
  int confirm(const char * msg1,const char * msg2,bool acexit=false,int y=40);
  bool confirm_overwrite();
  void invalid_varname();

#ifndef NO_NAMESPACE_XCAS
} // namespace giac
#endif // ndef NO_NAMESPACE_XCAS


#define COLOR_BLACK giac::_BLACK
#define COLOR_RED giac::_RED
#define COLOR_GREEN giac::_GREEN
#define COLOR_CYAN giac::_CYAN
#define COLOR_BLUE giac::_BLUE
#define COLOR_YELLOW giac::_YELLOW
#define COLOR_MAGENTA giac::_MAGENTA
#define COLOR_WHITE giac::_WHITE
#define COLOR_YELLOWDARK 64934
#define COLOR_BROWN 65000
#define TEXT_COLOR_BLACK giac::_BLACK
#define TEXT_COLOR_RED giac::_RED
#define TEXT_COLOR_GREEN giac::_GREEN
#define TEXT_COLOR_CYAN giac::_CYAN
#define TEXT_COLOR_BLUE giac::_BLUE
#define TEXT_COLOR_YELLOW giac::_YELLOW
#define TEXT_COLOR_WHITE giac::_WHITE
#define TEXT_COLOR_MAGENTA giac::_MAGENTA

// Character codes
#define KEY_CHAR_0          0x30
#define KEY_CHAR_1          0x31
#define KEY_CHAR_2          0x32
#define KEY_CHAR_3          0x33
#define KEY_CHAR_4          0x34
#define KEY_CHAR_5          0x35
#define KEY_CHAR_6          0x36
#define KEY_CHAR_7          0x37
#define KEY_CHAR_8          0x38
#define KEY_CHAR_9          0x39
#define KEY_CHAR_DP         0x2e
#define KEY_CHAR_EXP        0x0f
#define KEY_CHAR_PMINUS     30200
#define KEY_CHAR_PLUS       43
#define KEY_CHAR_MINUS      45
#define KEY_CHAR_MULT       42
#define KEY_CHAR_DIV        47
#define KEY_CHAR_FRAC       0xbb
#define KEY_CHAR_LPAR       0x28
#define KEY_CHAR_RPAR       0x29
#define KEY_CHAR_COMMA      0x2c
#define KEY_CHAR_STORE      0x0e
#define KEY_CHAR_LOG        0x95
#define KEY_CHAR_LN         0x85
#define KEY_CHAR_SIN        0x81
#define KEY_CHAR_COS        0x82
#define KEY_CHAR_TAN        0x83
#define KEY_CHAR_SQUARE     0x8b
#define KEY_CHAR_POW        0xa8
#define KEY_CHAR_IMGNRY     0x7f50
#define KEY_CHAR_LIST       0x7f51
#define KEY_CHAR_MAT        0x7f40
#define KEY_CHAR_EQUAL      0x3d
#define KEY_CHAR_PI         0xd0
#define KEY_CHAR_ANS        0xc0
#define KEY_SHIFT_ANS        0xc1
#define KEY_CHAR_LBRCKT     0x5b
#define KEY_CHAR_RBRCKT     0x5d
#define KEY_CHAR_LBRACE     0x7b
#define KEY_CHAR_RBRACE     0x7d
#define KEY_CHAR_CR         0x0d
#define KEY_CHAR_CUBEROOT   0x96
#define KEY_CHAR_RECIP      0x9b
#define KEY_CHAR_ANGLE      0x7f54
#define KEY_CHAR_EXPN10     0xb5
#define KEY_CHAR_EXPN       0xa5
#define KEY_CHAR_ASIN       0x91
#define KEY_CHAR_ACOS       0x92
#define KEY_CHAR_ATAN       0x93
#define KEY_CHAR_ROOT       0x86
#define KEY_CHAR_POWROOT    0xb8
#define KEY_CHAR_SPACE      0x20
#define KEY_CHAR_DQUATE     0x22
#define KEY_CHAR_VALR       0xcd
#define KEY_CHAR_THETA      0xce
#define KEY_CHAR_FACTOR     0xda
#define KEY_CHAR_NORMAL     0xdb
#define KEY_CHAR_SHIFTMINUS     0xdc
#define KEY_CHAR_A          0x41
#define KEY_CHAR_B          0x42
#define KEY_CHAR_C          0x43
#define KEY_CHAR_D          0x44
#define KEY_CHAR_E          0x45
#define KEY_CHAR_F          0x46
#define KEY_CHAR_G          0x47
#define KEY_CHAR_H          0x48
#define KEY_CHAR_I          0x49
#define KEY_CHAR_J          0x4a
#define KEY_CHAR_K          0x4b
#define KEY_CHAR_L          0x4c
#define KEY_CHAR_M          0x4d
#define KEY_CHAR_N          0x4e
#define KEY_CHAR_O          0x4f
#define KEY_CHAR_P          0x50
#define KEY_CHAR_Q          0x51
#define KEY_CHAR_R          0x52
#define KEY_CHAR_S          0x53
#define KEY_CHAR_T          0x54
#define KEY_CHAR_U          0x55
#define KEY_CHAR_V          0x56
#define KEY_CHAR_W          0x57
#define KEY_CHAR_X          0x58
#define KEY_CHAR_Y          0x59
#define KEY_CHAR_Z          0x5a


// Control codes
#define KEY_CTRL_FORMAT     30203
#define KEY_CTRL_NOP        30202
#define KEY_CTRL_EXE        30201
#define KEY_CTRL_DEL        30025
#define KEY_CTRL_AC         30070
#define KEY_CTRL_FD         30046
#define KEY_CTRL_UNDO	    30045	
#define KEY_CTRL_XTT        30001
#define KEY_CTRL_EXIT       5
#define KEY_CTRL_OK       4
#define KEY_CTRL_SHIFT      30006
#define KEY_CTRL_ALPHA      30007
#define KEY_CTRL_OPTN       30008
#define KEY_CTRL_VARS       30030
#define KEY_CTRL_UP         1
#define KEY_CTRL_DOWN       2
#define KEY_CTRL_LEFT       0
#define KEY_CTRL_RIGHT      3
#define KEY_CTRL_F1         30009
#define KEY_CTRL_F2         30010
#define KEY_CTRL_F3         30011
#define KEY_CTRL_F4         30012
#define KEY_CTRL_F5         30013
#define KEY_CTRL_F6         30014
#define KEY_CTRL_F7         30015
#define KEY_CTRL_F8         30016
#define KEY_CTRL_F9         30017
#define KEY_CTRL_F10         30018
#define KEY_CTRL_F11         30019
#define KEY_CTRL_F12         30020
#define KEY_CTRL_F13         30021
#define KEY_CTRL_F14         30022
#define KEY_CTRL_CATALOG    30100
#define KEY_CTRL_CAPTURE    30055
#define KEY_CTRL_CLIP       30050
#define KEY_CTRL_CUT       30250
#define KEY_CTRL_PASTE      30036
#define KEY_CTRL_INS        30033
#define KEY_CTRL_MIXEDFRAC  30054
#define KEY_CTRL_FRACCNVRT  30026
#define KEY_CTRL_QUIT       30029
#define KEY_CTRL_PRGM       30028
#define KEY_CTRL_SETUP      30037
#define KEY_CTRL_PAGEUP     30052
#define KEY_CTRL_PAGEDOWN   30053
#define KEY_CTRL_MENU       30003
#define KEY_SHIFT_OPTN	    30059
#define KEY_CTRL_RESERVE1	30060
#define KEY_CTRL_RESERVE2	30061
#define KEY_SHIFT_LEFT		30062
#define KEY_SHIFT_RIGHT		30063

#define KEY_PRGM_ACON 10
#define KEY_PRGM_DOWN 37
#define KEY_PRGM_EXIT 47
#define KEY_PRGM_F1 79
#define KEY_PRGM_F2 69
#define KEY_PRGM_F3 59
#define KEY_PRGM_F4 49
#define KEY_PRGM_F5 39
#define KEY_PRGM_F6 29
#define KEY_PRGM_LEFT 38
#define KEY_PRGM_NONE 0
#define KEY_PRGM_RETURN 31
#define KEY_PRGM_RIGHT 27
#define KEY_PRGM_UP 28
#define KEY_PRGM_1 72
#define KEY_PRGM_2 62
#define KEY_PRGM_3 52
#define KEY_PRGM_4 73
#define KEY_PRGM_5 63
#define KEY_PRGM_6 53
#define KEY_PRGM_7 74
#define KEY_PRGM_8 64
#define KEY_PRGM_9 54
#define KEY_PRGM_A 76
#define KEY_PRGM_F 26
#define KEY_PRGM_ALPHA 77 
#define KEY_PRGM_SHIFT 78
#define KEY_PRGM_MENU 48

#endif // _KDISPLAY_H
#endif
