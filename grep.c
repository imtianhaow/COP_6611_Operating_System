// Simple grep.  Only supports ^ . * $ operators.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[1024];
int match(char*, char*);

static int ignore_case = 0;
static int show_line_numbers = 0;
static int invert_match = 0;

static char to_lower(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c + ('a' - 'A');
  }
  return c;
}

static int char_compare(char pattern, char text) {
  if (pattern == '.') {
    return 1; // match any character
  }
  // if ignore_case is set, convert both characters to lowercase before comparing
  if (ignore_case) {
    return to_lower(pattern) == to_lower(text);
  }

  return pattern == text;
}


void
grep(char *pattern, int fd)
{
  int n, m;
  char *p, *q;
  int line_number;
  int selected;

  m = 0;
  line_number = 0;
  // read from the file descriptor fd and process the input line by line
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    m += n;
    buf[m] = '\0';
    p = buf;

    // once a newline character is found, process the line and check for a match with the pattern
    while((q = strchr(p, '\n')) != 0){
      *q = 0;
      line_number++;
      selected = match(pattern, p);
      // if the invert_match flag is set, invert the selection
      if(invert_match)
        selected = !selected;
      
      if(selected){
        // if the show_line_numbers flag is set, print the line number before the matching line
        if(show_line_numbers){
          printf(1, "%d:", line_number);
        }
        *q = '\n';
        write(1, p, q+1 - p);
      }
      p = q+1;
    }
    // it could discard the remaining data in the buffer if its not enough to form a complete line
    // if(p == buf)
    //   m = 0;

    // if there is any remaining data in the buffer after processing the lines, 
    // move it to the beginning of the buffer for the next read
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }

  // make sure the last line is processed if it doesn't end with a newline character
  if(m > 0){
    buf[m] = '\0';
    line_number++;
    selected = match(pattern, buf);
    if(invert_match)
      selected = !selected;
    if(selected){
      if(show_line_numbers){
        printf(1, "%d:", line_number);
      }
      write(1, buf, m);
      write(1, "\n", 1);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd, i, j;
  char *pattern;

  // read the command line arguments and set the flags accordingly
  // achieve the flags combination
  i = 1;
  while(i < argc && argv[i][0] == '-' && argv[i][1] != '\0'){
    j = 1;
    while(argv[i][j] != '\0'){
      switch(argv[i][j]){
        case 'i':
          ignore_case = 1;
          break;
        case 'n':
          show_line_numbers = 1;
          break;
        case 'v':
          invert_match = 1;
          break;
        default:
          printf(2, "grep: unknown option: %c\n", argv[i][j]);
          exit();
      }
      j++;
    }
    i++;
  }

  // boundary check
  if(i >= argc){
    printf(2, "usage: grep pattern [file ...]\n");
    exit();
  }

  // get the pattern to search for
  pattern = argv[i];
  i++;

  // if no files are specified, read from standard input
  if(i >= argc){
    grep(pattern, 0);
    exit();
  }

  // else search for the pattern in each specified file
  for(; i < argc; i++){
    fd = open(argv[i], 0);
    if(fd < 0){
      printf(2, "grep: cannot open %s\n", argv[i]);
      exit();
    }
    grep(pattern, fd);
    close(fd);
  }
  exit();
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  // in this line, we use char_compare to compare the characters, 
  // which takes into account the ignore_case flag 
  if(*text!='\0' && char_compare(re[0], *text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && char_compare(c, *text++));
  return 0;
}

