#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
  char buffer[BSIZE];
  int target;
  int fd;
  int i;
  int result;

  if(argc != 2){
    printf(2, "usage: bigfile number-of-blocks\n");
    exit();
  }

  target = atoi(argv[1]);

  if(target <= 0){
    printf(2, "bigfile: invalid block count\n");
    exit();
  }

  // O_CREATE does not truncate an existing xv6 file.
  unlink("bigdata");

  fd = open("bigdata", O_CREATE | O_RDWR);

  if(fd < 0){
    printf(2, "bigfile: could not create bigdata\n");
    exit();
  }

  memset(buffer, 0, sizeof(buffer));

  for(i = 0; i < target; i++){
    // Store the logical block number for simple verification.
    ((int*)buffer)[0] = i;

    result = write(fd, buffer, sizeof(buffer));

    if(result != sizeof(buffer)){
      printf(1, "write stopped at block %d\n", i);
      close(fd);
      unlink("bigdata");
      exit();
    }

    if((i + 1) % 4096 == 0)
      printf(1, "wrote %d blocks\n", i + 1);
  }

  printf(1, "successfully wrote %d blocks\n", target);

  close(fd);

  // Reopen and verify the final block.
  fd = open("bigdata", O_RDONLY);

  if(fd < 0){
    printf(2, "bigfile: could not reopen bigdata\n");
    exit();
  }

  if(lseek(fd, (target - 1) * BSIZE) < 0){
    printf(2, "bigfile: lseek failed\n");
    close(fd);
    exit();
  }

  if(read(fd, buffer, sizeof(buffer)) != sizeof(buffer)){
    printf(2, "bigfile: final block read failed\n");
    close(fd);
    exit();
  }

  if(((int*)buffer)[0] != target - 1){
    printf(2, "bigfile: final block contains incorrect data\n");
    close(fd);
    exit();
  }

  printf(1, "verified final block %d\n", target - 1);

  close(fd);

  // Removing the file exercises itrunc().
  if(unlink("bigdata") < 0){
    printf(2, "bigfile: could not remove bigdata\n");
    exit();
  }

  printf(1, "removed file and released its blocks\n");
  exit();
}