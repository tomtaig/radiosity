#include "data.h"

//
// Variable global namespace
tern_t	*g_vars=NULL;

//
// DataExists
//
int DataExists(char *var) {

	return (TernarySearch(g_vars, var))?0:!0;
}

//
// DataCreate
//
int DataCreate(char *var, int type) {

	void	*data;

	if(TernarySearch(g_vars, var)) {
		return !0;
	}

	switch(type) {
	case DATA_INTEGER: 
		data=malloc(sizeof(int));
		break;
	case DATA_STRING:
		data=malloc(sizeof(char)*DATA_STRING_LEN);
		break;
	case DATA_PRECISION:
		data=malloc(sizeof(prc_t));
		break;
	}
		
	g_vars=TernaryString(g_vars, var, data);

	return 0;
}

//
// DataDestroy
//
int DataDestroy(char *var) {

	tern_t	*node;

	node=TernarySearch(g_vars, var);

	if(node==NULL) {
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}else{
		free(node->in);
		TernaryRemove(g_vars, var);
	}

	return (node)?!0:0;
}

//
// DataDestroyPrefixed
//
int	DataDestroyPrefixed(char *prefix) {

	char	match[DATA_VARIABLE_LEN];
	int		nump;
	int		i;
	tern_t	*node;

	nump=0;

	node=TernaryPrefix(g_vars, prefix, &nump, match);

	for(i=0; node; i++) {
		free(node->in);
		g_vars=TernaryRemove(g_vars, match);
		node=TernaryPrefix(g_vars, prefix, &nump, match);
	}

	return i;
}

//
// DataWriteInteger
//
int	DataWriteInteger(char *var, int value) {

	tern_t	*node;

	node=TernarySearch(g_vars, var);

	if(node) {
		*((int*)node->in)=value;
	}else{
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}

	return (node)?0:!0;
}

//
// DataWritePrecision
//
int	DataWritePrecision(char *var, prc_t value) {

	tern_t	*node;

	node=TernarySearch(g_vars, var);

	if(node) {
		*((prc_t*)node->in)=value;
	}else{
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}

	return (node)?0:!0;
}

//
// DataWriteString
//
int DataWriteString(char *var, char *value) {

	tern_t	*node;
	int		sizeo;

	sizeo=strlen(value)+1;
	sizeo=(sizeo<DATA_STRING_LEN)?sizeo:DATA_STRING_LEN-1;
	sizeo*=sizeof(char);
	
	node=TernarySearch(g_vars, var);

	if(node) {
		memcpy(node->in, value, sizeo);
	}else{
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}

	return (node)?0:!0;
}

//
// DataReadInteger
//
int	DataReadInteger(char *var) {

	tern_t	*node;

	node=TernarySearch(g_vars, var);

	if(node) {
		return *((int*)node->in);
	}else{
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}

	return 0;
}

//
// DataReadPrecision
//
prc_t DataReadPrecision(char *var) {

	tern_t	*node;

	node=TernarySearch(g_vars, var);

	if(node) {
		return *((prc_t*)node->in);
	}else{
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}

	return (prc_t)0;
}

//
// DataReadString
//
char *DataReadString(char *var, char *br) {

	tern_t	*node;

	node=TernarySearch(g_vars, var);

	if(node) {
		strcpy(br, (char*)node->in);
	}else{
		ErrorInvoke(ERROR_VAR_NOT_FOUND);
	}

	return (node)?br:NULL;
}
