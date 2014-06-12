#ifndef H_CONSTANTS
#define H_CONSTANTS

/* All libraries needed */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <signal.h>
#include "opcodes.h"

/* Change this to see more debug info: 
* 0 -> NO MSG INFO
* 1 -> ERROR INFO 
* 2 -> SUCCESS INFO
* 3 -> NOTICE INFO
* 4 -> DEBUG INFO (All messages)
* Default is 4
* Recommended max level for prod is 2.
*/
#define LOGLVL 4 

/* Change this to see different process at the screen:
* 0 -> Monitor
* 1 -> Server
* 2 -> Drivers
* Default is 1
* Recommended level for prod is 0
*/
#define DISPMODE 0

// Permission to Shared Memory
#define PERMS 0666

/* Sema Channel */
#define TMP1 0

/* GENERAL */
#define DRIVERS 22

/* weather constants */
#define DRY 1.0
#define WET 0.9
#define RAIN 0.75

/* Tire Constants (x4 for Sets) */
#define SLICKS 13
#define INTERMEDIATES 4
#define WETS 3
#define TIREWEARMAX 1.0 
#define TIREWEARMIN 0.4
#define TIREWEARLIMIT 10
#define DRYFACTOR 2.5
#define WETFACTOR 1.2

/* random loss percentage constants (by lap)*/
#define CRASH 0.5
#define BREAK 0.003
#define DAMAGEFACTOR 0.7

/* sector length (in meters) */
#define S1 2300
#define S2 1800
#define S3 2900

/* Qualifiers (in seconds) */
#define Q1 1080
#define Q2 600
#define Q3 720

/* Lap number for the GP */
#define LAPGP 150

/* speed limits (in kmph) */
#define MINSPEED 285.1
#define MAXSPEED 315.0

/* pit stop limits */
#define MINTIME 4
#define MAXTIME 6
#define MINCHANGE 5
#define MAXCHANGE 7
#define MINREPAIR 5
#define MAXREPAIR 10
#endif
