#include <stdio.h>
#include <unistd.h>
//#include <malloc/malloc.h>
#include <string.h>
#include <time.h>
#include <math.h>

void XGRead(void *ptr,int size,int nitems,FILE * stream,char const* type);
void XGWrite(char *ptr,int size,int nitems,FILE * stream, char const * type);
void sun_read(char *ptr, int size, int nitems, FILE * stream);
void sun_write(char *ptr, int size, int nitems, FILE * stream);

#ifdef _CRAYT3E
#define BIGEND64
#endif

/**********************/
/* Endianness of host */


typedef enum { LITTLE, BIG } ENDIAN;
ENDIAN		theEndian;
/******************************************************/

void sun_read(ptr, size, nitems, stream)
  char           *ptr;
  int             size;
  int             nitems;
  FILE           *stream;
{
  int             i, j;

  for (j = 0; j < nitems; j++)
    for (i = size - 1; i >= 0; i--)
      fread(ptr + j * size + i, 1, 1, stream);
}

/******************************************************/

void sun_write(ptr, size, nitems, stream)
  char           *ptr;
  int             size;
  int             nitems;
  FILE           *stream;
{
  int             i, j;

  for (j = 0; j < nitems; j++)
    for (i = size - 1; i >= 0; i--)
      fwrite(ptr + j * size + i, 1, 1, stream);
}

/*****************************************************************/

#ifndef BIGEND64

void XGRead(ptr, size, nitems, stream, type)
  void           *ptr; 
   char const      	  * type;
  FILE           *stream;
  int             size, nitems;
{
  if(!strcmp(type,"long")) { /* longs have to be treated specially, since
										  they can be either 4 or 8 bytes on different
										  arches. */
	 int dummy;
	 int i;
	 long *data;
	 	 
	 for(i=0; i< nitems; i++) {
		/*read the int, convert it to a long, put the long in mem. */
		XGRead((char *)(&dummy),sizeof(int),1,stream,"int");
		data =(long *)ptr; 
		*data = dummy;
		data++;
		ptr=(void *)data;
	 }
	 return;
  }

  if (theEndian == LITTLE) {
    fread(ptr, size, nitems, stream);
  } else {
    sun_read(ptr, size, nitems, stream);
  }
}
#else /* BIGEND64 */

void XGRead(ptr, size, nitems, stream, type)
  void           *ptr; 
  char const      	  * type;
  FILE           *stream;
  int             size, nitems;
{

  if(!strcmp(type,"long")|| !strcmp(type,"int")) { 
	  /* ints longs have to be treated specially, since
		  they are 8 bytes on the Cray. */
	 short dummy;
	 int i;
	 long *data;
	 	 
	 for(i=0; i< nitems; i++) {
		/*read the int, convert it to a long, put the long in mem. */
		XGRead((const char *)(&dummy),sizeof(short),1,stream,"short");
		data =(long *)ptr; 
		*data = dummy;
		data++;
		ptr=(void *)data;
	 }
	 return;
  }

  if (theEndian == LITTLE) {
    fread(ptr, size, nitems, stream);
  } else {
    sun_read(ptr, size, nitems, stream);
  }
}
#endif /* BIGEND64 */
/******************************************************/

#ifndef BIGEND64
void XGWrite(ptr, size, nitems, stream, type)
  char          *ptr;
  const char     * type;
  FILE           *stream;
  int             size, nitems;
{

  if(!strcmp(type,"long")){   /* longs require special treatment:  we store them
											as 4 bytes no matter what, though on diff. archs
											they can be 8 bytes. */
	 int dummy;
	 int i;
	 long *data;
	 data = (long *) ptr;
	 
	 for(i=0; i< nitems; i++) {
		dummy = *data;  /*Convert the long to an int, then write the int. */
		XGWrite((char *)(&dummy),sizeof(int),1,stream,"int");
		data++;
	 }
	 return;
  }

  if (theEndian == LITTLE) {
    fwrite(ptr, size, nitems, stream);
  } else {
    sun_write(ptr, size, nitems, stream);
  }
}

#else  /* BIGEND64 */
void XGWrite(ptr, size, nitems, stream, type)
    char            *ptr;
    const char      * const type;
	  FILE           *stream;
	  int             size, nitems;
{

	/* ints and longs on Cray T3E are 8 bytes. */
	if(!strcmp(type,"long")||!strcmp(type,"int")){ 
		short dummy;
		int i;
		int *data;
		data = (int *) ptr;
	 
		for(i=0; i< nitems; i++) {
			dummy = *data;  /*Convert the int to a short, then write the short.*/
			XGWrite((char *)(&dummy),sizeof(short),1,stream,"short");
			data++;
		}
		return;
	}

	if (theEndian == LITTLE) {
		fwrite(ptr, size, nitems, stream);
	} else {
		sun_write(ptr, size, nitems, stream);
	}
}

#endif /* BIGEND64 */


/******************************************************/

#ifdef UNICOS
unicos_read_float(ptr, nitems, stream)
  float          *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    fread(&d, 1, 1, stream);
    fread(&c, 1, 1, stream);
    fread(&b, 1, 1, stream);
    fread(&a, 1, 1, stream);

    if (a == 0 && b == 0 && c == 0 && d == 0)
      *iptr = 0;
    else {
      t = ((a << 1) | (b >> 7));
      *iptr = ((a >> 7) << 63) | ((t - 126 + 040000) << 48);
      *iptr |= (b | 0x80) << 40;
      *iptr |= c << 32;
      *iptr |= d << 24;
    }
  }
}
#endif

/******************************************************/

#ifdef UNICOS
unicos_read_int(ptr, nitems, stream)
  int            *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    fread(&a, 1, 1, stream);
    fread(&b, 1, 1, stream);
    fread(&c, 1, 1, stream);
    fread(&d, 1, 1, stream);

    *iptr = d << 24 | c << 16 | b << 8 | a;
    *iptr |= (d >> 7) ? 0xffffffff00000000 : 0x0;
  }
}
#endif

/******************************************************/

#ifdef UNICOS
unicos_write_float(ptr, nitems, stream)
  float          *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    t = ((*iptr >> 48) & 0xff) - 040000 + 126;

    a = ((*iptr & (1 << 63)) ? 1 << 7 : 0) | t >> 1;
    b = (t & 0x1) << 7 | ((*iptr >> 40) & 0x7f);
    c = *iptr >> 32;
    d = *iptr >> 24;

    fwrite(&d, 1, 1, stream);
    fwrite(&c, 1, 1, stream);
    fwrite(&b, 1, 1, stream);
    fwrite(&a, 1, 1, stream);
  }
}
#endif

/******************************************************/

#ifdef UNICOS
unicos_write_int(ptr, nitems, stream)
  int            *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    a = *iptr & 0xff;
    b = *iptr >> 8 & 0xff;
    c = *iptr >> 16 & 0xff;
    d = *iptr >> 24 & 0xff;
    fwrite(&a, 1, 1, stream);
    fwrite(&b, 1, 1, stream);
    fwrite(&c, 1, 1, stream);
    fwrite(&d, 1, 1, stream);
  }
}
#endif

/****************************************************************/
