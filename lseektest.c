#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define TEST_SIZE 9

int
main(void)
{
  char data[TEST_SIZE];
  char expected[TEST_SIZE] = {
    'A', 'B', 'C', 0, 0, 0, 0, 0, 'Z'
  };

  int fd;
  int position;
  int count;
  int i;

  // xv6 open with O_CREATE does not necessarily truncate an old file.
  unlink("seekfile");

  fd = open("seekfile", O_CREATE | O_RDWR);

  if(write(fd, "ABC", 3) != 3){
    printf(2, "lseektest: initial write failed\n");
    close(fd);
    exit();
  }

  // Current offset is 3. Move forward by 5 bytes.
  position = lseek(fd, 5);
  printf(1, "forward seek returned %d\n", position);

  if(position != 8){
    printf(2, "lseektest: expected offset 8\n");
    close(fd);
    exit();
  }

  if(write(fd, "Z", 1) != 1){
    printf(2, "lseektest: final write failed\n");
    close(fd);
    exit();
  }

  // Current offset is 9. Move backward to the beginning.
  position = lseek(fd, -9);
  printf(1, "backward seek returned %d\n", position);

  if(position != 0){
    printf(2, "lseektest: expected offset 0\n");
    close(fd);
    exit();
  }

  // Seeking before the beginning must fail.
  if(lseek(fd, -1) != -1){
    printf(2, "lseektest: invalid negative seek succeeded\n");
    close(fd);
    exit();
  }

  count = read(fd, data, sizeof(data));

  if(count != TEST_SIZE){
    printf(2, "lseektest: expected %d bytes, received %d\n",
           TEST_SIZE, count);
    close(fd);
    exit();
  }

  printf(1, "file bytes:");

  for(i = 0; i < TEST_SIZE; i++){
    printf(1, " %d", data[i]);

    if(data[i] != expected[i]){
      printf(2, "\nlseektest: incorrect value at byte %d\n", i);
      close(fd);
      exit();
    }
  }

  printf(1, "\nlseek test passed\n");

  close(fd);
  exit();
}