#ifndef _ERROR_H_
#define _ERROR_H_

// System includes
#include <stdio.h>

//
// Error codes (always >0)
#define	ERROR_TEXPACK_NOT_FOUND			1
#define	ERROR_TEXTURE_NOT_FOUND			2
#define	ERROR_MAP_NOT_FOUND				3
#define ERROR_LMP_NOT_WRITTEN			4
#define ERROR_VAR_NOT_FOUND				5
#define	ERROR_FORM_FACTOR_ONE			6
#define	ERROR_FORM_FACTOR_ZERO			7
#define ERROR_VARIABLE_FILE_NOT_FOUND	8
#define ERROR_VARIABLE_PARSE			9

//
// Error functions
void ErrorInvoke(int);
int  ErrorWasInvoked(void);

//
// _ERROR_H_
#endif
