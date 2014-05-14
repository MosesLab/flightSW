#include "system.h"
#include <termios.h>
#include "lockingQueue.h"
#include <signal.h>

#define UPBAUD B1200
#define HKUP "/dev/hkup"
#define _POSIX_SOURCE 1 /*POSIX compliant source*/


extern volatile sig_atomic_t ts_alive;

pid_t parent_pid;
extern LockingQueue hkdownQueue;

int fdown;






/*Functions*/


