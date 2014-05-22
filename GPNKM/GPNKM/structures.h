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
	double ltime; /* Time for a lap */
	double avgSpeed; /* Average Speed for a lap */
	TSect tabSect[3];
} TLap;

/* car type */
typedef struct TCar {
	const char * teamName;
	bool retired; /* is retired (true) */
	bool crashed; /* is crashed (true) */
	bool ready;
	int num;
	int start_position; 
	int lnum;
	int pitstop;
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

typedef struct TResults{
	int num;
	const char * teamName;
	int lnum;
	double timeLastLap;
	double timeGlobal;
	bool retired; /* is retired (true) */
	bool pitstop; 
} TResults;

typedef struct TBest{
	int num;
	const char * teamName;
	double time;
} TBest;

typedef struct TSharedStock{
	int type;
	struct TCar tabCar[22];
} TSharedStock;

// To use when you are going to work in shared memo
void semDown(int sem_id, int sem_channel);

// To use when you finish your work in shared memo
void semUp(int sem_id, int sem_channel);

// Check if shared mem is readable
bool isShMemReadable(int sem_id, int sem_channel);

// Reset the sema to 1
void semReset(int sem_id, int sem_channel);

// sig = value to put in sema
// Put a sig in the corresponding channel of the corresponding sema
void sendSig(int sig, int sem_id, int sem_channel);
// force to wait the sig parameter in the corresponding sema
void waitSig(int sig, int sem_id, int sem_channel);
// check once the sig value in the sema and compare to the parameter 
// return true if they correspond else return false
bool checkSig(int sig, int sem_id, int sem_channel);

void show_notice(const char *env, const char *msg);
void show_error(const char *env, const char *msg);
void show_debug(const char *env, const char *msg);
void show_success(const char *env, const char *msg);

#endif
