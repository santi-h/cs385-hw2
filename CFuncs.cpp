/**************************************************************************************************
* Author: Santiago Herrera
* Email: sherre9@uic.edu
* ACCC username: sherre9
* CS username: sherrer1
* Date: 03/2013
* Couse: CS385 - Operating Systems
* University: University of Illinois at Chicago
**************************************************************************************************/
#include "CFuncs.h"

void finish( const char* msg, int stat, int err)
{
	if( err == W_ERRNO)
		perror( msg);
	else
		write( STDOUT_FILENO, msg, strlen( msg));

	exit( stat);
}

void* cMalloc( size_t size)
{
	void* ret = malloc( size);
	if( !ret)
		finish("malloc() failed", 1, NO_ERRNO);

	return ret;
}

void* cpalloc( const void* ptr, size_t size)
{
	void* ret = cMalloc( size);
	memcpy( ret, ptr, size);
	return ret;
}

int cPipe( int pipefd[2])
{
	if( pipe( pipefd) < 0)
		finish( "pipe() failed", 1, W_ERRNO);

	return 0;
}

int cClose( int fd)
{
	int ret;
	if( (ret=close( fd)) < 0)
		finish( "close() failed", 1, W_ERRNO);

	return ret;
}

int cDup( int oldfd)
{
	int ret;
	if( (ret=dup( oldfd)) < 0)
		finish( "dup() failed", 1, W_ERRNO);

	return ret;
}

int cDup2( int oldfd, int newfd)
{
	int ret;
	if( (ret=dup2( oldfd, newfd)) < 0)
		finish( "dup2() failed", 1, W_ERRNO);
	
	return ret;
}

int cExecvp( const char* file, char* const argv[])
{
	int ret = execvp( file, argv);
	finish( "execvp() failed", 1, W_ERRNO);

	return ret; // never reaches this point
}

int cFopen( const char* path, const char* mode)
{
	FILE* f = fopen( path, mode);
	if( !f) finish( "fopen() failed", 1, W_ERRNO);
	return fileno( f);
}

pid_t cFork()
{
	pid_t ret;
	if( (ret = fork()) < 0)
		finish( "fork() failed", 1, W_ERRNO);

	return ret;
}

pid_t cWait4( pid_t pid, int* status, int options, struct rusage* rusage)
{
	pid_t ret = wait4( pid, status, options, rusage);
	if( ret<0) finish( "wait4() failed", 1, W_ERRNO);
	return ret;
}

int cMsgget( key_t key, int msgflg)
{
	int ret;
	if( (ret = msgget( key, msgflg)) == -1) 
		finish( "msgget() failed", 1, W_ERRNO);

	return ret;
}

int cMsgsnd( int msqid, const void *msgp, size_t msgsz, int msgflg)
{
	int ret = msgsnd( msqid, msgp, msgsz, msgflg);
	if( ret == -1) finish( "msgsnd() failed",1,W_ERRNO);
	return ret;
}

ssize_t cMsgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
{
	ssize_t ret = msgrcv( msqid, msgp, msgsz, msgtyp, msgflg);
	if( ret == -1)
		finish( "msgrcv() failed", 1, W_ERRNO);
	return ret;
}

int cShmget( key_t key, size_t size, int shmflg)
{
	int ret = shmget( key, size, shmflg);
	if( ret == -1)
		finish( "shmget() failed",1,W_ERRNO);

	return ret;
}

void* cShmat( int shmid, const void* shmaddr, int shmflg)
{
	void* ret = shmat( shmid, shmaddr, shmflg);
	if( ret == (void*)-1)
		finish("shmat() failed",1,W_ERRNO);

	return ret;
}

int cShmdt( const void* shmaddr)
{
	int ret = shmdt( shmaddr);
	if( ret==-1)
		finish( "shmdt() failed",1,W_ERRNO);

	return ret;
}

int cSemget(key_t key, int nsems, int semflg)
{
	int ret = semget( key, nsems, semflg);
	if( ret<0)
		finish( "semget() failed",1,W_ERRNO);
	return ret;
}






















