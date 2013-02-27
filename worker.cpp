/*worker workerID sleepTime msgID shmID [ semID ]

workerID is a unique identifier
passed by the parent when forking and execing the children,
starting with 1 for the first child and continuing to nWorkers for the last child.

sleepTime is the amount of time the workers will spend sleeping in each work cycle,
generated by master as a series of random numbers in decreasing order. ( See below. ) Note
that this is a floating pointnumber, not an integer.

msgID, shmID, and semID are the integer ID numbers for the message queue, shared memory
block, and semaphore block, as returned by msgget(), shmget( ), and semget( ), described
below. 
//*/

#include <stdio.h>

#include "CFuncs.h"

void getParameters( int, char**, int&, float&, int&, int&, int&);

/**************************************************************************************************
**************************************************************************************************/
int main( int argc, char** argv)
{
	float sleepTime;
	int workerID, msgID, shmID, semID;
	getParameters( argc, argv, workerID, sleepTime, msgID, shmID, semID);

	char msg[256];
	*((long*)msg) = (long)workerID;
	sprintf( msg+sizeof(long), "%f hello from worker %d", sleepTime, workerID);
	cMsgsnd( msgID, msg, strlen( msg+sizeof(long))+1, 0);
	int* shptr = (int*)cShmat( shmID, NULL, 0);
	shptr[workerID] = workerID;	

	return 0;
}


/**************************************************************************************************
**************************************************************************************************/
void getParameters( int argc, char** argv, int& workerID, float& sleepTime, int& msgID, int& shmID, int& semID)
{
	char errcstr[50]; sprintf( errcstr, "[%d] wrong parameters to worker\n", argc);
	if( argc<5 || argc>6) finish( errcstr,1,0);
	int r;

	if( sscanf( argv[1], "%d", &workerID) != 1) finish( errcstr,1,0);
	if( sscanf( argv[2], "%f", &sleepTime) != 1) finish( errcstr,1,0);
	if( sscanf( argv[3], "%d", &msgID) != 1) finish( errcstr,1,0);
	if( sscanf( argv[4], "%d", &shmID) != 1) finish( errcstr,1,0);
	
	if( argc>5)
	{
		if(sscanf( argv[5], "%d", &semID) != 1) finish( errcstr,1,0);
	}

	/* PRINT PARAMETERS
	printf( "worker\n");
	printf( "workerID[%d]\n", workerID);
	printf( "sleepTime[%f]\n", sleepTime);
	printf( "msgID[%d]\n", msgID);
	printf( "shmID[%d]\n", shmID);
	printf( "semID[%d]\n", semID);
	printf( "\n");
	//*/
	
}












