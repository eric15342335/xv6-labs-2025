#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  // Your code here.
  //processes fmt until we hit null terminator
  while (fmt // checks null ptr
    && *fmt // checks \0
    ) {
    switch (*fmt++) {
      case 'i':
        printf("%d", *(int*) data);
        data += sizeof(int);
        break;
      case 'p':
        printf("%lx", *(uint64*) data);
        data += sizeof(uint64);
        break;
      case 'h':
        printf("%d", *(uint16*) data);
        /*
        question: why %d instead of %x;
        ans: since %d is for decimal, %x is for hex display format.
          program reads 16 bits of data in "*(uint16*) data".
          printf format string does not controls the output
        */
        data += sizeof(uint16);
        break;
      case 'c':
        printf("%c", *data);
        data += sizeof(char);
        break;
      case 's': {// 64-bit pointer to a C string, print the string
        printf("%s", (char*) *((uint64*) data));
        data += sizeof(uint64);
        break;
      }
      case 'S':
        printf("%s", data);
        while(*(data++))
          ;
        break;
      default:
        break;
    }
    printf("\n");
  }
}
