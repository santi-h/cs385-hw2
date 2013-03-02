#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>

#include "CFuncs.h"
#include "mytypes.h"

// session variables
int workerID, nBuffers, msgID, shmID, semID;
float sleepTime;
int* shptr;

// forward declarations
void getParameters( int, char**);
void sendMsg( const char*);
void readRound( int);
void writeRound( int);
void cleanup();
void cleanup( int);

/**************************************************************************************************
* ENTRY POINT
**************************************************************************************************/
int main( int argc, char** argv)
{
	getParameters( argc, argv);	
	shptr = (int*)cShmat( shmID, NULL, 0);
	atexit( cleanup);
	signal(SIGINT, cleanup);	
	
	int idx, i;
	//*
	for( i=0, idx=workerID; i<nBuffers; i++)
	{
		readRound( idx);
		idx = (idx+workerID) % nBuffers;
		readRound( idx);
		idx = (idx+workerID) % nBuffers;
		writeRound( idx);
		idx = (idx+workerID) % nBuffers;
	}
	//*/
	
	msg_hdr msg = {workerID, DONE};
	cMsgsnd( msgID, &msg, sizeof(msg),0);	
	return 0;
}

/**************************************************************************************************
* cleanup
**************************************************************************************************/
void cleanup( int i){ cleanup();}
void cleanup()
{
	cShmdt( (void*)shptr);
}
/**************************************************************************************************
* sends <msg> with type <workerID> through queue <msgID>
**************************************************************************************************/
void sendMsg( const char* msg)
{
	int msglen = strlen( msg);
	int segment_offset = sizeof( long);
	int segment_size = msglen+segment_offset+1; //+1 for str terminator \0
	uint8_t* segment = (uint8_t*)malloc( segment_size);
	
	*((long*)segment) = (long)workerID;
	strcpy( (char*)(segment+segment_offset), msg);
	
	cMsgsnd( msgID, segment, msglen+1, 0);
	free( segment);
}

/**************************************************************************************************
* A read operation will consist of the following steps:
* Read the initial value in the buffer.
* Sleep for sleepTime seconds
* Read the value of the buffer again
* Send a message to the master process if the value changed while the worker was sleeping. The me
* ssage should include the ID of the worker sending the message, the buffer which changed, the
* initial value, and the final value.
**************************************************************************************************/
void readRound( int idx)
{
	int initialVal = shptr[ idx];
	usleep( (useconds_t)sleepTime*1000000);
	int val = shptr[ idx];

	if( val != initialVal)
	{
		const int buf_size = sizeof( msg_hdr)+sizeof(msg_readerr);
		uint8_t* buf = (uint8_t*)malloc( buf_size);
		msg_hdr* buf_hdr = (msg_hdr*)buf;
		msg_readerr* buf_readerr = (msg_readerr*)(buf+sizeof(msg_hdr));

		buf_hdr->workerID = workerID;
		buf_hdr->op = READ_ERR;
		buf_readerr->buf_idx = idx;
		buf_readerr->ini_value = initialVal;
		buf_readerr->fin_value = val;

		cMsgsnd( msgID, buf, buf_size, 0);
		free( buf);
	}

}

/**************************************************************************************************
* A write operation will consist of the following steps:
* Read the initial value of the buffer.
* Sleep for sleepTime seconds. ( See above. )
* Add 1 << ( ID – 1 ) to the value read in step (a), and store the result back into the buffer.
* ( E.g. worker number 3 adds binary 000001 shifted left 2 places = 000100 = 4 ).
* Each worker will add a single binary bit in a different position, which will make it possible to
* later identify which worker(s) were involved in any error conditions that erased write operations. 
**************************************************************************************************/
void writeRound( int idx)
{
	int initialVal = shptr[ idx];
	usleep( (useconds_t)sleepTime*1000000);
	initialVal = initialVal | ( 1 << (workerID-1));
	
	shptr[ idx] = initialVal;
}

/**************************************************************************************************
**************************************************************************************************/
void getParameters( int argc, char** argv)
{
	char errcstr[50]; sprintf( errcstr, "[%d] wrong parameters to worker\n", argc);
	if( argc<6 || argc>7) finish( errcstr,1,0);

	if( sscanf( argv[1], "%d", &workerID) != 1) finish( errcstr,1,0);
	if( sscanf( argv[2], "%d", &nBuffers) != 1) finish( errcstr,1,0);
	if( sscanf( argv[3], "%f", &sleepTime) != 1) finish( errcstr,1,0);
	if( sscanf( argv[4], "%d", &msgID) != 1) finish( errcstr,1,0);
	if( sscanf( argv[5], "%d", &shmID) != 1) finish( errcstr,1,0);
	
	if( argc>5)
	{
		if(sscanf( argv[5], "%d", &semID) != 1) finish( errcstr,1,0);
	}

	/* PRINT PARAMETERS
	printf( "worker\n");
	printf( "workerID[%d]\n", workerID);
	printf( "nBuffers[%d]\n", nBuffers);
	printf( "sleepTime[%f]\n", sleepTime);
	printf( "msgID[%d]\n", msgID);
	printf( "shmID[%d]\n", shmID);
	printf( "semID[%d]\n", semID);
	printf( "\n");
	//*/
	
}













