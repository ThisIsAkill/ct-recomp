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
void (*ct_trace_hook)(const CPU *cpu, uint32_t addr);
void (*ct_tick_hook)(CPU *cpu, uint32_t addr, uint8_t op);
uint64_t ct_budget;
long ct_test_cap;
void (*ct_exec_hook)(CPU *cpu, const ct_exec_until *until);

int ct_exec_done(const CPU *c, const ct_exec_until *u)
{
    if (u->back == ~0u)
        return c->S > u->s;
    return c->S == u->s && ((uint32_t)c->PB << 16 | c->PC) == u->back;
}


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
