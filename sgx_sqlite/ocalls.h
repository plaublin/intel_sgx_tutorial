#ifndef OCALLS_H_
#define OCALLS_H_

#include <stddef.h>

void ocall_print_string(const char* str);
void ocall_println_string(const char* str);
void ocall_exit(int s);
void* ocall_malloc(size_t size);
void* ocall_realloc(void* ptr, size_t size);
void* ocall_calloc(size_t nmemb, size_t size);
void ocall_free(void* ptr);
void* ocall_fopen(const char *path, const char *mode);
size_t ocall_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream);
size_t ocall_fwrite_copy(const void *ptr, size_t size, size_t nmemb, void *stream);
int ocall_fflush(void* stream);
int ocall_fclose(void* fp);
int ocall_close(int fd);
char* ocall_fgets(char *s, size_t size, void *stream);
void* ocall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int ocall_open(const char *pathname, int flags, mode_t mode);
int ocall_open64(const char *pathname, int flags, mode_t mode);
int ocall_stat(const char *path, struct stat *buf, size_t size);
int ocall_ftruncate(int fd, off_t length);
int ocall_fstat(int fd, struct stat *buf, size_t size);
int ocall_lstat(const char *path, struct stat *buf, size_t size);
int ocall_read(int fd, void *buf, size_t count);
int ocall_write(int fd, const void *buf, size_t count);
long int ocall_time(long int *t);
void ocall__gettimeofday(char* tv, char* tz, int tvs, int tzs);
pid_t ocall_getpid();
uid_t ocall_getuid();
int ocall_unlink(const char* pathname);
int ocall_fsync(int fd);
ssize_t ocall_pread(int fd, void *buf, size_t count, off_t offset);
ssize_t ocall_pwrite(int fd, const void *buf, size_t count, off_t offset);
long ocall_sysconf(int name);

#endif
