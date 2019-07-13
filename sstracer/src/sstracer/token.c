#include "token.h"

//
// Token globals
char	g_token[128];
int		g_preservecycles=0;

//
// TokenLoad
//
char *TokenLoad(FILE *fp) {

	if(g_preservecycles) {
		g_preservecycles--;
	}else{
		fscanf(fp, "%s", g_token);
	}

	if(feof(fp)) {
		g_token[0]='\0';
	}

	return g_token;
}

//
// TokenNewFile
//
void TokenNewFile(void) {

	g_preservecycles=0;

	return;
}

//
// TokenPreserve
//
void TokenPreserve(void) {

	g_preservecycles++;

	return;
}

//
// TokenValue
//
char *TokenValue(void) {

	return g_token;
}