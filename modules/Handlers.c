#include "Handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

extern volatile sig_atomic_t sig_int_raised;

// void catch_int(int signo) {
//     // sig_int_raised = signo;
//     fprintf(stderr, "\nCatching : signo = %d\n", signo);
// }