#include "texpack.h"

//
// TexpackLoadTexture
//
int TexpackLoadTexture(FILE *fp, patch_t *pmap, char *p, int params) {

	unsigned char	a[TEXTURE_XY_SIZE];
	unsigned char	b[TEXTURE_XY_SIZE][3];
	unsigned int	i;
	unsigned int	x;
	unsigned int	y;
	unsigned int	n;
	vec_t			s;
	vec_t			r;

	// grab reflection value
	r=DataReadPrecision("rad_reflection");

	// keep file position
	n=ftell(fp);

	// new file position
	fread(&i, sizeof(int), 1, fp);
	fseek(fp, i-1, SEEK_SET);

	// read the texture
	for(i=0; i<TEXTURE_XY_SIZE; i++) {
		fread(b+i, sizeof(char), 3, fp);
		fread(a+i, sizeof(char), 1, fp);
	}

	// initialise patches
	for(i=0; i<PATCH_XY_SIZE; i++) {
		VectorClear(pmap[i].drad);
		VectorClear(pmap[i].rrad);
		VectorClear(pmap[i].reflection);
	}

	// scale value
	s=(vec_t)TEXTURE_XY_SIZE/(vec_t)PATCH_XY_SIZE;

	// render into patches
	for(i=0; i<TEXTURE_XY_SIZE; i++) {
		x=(int)(((vec_t)(i%TEXTURE_X_SIZE))/TEXTURE_X_SIZE*PATCH_X_SIZE);
		y=(int)(((vec_t)(i/TEXTURE_X_SIZE))/TEXTURE_Y_SIZE*PATCH_Y_SIZE);

		pmap[y*PATCH_X_SIZE+x].drad[0]+=((vec_t)a[i])/s;
		pmap[y*PATCH_X_SIZE+x].drad[1]+=((vec_t)a[i])/s;
		pmap[y*PATCH_X_SIZE+x].drad[2]+=((vec_t)a[i])/s;

		pmap[y*PATCH_X_SIZE+x].reflection[0]+=(((vec_t)b[i][0])/255.0F/s);
		pmap[y*PATCH_X_SIZE+x].reflection[1]+=(((vec_t)b[i][1])/255.0F/s);
		pmap[y*PATCH_X_SIZE+x].reflection[2]+=(((vec_t)b[i][2])/255.0F/s);
	}

	for(i=0; i<PATCH_XY_SIZE; i++) {
		VectorScaleInv(pmap[i].reflection, r);
	}

	// move to old file position
	fseek(fp, n, SEEK_SET);

	// copy over texture
	if(params&TEXPACK_LOAD_TEXTURE && p) {
		memcpy(p, b, TEXTURE_XY_SIZE*3*sizeof(char));
	}

	return TEXPACK_LOAD_SUCCESS;
}

//
// TexpackLoad
//
int TexpackLoad(char *texpack, char *texxer, patch_t *pmap, char *p, int params) {

	FILE	*fp;
	char	name2[9];
	char	name[81];
	int		i;
	int		n;
	int		r;
	short	numtexxrs;

	r=TEXPACK_LOAD_FAILED;

	if(texxer[0]=='\0' || texpack[0]=='\0') {
		return r;
	}

	if(!(fp=fopen(texpack, "rb"))) {
		ErrorInvoke(ERROR_TEXPACK_NOT_FOUND);
		return r;
	}

	fread(name, sizeof(char), 80, fp);
	fread(&numtexxrs, sizeof(short), 1, fp);

	for(i=0; i<numtexxrs; i++) {
		fread(name2, sizeof(char), 8, fp);
		fread(name, sizeof(char), 40, fp);

		name[40]='\0';
		name2[8]='\0';

		for(n=0; name2[n]!=' ' && name2[n]!='\0'; n++);
		name2[n]='\0';

		if(strcmp(name2, texxer)) {
			fseek(fp, sizeof(int), SEEK_CUR);
			continue;
		}
		
		r=TexpackLoadTexture(fp, pmap, p, params);
		break;
	}

	fclose(fp);

	if(r==TEXPACK_LOAD_FAILED && texxer[0]!='\0') {
		ErrorInvoke(ERROR_TEXTURE_NOT_FOUND);
	}

	return r;
}