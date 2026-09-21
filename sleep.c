#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int ticks;

    // limit the number of arguments to 2
    if (argc != 2) {
        printf(2, "Error: usage sleep <integer>\n");
        exit();
    }

    ticks = atoi(argv[1]); // convert string to integer
    sleep(ticks); // call the sleep system call

    exit();
}