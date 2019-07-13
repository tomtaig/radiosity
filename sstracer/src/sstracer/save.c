#include "save.h"

//
// SaveLmpPatchG
//
void SaveLmpPatchG(FILE *fp, face_t *face, int s) {

	vec_t	r;
	char	p;

	r=(face->patches[s].rrad[0]+face->patches[s].rrad[1]+face->patches[s].rrad[2])/3.0F;
	r=(r>1.0F)?1.0F:(r<0.0F)?0.0F:r;

	p=(char)(r*63);
	p=(p>63)?0:(p<0)?63:63-p;

	fwrite(&p, sizeof(char), 1, fp);

	return;
}

//
// SaveLmpPatchRGB
//
void SaveLmpPatchRGB(FILE *fp, face_t *face, int s) {

	vec_t	r;
	char	p;

	r=face->patches[s].rrad[0];
	r=(r>1.0F)?1.0F:(r<0.0F)?0.0F:r;
	p=(char)(r*63);
	p=(p>63)?0:(p<0)?63:63-p;
	fwrite(&p, sizeof(char), 1, fp);

	r=face->patches[s].rrad[1];
	r=(r>1.0F)?1.0F:(r<0.0F)?0.0F:r;
	p=(char)(r*63);
	p=(p>63)?0:(p<0)?63:63-p;
	fwrite(&p, sizeof(char), 1, fp);

	r=face->patches[s].rrad[2];
	r=(r>1.0F)?1.0F:(r<0.0F)?0.0F:r;
	p=(char)(r*63);
	p=(p>63)?0:(p<0)?63:63-p;
	fwrite(&p, sizeof(char), 1, fp);

	return;
}

//
// SaveLmpFaceG
//
void SaveLmpFaceG(FILE *fp, face_t *faces, int n) {

	face_t	*face;
	int		s;

	for(face=faces; face; face=face->next) {
		if(face->index==n) break;
	}

	if(face==NULL) {
		return;
	}

	for(s=0; s<PATCH_XY_SIZE; s++) {
		SaveLmpPatchG(fp, face, s);
	}

	return;
}

//
// SaveLmpFaceRGB
//
void SaveLmpFaceRGB(FILE *fp, face_t *faces, int n) {

	face_t	*face;
	int		s;

	for(face=faces; face; face=face->next) {
		if(face->index==n) break;
	}

	if(face==NULL) {
		return;
	}

	for(s=0; s<PATCH_XY_SIZE; s++) {
		SaveLmpPatchRGB(fp, face, s);
	}

	return;
}

//
// SaveLmp
//
int SaveLmp(char *name, face_t *faces) {

	face_t	*face;
	FILE	*fp;
	short	i;
	short	n;

	if(!(fp=fopen(name, "wb"))) {
		ErrorInvoke(ERROR_LMP_NOT_WRITTEN);
		return SAVE_FAILED;
	}

	i=0;

	for(face=faces; face; face=face->next) {
		i++;
	}

	fwrite(&i, sizeof(short), 1, fp);

	// Greyscale data
	for(n=0; n<i; n++) {
		SaveLmpFaceG(fp, faces, n);
	}

	// Coloured data
	for(n=0; n<i; n++) {
		SaveLmpFaceRGB(fp, faces, n);
	}

	fclose(fp);

	return SAVE_SUCCESS;
}