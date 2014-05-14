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
	double stime; /* Time spent in one sector */
	double speed; /* Speed during this sector */
} TSect;

/* lap type */
typedef struct TLap {
	int lnum;
	double ltime; /* Time for a lap */
	double averTime; /* Average time by secor for a lap */
	TSect tabSect[3];
} TLap;

/* car type */
typedef struct TCar {
	const char * teamName;
	bool retired; /* is retired (true) */
	bool damaged; /* is damaged (true) */
	bool crashed; /* is crashed (true) */
	int num;
	int start_position;
	int tires; /* sets of tires */
	double avgSpeed; /* Average speed in real time */
	double fuelStock; /* fuel stock in liter */
	TLap* lapTimes; /* it's a pointer because laps number depends of the race type (I suppose ^^) */
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
	pid_t tabD[23];  /* PID data */
	int weather; /* weather data */
} TmsgbufAdr;

typedef struct msgbufPilot {
	long mtype;     /* message type, must be > 0 */
	struct TCar car;  /* message data */
} TmsgbufPilot;

typedef struct TSharedStock{
	int type;
	struct TCar tabCar[22];
} TSharedStock;

// To use when you are going to work in shared memo
void semDown(int sem_id, int sem_channel);

// To use when you finish your work in shared memo
void semUp(int sem_id, int sem_channel);

void show_notice(const char *env, const char *msg);
void show_error(const char *env, const char *msg);
void show_debug(const char *env, const char *msg);
void show_success(const char *env, const char *msg);

#endif
