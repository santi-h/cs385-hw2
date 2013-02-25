#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <float.h>
#include <stdlib.h>

#include "Command.h"
#include "CFuncs.h"

using namespace std;

vector<float> getRandoms( int = 1, float = 0, float = FLT_MAX);
vector<float>& sortRandoms( vector<float>&);
void printFloatVector( vector<float>&);

/**************************************************************************************************
* ENTRY POINT
**************************************************************************************************/
int main( int argc, char** argv)
{
	const int NBUFFERS = 1;
	const int NWORKERS = 2;
	const int SLEEPMIN = 3;
	const int SLEEPMAX = 4;

	int i,r;
	int nBuffers, nWorkers;
	float sleepMin, sleepMax;
	int randSeed = time( 0);
	bool lck = 0;
	
	const char* errcstr = "Bad parameters\n";

	if( argc < 5 || argc>7) finish( errcstr,1,0);

	//if( (nBuffers = atoi( argv[NBUFFERS])) <=0)
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
		if( string(adarg[i])=="-lock")
			lck = 1;
		else if( string(adarg[i]) == "-nolock")
			lck = 0;
		else if( !(r=sscanf( adarg[i], "%d", &randSeed)) || r==EOF)
			finish( errcstr,1,0);
	}

	//* PRINT PARAMETERS
	printf("nBuffers[%d]\n", nBuffers); 
	printf("nWorkers[%d]\n", nWorkers); 
	printf("sleepMin[%.2f]\n",sleepMin);
	printf("sleepMax[%.2f]\n", sleepMax);
	printf("randSeed[%d]\n", randSeed);
	printf("lck[%d]\n", lck);
	//*/

	srand( time(0));
	vector<float> res = getRandoms( 10, 0, 100);
	
	printFloatVector( res);	
	sortRandoms( res);
	printFloatVector( res);

	return 0;
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
	Command( "sort -nr").execute( feedPipe[0], readPipe[1], closeList);
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


