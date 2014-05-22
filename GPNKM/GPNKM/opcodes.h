#ifndef H_OPCODES
#define H_OPCODES
/* Signals for control */

// End of program
#define SIGEND 0
// No action sent  
#define SIGFREE 1

// Time out for the race
#define SIGTIMEOUT -1
// Start the race
#define SIGSTART -2

// Trial sig
#define SIGTR1 -10
#define SIGTR2 -11
#define SIGTR3 -12

// Qualif sig
#define SIGQU1 -20
#define SIGQU2 -21
#define SIGQU3 -22

// GP sig
#define SIGGP -30

#endif