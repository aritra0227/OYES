#ifndef INCLUDE_SCHEDULER_H
#define INCLUDE_SCHEDULER_H
#include "process.h"

/**
 * Returns -1 on failure
*/
void scheduler_override_current_process(PROCESS_T *proc);


#endif