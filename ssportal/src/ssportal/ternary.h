#ifndef _TERNARY_H_
#define _TERNARY_H_

//
// System includes
#include <memory.h>
#include <stdlib.h>

//
// Ternary structure
typedef struct tern_t {
	void	*in;		// Generic information storage
	char	eqv;	
	int		shr;
	struct tern_t *hi;
	struct tern_t *lo;
	struct tern_t *eq;
}tern_t;

//
// Ternary functions
tern_t	*TernaryTraverse(tern_t*, int*, char*);
tern_t	*TernarySearch(tern_t*, char*);
tern_t	*TernaryPrefix(tern_t*, char*, int*, char*);
tern_t	*TernaryString(tern_t*, char*, void*);
tern_t	*TernaryRemove(tern_t*, char*);
void	TernaryDestroy(tern_t*);

//
// _TERNARY_H_
#endif