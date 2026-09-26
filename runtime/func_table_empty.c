/* Empty function tables: links the runtime, interpreter, and generic tests
 * without any game's generated code. */
#include "func_table.h"

const ct_func ct_funcs[] = {{0, 0, 0, 0, 0, -1, -1, 0}};
const unsigned ct_func_count = 0;
const ct_extern ct_externs[] = {{0, 0, 0}};
const unsigned ct_extern_count = 0;
const ct_jumptable ct_jumptables[] = {{0, 0}};
const unsigned ct_jumptable_count = 0;
