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
#include <stdbool.h>

/* weather constants */
#define DRY 1
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

/* F1 car variables 
	speed = ;
	fuel = ;
	twear = ;
*/

/* sector type */
typedef struct TSect {
	bool retired; /* is retired (true) */
	bool crashed; /* is crashed (true) */
	bool pitstop; /* is into the pitstop (true) */
	double stime;
	double speed;
} TSect;

/* lap type */
typedef struct TLap {
	double ltime;
	double averTime;
	TSect tabSect[3];
} TLap;

/* car type */
typedef struct TCar {
	bool retired; /* is retired (true) */
	bool crashed; /* is crashed (true) */
	bool pitstop; /* is into the pitstop (true) */
	int num;
	int start_position;
	int position;
	int tire; /* tire type */
	double avgSpeed;
	double fuelStock; /* fuel stock in liter */
	double fuelCon; /* fuel consumption (in liter) by minute */

	TLap* lapTimes; /* it's a pointer beacause laps number depends of the race type (I suppose ^^) */
} TCar;
 
//TODO complete all the race types!
/* Trial type */
typedef struct TTrial {
 	int weather;
 	TCar carList[24];
} TTrial;

 /* Qualif type */
typedef struct TQualif {
	int weather;
	TCar* carLost; /* The last cars that are retrieved from the qualif */
	TCar carList[24];

} TQualif;

/* Grand Prix type */
typedef struct TGP {
	int weather;
	TCar carList[24];
} TGP;

/* Podium type */
typedef struct TPod {
	TCar first;
	TCar second;
	TCar third;
} TPod;

/* Team Type */
typedef struct TTeam {
	const char *team;
	TCar* tabCar[2];
	bool pitStop;
} TTeam;

#endif
