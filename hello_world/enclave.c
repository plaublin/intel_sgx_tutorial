#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "enclave_t.h"

int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_printf(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

void ecall_entrypoint() {
	printf("Hello World!\n");
}
