/* isin and icos return a value from -16384 to +16384 */
int isin(unsigned degrees); /* input must be between 0 and 360 */
int icos(unsigned degrees); /* input must be between 0 and 360 */
/* these return a value from -1 to +1 */
double fsin(double rad); /* arbitrary input */
double fcos(double rad); /* arbitrary input */
double ftan(double rad); /* arbitrary input */
