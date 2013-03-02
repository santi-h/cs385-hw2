#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <float.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "Command.h"
#include "CFuncs.h"
#include "mytypes.h"

using namespace std;

int mid; //message queue
int sid; //shared memory
int* shptr;


vector<float> getRandoms( int = 1, float = 0, float = FLT_MAX);
vector<float>& sortRandoms( vector<float>&);
void printFloatVector( vector<float>&);
void getParameters( int, char**, int&, int&, float&, float&, int&, bool&);
string strbits( int);
string strhexs( int);
void cleanup();
void cleanup(int); 

/**************************************************************************************************
* ENTRY POINT
**************************************************************************************************/
int main( int argc, char** argv)
{
	int nBuffers, nWorkers, randSeed;
	float sleepMin, sleepMax;
	bool lck;
	int i;

	//==============================================
	//* populate session variables
	//==============================================
	getParameters( argc, argv, nBuffers, nWorkers, sleepMin, sleepMax, randSeed, lck);

	srand( randSeed);
	vector<float> res = getRandoms( nWorkers, sleepMin, sleepMax);
	
	printf("unsorted: ");printFloatVector( res);
	sortRandoms( res);
	printf("  sorted: ");printFloatVector( res);

	int ssize = nBuffers*sizeof(int);
	mid = cMsgget( IPC_PRIVATE, 0200 | 0400);//create queue
	sid = cShmget( IPC_PRIVATE, ssize, 0200 | 0400);//create shared memory
	shptr = (int*)cShmat( sid, NULL, 0);// attach to shared memory
	atexit( cleanup);
	signal(SIGINT, cleanup);
	memset( shptr, 0, ssize); // zero out shared memory
	//*/	

	//==============================================
	//* execute workers
	//==============================================
	vector<pid_t> workers; // workers[i] has workerID=i+1
	char buf[ 256];
	
	for( i=0; i<nWorkers; i++)
	{
		sprintf( buf, "./worker %d %d %f %d %d %d", i+1, nBuffers, res[i], mid, sid, -1);
		workers.push_back( Command( buf).execute());
	}
	//*/
	
	//==============================================
	//* get messages from message queue
	//==============================================
	int runningWorkers = nWorkers;
	msg_hdr* buf_hdr;
	msg_readerr* buf_readerr;
	while( runningWorkers)
	{
		cMsgrcv( mid, buf, 256, 0, 0);
		buf_hdr = (msg_hdr*)buf;
		if( buf_hdr->op == READ_ERR)
		{
			buf_readerr = (msg_readerr*)(buf+sizeof(msg_hdr));
			printf( "worker[%ld] read error\n", buf_hdr->workerID);
			printf( "\tat buffer index\t%d\n", buf_readerr->buf_idx);
			printf( "\tinitial value\t%d\n", buf_readerr->ini_value);
			printf( "\tfinal value\t%d\n", buf_readerr->fin_value);
		} 
		if( buf_hdr->op == DONE)
		{
			printf( "worker[%ld] done\n", buf_hdr->workerID);
			runningWorkers--;
		}
		
	}
	int stat = -1;
	for( i=0; i<(int)workers.size(); i++) waitpid( workers[i], &stat, 0);
	//*/

	//==============================================
	//* read shared memory
	//==============================================
	printf("Buffers:\n");
	for( i=0; i<nBuffers; i++) printf("[%5d%5s]", i,""); printf("\n");
	for( i=0; i<nBuffers; i++) printf("[0x%08x]", shptr[i]); printf("\n");
	//*/

	return 0;
}

/**************************************************************************************************
* Cleanup on exit/interrupt
**************************************************************************************************/
void cleanup( int i) { cleanup();}
void cleanup()
{
	msgctl( mid, IPC_RMID, NULL); //remove queue
	shmctl( sid, IPC_RMID, NULL); //remove shared memory
}

/**************************************************************************************************
* Returns string representation of 0b<n>
**************************************************************************************************/
string strbits( int n)
{
	string ret;
	int i, m;
	for( i=0, m=1<<31; i<32; i++, n <<= 1)
		ret += (n&m)?'1':'0';
	return ret;
}

/**************************************************************************************************
* Returns string representation of 0x<n>
**************************************************************************************************/
string strhexs( int n)
{
	char buf[265];
	sprintf( buf, "%08x", n);
	return string( buf);
}

/**************************************************************************************************
* Creates and returns a vector of <n> random floats within the range [<min>, <max>]
**************************************************************************************************/
vector<float> getRandoms( int n, float min, float max)
{
	vector<float> ret;

	for( int i=0; i<n; i++)
		ret.push_back( min + (max-min)*rand()/RAND_MAX);
	
	return ret;
}

/**************************************************************************************************
* Sorts the elements in <randoms> with a call to "sort -nr"
**************************************************************************************************/
vector<float>& sortRandoms( vector<float>& randoms)
{
	int feedPipe[2] = {0,0};
	int readPipe[2] = {0,0};
	int feed, reed;
	vector<float>::size_type s;

	//* Create pipes and set feed and reed
	cPipe( feedPipe);
	cPipe( readPipe);	
	feed = feedPipe[1];
	reed = readPipe[0];
	//*/

	//* Execute sort and close unused fd's
	vector<int> closeList;
	closeList.push_back( feed);
	closeList.push_back( reed);
	pid_t pid = Command( "sort -nr").execute( feedPipe[0], readPipe[1], closeList);
	close( feedPipe[0]);
	close( readPipe[1]);
	//*/

	//* Feed sort with random numbers
	char buf[256];
	for( s=0; s<randoms.size(); s++)
	{
		sprintf( buf, "%f\n", randoms[s]);
		write( feed, buf, strlen( buf));
	}
	close( feed);
	//*/	

	//* Retrieve results from pipe
	float t;
	int stat = -1;
	waitpid( pid, &stat, 0);
	buf[ read( reed, buf, 256)] = 0;
	close( reed);
	char* str = buf;
	s=0;
	while( str && sscanf( str, "%f", &t)!=EOF)
	{
		randoms[s++] = t;
		if( (str = strchr( str, '\n'))) str++;
	}
	//*/

	return randoms;
}

/**************************************************************************************************
**************************************************************************************************/
void printFloatVector( vector<float>& vec)
{
	printf("[");
	for( vector<float>::size_type i=0; i<vec.size(); i++)
		printf("%f%s", vec[i], (i==vec.size()-1?"]\n":", "));
}

/**************************************************************************************************
**************************************************************************************************/
void getParameters(	int argc, char** argv,
					int& nBuffers, int& nWorkers,
					float& sleepMin, float& sleepMax,
					int& randSeed, bool& lck)
{
	const int NBUFFERS = 1;
	const int NWORKERS = 2;
	const int SLEEPMIN = 3;
	const int SLEEPMAX = 4;
	
	const char* errcstr = "Bad parameters\n";	
	int i,r;
	randSeed = time( 0);
	lck = 0;
	if( argc < 5 || argc>7) finish( errcstr,1,0);

	if( !(r=sscanf(argv[NBUFFERS],"%d",&nBuffers)) || r==EOF)
		finish( errcstr,1,0);
	if( !(r=sscanf(argv[NWORKERS],"%d",&nWorkers)) || r==EOF || nWorkers>=nBuffers || nWorkers<=0)
		finish( errcstr,1,0);
	if( !(r=sscanf( argv[SLEEPMIN], "%f", &sleepMin)) || r==EOF)
		finish( errcstr,1,0);
	if( !(r=sscanf( argv[SLEEPMAX], "%f", &sleepMax)) || r==EOF || sleepMax<=sleepMin)
		finish( errcstr,1,0);
	
	char* adarg[2] = {argc>5?argv[5]:0, argc>6?argv[6]:0};
	
	for( i=0; i<2; i++) if( adarg[i])
	{
		int newSeed = 0;
		if( string(adarg[i])=="-lock")
			lck = 1;
		else if( string(adarg[i]) == "-nolock")
			lck = 0;
		else if( sscanf( adarg[i], "%d", &newSeed) != 1)
			finish( errcstr,1,0);
		else
			randSeed = newSeed;
	}

	/* PRINT PARAMETERS
	printf("nBuffers[%d]\n", nBuffers); 
	printf("nWorkers[%d]\n", nWorkers); 
	printf("sleepMin[%.2f]\n",sleepMin);
	printf("sleepMax[%.2f]\n", sleepMax);
	printf("randSeed[%d]\n", randSeed);
	printf("lck[%d]\n", lck);
	//*/

}

