
#ifndef PROTOCOL_H__
#define PROTOCOL_H__


// Server Commands

#define SVC_BAD				0		// ungültiger Server Befehl
#define SVC_NOP				1		// kann einfach überlesen werden
#define SVC_GAMESTATE		2		// startdaten bei spielbeginn: configstrings, entities der baseline
#define SVC_CONFIGSTRING    3		// alter Q2 befehl, jetzt ungültig
#define SVC_BASELINE		4		// alter Q2 befehl, jetzt ungültig
#define SVC_SERVERCOMMAND	5		// server befehler wie print, centerprint, disconnect ...
#define SVC_DOWNLOAD		6		// alter Q2 befehl, jetzt ungültig
#define SVC_SNAPSHOT		7		// beschreibung des spielzustands zu einer bestimmten Zeit



































#endif 