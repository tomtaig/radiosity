#include "variable.h"

//
// VariableAllocate
//
void VariableAllocate(char *name, int type) {

	if(DataExists(name)) {
		return;
	}

	DataCreate(name, type);

	return;
}

//
// VariableTrim
//
void VariableTrim(char *str) {

	char *s;
	char *e;

	for(s=str; *s==' '; s++);
	for(e=str+strlen(str); *e==' ' || *e=='\n' || *e=='\0'; e--);

	*(e+1)='\0';

	if(str!=s) {
		strcpy(str, s);
	}

	return;
}

//
// VariableParseStringValue
//
void VariableParseStringValue(char *name, FILE *fp) {

	char	valu[1024];
	int		i;

	VariableAllocate(&name[1], DATA_STRING);

	*valu='\0';

	TokenLoad(fp);
	strcat(valu, TokenValue());

	if(valu[0]=='"') {
		strcpy(valu, valu+1);
	}else{
		ErrorInvoke(ERROR_VARIABLE_PARSE);
		return;
	}

	i=strlen(valu)-1;

	while(valu[i]!='"' && valu[i]!='\0') {
		strcat(valu, " ");
		TokenLoad(fp);
		strcat(valu, TokenValue());
		i=strlen(valu)-1;
	}

	if(valu[i]=='"') {
		valu[i]='\0';
	}else{
		ErrorInvoke(ERROR_VARIABLE_PARSE);
		return;
	}
	
	VariableTrim(valu);

	DataWriteString(&name[1], valu);

	return;
}

//
// VariableParseAndStore
//
void VariableParseAndStore(FILE *fp) {

	char name[128];
	char valu[128];

	TokenLoad(fp);
	strcpy(name, TokenValue());
	
	VariableTrim(name);
	
	switch(*name) {
	case '%':
		TokenLoad(fp);
		strcpy(valu, TokenValue());
		VariableTrim(valu);
		VariableAllocate(&name[1], DATA_INTEGER);
		DataWriteInteger(&name[1], atoi(valu));
		break;
	case '!':
		TokenLoad(fp);
		strcpy(valu, TokenValue());
		VariableTrim(valu);
		VariableAllocate(&name[1], DATA_PRECISION);
		DataWritePrecision(&name[1], (prc_t)atof(valu));
		break;
	case '$':
		VariableParseStringValue(name, fp);
		break;
	case ';':
		fgets(valu, 128, fp);
		break;
	case '\0':
		break;
	default:
		ErrorInvoke(ERROR_VARIABLE_PARSE);
	}

	return;
}
		
//
// VariableParseFile
//
int VariableParseFile(char *name) {

	FILE	*fp;

	fp=fopen(name, "r");

	if(!fp) return 0;

	while(!feof(fp)) {
		VariableParseAndStore(fp);
	}

	fclose(fp);

	return 1;
}

//
// VariableParseString
//
void VariableParseString(char *str) {

	FILE	*fp;

	fp=fopen("sstracer.dat", "w"); 

	fprintf(fp, "%s\n", str);

	fclose(fp);

	VariableParseFile("sstracer.dat");

	return;
}







