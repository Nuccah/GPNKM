#ifndef H_STRUCTURES
#define H_STRUCTURES
#include "constants.h"


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
	int tire; /* sets of tires */
	double avgSpeed;
	double fuelStock; /* fuel stock in liter */

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

typedef struct msgbufServ {
	long mtype;     /* message type, must be > 0 */
	int mInt;  /* message data */
} TmsgbufServ;

typedef struct TmsgbufAdr { // Afficheur de Resultat
	long mtype;     /* message type, must be > 0 */
	pid_t tabD[23];  /* message data */
} TmsgbufAdr;

typedef struct msgbufPilot {
	long mtype;     /* message type, must be > 0 */
	struct TCar car;  /* message data */
} TmsgbufPilot;
#endif
