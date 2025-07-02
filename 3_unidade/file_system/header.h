#include <stdlib.h>
#include <unistd.h>

int fs_open(const char* pathname, int flags);
int fs_create(const char* pathname, int flags, mode_t mode);
int fs_close(int fd);

ssize_t fs_write(int fd, const void* buf, size_t count);

// Copy the content of the block fd to the buffer and return the total size of what was copied.
ssize_t fS_read(int fd, void* buf, size_t count);
off_t fs_lseek(int fd, off_t offset, int whence);

int fs_ftruncate(int fd, off_t length);
int fs_unlink(const char *pathname);