/**************************************************************************************************
* Author: Santiago Herrera
* Email: sherre9@uic.edu
* Date: 02/2013
* Couse: CS385 - Operating Systems
* University: University of Illinois at Chicago
**************************************************************************************************/

#ifndef MISC_H
#define MISC_H

#include <string>
#include <vector>

enum TimeType { USER, SYSTEM};

using namespace std;

vector<string> tokenize( const string&, char = ' ');
string trim( const string&, char = ' ');

#endif //MISC_H







