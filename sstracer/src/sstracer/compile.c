#include "compile.h"

//
// keyboard buffer
char	g_keys[256];

//
// scene variables
bsp_t	*g_bsp;
face_t	*g_faces;

//
// camera & momentum
vec_t	g_camera[16]={1,0,0,0,0,-1,0,0,0,0,1,0,0,0,0,1};
vec_t	g_mo_cam_fwd=0;
vec_t	g_mo_cam_sde=0;

//
// frame counter
vec_t	g_frame_time[4]={1,1,1,1};
vec_t	g_frame_ave=0;
int		g_frames=0;
int		g_display=0;

//
// filter map
int		g_filter[3]={GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST};

//
// ExitFunc
//
void ExitFunc(int exvalue) {

	char output[DATA_STRING_LEN];

	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

	if(!DataReadInteger("app_windowed")) {
		glutLeaveGameMode();
	}

	glDeleteLists(0, 1);

	DataReadString("app_output", output);

	printf("-- dumping patches --\n");
	SaveLmp(output, g_faces);
	
	printf("-- destroying bsp --\n");
	SceneDestroy(g_bsp);

	printf("-- destroying variables --\n");
	DataDestroyPrefixed("");

	exit(exvalue);
}

//
// RedisplayFunc
//
void RedisplayFunc(void) {

	if(g_display) {
		return;
	}

	g_display=1;

	glutPostRedisplay();
}


//
// ShowFramesPerSecond
//
void ShowFramesPerSecond(void) {

	vec_t	fps;
	int		i;

	if(!DataReadInteger("app_showfps")) {
		return;
	}

	for(fps=0.0F, i=0; i<4; i++) {
		fps+=g_frame_time[i];
	}

	fps=1000.0F/(fps/4.0F);

	printf("%f\n", fps);
}

//
// FeedSceneToPatches
//
int FeedSceneToPatches(face_t *faces) {

	int		i=0;

	printf("-- preprocessing polygons --\n");

	while(faces && !ErrorWasInvoked()) {
		PatchPrepareFace(g_bsp, faces);
		faces=faces->next;
		i++;
	}

	return i;
}

//
// FeedPatchToOpenGL
//
void FeedPatchToOpenGL(face_t *face, int i, vec_t scl, unsigned char *p) {

	vec3_t	r;
	vec_t	s;
	
	switch(DataReadInteger("rnd_mode")) {
	case 0:
		VectorCopy(r, face->patches[i].rrad);
		r[0]=r[0]*face->patches[i].reflection[0]*scl;
		r[1]=r[1]*face->patches[i].reflection[1]*scl;
		r[2]=r[2]*face->patches[i].reflection[2]*scl;
		break;
	case 1:
		VectorCopy(r, face->patches[i].rrad);
		s=(r[0]+r[1]+r[2])/3.0F;
		r[0]=s*face->patches[i].reflection[0]*scl;
		r[1]=s*face->patches[i].reflection[1]*scl;
		r[2]=s*face->patches[i].reflection[2]*scl;
		break;
	case 2:
		VectorCopy(r, face->patches[i].rrad);
		s =r[0]*face->patches[i].reflection[0];
		s+=r[1]*face->patches[i].reflection[1];
		s+=r[2]*face->patches[i].reflection[2];
		r[0]=s*scl/3.0F;
		r[1]=s*scl/3.0F;
		r[2]=s*scl/3.0F;
		break;
	case 3:
		VectorCopy(r, face->patches[i].rrad);
		r[0]=(r[0]+r[1]+r[2])*face->patches[i].reflection[0]*scl;
		r[1]=(r[0]+r[1]+r[2])*face->patches[i].reflection[1]*scl;
		r[2]=(r[0]+r[1]+r[2])*face->patches[i].reflection[2]*scl;
		break;
	case 4:
		VectorCopy(r, face->patches[i].drad);
		r[0]=r[0]*face->patches[i].reflection[0]*scl;
		r[1]=r[1]*face->patches[i].reflection[1]*scl;
		r[2]=r[2]*face->patches[i].reflection[2]*scl;
		break;
	case 5:
		VectorCopy(r, face->patches[i].drad);
		r[0]=(r[0]+r[1]+r[2])*face->patches[i].reflection[0]*scl;
		r[1]=(r[0]+r[1]+r[2])*face->patches[i].reflection[1]*scl;
		r[2]=(r[0]+r[1]+r[2])*face->patches[i].reflection[2]*scl;
		break;
	}

	r[0]=(r[0]>1.0F)?1.0F:(r[0]<0.0F)?0.0F:r[0];
	r[1]=(r[1]>1.0F)?1.0F:(r[1]<0.0F)?0.0F:r[1];
	r[2]=(r[2]>1.0F)?1.0F:(r[2]<0.0F)?0.0F:r[2];

	p[0]=(unsigned char)(r[0]*255);
	p[1]=(unsigned char)(r[1]*255);
	p[2]=(unsigned char)(r[2]*255);

	return;
}

//
// FeedPatchesToOpenGL
//
void FeedPatchesToOpenGL(face_t *face, vec_t scl) {

	unsigned char p[TEXTURE_XY_SIZE][3];
	int		i;
	int		r;
	
	r=g_filter[DataReadInteger("rnd_filter")];

	for(i=0; i<PATCH_XY_SIZE; i++) {
		FeedPatchToOpenGL(face, i, scl, p[i]);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, face->binder);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, r);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, r);

	if(DataReadInteger("rnd_mipmap")) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, PATCH_X_SIZE, PATCH_Y_SIZE, GL_RGB, GL_UNSIGNED_BYTE, p);
	}else{
		glTexImage2D(GL_TEXTURE_2D, 0, 3, PATCH_X_SIZE, PATCH_Y_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, p);
	}

	return;
}

//
// FeedSceneToOpenGL
//
void FeedSceneToOpenGL(face_t *scene) {

	face_t	*a;
	vec2_t	bnd;
	vec_t	dev;

	dev=DataReadPrecision("rnd_brightness");

	RangeSceneMinMax(g_faces, bnd);

	for(a=scene; a; a=a->next) {
		FeedPatchesToOpenGL(a, 1.0F/((1.0F/dev)*bnd[1]));
	}

	return;
}

//
// DisplayFaceTexel
//
void DisplayFaceTexel(face_t *face, int i) {

	vec3_t	r;

	if(!(face->patches[i].params&PARM_ISUSED)) {
		return;
	}

	PatchXYToXYZQuick(face, (i%PATCH_X_SIZE), (i/PATCH_X_SIZE), r);

	VectorScaleInv(face->plane, sqrt(face->tarea));
	VectorAdd(r, face->plane, r);
	VectorScale(face->plane, sqrt(face->tarea));

	glVertex3fv(r);

	return;
}

//
// DisplayFaceTexels
//
void DisplayFaceTexels(face_t *face) {

	int		i;

	glPointSize(3.0F);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POINTS);

	glColor3f(1.0F, 0.0F, 0.0F);

	for(i=0; i<PATCH_XY_SIZE; i++) {
		DisplayFaceTexel(face, i);
	}

	glEnd();
	glEnable(GL_TEXTURE_2D);

	return;
}

//
// DisplaySceneTexels
//
void DisplaySceneTexels(face_t *scene) {

	for(; scene; scene=scene->next) {
		DisplayFaceTexels(scene);
	}

	return;
}

//
// DisplayFace
//
void DisplayFace(face_t *face, vec3_t col) {

	int		i;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, face->binder);

	glColor3f(col[0], col[1], col[2]);
	glBegin(GL_POLYGON);

	for(i=0; i<face->numverts; i++) {
		glTexCoord2f(face->terts[i][0], face->terts[i][1]);
		glVertex3fv(face->verts[i]);
	}

	glEnd();

	return;
}

//
// DisplayTree
//
void DisplayTree(bsp_t *node) {

	vec3_t	col;
	int		i;

	if(!node) {
		return;
	}

	if(!(node->params&PARM_ISLEAF)) {
		DisplayTree(node->front);
		DisplayTree(node->back);
		return;
	}

	if(!(node->params&PARM_HOLLOW)) {
		return;
	}

	col[0]=1.0F;
	col[1]=1.0F;
	col[2]=1.0F;

	for(i=0; i<node->numfaces; i++) {
		DisplayFace(&node->faces[i], col);
	}

	return;
}

//
// DisplayWorldAxis
//
void DisplayWorldAxis(void) {

	vec_t	scl;

	scl=DataReadPrecision("rnd_scenescale");

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1/scl);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1/scl, 0);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1/scl, 0, 0);
	glEnd();

	return;
}

//
// DisplayFunc
//
void DisplayFunc(void) {

	static	int	old_time=0;
	static	int	new_time=0;

	vec3_t	org;
	vec_t	scl;

	if(DataReadInteger("rnd_upload")) {
		FeedSceneToOpenGL(g_faces);
		DataWriteInteger("rnd_upload", 0);
	}

	old_time=new_time;
	new_time=GetTickCount();

	scl=DataReadPrecision("rnd_scenescale");
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(g_camera);
	glScaled(scl, scl, scl);

	if(DataReadInteger("rnd_showaxis")) {
		DisplayWorldAxis();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(DataReadInteger("rnd_cullbacks")) {
		glEnable(GL_CULL_FACE);
	}else{
		glDisable(GL_CULL_FACE);
	}

	org[0]=-VectorDotProduct(&g_camera[0], &g_camera[12])/scl;
	org[1]=-VectorDotProduct(&g_camera[4], &g_camera[12])/scl;
	org[2]=-VectorDotProduct(&g_camera[8], &g_camera[12])/scl;

	if(SceneVectorIntersection(g_bsp, org)) {
		glCullFace(GL_BACK);
	}else{
		glCullFace(GL_FRONT);
	}

	glEnable(GL_TEXTURE_2D);
	glCallList(1);

	if(DataReadInteger("rnd_texels")) {
		DisplaySceneTexels(g_faces);
	}

	g_frames++;

	glutSwapBuffers();

	g_frame_time[g_frames%4]=new_time-old_time;

	g_display=0;
}

//
// KeyboardFunc
//
void KeyboardFunc(unsigned char key, int x, int y) {

	if(key>='A' && key<='Z') {
		key-=('A'-'a');
	}

	g_keys[key]=!0;

	return;
}

//
// KeyupFunc
//
void KeyupFunc(unsigned char key, int x, int y) {

	if(key>='A' && key<='Z') {
		key-=('A'-'a');
	}

	g_keys[key]=0;

	return;
}

//
// MotionFunc
//
void MotionFunc(int x, int y) {

	static	ox;
	static	oy;
	int		cx;
	int		cy;

	if(x==ox && y==oy || !DataReadInteger("app_control")) {
		return;
	}

	cx=glutGet(GLUT_WINDOW_WIDTH )/2;
	cy=glutGet(GLUT_WINDOW_HEIGHT)/2;

	glutWarpPointer(cx, cy);

	glLoadIdentity();
	glRotatef((x-cx)/3, -g_camera[4], -g_camera[5], -g_camera[6]);
	glMultMatrixf(g_camera);
	glGetFloatv(GL_MODELVIEW_MATRIX, g_camera);

	glLoadIdentity();
	glRotatef((y-cy)/3, 1, 0, 0);
	glMultMatrixf(g_camera);
	glGetFloatv(GL_MODELVIEW_MATRIX, g_camera);

	ox=x;
	oy=y;

	RedisplayFunc();
}

//
// ResizeFunc
//
void ResizeFunc(int w, int h) {

	float Fw=w, Fh=h;

	glViewport(0, 0, Fw, Fh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (double)w/(double)h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);

	RedisplayFunc();
}

//
// MouseFunc
//
void MouseFunc(int button, int state, int x, int y) {

	int	r;

	if(button!=GLUT_LEFT_BUTTON) {
		return;
	}

	if(state!=GLUT_UP) {
		return;
	}

	r=DataReadInteger("app_control");
	DataWriteInteger("app_control", r^1);

	if(!r) {
		glutSetCursor(GLUT_CURSOR_NONE);
	}else{
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	RedisplayFunc();

	return;
}


//
// IdleFunc
//
void IdleFunc(void) {

	static	DWORD	stp=0;
	vec_t	scl;
	vec_t	dev;
	int		cul;
	int		axi;
	int		mod;
	int		mip;
	int		fil;
	int		con;

	con=DataReadInteger("app_control");
	
	if(stp<GetTickCount()-1000 || !stp) {
		ShowFramesPerSecond();
		stp=GetTickCount();
	}
	
	scl=DataReadPrecision("rnd_scenescale");
	dev=DataReadPrecision("rnd_brightness");
	cul=DataReadInteger("rnd_cullbacks");
	axi=DataReadInteger("rnd_showaxis");
	mod=DataReadInteger("rnd_mode");
	mip=DataReadInteger("rnd_mipmap");
	fil=DataReadInteger("rnd_filter");

	if(g_keys[27]==!0 && con) {
		ExitFunc(0x01);
	}

	if(g_keys['1']==!0 && con) {
		DataWriteInteger("app_iterate", 1);
		g_keys['1']=0;
		RedisplayFunc();
	}

	if(g_keys['2']==!0 && con) {
		DataWriteInteger("app_iterate", 0);
		g_keys['2']=0;
		RedisplayFunc();
	}

	if(g_keys['3']==!0 && con) {
		DataWriteInteger("rnd_showaxis", axi^1);
		g_keys['3']=0;
		RedisplayFunc();
	}

	if(g_keys['4']==!0 && con) {
		DataWriteInteger("rnd_cullbacks", cul^1);
		g_keys['4']=0;
		RedisplayFunc();
	}

	if(g_keys['5']==!0 && con) {
		DataWriteInteger("rnd_mipmap", mip^1);
		DataWriteInteger("rnd_upload", 1);
		g_keys['5']=0;
		RedisplayFunc();
	}

	if(g_keys['6']==!0 && con) {
		DataWriteInteger("rnd_filter", (fil+1)%3);
		DataWriteInteger("rnd_upload", 1);
		g_keys['6']=0;
		RedisplayFunc();
	}

	if(g_keys['w']==!0 && con) {
		g_mo_cam_fwd+=0.05;
		RedisplayFunc();
	}

	if(g_keys['s']==!0 && con) {
		g_mo_cam_fwd-=0.05;
		RedisplayFunc();
	}

	if(g_keys['a']==!0 && con) {
		g_mo_cam_sde+=0.05;
		RedisplayFunc();
	}

	if(g_keys['d']==!0 && con) {
		g_mo_cam_sde-=0.05;
		RedisplayFunc();
	}

	if(g_keys['r']==!0 && con) {
		DataWritePrecision("rnd_brightness", dev*1.2F);
		DataWriteInteger("rnd_upload", 1);
		RedisplayFunc();
	}

	if(g_keys['e']==!0 && con) {
		DataWritePrecision("rnd_brightness", dev/1.2F);
		DataWriteInteger("rnd_upload", 1);
		RedisplayFunc();
	}

	if(g_keys['x']==!0 && con) {
		DataWriteInteger("rnd_mode", (mod+1)%6);
		DataWriteInteger("rnd_upload", 1);
		g_keys['x']=0;
		RedisplayFunc();
	}

	if(g_keys['-']==!0 && con) {
		DataWritePrecision("rnd_scenescale", scl/1.05F);
		RedisplayFunc();
	}

	if(g_keys['=']==!0 && con) {
		DataWritePrecision("rnd_scenescale", scl*1.05F);
		RedisplayFunc();
	}

	glLoadIdentity();
	glTranslatef(g_mo_cam_sde, 0, 0);
	glMultMatrixf(g_camera);
	glGetFloatv(GL_MODELVIEW_MATRIX, g_camera);

	glLoadIdentity();
	glTranslatef(0, 0, g_mo_cam_fwd);
	glMultMatrixf(g_camera);
	glGetFloatv(GL_MODELVIEW_MATRIX, g_camera);

	// cap speed, 0.5 unit max
	g_mo_cam_fwd=(g_mo_cam_fwd>0.5)?0.5:(g_mo_cam_fwd<-0.5)?-0.5:g_mo_cam_fwd;

	// reduce camera momentum
	if(g_mo_cam_fwd!=0) {
		g_mo_cam_fwd=(g_mo_cam_fwd<0)?g_mo_cam_fwd+0.01:g_mo_cam_fwd-0.01;
	}

	// clamp speed when near 0
	if(g_mo_cam_fwd>-0.01 && g_mo_cam_fwd<0.01) g_mo_cam_fwd=0; else glutPostRedisplay();

	// cap speed, 0.5 unit max
	g_mo_cam_sde=(g_mo_cam_sde>0.5)?0.5:(g_mo_cam_sde<-0.5)?-0.5:g_mo_cam_sde;

	// reduce camera momentum
	if(g_mo_cam_sde!=0) {
		g_mo_cam_sde=(g_mo_cam_sde<0)?g_mo_cam_sde+0.01:g_mo_cam_sde-0.01;
	}

	// clamp speed when near 0
	if(g_mo_cam_sde>-0.01 && g_mo_cam_sde<0.01) g_mo_cam_sde=0; else glutPostRedisplay();

	// iterate if we want to
	if(DataReadInteger("app_iterate")) {
		PatchFindAndDistribute(g_bsp, g_faces);
		printf("iterating..\n");
	}

	return;
}

//
// InitVariables
//
void InitVariables(void) {

	DataCreate("app_iterate", DATA_INTEGER);
	DataWriteInteger("app_iterate", 0);

	DataCreate("rnd_upload", DATA_INTEGER);
	DataWriteInteger("rnd_upload", 1);

	DataCreate("app_control", DATA_INTEGER);
	DataWriteInteger("app_control", 0);

	return;
}

//
// ParseCommandLine
//
void ParseCommandLine(int argc, char** argv) {

	char	str[DATA_STRING_LEN];
	int		i;

	str[0]='\0';

	for(i=1; i<argc; i++) {
		strcat(str, argv[i]);
		strcat(str, " ");

		if(argv[i][0]=='$') {
			strcat(str, "\"");
			strcat(str, argv[++i]);
			strcat(str, "\"");
		}

		if(i!=argc-1) {
			strcat(str, " ");
		}
	}
	
	VariableParseString(str);

	return;
}

//
// main
//
int main(int argc, char** argv) {
	
	char wndmsg[DATA_STRING_LEN];
	char dismod[DATA_STRING_LEN];
	char inputf[DATA_STRING_LEN];

	// memory leak checking
#ifndef NDEBUG
	int		i;

	i=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	i|=_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(i);
#endif

	// initialising
	printf("-- initialising application --\n");

	// initialise keys
	memset(g_keys, 0, 255);

	// initialise faces
	g_faces=NULL;

	// initialise all variables
	VariableParseFile("variables.txt");

	// internal variables
	InitVariables();

	// setup pixel type thru GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);

	// command line variables
	ParseCommandLine(argc, argv);

	// input file variable
	DataReadString("app_input", inputf);

	// load bsp scene & faces
	g_bsp=LoadScene(inputf, &g_faces);

	// terminate if errors
	if(!g_bsp) {
		DataDestroyPrefixed("");
		return !0;
	}

	// prepare and create patches
	FeedSceneToPatches(g_faces);

	// terminate if errors
	if(ErrorWasInvoked()) {
		DataDestroyPrefixed("");
		return !0;
	}

	// window message, display mode
	DataReadString("app_message", wndmsg);
	DataReadString("app_display", dismod);

	// initialise GLUT window
	if(DataReadInteger("app_windowed")) {
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(700, 700);
		glutCreateWindow(wndmsg);
	}else{
		glutGameModeString(dismod);
		glutEnterGameMode();
		glutSetCursor(GLUT_CURSOR_NONE);
	}

	// init GLUT callbacks
	glutKeyboardFunc(KeyboardFunc);
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ResizeFunc);
	glutMotionFunc(MotionFunc);
	glutPassiveMotionFunc(MotionFunc);
	glutKeyboardUpFunc(KeyupFunc);
	glutIdleFunc(IdleFunc);
	glutMouseFunc(MouseFunc);

	// init OpenGL states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	printf("-- creating display lists --\n");
	glGenLists(1);
	glNewList(1, GL_COMPILE);
	DisplayTree(g_bsp);
	glEndList();

	printf("-- finished initialisation --\n");
	glutMainLoop();

	return 0;
}