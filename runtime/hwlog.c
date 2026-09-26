#include "hwlog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_NOTES 64
static char notes[MAX_NOTES][160];
static unsigned n_notes;

void hw_note(const char *fmt, ...)
{
    char buf[160];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    for (unsigned k = 0; k < n_notes; k++)
        if (!strcmp(notes[k], buf))
            return;
    if (n_notes < MAX_NOTES)
        snprintf(notes[n_notes++], sizeof notes[0], "%s", buf);
}

unsigned hw_note_count(void) { return n_notes; }
const char *hw_note_text(unsigned i) { return i < n_notes ? notes[i] : ""; }

int hw_needed_write(const char *path, const char *tool, const char *stop)
{
    FILE *f = fopen(path, "w");
    if (!f)
        return -1;
    fprintf(f, "# Needed hardware\n\nWritten by `%s --needed-hw`. Do not edit by hand.\n\n", tool);
    if (stop)
        fprintf(f, "Running from reset in the system-mode interpreter stops at:\n\n- %s\n\n", stop);
    else
        fprintf(f, "The run finished without a fatal error.\n\n");
    if (n_notes) {
        fprintf(f, "Stubbed on purpose (documented, first occurrence each):\n\n");
        for (unsigned k = 0; k < n_notes; k++)
            fprintf(f, "- %s\n", notes[k]);
    }
    return fclose(f) ? -1 : 0;
}
