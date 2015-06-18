int ozopenreadloc(unsigned loc);
int ozopenread(char *name);
int ozgetc(int handle);
int ozclose(int handle);
char *ozloadcluster(unsigned loc);
void ozinitfs(void);
unsigned ozinclocation(unsigned loc);
