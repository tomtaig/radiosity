#include "error.h"

//
// error occurance state
int	 g_occured=0;

//
// error messages for given codes
char g_strings[][128]={
"failed to open texpack",
"failed to find texture",
"failed to open map file",
"failed to dump lightmaps",
"essential variable is unspecified",
"form factor calculated above 1.0F",
"form factor calculated below 0.0F",
"polygon exceeded vertex limit",
"FaceEdgePlane encountered a degenerate triangle",
"error not found"
};

//
// error codes - parallel with g_strings
int	g_code[]={
ERROR_TEXPACK_NOT_FOUND,
ERROR_TEXTURE_NOT_FOUND,
ERROR_MAP_NOT_FOUND,
ERROR_LMP_NOT_WRITTEN,
ERROR_VAR_NOT_FOUND,
ERROR_FORM_FACTOR_ONE,
ERROR_FORM_FACTOR_ZERO,
ERROR_LARGE_POLYGON,
ERROR_EDGEPLANE_DEGENERATE,
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

	g_occured=!0;

	return;
}
