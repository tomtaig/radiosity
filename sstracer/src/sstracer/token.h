#ifndef _TOKEN_H_
#define _TOKEN_H_

// System includes
#include <stdio.h>

//
// Local includes
#include "error.h"

//
// Token functions
char *TokenLoad(FILE*);
void TokenNewFile(void);
void TokenPreserve(void);
char *TokenValue(void);

//
// _TOKEN_H_
#endif