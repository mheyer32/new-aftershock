
#ifndef PROTOCOL_H__
#define PROTOCOL_H__

#define PROTOCOL_VERSION	48

// the svc_strings[] array in cl_parse.c should mirror this
//
// server to client
//
enum svc_ops_e {
	svc_bad,
	svc_nop,
	svc_gamestate,
	svc_configstring,		// [short] [string] only in gamestate messages
	svc_baseline,			// only in gamestate messages
	svc_serverCommand,		// [string] to be executed by client game module
	svc_download,			// [short] size [size bytes]
	svc_snapshot
};


//
// client to server
//
enum clc_ops_e {
	clc_bad,
	clc_nop, 		
	clc_move,				// [usercmd_t]
	clc_moveNoDelta,		// [usercmd_t]
	clc_clientCommand		// [string] message
};

#endif 