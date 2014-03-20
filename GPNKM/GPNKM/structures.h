#ifndef H_STRUCTURES
#define H_STRUCTURES

/* All libraries needed */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>

/* weather constants */
#define DRY 1
#define WET 0.9
#define RAIN 0.75

/* random loss percentage constants (by lap)*/
#define CRASH 0.002
#define BREAK 0.04

/* sector length (in meters) */
#define S1 2300
#define S2 1800
#define S3 2900

/* ???? (in seconds) */
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
#define FUELMIN 0.5

/* F1 car variables */
//TODO Derek I let you this task ^^

/* sector type */
typedef struct TSect {
	double stime;
} TSect;

/* lap type */
typedef struct TLap {
	TSect tabSect[3];
	double ltime;
	double averTime;
} TLap;

/* car type */
typedef struct TCar {
	int num;
	int start_position;
	int position;
	double speed;
	bool retired; /* is retired (true) */
	bool crashed; /* is crashed (true) */
	bool pitstop; /* is into the pitstop (true) */
	double fuelStock; /* fuel stock in liter */
	double fuelCon; /* fuel consumption (in liter) by minute */
	int wheel; /* wheel type */
	TLap* lapTimes; /* it's a pointer beacause laps number depends of the race type (I suppose ^^) */
} TCar;
 
//TODO complete all the race types!
/* Trial type */
typedef struct TTrial {
 	TCar carList[24];
 	int weather;
} TTrial;

 /* Qualif type */
typedef struct TQualif {
	TCar carList[24];
	TCar* carLost; /* The last cars that are retrieved from the qualif */
	int weather;
} TQualif;

/* Grand Prix type */
typedef struct TGP {
	TCar carList[24];
	int weather;
} TGP;

/* Podium type */
typedef struct TPod {
	TCar first;
	TCar second;
	TCar third;
} TPod;

#endif
