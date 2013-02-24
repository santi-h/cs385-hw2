#include "Command.h"

/***************************************************************************************
* CONSTRUCTORS Y'ALL!
***************************************************************************************/
Command::Command( const string& str)
{
	reset();
	operator =( str);	
}

Command::Command()
{
	reset();
}

/***************************************************************************************
* Returns a string with the error
***************************************************************************************/
const string& Command::getError() const
{
	return err;
}

/***************************************************************************************
* For automatic cast to pid_t
***************************************************************************************/
Command::operator pid_t() const
{
	return pid;
}

/***************************************************************************************
* Returns a string that describes the object
***************************************************************************************/
string Command::toString() const
{
	string ret;

	for( unsigned int i=0; i<args.size(); i++)
	{
		ret += args[i];
		if( i<args.size()-1) ret += " ";
	}

	if( readFile.size() || writeFile.size()) ret += " ";

	if( readFile.size())
		(ret += "< ") += readFile;

	if( writeFile.size())
		(ret += "> ") += writeFile;

	return ret;
}

/***************************************************************************************
* Executes the command using <intput> as its standard input and <output> as its standard
* output.
* Returns:	-1: The command contains a sintax error and cannot be executed
*			>0: Child process created and execution attempted. The returned value is the
*			id of the new process
***************************************************************************************/
pid_t Command::execute( int input, int output, const vector<int>& closeFds)
{
	int i;
	int nargs = args.size(); // for argl[last] == 'NULL'
	char** argl;
	pid_t lpid;

	if( err.size())
	{
		printf("%s\n", err.c_str());
		return -1;
	}


	if( (lpid = cFork()) == 0)
	{
		for( vector<int>::size_type t=0; t<closeFds.size(); t++) close( closeFds[t]);

		//... no comming back from this block
		// SET STDIN
		if( input != STDIN_FILENO)
		{
			cClose( STDIN_FILENO);
			cDup2( input, STDIN_FILENO);
			close( input);
		}

		// SET STDOUT
		if( output != STDOUT_FILENO)
		{
			cClose( STDOUT_FILENO);
			cDup2( output, STDOUT_FILENO);
			cClose( output);			
		}
		
		// CREATE ARG LIST
		argl = (char* *)cMalloc( nargs*sizeof(void*)+1);
		for( i=0; i<nargs; i++) 
			argl[i] = (char*)cpalloc(args[i].c_str(), args[i].size()+1);
			
		argl[i] = NULL;

		// LAUNCH
		execvp( args[0].c_str(), argl);

		// EXIT PROCESS IF FAILED
		finish( "execvp failed()", 1, W_ERRNO);
	}
	else if( lpid>0)
		pid = lpid;
	
	running = 1;
	return pid;
}

/***************************************************************************************
* Zeroes out class members
***************************************************************************************/
void Command::reset()
{
	err = "No command to execute";
	pid = 0;
	args.clear();
	readFile = "";
	writeFile = "";
	running = 0;
	memset( &processInfo, 0, sizeof( struct rusage));
}

/***************************************************************************************
* Populates args and err
***************************************************************************************/
Command& Command::operator =( const string& str)
{
	reset();

	vector<string> tokens = tokenize( str);
	string newErr;	

	for( unsigned int i=0; i<tokens.size() && !newErr.size(); i++)
	{
		if( tokens[i] == "<" || tokens[i] == ">")
		{
			if( i == tokens.size()-1)
				newErr = "A file has to be specified for redirection";
			else if( i==0)
				newErr = "'"+tokens[i]+"'"+" is not executable";
			else
			{
				string* direction;
				direction = (tokens[i]=="<") ? (&readFile) : (&writeFile);
				*direction = tokens[i+1];
				
				tokens.erase( tokens.begin()+i+1);
				tokens.erase( tokens.begin()+i--);
			}
		}
		else
			args.push_back( tokens[i]);
	}
	
	// everything OK: !newErr.size() && args.size()
	if( newErr.size())
		err = newErr;
	else if( args.size())
		err = "";

	return *this;
} 

/***************************************************************************************
* Returns 1 if the unreaped child terminated (thus reaping it), 0 otherwise 
* Populates command info field if returned 1
***************************************************************************************/
bool Command::bgWait( )
{
	bool ret = 0;
	int status = -1;
	struct rusage rusage;

	if( pid && running)
	{
		cWait4( pid, &status, WNOHANG, &rusage);
		
		if( (WIFEXITED(status)) || (WIFSIGNALED(status)))
		{
			processInfo = rusage;
			running = 0;
			ret = 1;
		}
	}

	return ret;
}

/***************************************************************************************
* Returns 1 if it reaps its unreaped process, 0 otherwise.
* Populates <processInfo> field if returned 1
***************************************************************************************/
bool Command::fgWait()
{
	struct rusage rusage;
	bool ret = 0;

	if( pid && running)
	{
		cWait4( pid, NULL, 0, &rusage);

		processInfo = rusage;
		running = 0;
		ret = 1;
	}

	return ret;
}

/***************************************************************************************
* Returns the number of seconds elapsed in executing this command according to the 
* information in member processInfo
***************************************************************************************/
double Command::getTime( TimeType type) const
{
	long int sec;
	long int usec;
	if( type == USER)
	{
		sec = processInfo.ru_utime.tv_sec;
		usec = processInfo.ru_utime.tv_usec;
	}
	else
	{
		sec = processInfo.ru_stime.tv_sec;
		usec = processInfo.ru_stime.tv_usec;
	}

	return( (double)sec + usec/(double)1000000);
}


