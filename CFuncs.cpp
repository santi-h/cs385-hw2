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





