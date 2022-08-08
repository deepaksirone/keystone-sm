#ifndef _H_SM_TIME_H_
#define _H_SM_TIME_H_

#include <sbi/sbi_types.h>
unsigned long set_unix_time(uintptr_t unix_time);
unsigned long get_unix_time();
int init_timebase_freq();
unsigned long get_timebase_freq();

#endif
