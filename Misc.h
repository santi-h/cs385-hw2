/**************************************************************************************************
* Author: Santiago Herrera
* Email: sherre9@uic.edu
* ACCC username: sherre9
* CS username: sherrer1
* Date: 03/2013
* Couse: CS385 - Operating Systems
* University: University of Illinois at Chicago
**************************************************************************************************/
#ifndef MISC_H
#define MISC_H

#include <string>
#include <vector>
#include <math.h>

enum TimeType { USER, SYSTEM};

using namespace std;

vector<string> tokenize( const string&, char = ' ');
string trim( const string&, char = ' ');
int isPrime( int);

#endif //MISC_H







