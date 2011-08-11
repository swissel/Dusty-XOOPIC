/******************************************************************
* Header for XGRead/Write
*
******************************************************************/
#if defined (__GNUG__) || defined (__cplusplus)
extern "C" {
#endif
void XGWrite(void *ptr, int size, int nitems, FILE * stream,const char * type);
void XGRead(void *ptr,int size,int nitems,FILE * stream,const char * type);
#if defined (__GNUG__) || defined (__cplusplus)
}
#endif
