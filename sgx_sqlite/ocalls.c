#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>
#include <semaphore.h>

#include "enclaveshim_log.h"

#ifndef O_TMPFILE
/* a horrid kludge trying to make sure that this will fail on old kernels */
#define O_TMPFILE (__O_TMPFILE | O_DIRECTORY)
#endif

/******************** OCALLS ********************/
off_t ocall_lseek(int fd, off_t offset, int whence) {
	log_enter_ocall(__func__);
	off_t ret = lseek(fd, offset, whence);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_fsync(int fd){
        log_enter_ocall(__func__);
        int ret = fsync(fd);
        log_exit_ocall(__func__);
        return ret;
}

int ocall_unlink(const char* pathname){
	log_enter_ocall(__func__);
        int ret = unlink(pathname);
        log_exit_ocall(__func__);
        return ret;
}

ssize_t ocall_pread(int fd, void *buf, size_t count, off_t offset) {
	log_enter_ocall(__func__);
	ssize_t ret = pread(fd, buf, count, offset);
	log_exit_ocall(__func__);
	return ret;
}

ssize_t ocall_pwrite(int fd, const void *buf, size_t count, off_t offset) {
	log_enter_ocall(__func__);
	ssize_t ret = pwrite(fd, buf, count, offset);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_fcntl(int fd, int cmd, void* arg, size_t size) {
	log_enter_ocall(__func__);
	int ret = fcntl(fd, cmd, arg);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_fstat(int fd, struct stat *buf, size_t size) {
	log_enter_ocall(__func__);
	int ret = fstat(fd, buf);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_lstat(const char *pathname, struct stat *buf, size_t size) {
	log_enter_ocall(__func__);
	int ret = lstat(pathname, buf);
	log_exit_ocall(__func__);
	return ret;
}

void* ocall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset){
	log_enter_ocall(__func__);
	void* ret = mmap(addr, length, prot, flags, fd, offset);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_stat(const char *pathname, struct stat *buf, size_t size) {
	log_enter_ocall(__func__);
	int ret = stat(pathname, buf);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_ftruncate(int fd, off_t length) {
	log_enter_ocall(__func__);
        int ret = ftruncate(fd, length);
        log_exit_ocall(__func__);
        return ret;
}

char *ocall_getcwd(char *buf, size_t size) {
	log_enter_ocall(__func__);
	char* ret = getcwd(buf, size);
	log_exit_ocall(__func__);
	return ret;
}

void ocall_print_string(const char* str) {
	printf("%s", str);
	fflush(NULL);
}

void ocall_println_string(const char* str) {
	printf("%s\n", str);
	fflush(NULL);
}

void ocall_exit(int s) {
	exit(s);
}

void* ocall_malloc(size_t size) {
	log_enter_ocall(__func__);
	void* ret = malloc(size);
	log_exit_ocall(__func__);
	return ret;
}

void* ocall_realloc(void* ptr, size_t size) {
	log_enter_ocall(__func__);
	void* ret = realloc(ptr, size);
	log_exit_ocall(__func__);
	return ret;
}

void* ocall_calloc(size_t nmemb, size_t size) {
	log_enter_ocall(__func__);
	void* ret = calloc(nmemb, size);
	log_exit_ocall(__func__);
	return ret;
}

void ocall_free(void* ptr) {
	log_enter_ocall(__func__);
	free(ptr);
	log_exit_ocall(__func__);
}

void* ocall_fopen(const char *path, const char *mode) {
	log_enter_ocall(__func__);
	FILE* f = fopen(path, mode);
	log_exit_ocall(__func__);
	return (void*)f;
}

size_t _ocall_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream) {
	log_enter_ocall(__func__);
	size_t ret;
	if (!stream) {
		size_t i;
		for (i=0; i<size*nmemb; i++) {
			printf("%c", *((char*)ptr+i));
		}
		fflush(NULL);
		ret = size*nmemb;
	} else {
		ret = fwrite(ptr, size, nmemb, (FILE*)stream);
	}
	return ret;
	log_exit_ocall(__func__);
}

size_t ocall_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream) {
	return _ocall_fwrite(ptr, size, nmemb, stream);
}

size_t ocall_fwrite_copy(const void *ptr, size_t size, size_t nmemb, void *stream) {
	return _ocall_fwrite(ptr, size, nmemb, stream);
}

int ocall_fflush(void* stream) {
	return fflush((FILE*)stream);
}

int ocall_close(int fd) {
        log_enter_ocall(__func__);
        int ret = close(fd);
        log_exit_ocall(__func__);
        return ret;
}

int ocall_fclose(void* fp) {
	log_enter_ocall(__func__);
	int ret = fclose((FILE*)fp);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_open(const char *filename, int flags, mode_t mode) {
	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
		return open(filename, flags, mode);	
	} else {
		return open(filename, flags);
	}
}

int ocall_open64(const char *filename, int flags, mode_t mode) {
	/*
	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
		return open64(filename, flags, mode);	
	} else {
		return open64(filename, flags);
	}
	*/
	return ocall_open(filename, flags, mode);
}

char *ocall_fgets(char *s, size_t size, void *stream) {
	log_enter_ocall(__func__);
	char* ret = fgets(s, size, (FILE*)stream);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_read(int fd, void *buf, size_t count) {
	log_enter_ocall(__func__);
	int ret = (int)read(fd, buf, count);
	//printf("%s %d bytes\n", __func__, ret);
	log_exit_ocall(__func__);
	return ret;
}

int ocall_write(int fd, void *buf, size_t count) {
	log_enter_ocall(__func__);
	int ret = (int)write(fd, buf, count);
	//printf("%s %d bytes\n", __func__, ret);
	log_exit_ocall(__func__);
	return ret;
}

long int ocall_time(long int *t) {
	log_enter_ocall(__func__);
	long int ret = (long int)time((time_t*)t);
	log_exit_ocall(__func__);
	return ret;
}

uid_t ocall_getuid(){
	log_enter_ocall(__func__);
	uid_t ret = getuid();
	log_exit_ocall(__func__);
        return ret;
}

pid_t ocall_getpid(){
	log_enter_ocall(__func__);
	pid_t ret = getpid();
	log_exit_ocall(__func__);
	return ret;
	
}

int ocall_gettimeofday(struct timeval* tv, struct timezone* tz) {
	int ret = gettimeofday(tv, tz);
	return ret;
}

void ocall_nanosleep(unsigned long sec, unsigned long nanosec) {
	struct timespec ts;
	ts.tv_sec = sec;
	ts.tv_nsec = nanosec;
	nanosleep(&ts, NULL);
}

long ocall_sysconf(int name) {
	return sysconf(name);
}
