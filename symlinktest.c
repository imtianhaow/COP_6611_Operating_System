#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

static void
fail(char *message)
{
  printf(2, "symlinktest: %s\n", message);
  exit();
}

static void
cleanup(void)
{
  unlink("target");
  unlink("soft");
  unlink("chain");
  unlink("dangling");
  unlink("cycle1");
  unlink("cycle2");
}

int
main(void)
{
  struct stat st;
  char buffer[64];
  int fd;
  int count;

  cleanup();

  // Create the real target file.
  fd = open("target", O_CREATE | O_RDWR);

  if(fd < 0)
    fail("could not create target");

  if(write(fd, "hello", 5) != 5)
    fail("could not write target");

  close(fd);

  // Test normal symbolic-link following.
  if(symlink("target", "soft") < 0)
    fail("could not create soft link");

  fd = open("soft", O_RDONLY);

  if(fd < 0)
    fail("could not follow soft link");

  count = read(fd, buffer, 5);
  close(fd);

  if(count != 5)
    fail("could not read through soft link");

  buffer[5] = '\0';

  if(strcmp(buffer, "hello") != 0)
    fail("soft link returned incorrect data");

  printf(1, "follow test: %s\n", buffer);

  // Test O_NOFOLLOW.
  fd = open("soft", O_RDONLY | O_NOFOLLOW);

  if(fd < 0)
    fail("O_NOFOLLOW failed");

  if(fstat(fd, &st) < 0)
    fail("fstat failed");

  if(st.type != T_SYMLINK)
    fail("O_NOFOLLOW did not open the symlink inode");

  count = read(fd, buffer, sizeof(buffer) - 1);
  close(fd);

  if(count <= 0)
    fail("could not read symlink target");

  buffer[count] = '\0';

  if(strcmp(buffer, "target") != 0)
    fail("stored target pathname is incorrect");

  printf(1, "nofollow test: type %d, target %s\n",
         st.type, buffer);

  // Test a chain: chain -> soft -> target.
  if(symlink("soft", "chain") < 0)
    fail("could not create link chain");

  fd = open("chain", O_RDONLY);

  if(fd < 0)
    fail("could not follow link chain");

  count = read(fd, buffer, 5);
  close(fd);

  if(count != 5)
    fail("could not read through link chain");

  buffer[5] = '\0';
  printf(1, "chain test: %s\n", buffer);

  // Creating a link to a nonexistent target must succeed.
  if(symlink("missing", "dangling") < 0)
    fail("could not create dangling link");

  fd = open("dangling", O_RDONLY);

  if(fd >= 0){
    close(fd);
    fail("dangling link unexpectedly opened");
  }

  printf(1, "dangling link test passed\n");

  // Test cycle detection.
  if(symlink("cycle2", "cycle1") < 0)
    fail("could not create cycle1");

  if(symlink("cycle1", "cycle2") < 0)
    fail("could not create cycle2");

  fd = open("cycle1", O_RDONLY);

  if(fd >= 0){
    close(fd);
    fail("symbolic-link cycle unexpectedly opened");
  }

  printf(1, "cycle detection test passed\n");

  // unlink() should remove only the symbolic link.
  if(unlink("soft") < 0)
    fail("could not unlink soft link");

  fd = open("target", O_RDONLY);

  if(fd < 0)
    fail("unlinking symlink removed target");

  close(fd);

  printf(1, "unlink preserved target\n");
  printf(1, "all symlink tests passed\n");

  cleanup();
  exit();
}