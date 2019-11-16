#include <ion/archive.h>
#include <api.h>

#include <string.h>
#include <stdlib.h>

namespace Ion {
namespace Archive {

struct TarHeader
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[8];                /* 257 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char padding[167];            /* 345 */
} __attribute__((packed));

static_assert(sizeof(TarHeader) == 512);

bool isSane(const TarHeader* tar) {
  return !memcmp(tar->magic, "ustar  ", 8) && tar->name[0] != '\x00' && tar->name[0] != '\xFF';
}

bool fileAtIndex(size_t index, File &entry) {
  const TarHeader* tar = reinterpret_cast<const TarHeader*>(0x90200000);
  unsigned size = 0;

  // Sanity check.
  if (!isSane(tar)) {
    return false;
  }

  /**
   * TAR files are comprised of a set of records aligned to 512 bytes boundary
   * followed by data.
   */
  while (index-- > 0) {
    size = 0;
    for (int i = 0; i < 11; i++)
      size = size * 8 + (tar->size[i] - '0');

    // Move to the next TAR header.
    unsigned stride = (sizeof(TarHeader) + size + 511);
    stride = (stride >> 9) << 9;
    tar = reinterpret_cast<const TarHeader*>(reinterpret_cast<const char*>(tar) + stride);

    // Sanity check.
    if (!isSane(tar)) {
      return false;
    }
  }

  // File entry found, copy data out.
  entry.name = tar->name;
  entry.data = reinterpret_cast<const uint8_t*>(tar) + sizeof(TarHeader);
  entry.dataLength = size;

  return true;
}

typedef uint32_t (*entrypoint)(const uint32_t, const void *, void *, const uint32_t);

uint32_t executeFile(const char *name, void * heap, const uint32_t heapSize) {
  File entry;
  if(fileAtIndex(indexFromName(name), entry)) {
    uint32_t ep = *reinterpret_cast<const uint32_t*>(entry.data);
    if(ep >= 0x90200000 && ep < 0x90800000) {
      return ((entrypoint)ep)(API_VERSION, getApiPointers(), heap, heapSize);
    }
  }
  return -1;
}

int indexFromName(const char *name) {
  File entry;

  for (int i = 0; fileAtIndex(i, entry); i++) {
    if (strcmp(name, entry.name) == 0) {
      return i;
    }
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
