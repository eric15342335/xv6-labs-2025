#include "kernel/types.h" // user.h depends on this
#include "kernel/fcntl.h" // open()'s O_RDONLY
#include "user/user.h" // ulib.c strchr atoi

int _is_separator(const char *s) {
    char separators[7] = " \r\t\n./,";
    char *ptr = separators;
    while (*ptr) {
        if (strchr(s, *ptr++)) {
            return 1;
        }
    }
    return 0;
}

void print_candidate_number(uint64 num) {
    // check number for multiple of 5 or 6
    if (num != 0 &&
        (num % 5 == 0 ||
        num % 6 == 0)) {
        printf("%lu\n", num);
    }
}

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    // print an error message
    char error_message[] = "usage: sixfive [sixfive.txt]\n";
    write(
        2, // stderr
        error_message,
        strlen(error_message)
    );
  } else {
    for (int i = 1; i < argc; i++) {
        // read file
        int fd = open(argv[i], O_RDONLY);

        // init
        char buf[1] = "\0";
        int separated = 1;
        uint64 number = 0;

        // read 1 character at the time
        while (1) {
            int result = read(fd, buf, sizeof buf);
            if (result == 0) { // end of file
                print_candidate_number(number); // end of file is a implicit separator
                break;
            }
            if (result < 0) {
                fprintf(2, "read error\n");
                exit(1);
            }
            /* state machine:
            [wait for a number (read all decimals)] <--> [wait for a separator]
            */
            if (separated) {
                if ('0' <= *buf && *buf <= '9') {
                    number = number*10 + atoi(buf);
                } else { // the decimial digit sequence ended
                    print_candidate_number(number);
                    number = 0;
                    separated = 0;
                }
                if (_is_separator(buf)) separated = 1;
            } else {
                if (_is_separator(buf)) separated = 1;
            }
        }
        // terminates
        close(fd);
    }
  }
  exit(0);
}
