#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAXLINES 512 // maximum number of lines to be sorted
#define MAXLINE  512 // maximum length of a line

static char *lines[MAXLINES];
static int nlines;

static int reverse_order; // -r
static int numeric_order; // -n
static int unique_only; // -u


// save_line
// Save a line to the lines array
static int save_line(char *buf, int length) {
    char *copy;

    if (nlines >= MAXLINES) {
        printf(2, "sort: too many lines\n");
        return -1;
    }
    // Allocate memory for the line and copy it to the lines array
    copy = malloc(length + 1);
    
    if (copy == 0) {
        printf(2, "sort: out of memory\n");
        return -1;
    }

    memmove(copy, buf, length);
    copy[length] = 0; // null terminate the string
    lines[nlines] = copy;
    nlines++;
    return 0;
}


// read_lines
// Read lines from the file descriptor fd and store them in the lines array
static int read_lines(int fd) {
    char buf[MAXLINE];
    char c;
    int length = 0;
    int n = 0;

    while((n = read(fd, &c, 1)) > 0) {
        if(c == '\n') {
            if(save_line(buf, length) < 0) {
                return -1;
            }
            length = 0;
        } else {
            if(length >= MAXLINE - 1) {
                printf(2, "sort: line too long\n");
                return -1;
            }
            // 
            buf[length++] = c;
        }
    }

    if(n < 0) {
        printf(2, "sort: read error\n");
        return -1;
    }

    // If the last line does not end with a newline, save it
    if(length > 0) {
        if(save_line(buf, length) < 0) {
            return -1;
        }
    }

    return 0;
}

// sort_lines
// Sort the lines in the lines array based on the specified options
static void sort_lines(void) {
    char *current;
    int i, j;

    for(i = 1; i < nlines; i++) {
        current = lines[i];
        j = i;

        // Compare the current line with the previous lines and insert it in the correct position
        while (j > 0) {
            int cmp;
            if (numeric_order) {
                cmp = atoi(current) - atoi(lines[j - 1]);
            } else {
                cmp = strcmp(current, lines[j - 1]);
            }

            if ((reverse_order && cmp > 0) || (!reverse_order && cmp < 0)) {
                lines[j] = lines[j - 1];
                j--;
            } else {
                break;
            }
        }
        lines[j] = current;
    }
}

// print_lines
// Print the sorted lines to standard output, considering the unique_only option
static void print_lines(void) {
    int i;
    for (i = 0; i < nlines; i++) {
        // If unique_only is set, skip duplicate lines
        if (unique_only && i > 0 && strcmp(lines[i], lines[i - 1]) == 0) {
            continue;
        }
        // else, print the line
        printf(1, "%s\n", lines[i]);
    }
}

int main(int argc, char *argv[]) {
    int fd;
    int file_count;
    int i, j;

    reverse_order = 0;
    numeric_order = 0;
    unique_only = 0;
    file_count = 0;

    // Parse command line arguments
    for (i = 1; i < argc; i++) {
        // if the argument starts with '-', and the next character is not '\0', then it is an option
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'r':
                        reverse_order = 1;
                        break;
                    case 'n':
                        numeric_order = 1;
                        break;
                    case 'u':
                        unique_only = 1;
                        break;
                    default:
                        printf(2, "sort: invalid option -- '%c'\n", argv[i][j]);
                        exit();
                }
            }
        } else {
            file_count++;
        }
    }

    // read lines from standard input or from specified files
    // If no files are specified, read from standard input
    if (file_count == 0) {
        if(read_lines(0)<0){
            exit();        
        }
    } else {
        // Read lines from each specified file
        for (i = 1; i < argc; i++) {
            if (argv[i][0] != '-') {
                fd = open(argv[i], O_RDONLY);
                if (fd < 0) {
                    printf(2, "sort: cannot open %s\n", argv[i]);
                    exit();
                }
                if(read_lines(fd)<0){
                    close(fd);
                    exit();        
                }
                close(fd);
            }
        }
    }

    sort_lines();
    print_lines();

    // Free the memory allocated for each line
    for (i = 0; i < nlines; i++) {
        free(lines[i]);
    }

    exit();
}
