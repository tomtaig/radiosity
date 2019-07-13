#ifndef _DATA_H_
#define _DATA_H_

// System includes
#include <string.h>

//
// Local includes
#include "error.h"
#include "vector.h"
#include "ternary.h"

//
// Data precision type
typedef vec_t prc_t;

//
// Data variable types
#define		DATA_INTEGER		1
#define		DATA_PRECISION		2
#define		DATA_STRING			3

//
// Data variable constants
#define		DATA_STRING_LEN		256
#define		DATA_VARIABLE_LEN	256

//
// Data functions
int		DataExists(char*);
int		DataCreate(char*, int);
int		DataDestroy(char*);
int		DataDestroyPrefixed(char*);
int		DataWriteInteger(char*, int);
int		DataWritePrecision(char*, prc_t);
int		DataWriteString(char*, char*);
int		DataReadInteger(char*);
prc_t	DataReadPrecision(char*);
char	*DataReadString(char*, char*);

//
// _DATA_H_
#endif