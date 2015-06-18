/*
lib3d.h

Structs for standard Wizard 3d and 4d math functions

Copyright© 2002, Mark Hamilton

*/

#define PI 3.14159265358979

typedef struct {
	int x, y, z;
} Vector_t;

typedef struct {
	int x, y;
} Point_t;

typedef struct {
	int pitch, roll, yaw;
	int x, y, z;
} Cam_t;


/* protos */
void ozrotatepointx(Vector_t *v, int rot);
void ozrotatepointy(Vector_t *v, int rot);
void ozrotatepointz(Vector_t *v, int rot);
void ozplotpointcam(Vector_t *v, Cam_t *c, Point_t *p);
void ozplotpoint(Vector_t *v, Point_t *p);
void ozcopyvector(Vector_t *dest, Vector_t *src);
void oztranslatevector(Vector_t *v, Vector_t *offset);
