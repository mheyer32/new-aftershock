

#include "a_shared.h"
#include "network.h"
#include "net_msg.h"
#include "console.h"
#include "cgame.h"
#include "client.h"


// Currently everything Network Protocol related 
// is from ARGUS Copyright (C) 1999-2000 Martin Otten
// visit argus.splatterworld.de 





#define MAX_SNAPSHOTS_BUFFERED			32		// anzahl der gebufferten snapshots für delta (Ringbuffer)
#define	MAX_BASLINE_ENTITIES			1024	// entities der Baseline, also unveränderline Entities die aus der Map kommen und nur einmal beim Gamestate gestzt werden
#define MAX_RINGBUF_ENTITIES			2048	// entities des Ringbuffers in dem alle Snapshots ihr entities ablegen, teilweise aus der Basline kopiert, teilweise aus delta snapshots (ringbuffer)
#define	MAX_CLIENT_COMMANDS				32		// anzahl der client commands, die im buffer liegen können

static byte KnownBitMasks[32][7]=		// häufig gebraucht Bitmasken
	{
		{0x60,0x80,0x00,0x00,0x00,0x00,0x00},
		{0x60,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x60,0xC0,0x00,0x00,0x00,0x00,0x00},
		{0xE1,0x00,0x00,0x00,0x00,0x20,0x00},
		{0x60,0x80,0x00,0x00,0x00,0x10,0x00},
		{0xE0,0x80,0x00,0x00,0x00,0x00,0x00},
		{0xE0,0xC0,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x10,0x00},
		{0x40,0x80,0x00,0x00,0x00,0x00,0x00},
		{0x20,0x80,0x00,0x00,0x00,0x00,0x00},
		{0x60,0x80,0x00,0x00,0x01,0x00,0x00},
		{0xED,0x07,0x00,0x00,0x00,0x80,0x00},
		{0xE0,0x00,0x00,0x00,0x00,0x00,0x00},
		{0xED,0x07,0x00,0x00,0x00,0x30,0x00},
		{0x80,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x40,0x00,0x00,0x00,0x00,0x00,0x00},
		{0xE0,0xC0,0x00,0x00,0x00,0x10,0x00},
		{0x60,0x00,0x00,0x00,0x00,0x10,0x00},
		{0x20,0x00,0x00,0x00,0x00,0x00,0x00},
		{0xE1,0x00,0x00,0x00,0x04,0x20,0x00},
		{0xE1,0x00,0xC0,0x01,0x20,0x20,0x00},
		{0xE0,0xC0,0x00,0x00,0x01,0x00,0x00},
		{0x60,0x40,0x00,0x00,0x00,0x00,0x00},
		{0x40,0xC0,0x00,0x00,0x00,0x00,0x00},
		{0x60,0xC0,0x00,0x00,0x01,0x00,0x00},
		{0x60,0xC0,0x00,0x00,0x00,0x10,0x00},
		{0x60,0x80,0x00,0x00,0x01,0x00,0x01},
		{0x60,0x80,0x00,0x00,0x00,0x30,0x00},
		{0xE0,0x80,0x00,0x00,0x00,0x10,0x00},
		{0x20,0xC0,0x00,0x00,0x00,0x00,0x00},
		{0x60,0x80,0x00,0x00,0x00,0x00,0x02},
		{0xE0,0x40,0x00,0x00,0x00,0x00,0x00}
	}; 


entityState_t cl_ringbuff [MAX_RINGBUF_ENTITIES];
snapshot_t cl_snapshotbuffer [MAX_SNAPSHOTS_BUFFERED];




void CL_ParseEntity(sizebuf_t *p,int EntityIndex, entityState_t  * e,entityState_t  * olde)
{
	byte EntityBitMask[7]; // 56 Bits, 50 werden nur gebraucht
	unsigned int MaskIndex;
	char Text [MAX_STRING_CHARS];
	char dummy [MAX_STRING_CHARS];


	// entityState_t * e = &cent->currentState;	// hole aktuellen state des entities

	aboolean debug =0; // g_Options.DebugActive && g_Options.DebugShowEntities;	// so gehts schneller

	if ((EntityIndex<0) || (EntityIndex>=MAX_GENTITIES))
	{
		Con_Printf("Bad Delta entity number !");
	}
		
	if (MSG_ReadBits(p,1))
	{
		//if (debug) g_DebugText+=" Remove entity.\x0D\x0A";
		memset(e,0,sizeof(entityState_t));	// lösche das entity	
		// extra müssen noch die cg_entity einträge gelöscht werden

		// TODO !!!
		// hier valid = 0, wenn der letzt snapshot vom gleichen client war
		/*if (g_cgs.lastSnap->ps.clientNum==g_cgs.snap->ps.clientNum)
			g_cg_entities[EntityIndex].currentValid = 0;
		else
			g_cg_entities[EntityIndex].currentValid = g_cgs.clientnum;
		*/
		e->number=ENTITYNUM_NONE;
	}
	else	
	{	
		if (MSG_ReadBits (p,1))
		{
			memcpy(e,olde,sizeof(entityState_t));	// von mir
						
			MaskIndex = MSG_ReadBits (p,5);	// lies BitMasken Index
			if (MaskIndex==0x1F)		// keine bekannte bitmasken combination ?
			{
				//lies unbekannte 50 bit maske 
				for (MaskIndex=0;MaskIndex<6;MaskIndex++)
					EntityBitMask[MaskIndex]=MSG_ReadByte (p);
				
				EntityBitMask[6]= MSG_ReadBits (p,2) & 0x03 ;  // letzte 2 bit auch noch lesen
				//if (debug) g_DebugText+=" <uc> ";	// unknown combination
			}
			else
			{
				// kopiere fertige bitmaske aus array
				memcpy(&EntityBitMask[0],&KnownBitMasks[MaskIndex][0],7+sizeof(byte));

				//if (debug) { dummy.Format(" <%2i>",MaskIndex);Text+=dummy;}// Bitmask Index Number
			}

			if (EntityBitMask[0] & 1)	e->eType				=MSG_ReadByte (p);
			if (EntityBitMask[0] & 2)	e->eFlags				=MSG_ReadBits (p,16);
			if (EntityBitMask[0] & 4)	e->pos.trType			=MSG_ReadByte (p);
			if (EntityBitMask[0] & 8)	e->pos.trTime			=MSG_ReadLong (p);
			if (EntityBitMask[0] & 16)	e->pos.trDuration		=MSG_ReadLong (p); 
			if (EntityBitMask[0] & 32)	e->pos.trBase[0]		=MSG_ReadCoord (p);
			if (EntityBitMask[0] & 64)	e->pos.trBase[1]		=MSG_ReadCoord(p);
			if (EntityBitMask[0] & 128)	e->pos.trBase[2]		=MSG_ReadCoord(p);
			if (EntityBitMask[1] & 1)	e->pos.trDelta[0]		=MSG_ReadCoord (p);
			if (EntityBitMask[1] & 2)	e->pos.trDelta[1]		=MSG_ReadCoord(p);
			if (EntityBitMask[1] & 4)	e->pos.trDelta[2]		=MSG_ReadCoord (p);
			if (EntityBitMask[1] & 8)	e->apos.trType			=MSG_ReadByte (p);
			if (EntityBitMask[1] & 16)	e->apos.trTime			=MSG_ReadLong (p);
			if (EntityBitMask[1] & 32)	e->apos.trDuration		=MSG_ReadLong (p);
			if (EntityBitMask[1] & 64)	e->apos.trBase[0]		=MSG_ReadCoord (p);
			if (EntityBitMask[1] & 128)	e->apos.trBase[1]		=MSG_ReadCoord (p);
			if (EntityBitMask[2] & 1)	e->apos.trBase[2]		=MSG_ReadCoord (p);
			if (EntityBitMask[2] & 2)	e->apos.trDelta[0]		=MSG_ReadCoord (p);
			if (EntityBitMask[2] & 4)	e->apos.trDelta[1]		=MSG_ReadCoord (p);
			if (EntityBitMask[2] & 8)	e->apos.trDelta[2]		=MSG_ReadCoord (p);
			if (EntityBitMask[2] & 16)	e->time					=MSG_ReadLong (p);
			if (EntityBitMask[2] & 32)	e->time2				=MSG_ReadLong (p);
			if (EntityBitMask[2] & 64)	e->origin[0]			=MSG_ReadCoord (p);
			if (EntityBitMask[2] & 128)	e->origin[1]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 1)	e->origin[2]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 2)	e->origin2[0]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 4)	e->origin2[1]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 8)	e->origin2[2]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 16)	e->angles[0]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 32)	e->angles[1]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 64)	e->angles[2]			=MSG_ReadCoord (p);
			if (EntityBitMask[3] & 128)	e->angles2[0]			=MSG_ReadCoord (p);
			if (EntityBitMask[4] & 1)	e->angles2[1]			=MSG_ReadCoord (p);
			if (EntityBitMask[4] & 2)	e->angles2[2]			=MSG_ReadCoord (p);
			if (EntityBitMask[4] & 4)	e->otherEntityNum		=MSG_ReadBits (p,10);
			if (EntityBitMask[4] & 8)	e->otherEntityNum2		=MSG_ReadBits (p,10);
			if (EntityBitMask[4] & 16)	e->groundEntityNum		=MSG_ReadBits (p,10);
			if (EntityBitMask[4] & 32)	e->loopSound			=MSG_ReadByte (p);
			if (EntityBitMask[4] & 64)	e->constantLight		=MSG_ReadLong (p);
			if (EntityBitMask[4] & 128)	e->modelindex			=MSG_ReadByte (p);
			if (EntityBitMask[5] & 1)	e->modelindex2			=MSG_ReadByte (p);
			if (EntityBitMask[5] & 2)	e->frame				=MSG_ReadShort (p);
			if (EntityBitMask[5] & 4)	e->clientNum			=MSG_ReadByte (p);
			if (EntityBitMask[5] & 8)	e->solid				=MSG_ReadBits (p,24);
			if (EntityBitMask[5] & 16)	e->event				=MSG_ReadBits (p,10);
			if (EntityBitMask[5] & 32)	e->eventParm			=MSG_ReadByte (p);
			if (EntityBitMask[5] & 64)	e->powerups				=MSG_ReadShort (p);
			if (EntityBitMask[5] & 128)	e->weapon				=MSG_ReadByte (p);
			if (EntityBitMask[6] & 1)	e->legsAnim				=MSG_ReadByte (p);
			if (EntityBitMask[6] & 2)	e->torsoAnim			=MSG_ReadByte (p);
			
			e->number=EntityIndex;

			/*
			if (debug) 
			{
				if (EntityBitMask[0] & 1)	{dummy.Format("eType:%u ",e->eType);Text+=dummy;};
				if (EntityBitMask[0] & 2)	{dummy.Format("eFlags:%u ",e->eFlags);Text+=dummy;};
				if (EntityBitMask[0] & 4)	{dummy.Format("trType:%u ",e->pos.trType);Text+=dummy;};
				if (EntityBitMask[0] & 8)	{dummy.Format("trTime:%u ",e->pos.trTime);Text+=dummy;};
				if (EntityBitMask[0] & 16)	{dummy.Format("trDuration:%u ",e->pos.trDuration);Text+=dummy;};
				if (EntityBitMask[0] & 32)	{dummy.Format("trBase[0]:%f ",e->pos.trBase[0]);Text+=dummy;};
				if (EntityBitMask[0] & 64)	{dummy.Format("trBase[1]:%f ",e->pos.trBase[1]);Text+=dummy;};
				if (EntityBitMask[0] & 128)	{dummy.Format("trBase[2]:%f ",e->pos.trBase[2]);Text+=dummy;};
				if (EntityBitMask[1] & 1)	{dummy.Format("trDelta[0]:%f ",e->pos.trDelta[0]);Text+=dummy;};
				if (EntityBitMask[1] & 2)	{dummy.Format("trDelta[1]:%f ",e->pos.trDelta[1]);Text+=dummy;};
				if (EntityBitMask[1] & 4)	{dummy.Format("trDelta[2]:%f ",e->pos.trDelta[2]);Text+=dummy;};
				if (EntityBitMask[1] & 8)	{dummy.Format("apos.trType:%u ",e->apos.trType);Text+=dummy;};
				if (EntityBitMask[1] & 16)	{dummy.Format("apos.trTime:%u ",e->apos.trTime);Text+=dummy;};
				if (EntityBitMask[1] & 32)	{dummy.Format("apos.trDuration:%u ",e->apos.trDuration);Text+=dummy;};
				if (EntityBitMask[1] & 64)	{dummy.Format("apos.trBase[0]:%f ",e->apos.trBase[0]);Text+=dummy;};
				if (EntityBitMask[1] & 128)	{dummy.Format("apos.trBase[1]:%f ",e->apos.trBase[1]);Text+=dummy;};
				if (EntityBitMask[2] & 1)	{dummy.Format("apos.trBase[2]:%f ",e->apos.trBase[2]);Text+=dummy;};
				if (EntityBitMask[2] & 2)	{dummy.Format("apos.trDelta[0]:%f ",e->apos.trDelta[0]);Text+=dummy;};
				if (EntityBitMask[2] & 4)	{dummy.Format("apos.trDelta[1]:%f ",e->apos.trDelta[1]);Text+=dummy;};
				if (EntityBitMask[2] & 8)	{dummy.Format("apos.trDelta[2]:%f ",e->apos.trDelta[2]);Text+=dummy;};
				if (EntityBitMask[2] & 16)	{dummy.Format("time:%u ",e->time);Text+=dummy;};
				if (EntityBitMask[2] & 32)	{dummy.Format("time2:%u ",e->time2);Text+=dummy;};
				if (EntityBitMask[2] & 64)	{dummy.Format("origin[0]:%f ",e->origin[0]);Text+=dummy;};
				if (EntityBitMask[2] & 128)	{dummy.Format("origin[1]:%f ",e->origin[1]);Text+=dummy;};
				if (EntityBitMask[3] & 1)	{dummy.Format("origin[2]:%f ",e->origin[2]);Text+=dummy;};
				if (EntityBitMask[3] & 2)	{dummy.Format("origin2[0]:%f ",e->origin2[0]);Text+=dummy;};
				if (EntityBitMask[3] & 4)	{dummy.Format("origin2[1]:%f ",e->origin2[1]);Text+=dummy;};
				if (EntityBitMask[3] & 8)	{dummy.Format("origin2[2]:%f ",e->origin2[2]);Text+=dummy;};
				if (EntityBitMask[3] & 16)	{dummy.Format("angles[0]:%f ",e->angles[0]);Text+=dummy;};
				if (EntityBitMask[3] & 32)	{dummy.Format("angles[1]:%f ",e->angles[1]);Text+=dummy;};
				if (EntityBitMask[3] & 64)	{dummy.Format("angles[2]:%f ",e->angles[2]);Text+=dummy;};
				if (EntityBitMask[3] & 128)	{dummy.Format("angles2[0]:%f ",e->angles2[0]);Text+=dummy;};
				if (EntityBitMask[4] & 1)	{dummy.Format("angles2[1]:%f ",e->angles2[1]);Text+=dummy;};
				if (EntityBitMask[4] & 2)	{dummy.Format("angles2[2]:%f ",e->angles2[2]);Text+=dummy;};
				if (EntityBitMask[4] & 4)	{dummy.Format("otherEntityNum:%u ",e->otherEntityNum);Text+=dummy;};
				if (EntityBitMask[4] & 8)	{dummy.Format("otherEntityNum2:%u ",e->otherEntityNum2);Text+=dummy;};
				if (EntityBitMask[4] & 16)	{dummy.Format("groundEntityNum:%u ",e->groundEntityNum);Text+=dummy;};
				if (EntityBitMask[4] & 32)	{dummy.Format("loopSound:%u ",e->loopSound);Text+=dummy;};
				if (EntityBitMask[4] & 64)	{dummy.Format("constantLight:%u ",e->constantLight);Text+=dummy;};
				if (EntityBitMask[4] & 128)	{dummy.Format("modelindex:%u ",e->modelindex);Text+=dummy;};
				if (EntityBitMask[5] & 1)	{dummy.Format("modelindex2:%u ",e->modelindex2);Text+=dummy;};
				if (EntityBitMask[5] & 2)	{dummy.Format("frame:%u ",e->frame);Text+=dummy;};
				if (EntityBitMask[5] & 4)	{dummy.Format("clientNum:%u ",e->clientNum);Text+=dummy;};
				if (EntityBitMask[5] & 8)	{dummy.Format("solid:%u ",e->solid);Text+=dummy;};
				if (EntityBitMask[5] & 16)	{dummy.Format("event:%u ",e->event);Text+=dummy;};
				if (EntityBitMask[5] & 32)	{dummy.Format("eventParm:%u ",e->eventParm);Text+=dummy;};
				if (EntityBitMask[5] & 64)	{dummy.Format("powerups:%u ",e->powerups);Text+=dummy;};
				if (EntityBitMask[5] & 128)	{dummy.Format("weapon:%u ",e->weapon);Text+=dummy;};
				if (EntityBitMask[6] & 1)	{dummy.Format("legsAnim:%u ",e->legsAnim);Text+=dummy;};
				if (EntityBitMask[6] & 2)	{dummy.Format("torsoAnim:%u ",e->torsoAnim);Text+=dummy;};
				g_DebugText+=Text+"\x0D\x0A";
			}	// debug	*/

			
		}
		else
		{
			//if (debug) g_DebugText+=" Copy old Entity.\x0D\x0A";
			memcpy(e,olde,sizeof(entityState_t));
			e->number=EntityIndex;
		}
	}

}



void CL_ParseSnapshot(sizebuf_t * p)
{
	char dummy[255];
	byte delta,b,b2;
		
	WORD mask = 0;
	int i,min,sek, deltaSeqNr;

	snapshot_t * s;		// der aktuelle snapshot
	snapshot_t * ds;	// der passende Delta snapshot
	
	
	aboolean debug=0 ;//g_Options.DebugActive; // && g_Options.DebugHSowSnapshots

	// merke aktuellen als letzten
	//g_cgs.lastSnap = g_cgs.snap;

	//  berechne Position des neuen snaphot
	s = &cl_snapshotbuffer[p->Sequence_Number & 0x1F];	// maskieren da 32 Snapshots im buffer
	//g_cgs.snap=s;	// jetzt dem spiel den neuen snapshot auch mitteilen

	// TODO ?
	s->serverCommandSequence=p->Sequence_Number;	// dem snap auch sagen zu wem er genau gehört
		

	//if (debug) g_DebugText+="SNAPSHOT\x0D\x0A";

	s->numServerCommands=MSG_ReadLong(p);				// zuletzt empfangen client command nr
	//g_cgs.clientCommandSequence=s->numServerCommands;	// global gültig

	s->serverTime=MSG_ReadLong (p);		// lies serverzeit

	//g_cgs.lastSnapshotTime=g_cgs.ServerTime;	// merke letzte zeit
	//g_cgs.ServerTime=s->serverTime;		// global gültig

	delta=MSG_ReadByte (p);	// Detlaframe=Sequenznumber - DeltaComprssion oder -1 falls Deltacompression = 0
	
	/*if (debug)
	{
		min = s->serverTime/ 60000;
		sek = (s->serverTime / 1000) -min*60;
		sprintf(dummy,"ClientCmdNr %i, Time %.4u:%.2u Delta %u\x0D\x0A",s->numServerCommands,min,sek,delta);
		g_DebugText+=dummy;
	}
	*/

	// berechne delta snapshot
	if (delta)
	{
		deltaSeqNr=(p->Sequence_Number-delta);		// erstmal die richtige delta snapshot nummer ausrechen
		ds = &cl_snapshotbuffer[deltaSeqNr & 0x1F];	// maskiert aus dem buffer holen
		if (ds->serverCommandSequence!=deltaSeqNr) Con_Printf("Delta frame too old.\x0D\x0A");
		//if (g_ringbuff_pos - ds->ringbuff_pos > 1920 ) Con_Printf("Delta parseEntitiesNum too old.\x0D\x0A");
		
		memcpy(&s->ps,&ds->ps,sizeof(playerState_t));	// von mir alte player infos übernehmen
	}
	else
	{
		memset(&s->ps,0,sizeof(playerState_t));	// von mir
		ds=NULL;	// kein delta snapshot;
	}

	b=MSG_ReadByte (p);	// hat irgendwas mit delta zu tun sollte nicht 0 sein
	
	b=MSG_ReadByte (p );	
	// hier wird der Packetbitzähler in source code auf die nächste volle Byzepos gesetz ist aber nie nötig FIXME TODO
	for (i=0;i<b;i++)
	{
		
		// Perhaps Areamask :

		b2=MSG_ReadByte (p);	// wird hinetereiander abgespeichert bedeutung unklar
		/*if (debug)
		{
			sprintf(dummy,"Byte %u\x0D\x0A",b2);
			g_DebugText+=dummy;
		}*/
	}

	// lies Playerstate

	if(MSG_ReadBits (p,1)) s->ps.commandTime=		MSG_ReadLong (p);
	if(MSG_ReadBits (p,1)) s->ps.pm_type=			MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.bobCycle=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.pm_flags=		MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.pm_time=			MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.origin[0]=		MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.origin[1]=		MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.origin[2]=		MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.velocity[0]=		MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.velocity[1]=		MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.velocity[2]=		MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.weaponTime=		MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.gravity=			MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.speed=			MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.delta_angles[0]=	MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.delta_angles[1]=	MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.delta_angles[2]=	MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.groundEntityNum=	MSG_ReadBits(p,10);
	if(MSG_ReadBits (p,1)) s->ps.legsTimer=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.torsoTimer=		MSG_ReadBits(p,12);
	if(MSG_ReadBits (p,1)) s->ps.legsAnim=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.torsoAnim=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.movementDir=		MSG_ReadBits(p,4);
	if(MSG_ReadBits (p,1)) s->ps.eFlags=			MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.eventSequence=	MSG_ReadShort (p);
	if(MSG_ReadBits (p,1)) s->ps.events[0]=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.events[1]=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.eventParms[0]=	MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.eventParms[1]=	MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.externalEvent=	MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.externalEventParm=	MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.clientNum=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.weapon=			MSG_ReadBits(p,5);
	if(MSG_ReadBits (p,1)) s->ps.weaponstate=		MSG_ReadBits(p,4);
	if(MSG_ReadBits (p,1)) s->ps.viewangles[0]=	MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.viewangles[1]=	MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.viewangles[2]=	MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.viewheight=		MSG_ReadByte (p);
	if(MSG_ReadBits (p,1)) s->ps.damageEvent=		MSG_ReadByte (p);		
	if(MSG_ReadBits (p,1)) s->ps.damageYaw=		MSG_ReadByte (p);			// richtung aus der letzter scahden entstand
	if(MSG_ReadBits (p,1)) s->ps.damagePitch=		MSG_ReadByte (p);		// richtung aus der letzter scahden entstand
	if(MSG_ReadBits (p,1)) s->ps.damageCount=		MSG_ReadByte (p);		// wie stark der letzte Schaden war
	if(MSG_ReadBits (p,1)) s->ps.grapplePoint[0]=	MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.grapplePoint[1]=	MSG_ReadCoord (p);
	if(MSG_ReadBits (p,1)) s->ps.grapplePoint[2]=	MSG_ReadCoord (p);

	/*sprintf(dummy,"ps.clientNum %i\x0D\x0A",s->ps.clientNum);
	g_DebugText+=dummy;
	*/


	if (MSG_ReadBits (p,1))		// PS_STATS
	{
		mask = MSG_ReadShort (p);

		for (i=0;i<MAX_STATS;i++)
			if ((mask&(1<<i))!=0)	// bit gesetzt ?
			{
				s->ps.stats[i] = MSG_ReadLong (p);
				
				/*if (debug && g_Options.DebugShowPlayerstatus)
				{
					sprintf(dummy,"PS_Stats%u:%i\x0D\x0A",i,s->ps.stats[i]);
					g_DebugText+=dummy;
				}
				*/
			}
	}

	if (MSG_ReadBits (p,1))	// PS_Persistant
	{
		mask = MSG_ReadShort (p);

		for (i=0;i<MAX_PERSISTANT;i++)
			if ((mask&(1<<i))!=0)	// bit gesetzt ?
			{
				s->ps.persistant[i] = MSG_ReadShort (p);

				/*if (debug && g_Options.DebugShowPlayerstatus)
				{
					sprintf(dummy,"PS_Persistant%u:%u\x0D\x0A",i,s->ps.persistant[i]);
					g_DebugText+=dummy;
				}
				*/
			}
	}

	if (MSG_ReadBits (p,1))	// PS_Ammo
	{
	
		mask = MSG_ReadShort (p);
		for (i=0;i<MAX_WEAPONS;i++)
			if ((mask&(1<<i))!=0)	// bit gesetzt ?
			{
				s->ps.ammo[i] = MSG_ReadShort (p);
				/*if (debug && g_Options.DebugShowPlayerstatus)
				{
					sprintf(dummy,"PS_Ammo%u:%u\x0D\x0A",i,s->ps.ammo[i]);
					g_DebugText+=dummy;
				}
				*/
			}
	}

	if (MSG_ReadBits (p,1))	// PS_Powerups
	{
		mask = MSG_ReadShort (p);
		for (i=0;i<MAX_POWERUPS;i++)
			if ((mask&(1<<i))!=0)	// bit gesetzt ?
			{
				s->ps.powerups[i] = MSG_ReadLong (p);// ACHTUNG bei powerups 32 Bit lesen nicht 16
				/*if (debug && g_Options.DebugShowPlayerstatus)
				{
					sprintf(dummy,"PS_Powerups%u:%u\x0D\x0a",i,s->ps.powerups[i]);
					g_DebugText+=dummy;
				}
				*/
			}
	}

	//CL_ParseDeltaEntities(p,s,ds);	// so eine kleine Zeile kann so viel Ärger machen

	 // so snapshot komplett geparsed, jetzt aus werten

	 //GameUpdateSnapshot(s);

}





#if 0

// parsed  die alle entities im snapshot s und benutzt ds als deltasnapshot, falls ds!=NULL
void CL_ParseDeltaEntities(sizebuf_t * p, snapshot_t * s, snapshot_t * ds)
{
	int		zaehler=0;			// zähler der mit den delta entities hochzählt
	int		index=0;			// der neu gelesene index 10 bit
	int		ds_entity_num;		// entity nummer des aktuellen delta entity
	entityState_t * ds_entity;	// das aktuelle delta entity
	char	dummy[255];			// temp string für debug texte
	aboolean	debug = 0 ;//g_Options.DebugActive && g_Options.DebugShowEntities;	// so gehts schneller

	//s->ringbuff_pos=g_ringbuff_pos;	// position im ringbuffer des neuen pakets ist seine aktuelle Position
	s->numEntities=0;				// es wurden noch keine entities gelesen

	if (ds==NULL)	// falls kein delta snapshot vorhanden
	{
		ds_entity_num=99999;	// markiere als nodelta
	}
	else
	{
		if (ds->numEntities==0)	// hat das delta snapshot selbst keine Entities ?
		{
			ds_entity_num=99999;	// markiere als nodelta
		}
		else
		{
			ds_entity=&ds->entities[0];
			ds_entity_num= 0;

			//ds_entity=&g_ringbuff[ds->ringbuff_pos & 0x7FF];		// hole das 1. entity
			//ds_entity_num=g_ringbuff[ds->ringbuff_pos & 0x7FF].number;	// und seine Eigentliche Entity nummer		
		}
	}

	// START
	// EndOfPacket(p) check einbauen TODO FIXME

	while(ENTITYNUM_NONE!=(index=MSG_ReadBits(p,10)))	// solange der gerade gelesene index !=1023
	{
		while (ds_entity_num<index)	// solange noch delta entities vor diesem index im delta snapshot sind
		{
			// " unchanged ds_entity_num"
			/*if (debug)
			{
				sprintf(dummy,"unchanged: %u\x0D\x0A",ds_entity_num);
				g_DebugText+=dummy;
			};*/

			memcpy(&g_ringbuff[g_ringbuff_pos & 0x7FF],ds_entity,sizeof(entityState_t));	// übernimm entity daten
			if (g_ringbuff[g_ringbuff_pos & 0x7FF].number!=1023)
			{
				g_ringbuff_pos++;	// falls kein dummy entity 1023 kopiert wurde
				s->numEntities++;	// erhöhe die position und den zähler
			}

			zaehler++;	// im delta packet in entity weiter

			if (zaehler<ds->numEntities)	// bin ich noch nicht beim letzten delta entity
			{
				ds_entity_num=g_ringbuff[(ds->ringbuff_pos+zaehler) & 0x7FF].number;
				ds_entity=&g_ringbuff[(ds->ringbuff_pos+zaehler) & 0x7FF];
			}
			else
				ds_entity_num=99999;	//tja ab jetzt keine delta entities mehr

		}	// while (ds_entity_num<index)

		if (ds_entity_num==index)	// eine vorhandes delta entity wird verändert
		{
			// "delta: index"
			if (debug)
			{
				sprintf(dummy,"delta: %u",index);
				g_DebugText+=dummy;
			}

			CL_ParseEntity(p,index,&g_ringbuff[g_ringbuff_pos & 0x7FF],ds_entity);
			//if (ds_entity->number!=1023)	// oder gleich ds_entity_num	
			if (g_ringbuff[g_ringbuff_pos & 0x7FF].number != 1023)	// oder gleich ds_entity_num	TODO FIXME
			{
				g_ringbuff_pos++;
				s->numEntities++;
			}

			zaehler++;

			if (zaehler<ds->numEntities)	// bin ich noch nicht beim letzten delta entity
			{
				ds_entity_num=g_ringbuff[(ds->ringbuff_pos+zaehler) & 0x7FF].number;
				ds_entity=&g_ringbuff[(ds->ringbuff_pos+zaehler) & 0x7FF];
			}
			else
			{
				ds_entity_num=99999;	//tja ab jetzt keine delta entities mehr
			}
		}
		else
		{
			// "baseline <index>
			if (debug)
			{
				sprintf(dummy,"baseline: %u",index);
				g_DebugText+=dummy;
			}

			// hole dir hier als delta vorlage ein entity aus der baseline
			CL_ParseEntity(p,index,&g_ringbuff[g_ringbuff_pos & 0x7FF],&g_baseline[index]);
			if (g_ringbuff[g_ringbuff_pos & 0x7FF].number != 1023)
			{
				g_ringbuff_pos++;
				s->numEntities++;
			}

			// da gier kein delta entity aus dem delta snapshot gelesen wurde
			// brauch auch kein neues gestzt werden
		}

	} //while(1023!=(index=ReadnBits(p,10)))
	
	// falls delta snapshot müssen noch die restlichen entities unverändert übernommen werden
	// if (ds_entity_num==99999)	return;	// keine delta entities mehr vorhanden

	while (ds_entity_num!=99999)
	{
		// "unchanged: ds_entity_num"
	/*	if (debug)
		{
			sprintf(dummy,"unchanged: %u\x0D\x0A",ds_entity_num);
			g_DebugText+=dummy;
		}
	*/
		memcpy(&g_ringbuff[g_ringbuff_pos & 0x7FF],ds_entity,sizeof(entityState_t));	// übernimm entity daten

		if (g_ringbuff[g_ringbuff_pos & 0x7FF].number!=1023)
		{
			g_ringbuff_pos++;	// falls kein dummy entity 1023 kopiert wurde
			s->numEntities++;	// erhöhe die position und den zähler
		}

		zaehler++;	// im delta packet ein entity weiter

		if (zaehler<ds->numEntities)	// bin ich noch nicht beim letzten delta entity
		{
			ds_entity_num=g_ringbuff[(ds->ringbuff_pos+zaehler) & 0x7FF].number;
			ds_entity=&g_ringbuff[(ds->ringbuff_pos+zaehler) & 0x7FF];
		}
		else
			return;	// das letzte delta entity wurde bereits kopiert
	}

}	

#endif 

