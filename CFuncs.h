/**************************************************************************************************
* Author: Santiago Herrera
* Email: sherre9@uic.edu
* Date: 02/2013
* Couse: CS385 - Operating Systems
* University: University of Illinois at Chicago
**************************************************************************************************/

#ifndef CFUNCS_C
#define CFUNCS_C

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define W_ERRNO 1
#define NO_ERRNO 0

void finish( const char*, int, int);
void* cpalloc( const void*, size_t);

void* cMalloc( size_t);
int cPipe( int[2]);
int cClose( int);
int cDup( int);
int cDup2( int, int);
int cExecvp( const char*, char* const);
int cFopen( const char*, const char*);
pid_t cFork();
pid_t cWait4( pid_t, int*, int, struct rusage*);

#endif //CFUNCS_C




