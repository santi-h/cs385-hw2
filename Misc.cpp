/**************************************************************************************************
* Author: Santiago Herrera
* Email: sherre9@uic.edu
* ACCC username: sherre9
* CS username: sherrer1
* Date: 03/2013
* Couse: CS385 - Operating Systems
* University: University of Illinois at Chicago
**************************************************************************************************/
#include"Misc.h"


/**************************************************************************************************
*
**************************************************************************************************/
vector<string> tokenize( const string& str, char del)
{
	int strlen = str.size();
	char ch;
	int i;
	vector<string> ret;
	string token;

	for( i=0; i<=strlen; i++)
	{
		ch = str[ i];
		if( ch==del || ch==0)
		{
			if( token.size()) ret.push_back( token);
			token = "";
		}
		else
			token += ch;
	}	

	return ret;
}

/**************************************************************************************************
*
**************************************************************************************************/
string trim( const string& str, char ch)
{
	string ret;
	int strlen = str.size();
	int i = 0;
	int j = strlen - 1;
	while( i<strlen && str[i] == ch) i++;
	while( j>=0 && str[j] == ch) j--;
	while( i<=j) ret += str[i++];
	return ret;
}

/**************************************************************************************************
*
**************************************************************************************************/
int isPrime( int n)
{
	int limit = (int)sqrt( (double)n);
	int i;
	for( i=2; i<limit; i++) if( !(n%i)) return 0;
	return 1;
}







