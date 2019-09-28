#include <ion/archive.h>

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

bool fileAtIndex(size_t index, File &entry) {
  const TarHeader* tar = reinterpret_cast<const TarHeader*>(0x90000000);
  unsigned size = 0;

  /**
   * TAR files are comprised of a set of records aligned to 512 bytes boundary
   * followed by data. First record is always us, so don't bother checking its
   * vailidy.
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
    if (memcmp(tar->magic, "ustar  ", 8) || tar->name[0] == '\x00' || tar->name[0] == '\xFF')
        return false;
  }

  // File entry found, copy data out.
  strlcpy(entry.name, tar->name, MaxNameLength);
  entry.data = reinterpret_cast<const uint8_t*>(tar) + sizeof(TarHeader);
  entry.dataLength = size;

  return true;
}

bool executeFile(const char *name) {
  return false;
}

}
}
