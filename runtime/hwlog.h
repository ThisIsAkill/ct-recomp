/* Hardware the runtime deliberately does not model, noted once per distinct
 * message, and the needed_hw.md report built from those notes plus the
 * fatal error (if any) that stopped a run. */
#ifndef CT_HWLOG_H
#define CT_HWLOG_H

#include "cpu.h"

/* Record a note; repeats of the same text are dropped. */
void hw_note(const char *fmt, ...) CT_PRINTF(1, 2);
unsigned hw_note_count(void);
const char *hw_note_text(unsigned i);

/* Write needed_hw.md: `stop` is the fatal error line (NULL if the run
   finished), followed by the notes. Returns 0 on success. */
int hw_needed_write(const char *path, const char *tool, const char *stop);

#endif
