#ifndef ENCLAVESHIM_OCALLS_H_
#define ENCLAVESHIM_OCALLS_H_

#include <stdio.h>
#include <stddef.h>

int my_fprintf(FILE *stream, const char *format, ...);
int my_printf(const char *format, ...);
void exit(int status);

#endif
