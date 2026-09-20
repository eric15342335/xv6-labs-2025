#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

void execifmatch(char *path, char *filename, void (*callback)(char*))
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if (strcmp(p, filename) == 0) callback(path);
}

void
find(char *path, char *filename, void (*callback)(char*))
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  //debug
  //printf("find: path=%s, filename=%s, st.type=%d\n", path, filename, st.type);

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    execifmatch(path, filename, callback);
    break;

  case T_DIR:
    /* xv6 textbook page 99
    A directory is implemented internally much like a file. Its inode has type T_DIR and its data is
    a sequence of directory entries. Each entry is a struct dirent, which contains a name
    and an inode number. The name is at most DIRSIZ (14) characters; if shorter, it is terminated by a
    NULL (0) byte. Directory entries with inode number zero are free.
    */
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf); // move `p` to \0's ptr
    *p++ = '/'; // path="." -> "./"
    while(read(fd, &de, sizeof(de)) == sizeof(de)){ // read() returns the number of bytes read
      // we read dirent of `path` to check stuff inside directory `path`
      if(de.inum == 0 // skip empty directory. inum == 0 implies free dirent
        || strcmp(de.name, ".") == 0
        || strcmp(de.name, "..") == 0) // skip `.` and `..`
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0; // \0 terminate the string manually, 0-indexing
      find(buf, filename, callback);
    }
    break;
  }
  close(fd);
}

char **exec_argv;
int exec_argc = 0; // argc value to pass to cmd

void exec_cmd(char *path)
{
  if (exec_argc == 0) {
    printf("%s\n", path);
    return;
  }

  int child_pid = fork();

  if (child_pid == 0) {
    char *args[MAXARG];

    for(int i = 0; i < exec_argc; i++)
      args[i] = exec_argv[i];

    args[exec_argc] = path;
    args[exec_argc + 1] = 0; // \0 terminate the array of char

    exec(
      args[0], // program name
      args
    );

    // exec never returns unless error
    fprintf(2, "ERROR: could not execute exec\n");
    exit(1);
  } else {
    int _status; //unused
    wait(&_status);
  }
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "usage: find directory filename [-exec cmd]\n");
    exit(1);
  }

  // parse -exec
  if (argc >= 4) {
    if (strcmp(argv[3], "-exec") == 0) {
      exec_argc = argc - 4;
      exec_argv = &argv[4];
    } else {
      fprintf(2, "usage: find directory filename [-exec cmd]\n");
      exit(1);
    }
  }

  find(argv[1], argv[2], exec_cmd);
  exit(0);
}
