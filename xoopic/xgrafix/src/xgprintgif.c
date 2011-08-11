#ifdef XPMENABLED

/* xpmtoppm.c - read an X11 pixmap file and produce a portable pixmap
**
** Copyright (C) 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
**
** Upgraded to support XPM version 3 by
**   Arnaud Le Hors (lehors@mirsa.inria.fr)
**   Tue Apr 9 1991
**
** Rainer Sinkwitz sinkwitz@ifi.unizh.ch - 21 Nov 91:
**  - Bug fix, no advance of read ptr, would not read 
**    colors like "ac c black" because it would find 
**    the "c" of "ac" and then had problems with "c"
**    as color.
**    
**  - Now understands multword X11 color names
**  
**  - Now reads multiple color keys. Takes the color
**    of the hightest available key. Lines no longer need
**    to begin with key 'c'.
**    
**  - expanded line buffer to from 500 to 2048 for bigger files
*/

#include "xgprintgif.h"
#include <X11/xpm.h>
#include <ctype.h>
#include <string.h>

/*******************************************************************/
/*******************************************************************/

#define BITS    12
#define HSIZE  5003            /* 80% occupancy */
#define TRUE 1
#define FALSE 0
#define NKEYS 5
#define MAXCOLORS 256
#define MAX_LINE 2048
#define HASH_SIZE 20023
#define ppm_hashpixel(p) ( ( ( (long) PPM_GETR(p) * 33023 + (long) PPM_GETG(p) * 30013 + (long) PPM_GETB(p) * 27011 ) & 0x7fffffff ) % HASH_SIZE )
#define ARGVAL() (*++(*argv) || (--argc && *++argv))
#ifdef COMPATIBLE               /* But wrong! */
# define MAXCODE(n_bits)        ((code_int) 1 << (n_bits) - 1)
#else /*COMPATIBLE*/
# define MAXCODE(n_bits)        (((code_int) 1 << (n_bits)) - 1)
#endif /*COMPATIBLE*/
#define HashTabOf(i)       htab[i]
#define CodeTabOf(i)    codetab[i]
#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i)        ((char_type*)(htab))[i]
#define de_stack               ((char_type*)&tab_suffixof((code_int)1<<BITS))

/*******************************************************************/

typedef int (* ifunptr)();
typedef int code_int;

#ifdef NO_UCHAR
typedef char   char_type;
#else /*NO_UCHAR*/
typedef        unsigned char   char_type;
#endif /*NO_UCHAR*/

#ifdef SIGNED_COMPARE_SLOW
typedef unsigned long int count_int;
typedef unsigned short int count_short;
#else /*SIGNED_COMPARE_SLOW*/
typedef long int          count_int;
#endif /*SIGNED_COMPARE_SLOW*/

/*******************************************************************/

static int colorstobpp ( int colors );
static int GetPixel ( int x, int y );
static void BumpPixel ( void );
static int GIFNextPixel ( ifunptr getpixel );
static void GIFEncode ( FILE* fp, int GWidth, int GHeight, int GInterlace,
			    int Background, int BitsPerPixel, int Red[], 
			    int Green[], int Blue[], ifunptr GetPixel );
static void Putword ( int w, FILE* fp );
static void compress ( int init_bits, FILE* outfile, ifunptr ReadValue );
static void output ( code_int code );
static void cl_block ( void );
static void cl_hash ( count_int hsize );
static void char_init ( void );
static void char_out ( int c );
static void flush_char ( void );
void ReadXPMFile(char **stream, int *widthP, int *heightP, int *ncolorsP,
		 int *chars_per_pixelP, pixel **colorsP);
void getaline( char *line, int size, char **stream);
void ToGif(char *stream, char *outFile);
void XpmToGif(char **xpmData, char *outFile);
int ppm_getint(char *stream);

/*******************************************************************/

char *colorKeys[] = { "s", "m", "g4", "g", "c" };
int read_index;
int write_index;
pixval ppm_pbmmaxval;
pixel** pixels;
colorhash_table cht;
int Width, Height;
int curx, cury;
long CountDown;
int Pass = 0;
int Interlace;
int n_bits;                        /* number of bits/code */
int maxbits;                       /* user settable max # bits/code */
code_int maxcode;                  /* maximum code, given n_bits */
code_int maxmaxcode;               /* should NEVER generate this code */
count_int htab [HSIZE];
unsigned short codetab [HSIZE];
code_int hsize = HSIZE;                 /* for dynamic table sizing */
code_int free_ent;                      /* first unused entry */
int clear_flg;
int offset;
long int in_count;                /* length of input */
long int out_count;               /* # of codes output (for debugging)*/
int g_init_bits;
FILE* g_outfile;
int ClearCode;
int EOFCode;
unsigned long cur_accum;
int cur_bits;
int a_count;
char accum[ 256 ];
int getaline_index;
unsigned long masks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
			    0x001F, 0x003F, 0x007F, 0x00FF,
			    0x01FF, 0x03FF, 0x07FF, 0x0FFF,
			    0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

/**********************************************************************/
/**********************************************************************/

void *my_malloc(size_t size) {
  void *temp = (void *)malloc(size);
  if (temp==NULL) {
    printf("Out of memory\n");
    exit(-1);
  }
  return temp;
}

void *my_realloc(void *old, size_t size) {
  void *temp = (void *)realloc(old, size);
  if (temp==NULL) {
    printf("Out of memory\n");
    exit(-1);
  }
  return temp;
}

void ppm_freecolorhist( colorhist_vector chv ) {
  free( chv );
}

void ppm_freecolorhash( colorhash_table cht ) {
  int i;
  colorhist_list chl, chlnext;

  if (cht!=NULL) {
    for ( i = 0; i < HASH_SIZE; ++i )
      for ( chl = cht[i]; chl != (colorhist_list) 0; chl = chlnext )
	{
	  chlnext = chl->next;
	  free( chl );
	}
    free( cht );
  }
}

colorhash_table ppm_alloccolorhash() {
  colorhash_table cht;
  int i;
  
  cht = (colorhash_table) my_malloc( HASH_SIZE * sizeof(colorhist_list) );
  
  for ( i = 0; i < HASH_SIZE; ++i )
    cht[i] = (colorhist_list) 0;
  
  return cht;
}

colorhash_table ppm_computecolorhash(pixel** pixels, int cols, int rows, 
				     int maxcolors, int* colorsP) {
  colorhash_table cht = NULL;
  register pixel* pP;
  colorhist_list chl;
  int col, row, hash;
  
  cht = ppm_alloccolorhash( );
  *colorsP = 0;
  
  /* Go through the entire image, building a hash table of colors. */
  for ( row = 0; row < rows; ++row )
    for ( col = 0, pP = pixels[row]; col < cols; ++col, ++pP ) {
      hash = ppm_hashpixel( *pP );
      for ( chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next )
	if ( PPM_EQUAL( chl->ch.color, *pP ) )
	  break;
      if ( chl != (colorhist_list) 0 )
	++(chl->ch.value);
      else {
	if ( ++(*colorsP) > maxcolors ) {
	  ppm_freecolorhash( cht );
	  return (colorhash_table) 0;
	}
	chl = (colorhist_list) my_malloc( sizeof(struct colorhist_list_item) );
	chl->ch.color = *pP;
	chl->ch.value = 1;
	chl->next = cht[hash];
	cht[hash] = chl;
      }
    }
  
  return cht;
}

int ppm_lookupcolor( colorhash_table cht, pixel* colorP) {
  int hash;
  colorhist_list chl;
  
  hash = ppm_hashpixel( *colorP );
  for ( chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next )
    if ( PPM_EQUAL( chl->ch.color, *colorP ) )
      return chl->ch.value;
  
  return -1;
}

colorhash_table ppm_colorhisttocolorhash( colorhist_vector chv, int colors) {
  colorhash_table cht;
  int i, hash;
  pixel color;
  colorhist_list chl;
  
  cht = ppm_alloccolorhash( );
  
  for ( i = 0; i < colors; ++i ) {
    color = chv[i].color;
    hash = ppm_hashpixel( color );
    chl = (colorhist_list) my_malloc( sizeof(struct colorhist_list_item) );
    chl->ch.color = color;
    chl->ch.value = i;
    chl->next = cht[hash];
    cht[hash] = chl;
  }
  
  return cht;
}

colorhist_vector ppm_colorhashtocolorhist(colorhash_table cht, int maxcolors) {
  colorhist_vector chv;
  colorhist_list chl;
  int i, j;
  
  /* Now collate the hash table into a simple colorhist array. */
  chv = (colorhist_vector) my_malloc( maxcolors * sizeof(struct colorhist_item) );
  
  /* Loop through the hash table. */
  j = 0;
  for ( i = 0; i < HASH_SIZE; ++i )
    for ( chl = cht[i]; chl != (colorhist_list) 0; chl = chl->next ) {
      /* Add the new entry. */
      chv[j] = chl->ch;
      ++j;
    }
  
  /* All done. */
  return chv;
}

colorhist_vector ppm_computecolorhist( pixel** pixels, int cols, int rows, 
				      int maxcolors, int* colorsP) {
  colorhash_table cht = NULL;
  colorhist_vector chv;
  
  cht = ppm_computecolorhash( pixels, cols, rows, maxcolors, colorsP );
  if ( cht == (colorhash_table) 0 )
    return (colorhist_vector) 0;
  chv = ppm_colorhashtocolorhist( cht, maxcolors );
  ppm_freecolorhash( cht );
  return chv;
}

char *ppm_writeppminit( char *stream, int cols, int rows, pixval maxval,
		       int forceplain ) {
  char temp[80];
  
  if ( maxval <= 255 && ! forceplain )
    sprintf(temp, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, RPPM_MAGIC2,
	    cols, rows, maxval );
  else
    sprintf(temp, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, PPM_MAGIC2,
	    cols, rows, maxval );

  write_index = strlen(temp);
  stream = (char*)my_malloc(sizeof(char)*(write_index+1));
  strcpy(stream, temp);
  return stream;
}

char *pm_allocrow(int cols, int size) {
  register char* itrow;
  
  itrow = (char*) my_malloc( cols * size );
  return itrow;
}

void putus( unsigned short n, char *stream) {
  if ( n >= 10 )
    putus( n / 10, stream );
  stream[write_index++] = n % 10 + '0';
}

void ppm_writeppmrowraw( char *stream, pixel* pixelrow, int cols,
			pixval maxval) {
  register int col;
  register pixel* pP;
  register pixval val;
  
  for ( col = 0, pP = pixelrow; col < cols; ++col, ++pP ) {
    val = PPM_GETR( *pP );
    stream[write_index++] = val;
    val = PPM_GETG( *pP );
    stream[write_index++] = val;
    val = PPM_GETB( *pP );
    stream[write_index++] = val;
  }
}

void ppm_writeppmrowplain( char *stream, pixel* pixelrow, int cols,
			  pixval maxval) {
  register int col, charcount;
  register pixel* pP;
  register pixval val;
  
  charcount = 0;
  for ( col = 0, pP = pixelrow; col < cols; ++col, ++pP ) {
    if ( charcount >= 65 ) {
      stream[write_index++] = '\n';
      charcount = 0;
    }
    else if ( charcount > 0 ) {
      stream[write_index++] = ' ';
      stream[write_index++] = ' ';
      charcount += 2;
    }
    val = PPM_GETR( *pP );
    putus( val, stream );
    stream[write_index++] = ' ';
    val = PPM_GETG( *pP );
    putus( val, stream );
    stream[write_index++] = ' ';
    val = PPM_GETB( *pP );
    putus( val, stream );
    charcount += 11;
  }
  if ( charcount > 0 )
    stream[write_index++] = '\n';
}

void ppm_writeppmrow( char *stream, pixel* pixelrow, int cols, pixval maxval, 
		     int forceplain )
{
  if ( maxval <= 255 && ! forceplain )
    ppm_writeppmrowraw( stream, pixelrow, cols, maxval );
  else
    ppm_writeppmrowplain( stream, pixelrow, cols, maxval );
}


long rgbnorm( long rgb, long lmaxval, int n, unsigned char* colorname) {
  switch ( n ) {
  case 1:
    if ( lmaxval != 15 )
      rgb = rgb * lmaxval / 15;
    break;
  case 2:
    if ( lmaxval != 255 )
      rgb = rgb * lmaxval / 255;
    break;
  case 3:
    if ( lmaxval != 4095 )
      rgb = rgb * lmaxval / 4095;
    break;
  case 4:
    if ( lmaxval != 65535L )
      rgb = rgb * lmaxval / 65535L;
    break;
  }
  return rgb;
}

pixel ppm_parsecolor( unsigned char* colorname, pixval maxval ) {
  int hexit[256], i;
  pixel p;
  long lmaxval, r, g, b;
  
  for ( i = 0; i < 256; ++i )
    hexit[i] = 1234567890;
  hexit['0'] = 0;
  hexit['1'] = 1;
  hexit['2'] = 2;
  hexit['3'] = 3;
  hexit['4'] = 4;
  hexit['5'] = 5;
  hexit['6'] = 6;
  hexit['7'] = 7;
  hexit['8'] = 8;
  hexit['9'] = 9;
  hexit['a'] = hexit['A'] = 10;
  hexit['b'] = hexit['B'] = 11;
  hexit['c'] = hexit['C'] = 12;
  hexit['d'] = hexit['D'] = 13;
  hexit['e'] = hexit['E'] = 14;
  hexit['f'] = hexit['F'] = 15;
  
  lmaxval = maxval;
  if ( strncmp( (char *)colorname, "rgb:", 4 ) == 0 ) {
    /* It's a new-X11-style hexadecimal rgb specifier. */
    unsigned char* cp;
    
    cp = colorname + 4;
    r = g = b = 0;
    for ( i = 0; *cp != '/'; ++i, ++cp )
      r = r * 16 + hexit[*cp];
    r = rgbnorm( r, lmaxval, i, colorname );
    for ( i = 0, ++cp; *cp != '/'; ++i, ++cp )
      g = g * 16 + hexit[*cp];
    g = rgbnorm( g, lmaxval, i, colorname );
    for ( i = 0, ++cp; *cp != '\0'; ++i, ++cp )
      b = b * 16 + hexit[*cp];
    b = rgbnorm( b, lmaxval, i, colorname );
  }
  else if ( strncmp( (char *)colorname, "rgbi:", 5 ) == 0 ) {
    /* It's a new-X11-style decimal/float rgb specifier. */
    float fr, fg, fb;
    
    sscanf((char *)colorname,"rgbi:%f/%f/%f",&fr,&fg,&fb);
    r = fr * lmaxval;
    g = fg * lmaxval;
    b = fb * lmaxval;
  }
  else if ( colorname[0] == '#' ) {
    /* It's an old-X11-style hexadecimal rgb specifier. */
    switch ( strlen( (char *)colorname ) ) {
    case 4:
      r = hexit[colorname[1]];
      g = hexit[colorname[2]];
      b = hexit[colorname[3]];
      r = rgbnorm( r, lmaxval, 1, colorname );
      g = rgbnorm( g, lmaxval, 1, colorname );
      b = rgbnorm( b, lmaxval, 1, colorname );
      break;

    case 7:
      r = ( hexit[colorname[1]] << 4 ) + hexit[colorname[2]];
      g = ( hexit[colorname[3]] << 4 ) + hexit[colorname[4]];
      b = ( hexit[colorname[5]] << 4 ) + hexit[colorname[6]];
      r = rgbnorm( r, lmaxval, 2, colorname );
      g = rgbnorm( g, lmaxval, 2, colorname );
      b = rgbnorm( b, lmaxval, 2, colorname );
      break;
      
    case 10:
      r = ( hexit[colorname[1]] << 8 ) + ( hexit[colorname[2]] << 4 ) +
	hexit[colorname[3]];
      g = ( hexit[colorname[4]] << 8 ) + ( hexit[colorname[5]] << 4 ) +
	hexit[colorname[6]];
      b = ( hexit[colorname[7]] << 8 ) + ( hexit[colorname[8]] << 4 ) +
	hexit[colorname[9]];
      r = rgbnorm( r, lmaxval, 3, colorname );
      g = rgbnorm( g, lmaxval, 3, colorname );
      b = rgbnorm( b, lmaxval, 3, colorname );
      break;
      
    case 13:
      r = ( hexit[colorname[1]] << 12 ) + ( hexit[colorname[2]] << 8 ) +
	( hexit[colorname[3]] << 4 ) + hexit[colorname[4]];
      g = ( hexit[colorname[5]] << 12 ) + ( hexit[colorname[6]] << 8 ) +
	( hexit[colorname[7]] << 4 ) + hexit[colorname[8]];
      b = ( hexit[colorname[9]] << 12 ) + ( hexit[colorname[10]] << 8 ) +
	( hexit[colorname[11]] << 4 ) + hexit[colorname[12]];
      r = rgbnorm( r, lmaxval, 4, colorname );
      g = rgbnorm( g, lmaxval, 4, colorname );
      b = rgbnorm( b, lmaxval, 4, colorname );
      break;

    default:
      printf("Unsupported color specification: %s\n",colorname);
      r = g = b = 0;
      break;
    }
  }
  else if ( ( colorname[0] >= '0' && colorname[0] <= '9' ) ||
	   colorname[0] == '.' ) {
    /* It's an old-style decimal/float rgb specifier. */
    float fr, fg, fb;
    
    sscanf((char *)colorname,"%f , %f , %f",&fr,&fg,&fb);
    r = fr * lmaxval;
    g = fg * lmaxval;
    b = fb * lmaxval;
  }
  else {
    printf("XpmToGif: color name lookup not implemented yet.\n");
    r = b = g = 0;
  }
  PPM_ASSIGN( p, r, g, b );
  return p;
}

char **pm_allocarray(int cols, int rows, int size) {
  char** its;
  int i;

  its = (char**) my_malloc( rows * sizeof(char*) );
  its[0] = (char*) my_malloc( rows * cols * size );
  for ( i = 1; i < rows; ++i )
    its[i] = &(its[0][i * cols * size]);
  return its;
}

void ppm_readppminitrest(char *stream, int* colsP, int* rowsP, 
			 pixval* maxvalP) {
  int maxval;
  
  /* Read size. */
  *colsP = ppm_getint( stream );
  *rowsP = ppm_getint( stream );
  
  /* Read maxval. */
  maxval = ppm_getint( stream );
  *maxvalP = maxval;
}

int ppm_readmagicnumber(char *stream) {
  int ich1, ich2;
  
  ich1 = stream[read_index++];
  ich2 = stream[read_index++];
  return ich1 * 256 + ich2;
}

int ppm_getint(char *stream) {
  register char ch;
  register int i;
  
  do {
    ch = stream[read_index++];
  } while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );
  
  i = 0;
  do {
    i = i * 10 + ch - '0';
    ch = stream[read_index++];
  } while ( ch >= '0' && ch <= '9' );
  
  return i;
}

void ppm_readppmrow( char *stream, pixel* pixelrow, int cols, pixval maxval,
		    int format ) {
  register int col;
  register pixel* pP;
  register pixval r, g, b;
  
  for ( col = 0, pP = pixelrow; col < cols; ++col, ++pP ) {
    r = stream[read_index++];
    g = stream[read_index++];
    b = stream[read_index++];
    PPM_ASSIGN( *pP, r, g, b );
  }
}

pixel **ppm_readppm(char *stream, int* colsP, int* rowsP, pixval* maxvalP) {
  pixel** pixels;
  int row;
  int format = 0;

  ppm_readppminitrest( stream, colsP, rowsP, maxvalP);
  
  pixels = ppm_allocarray( *colsP, *rowsP );
  
  for ( row = 0; row < *rowsP; ++row )
    ppm_readppmrow( stream, pixels[row], *colsP, *maxvalP, format );
  
  return pixels;
}



/*******************************************************************/
/*******************************************************************/

void XpmToGif(char **xpmData, char *outFile) {
  pixel *pixrow = NULL, *colors = NULL;
  register pixel *pP;
  int rows, cols, ncolors, chars_per_pixel, row;
  register int col;
  register char *ptr;
  char *stream=NULL;
  int length;
  int size=1;
  
  read_index=3;
  write_index=0;
  ppm_pbmmaxval = 1;
  maxbits = BITS;
  maxmaxcode = (code_int)1 << BITS;
  hsize = HSIZE;
  free_ent = 0;
  clear_flg = 0;
  in_count = 1;
  out_count = 0;
  cur_accum = 0;
  cur_bits = 0;
  getaline_index = 0;
  pixels = NULL;
  cht = NULL;

  ReadXPMFile(xpmData, &cols, &rows, &ncolors, &chars_per_pixel, &colors);
  
  stream = ppm_writeppminit(stream, cols, rows, (pixval) PPM_MAXMAXVAL, 0);
  length = strlen(stream);

  pixrow = ppm_allocrow(cols);
  
  for (row = 0, ptr = xpmData[row+ncolors+1]; row < rows; ++row,
       ptr = xpmData[row+ncolors+1]) {
    for (col = 0, pP = pixrow; col < cols; ++col, ++pP, ++ptr)
      *pP = colors[(int)*ptr];
    stream = (char *)my_realloc(stream, size*cols*3*sizeof(char)+length);
    ppm_writeppmrow(stream, pixrow, cols, (pixval) PPM_MAXMAXVAL, 0);
    size++;
  }

  ToGif(stream, outFile);

  free(pixrow);
  free(stream);
  free(pixels[0]);
  free(pixels);
  free(colors);
}

void ReadXPMFile(char **stream, int *widthP, int *heightP, int *ncolorsP,
		 int *chars_per_pixelP, pixel **colorsP) {
  char line[MAX_LINE], str1[MAX_LINE], str2[MAX_LINE];
  char *t1;
  char *t2;
  int format, v;
  register int i, j;
  int flag;
  
  unsigned int curkey, key, highkey;	/* current color key */
  unsigned int lastwaskey;		/* key read */
  char curbuf[BUFSIZ];		/* current buffer */
  
  *widthP = *heightP = *ncolorsP = *chars_per_pixelP = format = -1;
  flag = 0;				/* to avoid getting twice a line */
  
  /* Read the hints line */
  getaline(line, sizeof(line), stream);
  sscanf(line, "%d %d %d %d", widthP, heightP, ncolorsP, chars_per_pixelP);
  
  /* Allocate space for color table. */
  /* Up to two chars per pixel, we can use an indexed table. */
  v = 1;
  for (i = 0; i < *chars_per_pixelP; ++i)
    v *= 256;
  *colorsP = ppm_allocrow(v);
  
  /* Read the color table */
  for (i = 0; i < *ncolorsP; i++) {
    getaline(line, sizeof(line), stream);
    
    t1 = line;
    strncpy(str1, t1, *chars_per_pixelP);
    str1[*chars_per_pixelP] = '\0';
    t1++; t1++;
    
    v = 0;
    for (j = 0; j < *chars_per_pixelP; ++j)
      v = (v << 8) + str1[j];
    /*
     * read color keys and values 
     */
    curkey = 0; 
    highkey = 1;
    lastwaskey = 0;
    t2 = t1;
    while ( 1 ) {
      for (t1=t2 ;; t1++)
	if (*t1 != ' ' && *t1 != '	')
	  break;
      for (t2 = t1;; t2++)
	if (*t2 == ' ' || *t2 == '	' || *t2 == '\0')
	  break;
      if (t2 == t1) break;
      strncpy(str2, t1, t2 - t1);
      str2[t2 - t1] = '\0';
      
      if (!lastwaskey) {
	for (key = 1; key < NKEYS + 1; key++)
	  if (!strcmp(colorKeys[key - 1], str2))
	    break;
      } else 
	key = NKEYS + 1;
      if (key > NKEYS) {			/* append name */
	if (!lastwaskey) 
	  strcat(curbuf, " ");		/* append space */
	strcat(curbuf, str2);		/* append buf */
	lastwaskey = 0;
      }
      if (key <= NKEYS) { 			/* new key */
	if (curkey > highkey) {	/* flush string */
	  /* Index into table. */
	  (*colorsP)[v] = ppm_parsecolor((unsigned char*)curbuf, (pixval) PPM_MAXMAXVAL);
	  highkey = curkey;
	}
	curkey = key;			/* set new key  */
	curbuf[0] = '\0';		/* reset curbuf */
	lastwaskey = 1;
      }
      if (*t2 == 0) break;
    }
    if (curkey > highkey) {
      /* Index into table. */
      (*colorsP)[v] = ppm_parsecolor((unsigned char*)curbuf, (pixval) PPM_MAXMAXVAL);
      highkey = curkey;
    }
  }
}

void getaline( char *line, int size, char **stream) {
  static int length, i, j, k, l, max;

  if (getaline_index == 0) {
    sscanf(stream[0], "%d %d %d %d", &i, &j, &k, &l);
    max = j+k;
  }

  if (getaline_index <= max) {
    length = strlen(stream[getaline_index]);
    i = (size > length) ? length : size;
    strncpy(line, stream[getaline_index], i);
    line[i] = 0;

    getaline_index++;
  }
}


void ToGif(char* stream, char *outFile) {
  int rows, cols, colors, i, BitsPerPixel;
  pixval maxval;
  colorhist_vector chv = NULL;
  int Red[MAXCOLORS], Green[MAXCOLORS], Blue[MAXCOLORS];
  FILE *file;

  if((file = fopen(outFile, "wb"))==NULL) {
      printf("\nError: could not write GIF file, open failed.");
      return;
  }
  
  pixels = ppm_readppm( stream, &cols, &rows, &maxval );
  
  /* Figure out the colormap. */
  chv = ppm_computecolorhist( pixels, cols, rows, MAXCOLORS, &colors );
  
  /* Now turn the ppm colormap into the appropriate GIF colormap. */
  for ( i = 0; i < colors; ++i ) {
    Red[i] = PPM_GETR( chv[i].color );
    Green[i] = PPM_GETG( chv[i].color );
    Blue[i] = PPM_GETB( chv[i].color );
  }
  BitsPerPixel = colorstobpp( colors );
  
  /* And make a hash table for fast lookup. */
  cht = ppm_colorhisttocolorhash( chv, colors );
  ppm_freecolorhist( chv );
  
  /* All set, let's do it. */
  GIFEncode(file, cols, rows, 0, 0, BitsPerPixel, Red, Green, Blue, 
	    GetPixel );

  ppm_freecolorhash( cht ); 
  fclose(file);
}

static int colorstobpp( int colors) {
  int bpp;
  
  if ( colors <= 2 )
    bpp = 1;
  else if ( colors <= 4 )
    bpp = 2;
  else if ( colors <= 8 )
    bpp = 3;
  else if ( colors <= 16 )
    bpp = 4;
  else if ( colors <= 32 )
    bpp = 5;
  else if ( colors <= 64 )
    bpp = 6;
  else if ( colors <= 128 )
    bpp = 7;
  else if ( colors <= 256 )
    bpp = 8;
  
  return bpp;
}

static int GetPixel( int x, int y) {
  int color;
  
  color = ppm_lookupcolor( cht, &pixels[y][x] );
  return color;
}


/*****************************************************************************
 *
 * GIFENCODE.C    - GIF Image compression interface
 *
 * GIFEncode( FName, GHeight, GWidth, GInterlace, Background,
 *            BitsPerPixel, Red, Green, Blue, GetPixel )
 *
 *****************************************************************************/

/*
 * Bump the 'curx' and 'cury' to point to the next pixel
 */
static void BumpPixel() {
  /*
   * Bump the current X position
   */
  ++curx;
  
  /*
   * If we are at the end of a scan line, set curx back to the beginning
   * If we are interlaced, bump the cury to the appropriate spot,
   * otherwise, just increment it.
   */
  if( curx == Width ) {
    curx = 0;
    
    if( !Interlace )
      ++cury;
    else {
      switch( Pass ) {
	
      case 0:
	cury += 8;
	if( cury >= Height ) {
	  ++Pass;
	  cury = 4;
	}
	break;
	
      case 1:
	cury += 8;
	if( cury >= Height ) {
	  ++Pass;
	  cury = 2;
	}
	break;
	
      case 2:
	cury += 4;
	if( cury >= Height ) {
	  ++Pass;
	  cury = 1;
	}
	break;
	
      case 3:
	cury += 2;
	break;
      }
    }
  }
}

/*
 * Return the next pixel from the image
 */
static int GIFNextPixel( ifunptr getpixel) {
  int r=0;
  
  if( CountDown == 0 )
    return EOF;
  
  --CountDown;
  
  r = ( * getpixel )( curx, cury );
  
  BumpPixel();
  
  return r;
}

/* public */

static void GIFEncode(FILE* fp, int GWidth, int GHeight, int GInterlace, 
		      int Background, int BitsPerPixel, int Red[], int Green[],
		      int Blue[], ifunptr GetPixel) {
  int B;
  int RWidth, RHeight;
  int LeftOfs, TopOfs;
  int Resolution;
  int ColorMapSize;
  int InitCodeSize;
  int i;
  
  Interlace = GInterlace;
  
  ColorMapSize = 1 << BitsPerPixel;
  
  RWidth = Width = GWidth;
  RHeight = Height = GHeight;
  LeftOfs = TopOfs = 0;
  
  Resolution = BitsPerPixel;
  
  /*
   * Calculate number of bits we are expecting
   */
  CountDown = (long)Width * (long)Height;
  
  /*
   * Indicate which pass we are on (if interlace)
   */
  Pass = 0;
  
  /*
   * The initial code size
   */
  if( BitsPerPixel <= 1 )
    InitCodeSize = 2;
  else
    InitCodeSize = BitsPerPixel;
  
  /*
   * Set up the current x and y position
   */
  curx = cury = 0;
  
  /*
   * Write the Magic header
   */
  fwrite( "GIF87a", 1, 6, fp );
  
  /*
   * Write out the screen width and height
   */
  Putword( RWidth, fp );
  Putword( RHeight, fp );
  
  /*
   * Indicate that there is a global colour map
   */
  B = 0x80;       /* Yes, there is a color map */
  
  /*
   * OR in the resolution
   */
  B |= (Resolution - 1) << 5;
  
  /*
   * OR in the Bits per Pixel
   */
  B |= (BitsPerPixel - 1);
  
  /*
   * Write it out
   */
  fputc( B, fp );
  
  /*
   * Write out the Background colour
   */
  fputc( Background, fp );
  
  /*
   * Byte of 0's (future expansion)
   */
  fputc( 0, fp );
  
  /*
   * Write out the Global Colour Map
   */
  for( i=0; i<ColorMapSize; ++i ) {
    fputc( Red[i], fp );
    fputc( Green[i], fp );
    fputc( Blue[i], fp );
  }
  
  /*
   * Write an Image separator
   */
  fputc( ',', fp );
  
  /*
   * Write the Image header
   */
  
  Putword( LeftOfs, fp );
  Putword( TopOfs, fp );
  Putword( Width, fp );
  Putword( Height, fp );
  
  /*
   * Write out whether or not the image is interlaced
   */
  if( Interlace )
    fputc( 0x40, fp );
  else
    fputc( 0x00, fp );
  
  /*
   * Write out the initial code size
   */
  fputc( InitCodeSize, fp );
  
  /*
   * Go and actually compress the data
   */
  compress( InitCodeSize+1, fp, GetPixel );
  
  /*
   * Write out a Zero-length packet (to end the series)
   */
  fputc( 0, fp );
  
  /*
   * Write the GIF file terminator
   */
  fputc( ';', fp );
  
  /*
   * And let the calling routine close the file
   */
  /* fclose( fp ); */
}

/*
 * Write out a word to the GIF file
 */
static void Putword( int w, FILE* fp) {
  fputc( w & 0xff, fp );
  fputc( (w / 256) & 0xff, fp );
}


/***************************************************************************
 *
 *  GIFCOMPR.C       - GIF Image compression routines
 *
 *  Lempel-Ziv compression based on 'compress'.  GIF modifications by
 *  David Rowley (mgardi@watdcsu.waterloo.edu)
 *
 ***************************************************************************/

/*
 * General DEFINEs
 */

/*
 *
 * GIF Image compression - modified 'compress'
 *
 * Based on: compress.c - File compression ala IEEE Computer, June 1984.
 *
 * By Authors:  Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *              Jim McKie               (decvax!mcvax!jim)
 *              Steve Davies            (decvax!vax135!petsd!peora!srd)
 *              Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *              James A. Woods          (decvax!ihnp4!ames!jaw)
 *              Joe Orost               (decvax!vax135!petsd!joe)
 *
 */


/*
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */


/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */

static void compress( int init_bits, FILE* outfile, ifunptr ReadValue) {
  register long fcode;
  register code_int i /* = 0 */;
  register int c;
  register code_int ent;
  register code_int disp;
  register code_int hsize_reg;
  register int hshift;
  
  /*
   * Set up the globals:  g_init_bits - initial number of bits
   *                      g_outfile   - pointer to output file
   */
  g_init_bits = init_bits;
  g_outfile = outfile;
  
  /*
   * Set up the necessary values
   */
  offset = 0;
  out_count = 0;
  clear_flg = 0;
  in_count = 1;
  maxcode = MAXCODE(n_bits = g_init_bits);
  
  ClearCode = (1 << (init_bits - 1));
  EOFCode = ClearCode + 1;
  free_ent = ClearCode + 2;
  
  char_init();
  
  ent = GIFNextPixel( ReadValue );
  
  hshift = 0;
  for ( fcode = (long) hsize;  fcode < 65536L; fcode *= 2L )
    ++hshift;
  hshift = 8 - hshift;                /* set hash code range bound */
  
  hsize_reg = hsize;
  cl_hash( (count_int) hsize_reg);            /* clear hash table */
  
  output( (code_int)ClearCode );
  
#ifdef SIGNED_COMPARE_SLOW
  while ( (c = GIFNextPixel( ReadValue )) != (unsigned) EOF ) {
#else /*SIGNED_COMPARE_SLOW*/
  while ( (c = GIFNextPixel( ReadValue )) != EOF ) {	/* } */
#endif /*SIGNED_COMPARE_SLOW*/

    ++in_count;
    
    fcode = (long) (((long) c << maxbits) + ent);
    i = (((code_int)c << hshift) ^ ent);    /* xor hashing */
    
    if ( HashTabOf (i) == fcode ) {
      ent = CodeTabOf (i);
      continue;
    } else if ( (long)HashTabOf (i) < 0 )      /* empty slot */
      goto nomatch;
    disp = hsize_reg - i;           /* secondary hash (after G. Knott) */
    if ( i == 0 )
      disp = 1;
  probe:
    if ( (i -= disp) < 0 )
      i += hsize_reg;
    
    if ( HashTabOf (i) == fcode ) {
      ent = CodeTabOf (i);
      continue;
    }
    if ( (long)HashTabOf (i) > 0 )
      goto probe;
  nomatch:
    output ( (code_int) ent );
    ++out_count;
    ent = c;
#ifdef SIGNED_COMPARE_SLOW
    if ( (unsigned) free_ent < (unsigned) maxmaxcode) {
#else /*SIGNED_COMPARE_SLOW*/
    if ( free_ent < maxmaxcode ) {	/* } */
#endif /*SIGNED_COMPARE_SLOW*/
      CodeTabOf (i) = free_ent++; /* code -> hashtable */
      HashTabOf (i) = fcode;
    } else
      cl_block();
  }
  /*
   * Put out the final code.
   */
  output( (code_int)ent );
  ++out_count;
  output( (code_int) EOFCode );
}

/*****************************************************************
 * TAG( output )
 *
 * Output the given code.
 * Inputs:
 *      code:   A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *              that n_bits =< (long)wordsize - 1.
 * Outputs:
 *      Outputs code to the file.
 * Assumptions:
 *      Chars are 8 bits long.
 * Algorithm:
 *      Maintain a BITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */

static void output(code_int  code) {
  cur_accum &= masks[ cur_bits ];
  
  if( cur_bits > 0 )
    cur_accum |= ((long)code << cur_bits);
  else
    cur_accum = code;
  
  cur_bits += n_bits;

  while( cur_bits >= 8 ) {
    char_out( (unsigned int)(cur_accum & 0xff) );
    cur_accum >>= 8;
    cur_bits -= 8;
  }
  
  /*
   * If the next entry is going to be too big for the code size,
   * then increase it, if possible.
   */
  if ( free_ent > maxcode || clear_flg ) {
    
    if( clear_flg ) {
      
      maxcode = MAXCODE (n_bits = g_init_bits);
      clear_flg = 0;
      
    } else {
      
      ++n_bits;
      if ( n_bits == maxbits )
	maxcode = maxmaxcode;
      else
	maxcode = MAXCODE(n_bits);
    }
  }
  
  if( code == EOFCode ) {
    /*
     * At EOF, write the rest of the buffer.
     */
    while( cur_bits > 0 ) {
      char_out( (unsigned int)(cur_accum & 0xff) );
      cur_accum >>= 8;
      cur_bits -= 8;
    }
    
    flush_char();
    
    fflush( g_outfile );
    
  }
}

/*
 * Clear out the hash table
 */
static void cl_block () {             /* table clear for block compress */
  cl_hash ( (count_int) hsize );
  free_ent = ClearCode + 2;
  clear_flg = 1;
  
  output( (code_int)ClearCode );
}

static void cl_hash(register count_int hsize) {         /* reset code table */
  register count_int *htab_p = htab+hsize;
  
  register long i;
  register long m1 = -1;
  
  i = hsize - 16;
  do {                            /* might use Sys V memset(3) here */
    *(htab_p-16) = m1;
    *(htab_p-15) = m1;
    *(htab_p-14) = m1;
    *(htab_p-13) = m1;
    *(htab_p-12) = m1;
    *(htab_p-11) = m1;
    *(htab_p-10) = m1;
    *(htab_p-9) = m1;
    *(htab_p-8) = m1;
    *(htab_p-7) = m1;
    *(htab_p-6) = m1;
    *(htab_p-5) = m1;
    *(htab_p-4) = m1;
    *(htab_p-3) = m1;
    *(htab_p-2) = m1;
    *(htab_p-1) = m1;
    htab_p -= 16;
  } while ((i -= 16) >= 0);
  
  for ( i += 16; i > 0; --i )
    *--htab_p = m1;
}

/******************************************************************************
 *
 * GIF Specific routines
 *
 ******************************************************************************/

/*
 * Number of characters so far in this 'packet'
 */

/*
 * Set up the 'byte output' routine
 */
static void char_init() {
  a_count = 0;
}

/*
 * Define the storage for the packet accumulator
 */

/*
 * Add a character to the end of the current packet, and if it is 254
 * characters, flush the packet to disk.
 */
static void char_out( int c ) {
  accum[ a_count++ ] = c;
  if( a_count >= 254 )
    flush_char();
}

/*
 * Flush the packet to disk, and reset the accumulator
 */
static void flush_char() {
  if( a_count > 0 ) {
    fputc( a_count, g_outfile );
    fwrite( accum, 1, a_count, g_outfile );
    a_count = 0;
  }
}

/* The End */

#endif
