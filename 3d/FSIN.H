/* isin and icos return a value from -16384 to +16384 */
int isin(unsigned degrees); /* input must be between 0 and 360 */
int icos(unsigned degrees); /* input must be between 0 and 360 */
int div16384(long value); /* divide by 16384 */
