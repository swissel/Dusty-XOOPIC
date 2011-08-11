#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT_FILE_NAME     6
#define MAX_CAPTION             2

char InputFileName[MAX_INPUT_FILE_NAME][31], Caption[MAX_CAPTION][61], OutputFileName[31];
FILE *InputFileHandler[MAX_INPUT_FILE_NAME], *OutputFileHandler;

int main()
{
  int i, l;
  char temp[81];

  /***************************************************************/
  /* Information stuff for the user.                             */

  printf("\n\nThis program will take 6 file names, which are\n");
  printf("PostScript outputs of XGRAFIX, and one (and/or two)\n");
  printf("caption line(s) and will create a PostScript file\n");
  printf("with the following VERTICAL configuration:\n\n");
  printf(" -----caption1----- \n");
  printf(" -----caption2----- \n");
  printf("  (file1)  (file2)  \n");
  printf("  (file3)  (file4)  \n");
  printf("  (file5)  (file6)  \n\n");

  /***************************************************************/
  /* Get the captions and file names from the user.              */

  printf("Please enter the caption(s) not exceeding 60\n");
  printf("characters per line.\n\n");
  for(i=0; i< MAX_CAPTION; i++)
  {
    printf("Caption %d->", i+1); 
    fgets(Caption[i],61,stdin);
    l = strlen(Caption[i])-1;
    if (Caption[i][l] == '\n') Caption[i][l] = '\0';
    if(!strcmp(Caption[i], "exit") || !strcmp(Caption[i], "quit")) exit(1);
  }

  printf("\nPlease enter the PS file names not exceeding 30\n");
  printf("characters per file name.\n\n");
  for(i=0; i< MAX_INPUT_FILE_NAME; i++)
  {
    printf("Input File %d->", i+1); 
    fgets(InputFileName[i],31,stdin);
    l = strlen(InputFileName[i])-1;
    if (InputFileName[i][l] == '\n') InputFileName[i][l] = '\0';
    if(!strcmp(InputFileName[i], "exit") || !strcmp(InputFileName[i], "quit")) exit(1);
    
    if(strcmp(InputFileName[i], "skip") &&
       !(InputFileHandler[i] = fopen(InputFileName[i], "r")))
    {
      printf("\nFile does not exist.  Try again!\n"); i--;
    }
  }
  
  printf("\nPlease enter the output file name not exceeding 30\n");
  printf("characters.\n\n");
  
  printf("Output File ->");
  fgets(OutputFileName,31,stdin);
  l = strlen(OutputFileName)-1;
  if (OutputFileName[l] == '\n') OutputFileName[l] = '\0';
  if(!strcmp(OutputFileName, "exit") || !strcmp(OutputFileName, "quit")) exit(1);
  OutputFileHandler = fopen(OutputFileName, "w");

  /**************************************************************/
  /* Read the input files and write into the output file        */

  /*****  The first file with the header  *******/

  while(strcmp(fgets(temp, 80, InputFileHandler[0]), "612 0 translate\n"))
    fputs(temp, OutputFileHandler);

  fgets(temp, 80, InputFileHandler[0]);
  fputs(".38194 .38194 scale\n", OutputFileHandler);
  fputs("70 1150 translate\n\n", OutputFileHandler);
  
  while(strcmp(fgets(temp, 80, InputFileHandler[0]), "showpage\n"))
  {
    if(!strcmp(temp, "/Times-Bold findfont 12 scalefont setfont\n"))
      fputs("/Times-Bold findfont 18 scalefont setfont\n", OutputFileHandler);
    else if(!strcmp(temp, "/Times-BoldItalic findfont 12 scalefont setfont\n"))
      fputs("/Times-BoldItalic findfont 18 scalefont setfont\n", OutputFileHandler);
    else if(!strcmp(temp, "/Times-Bold findfont 14 scalefont setfont\n"))
      fputs("/Times-Bold findfont 20 scalefont setfont\n", OutputFileHandler);
    else
      fputs(temp, OutputFileHandler);
  }
  fclose(InputFileHandler[0]);
  
  /*****  The rest of the files without the header  *******/

  for(i=1; i< MAX_INPUT_FILE_NAME; i++)
  {
    if(i%2) fputs("680 0 translate\n\n", OutputFileHandler);
    else    fputs("-680 -500 translate\n\n", OutputFileHandler);
    
    if(strcmp(InputFileName[i], "skip"))
    {
      while(strcmp(fgets(temp, 80, InputFileHandler[i]), "612 0 translate\n"));
      fgets(temp, 80, InputFileHandler[i]);
      
      while(strcmp(fgets(temp, 80, InputFileHandler[i]), "showpage\n"))
      {
	if(!strcmp(temp, "/Times-Bold findfont 12 scalefont setfont\n"))
	  fputs("/Times-Bold findfont 18 scalefont setfont\n", OutputFileHandler);
	else if(!strcmp(temp, "/Times-BoldItalic findfont 12 scalefont setfont\n"))
	  fputs("/Times-BoldItalic findfont 18 scalefont setfont\n", OutputFileHandler);
	else if(!strcmp(temp, "/Times-Bold findfont 14 scalefont setfont\n"))
	  fputs("/Times-Bold findfont 20 scalefont setfont\n", OutputFileHandler);
	else
	  fputs(temp, OutputFileHandler);
      }
      fclose(InputFileHandler[i]);
    }
  }
  
  /*****  The caption(s) and finally showpage  *******/
  
  fputs("-750 -150 translate\n", OutputFileHandler);
  fputs("2.618 2.618 scale\n", OutputFileHandler);
  fputs("/Times-Bold findfont 16 scalefont setfont\n", OutputFileHandler);

  for(i=0; i< MAX_CAPTION; i++)
  {
    fprintf(OutputFileHandler, "306 %d moveto\n", 720-20*i);
    fprintf(OutputFileHandler, "(%s) prtstrfrommidup\n", Caption[i]);
  }
  fputs("showpage\n", OutputFileHandler);
  fclose(OutputFileHandler);
  return 0;
}
