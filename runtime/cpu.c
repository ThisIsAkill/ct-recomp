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

void (*ct_fatal_hook)(const char *msg);
uint64_t ct_budget;

void ct_fatal(const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    if (ct_fatal_hook)
        ct_fatal_hook(buf);
    fflush(stdout);
    fprintf(stderr, "ct: %s\n", buf);
    exit(3);
}
