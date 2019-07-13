#ifndef _ERROR_H_
#define _ERROR_H_

// System includes
#include <stdio.h>

//
// Error codes (always >0)
#define	ERROR_CMD_LINE_PARSE	1
#define ERROR_VAR_NOT_FOUND		2

//
// Error functions
void ErrorInvoke(int);
int  ErrorWasInvoked(void);

//
// _ERROR_H_
#endif
