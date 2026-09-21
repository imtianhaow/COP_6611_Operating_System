#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path, int isdir)
{
  // in case the file is a directory, we need to add a "/" at the end of the name
  // so the size should be DIRSIZ + 2, one for "/", another for the null terminator
  static char buf[DIRSIZ+2];
  char *p;
  int n; // length of the name

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  n = strlen(p);  

  // Return blank-padded name.
  if(n > DIRSIZ)
    n = DIRSIZ;

  memmove(buf, p, n);

  // check if the path is a directory, if so, add a "/" at the end of the name
  if (isdir) {
    buf[n++] = '/';
  }

  memset(buf+n, ' ', DIRSIZ-n+1);
  buf[DIRSIZ+1] = 0; // null terminate the string
  return buf;
}

void
ls(char *path, int show_dir)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  // file
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path, 0), st.type, st.ino, st.size);
    break;
  // directory -  
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      // skip hidden files if show_dir is false
      if(!show_dir && de.name[0] == '.')
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      // add '/' at the end of the name
      printf(1, "%s %d %d %d\n", fmtname(buf, st.type == T_DIR), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  int show_dir = 0; // flag to indicate whether to show hidden files or not
  int path_count = 0; // count of paths provided

  show_dir = 0;
  path_count = 0;

  // traverse the arguments to check for -a option
  for(i = 1; i < argc; i++){
    if(strcmp(argv[i], "-a") == 0){
      // printf(1, "Option -a detected: showing hidden files.\n");
      show_dir = 1;
    }
  }

  // traverse the arguments again 
  for (i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-a") == 0) {
      continue;
    }
    // printf(1, "Listing directory: %s\n", argv[i]);
    ls(argv[i], show_dir);
    path_count++;
  }

  // if no paths are provided, list the current directory
  if (path_count == 0) {
    // printf(1, "current show_dir value: %d\n", show_dir);
    ls(".", show_dir);
  }

  exit();
}
