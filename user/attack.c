#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define SCAN_PAGES 1024
#define SECRET_OFFSET 16

int
main(void)
{
  const char marker[] = "This may help.";
  int size = SCAN_PAGES * PGSIZE;
  char *mem = sbrk(size);

  if (mem == SBRK_ERROR)
    exit(1);

  for (int i = 0; i + SECRET_OFFSET < size; i++) {
    if (memcmp(mem + i, marker, sizeof(marker)) != 0)
      continue;

    int end = i + SECRET_OFFSET;
    while (end < size && mem[end] != '\0')
      end++;

    if (end < size) {
      printf("%s\n", mem + i + SECRET_OFFSET);
      exit(0);
    }
  }

  exit(1);
}
