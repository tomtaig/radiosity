#include "error.h"

//
// error occurance state
int		g_occured=0;

//
// error messages for given codes
char	g_strings[][128]={
"could not parse command line",
"variable not found",
"error not found"
};

//
// error codes - parallel with g_strings
int		g_code[]={
ERROR_CMD_LINE_PARSE,
ERROR_VAR_NOT_FOUND,
0
};

//
// ErrorWasInvoked
//
int ErrorWasInvoked(void) {

	return g_occured;
}

//
// ErrorFindString
//
char *ErrorFindString(int code) {

	int		i;

	for(i=0; g_code[i]!=0 && g_code[i]!=code; i++);

	return g_strings[i];
}

//
// ErrorInvoke
//
void ErrorInvoke(int code) {

	char *s=ErrorFindString(code);

	printf("Error %d: %s\n", code, s);

	g_occured=1;

	return;
}
