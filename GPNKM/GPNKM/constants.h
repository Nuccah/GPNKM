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
#include <stdbool.h>


/* weather constants */
#define DRY 1.0
#define WET 0.9
#define RAIN 0.75

/* Tires Sets (x4) */
#define SLICKS 13
#define INTERMEDIATES 4
#define WETS 3

/* random loss percentage constants (by lap)*/
#define CRASH 0.002
#define BREAK 0.04

/* sector length (in meters) */
#define S1 2300
#define S2 1800
#define S3 2900

/* Qualifiers (in seconds) */
#define Q1 1080
#define Q2 600
#define Q3 720

/* speed limits (in kmph) */
#define MINSPEED 285.1
#define MAXSPEED 315.0

/* fuel data (in liter) */
#define FUELSMAX 100.0
#define FUELSMIN 90.0
#define FUELCMAX 2.0
#define FUELCMIN 0.5

#endif
