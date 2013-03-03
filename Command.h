/**************************************************************************************************
* Author: Santiago Herrera
* Email: sherre9@uic.edu
* Date: 03/2013
* Couse: CS385 - Operating Systems
* University: University of Illinois at Chicago
**************************************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <unistd.h>

#include "CFuncs.h"
#include "Misc.h"

#define USERTIME 1
#define SYSTEMTIME 0


using namespace std;

class Command
{
private:
	string err;
	pid_t pid;
	vector<string> args;// args[0]: command
	string readFile;
	string writeFile;
	bool running; // 1: forked and not waited for. 0: otherwise
	struct rusage processInfo; // valid iff !running && pid

	void reset();

public:
	Command();
	Command( const string&);

	pid_t execute( int = STDIN_FILENO, int = STDOUT_FILENO, const vector<int>& = vector<int>());
	void set( const string&);
	Command& operator =( const string&);
	bool bgWait(); // background wait
	bool fgWait(); // foreground wait

	string readFilename() const;
	string writeFilename() const;
	const string& getError() const;
	string toString() const;
	operator pid_t() const;
	bool isRunning() const;
	double getTime( TimeType) const;

}; 

inline bool Command::isRunning() const { return running; }
inline string Command::readFilename() const { return readFile;}
inline string Command::writeFilename() const { return writeFile;}

#endif //COMMAND_H

