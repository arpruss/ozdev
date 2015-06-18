/*
lib3d.c

Standard Wizard 3d and 4d math functions

Copyright© 2002, Mark Hamilton

*/
#include <oz.h>
#include "lib3d.h"
#include "fsin.h"

int sq(int num);

void ozrotatepointx(Vector_t *_v, int rot)
{
    static long y, z;
    register Vector_t *v=_v;
	y = v->y;
	z = v->z;
    v->y = div16384(y * icos(rot) - z * isin(rot));
    v->z = div16384(y * isin(rot) + z * icos(rot));
}

void ozrotatepointy(Vector_t *_v, int rot)
{
    static long x, z;
    register Vector_t *v=_v;
	x = v->x;
	z = v->z;
    v->x = div16384(x * icos(rot) - z * isin(rot));
    v->z = div16384(x * isin(rot) + z * icos(rot));
}

void ozrotatepointz(Vector_t *_v, int rot)
{
    static long x, y;
    register Vector_t *v=_v;
	x = v->x;
	y = v->y;
    v->x = div16384(x * icos(rot) - y * isin(rot));
    v->y = div16384(x * isin(rot) + y * icos(rot));
}


void ozplotpointcam(Vector_t *v, Cam_t *c, Point_t *p)
{
    static Vector_t temp;
    static Vector_t offset;
	temp.x = v->y;
	temp.y = v->x;
	temp.z = v->z + 256;
	offset.x = -c->x;
	offset.y = -c->y;
	offset.z = -c->z;
	ozrotatepointx(&temp, -c->pitch);
	ozrotatepointy(&temp, -c->roll);
	ozrotatepointz(&temp, -c->yaw);
	oztranslatevector(&temp, &offset);
	oztranslatevector(&temp, &offset);
	oztranslatevector(&temp, &offset);
	p->x = temp.x * 256 / temp.z;
	p->y = temp.y * 256 / temp.z;
}


void ozplotpoint(Vector_t *v, Point_t *p)
{
	Vector_t temp;

	/* flip x and y to rotate points 90° */
	temp.x = v->y;
	temp.y = v->x;
	temp.z = v->z + 256; /* add a large number so it doesn't look too big */
	/* add perspective */
	p->x = temp.x * 256 / temp.z;
	p->y = temp.y * 256 / temp.z;
}


void oztranslatevector(Vector_t *v, Vector_t *offset)
{
	v->x += offset->x;
	v->y += offset->y;
	v->z += offset->z;
}

int sq(int num)
{
	return num*num;
}

void ozcopyvector(Vector_t *dest, Vector_t *src)
{
    memcpy(dest,src,sizeof(Vector_t));
}
