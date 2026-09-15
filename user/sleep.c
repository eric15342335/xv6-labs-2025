#include "kernel/types.h" // user.h depends on this
#include "user/user.h" // ulib.c atoi

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    // print an error message
    char error_message[] = "usage: sleep [ticks]\n";
    write(
        2, // stderr
        error_message,
        strlen(error_message)
    );
  } else {
    pause(atoi(argv[1]));
  }
  exit(0);
}
