#include <ion/archive.h>

#include <string.h>

namespace Ion {
namespace Archive {

int indexFromName(const char *name) {
  File entry;

  for (int i = 0; fileAtIndex(i, entry); i++)
    if (strcmp(name, entry.name) == 0) {
      return i;
    }

  return -1;
}

size_t numberOfFiles() {
  File dummy;
  size_t count;

  for (count = 0; fileAtIndex(count, dummy); count++);

  return count;
}

}
}
