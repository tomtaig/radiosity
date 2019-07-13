#ifndef _PORTAL_H_
#define _PORTAL_H_

// Local includes
#include "scene.h"
#include "error.h"
#include "face.h"

//
// Local functions
int		PortalCalculateHull(bsp_t*, face_t**);
void	PortalFromNodes(bsp_t*, bsp_t*);
void	PortalWriteFile(char*, face_t*);

//
// _PORTAL_H_
#endif