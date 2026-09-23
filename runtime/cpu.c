#include "cpu.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cpu_init(CPU *cpu)
{
    memset(cpu, 0, sizeof *cpu);
    cpu->S = 0x01FF;
    cpu->m = 1;
    cpu->x = 0;
    cpu->e = 0;
    cpu->i = 1;
}

void ct_fatal(const char *fmt, ...)
{
    va_list ap;
    fflush(stdout);
    fputs("ct: ", stderr);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(3);
}
