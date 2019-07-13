#include "face.h"

//
// FacePush
//
void FacePush(face_t **sp, face_t *face) {

	face->next=*sp;
	*sp=face;

	return;
}

//
// FacePop
//
face_t *FacePop(face_t **sp) {

	face_t	*temp;

	temp=*sp;
	*sp=(*sp)->next;

	return temp;
}

//
// FaceRemove
//
void FaceRemove(face_t **sp, face_t *face) {

	return;
}

//
// FaceCopy
// 
face_t *FaceCopy(face_t *face) {

	face_t *temp;

	temp=malloc(sizeof(face_t));
	memcpy(temp, face, sizeof(face_t));

	return temp;
}

//
// FaceTriangleArea
//
vec_t FaceTriangleArea(vec3_t a, vec3_t b, vec3_t c) {

	vec3_t	b2;
	vec3_t	c2;
	vec3_t	d2;

	b2[0]=b[0]-a[0];
	b2[1]=b[1]-a[1];
	b2[2]=b[2]-a[2];

	c2[0]=c[0]-a[0];
	c2[1]=c[1]-a[1];
	c2[2]=c[2]-a[2];

	// inlined cross product (3d)
	d2[0]=b2[1]*c2[2]-b2[2]*c2[1];
	d2[1]=b2[2]*c2[0]-b2[0]*c2[2];
	d2[2]=b2[0]*c2[1]-b2[1]*c2[0];

	return sqrt(VectorDotProduct(d2, d2))/2.0F;
}

//
// FaceArea
//
void FaceArea(face_t *face) {

	int		i;

	face->area=0.0F;

	for(i=2; i<face->numverts; i++) {
		face->area+=FaceTriangleArea(face->verts[0], face->verts[i-1], face->verts[i]);
	}

	return;
}

//
// FaceBounds
//
void FaceBounds(face_t *face) {

	vec3_t	tmp;
	vec_t	dis;
	int		i=0;

	// Bounds are recalced during the bsping so keep it simple

	VectorClear(face->center);

	for(i=0; i<face->numverts; i++) {
		VectorAdd(face->center, face->verts[i], face->center);
	}

	VectorScale(face->center, face->numverts);

	face->radius=0;

	for(i=0; i<face->numverts; i++) {
		VectorSub(face->center, face->verts[i], tmp);
		dis=VectorDotProduct(tmp, tmp);
		if(face->radius<dis)face->radius=dis;
	}
	
	face->radius=sqrt(face->radius);

	return;
}

//
// FaceIntersectLinePlane
//
vec_t FaceIntersectLinePlane(vec3_t line1, vec2_t tline1, vec3_t line2, vec2_t tline2, plane_t plane, vec3_t res, vec2_t tres) {

	vec3_t	temp1;
	vec3_t	temp2;
	vec_t	numer;
	vec_t	denom;
	vec_t	time;

	VectorCopy(temp2, plane);
	VectorScaleInv(temp2, plane[3]);

	VectorSub(line1, temp2, temp1);
	VectorSub(line1, line2, temp2);

	numer=VectorDotProduct(plane, temp1);
	denom=VectorDotProduct(plane, temp2);

	if(denom==0) {
		return -1;
	}

	time=numer/denom;

	res[0]=line1[0]+((line2[0]-line1[0])*time);
	res[1]=line1[1]+((line2[1]-line1[1])*time);
	res[2]=line1[2]+((line2[2]-line1[2])*time);

	tres[0]=tline1[0]+((tline2[0]-tline1[0])*time);
	tres[1]=tline1[1]+((tline2[1]-tline1[1])*time);

	return time;
}

//
// FaceOptimiseVerts
//
void FaceOptimiseVerts(face_t *face) {

	face_t	a;
	int		i;
	int		n;

	// removes extra vertices after clipping (assumes adjacency)
	VectorCopy(a.verts[0], face->verts[0]);
	VectorCopy2(a.terts[0], face->terts[0]);

	for(i=1, n=1; i<face->numverts; i++) {
		if(VectorDistance(face->verts[i], face->verts[i-1])<0.01F) continue;
		VectorCopy2(a.terts[n], face->terts[i]);
		VectorCopy(a.verts[n++], face->verts[i]);
	}

	if(VectorDistance(face->verts[i-1], face->verts[0])<0.01F) {
		n--;
	}

	face->numverts=n;
	memcpy(face->verts, a.verts, n*sizeof(vec3_t));
	memcpy(face->terts, a.terts, n*sizeof(vec2_t));

	return;
}

//
// FaceClip
//
void FaceClip(face_t *face, plane_t plane, face_t *fside, face_t *bside) {

	static vec3_t	tvb[FACE_MAXVERTS];
	static vec2_t	ttb[FACE_MAXVERTS];
	vec2_t	dis;
	vec3_t	vin;
	vec2_t	tin;
	int		i;
	int		k;
	int		m=0;
	int		n=0;
	
	// FaceClip must not use face inside the clipping loop 
	// so fside and bside can equal face with no problems
	memcpy(tvb, face->verts, sizeof(vec3_t)*face->numverts);
	memcpy(ttb, face->terts, sizeof(vec2_t)*face->numverts);

	for(i=0; i<face->numverts; i++) {
		k=(i+1)%face->numverts;

		// calc distances, sign determines side
		dis[0]=VectorDotProduct(tvb[i], plane)-plane[3];
		dis[1]=VectorDotProduct(tvb[k], plane)-plane[3];

		// fully inside, no clip
		if(dis[0]<=0 && dis[1]<=0) {
			VectorCopy2(bside->terts[m], ttb[k]);
			VectorCopy(bside->verts[m++], tvb[k]);
		}

		// fully outside, no clip
		if(dis[0]>0 && dis[1]>0) {
			VectorCopy2(fside->terts[n], ttb[k]);
			VectorCopy(fside->verts[n++], tvb[k]);
		}

		// calc intersection vector
		FaceIntersectLinePlane(face->verts[i], face->terts[i], tvb[k], ttb[k], plane, vin, tin);

		// inside coming out, clip
		if(dis[0]<=0 && dis[1]>0) {
			VectorCopy2(fside->terts[n], tin);
			VectorCopy(fside->verts[n++], vin);
			VectorCopy2(fside->terts[n], ttb[k]);
			VectorCopy(fside->verts[n++], tvb[k]);
			VectorCopy2(bside->terts[m], tin);
			VectorCopy(bside->verts[m++], vin);
		}

		// outside coming in, clip
		if(dis[0]>0 && dis[1]<=0) {
			VectorCopy2(bside->terts[m], tin);
			VectorCopy(bside->verts[m++], vin);
			VectorCopy2(bside->terts[m], ttb[k]);
			VectorCopy(bside->verts[m++], tvb[k]);
			VectorCopy2(fside->terts[n], tin);
			VectorCopy(fside->verts[n++], vin);
		}
	}

	bside->numverts=m;
	fside->numverts=n;

	return;
}