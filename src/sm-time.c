#include "sm-time.h"
#include <sbi/riscv_locks.h>
#include <limits.h>

//TODO: Get timebase freq from fdt blob
#define TIMEBASE_FREQ 10000000

static spinlock_t time_lock = SPIN_LOCK_INITIALIZER;
static unsigned long int saved_unix_timestamp = ULONG_MAX;
static unsigned long int saved_mtime_value = ULONG_MAX;

typedef unsigned long cycles_t;

static inline cycles_t get_cycles(void)
{
        cycles_t n;

        __asm__ __volatile__ (
                "rdtime %0"
                : "=r" (n));
        return n;
}

unsigned long set_unix_time(uintptr_t unix_time)
{
   spin_lock(&time_lock);
   //TODO: verify that the set call is made from a valid ntp enclave
   saved_mtime_value = get_cycles();
   saved_unix_timestamp = unix_time;
   
   spin_unlock(&time_lock);
   return 0;
}

unsigned long get_unix_time()
{
   if (saved_unix_timestamp == ULONG_MAX) {
      return ULONG_MAX;
   }

   unsigned long current_mtime = get_cycles();
   unsigned long ret = saved_unix_timetamp + (current_mtime - saved_mtime_value) / TIMEBASE_FREQ;
   return ret;
}

