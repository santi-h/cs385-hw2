#include"Misc.h"


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


