#ifndef _LOAD_H_
#define _LOAD_H_

// System includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
// Local includes
#include "scene.h"
#include "error.h"

//
// Local functions
bsp_t	*LoadNode(FILE*);
bsp_t	*LoadScene(char*);

//
// _LOAD_H_
#endif