
/*  Copyright 1997 Peter Mardahl

    Program for playing movies generated from XGrafix 2.40.
    */
#include "list.h"
#include "xgmovie.h"


SCALAR thetimestep;
double theTime;
SCALAR maxTime;  //  the maximum time found in any of the graphs read
SCALAR minTime;  //  the minimum time found in any of the graphs read

int MAX_LIN = 12000;

int TIME_SKIP = 2;
int X_SKIP = 2;
int Y_SKIP = 2;

extern "C" {
   void XGMainLoop();
   void Dump(const char *);
   void Quit();
}

void ReadStructures(List<StructureData> &Structs,char *filename)
{
   List<StructureData> theStructs;
   char buf[512];
   int n;
   FILE *fp;
   sprintf(buf,"%s.str",filename);
   if((fp = fopen(buf,"r"))==NULL) {
      sprintf(buf,"%s_1.str",filename);
      if((fp=fopen(buf,"r"))==NULL) return;
   }
   // read in the structures
   while(XGRead(&n,sizeof(int),1,fp,"int"),n!=-1) {
      StructureData *s = new StructureData;
      s->n = n;
      XGRead(&(s->fillFlag),sizeof(int),1,fp,"int");
      XGRead(&(s->lineColor),sizeof(int),1,fp,"int");
      XGRead(&(s->fillColor),sizeof(int),1,fp,"int");
      s->points = (SCALAR *)malloc (2*n*sizeof(SCALAR));
      XGRead(s->points,sizeof(SCALAR),2*n,fp,SCALAR_CHAR);
      Structs.add(s);
   }
   //  Structs=theStructs;  // reverse the list
   fclose(fp);
}

int getCount(char *filename) {
   FILE *fp;
   int l;
   char buf[512];

   sprintf(buf,"%s.num",filename);
   if((fp = fopen(buf,"r"))==NULL) {
      fprintf(stderr,"Error, cannot open file %s",buf);
      return 0;
   }
   fscanf(fp,"%d",&l);
   fclose(fp);
   return l;
}

LabelType ReadLabel(FILE *fp) {
   int type;
   char buf[100];
   char *labels;
   int l;
   LabelType label = (LabelType)malloc(sizeof(struct label));
   SCALAR scale,min,max;
   //Read 1st string
   XGRead(&l,sizeof(int),1,fp,"int");  //size
   labels = (char *)malloc((l+1) * sizeof(char));
   if(l>0) {
      XGRead(labels,sizeof(char),l+1,fp,"char");
      label->X_Label = labels;
   }

   //Read 2st string
   XGRead(&l,sizeof(int),1,fp,"int");  //size
   labels = (char *)malloc((l+1) * sizeof(char));
   if(l>0) {
      XGRead(labels,sizeof(char),l+1,fp,"char");
      label->Y_Label = labels;
   }
   //Read 3st string
   XGRead(&l,sizeof(int),1,fp,"int");  //size
   labels = (char *)malloc((l+1) * sizeof(char));
   if(l>0) {
      XGRead(labels,sizeof(char),l+1,fp,"char");
      label->Z_Label = labels;
   }
   // read the info on the X axis
   XGRead(&scale,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->X_Scale = 1;//scale;
   XGRead(&min,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->X_Min=min;
   XGRead(&max,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->X_Max=max;
   XGRead(&(label->X_Auto_Rescale),sizeof(int),1,fp,"int");


   // read the info on the Y axis
   XGRead(&scale,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->Y_Scale = 1;//scale;
   XGRead(&min,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->Y_Min=min;
   XGRead(&max,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->Y_Max=max;
   XGRead(&(label->Y_Auto_Rescale),sizeof(int),1,fp,"int");


   // read the info on the Z axis
   XGRead(&scale,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->Z_Scale = 1;//scale;
   XGRead(&min,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->Z_Min=min;
   XGRead(&max,sizeof(SCALAR),1,fp,SCALAR_CHAR);
   label->Z_Max=max;
   XGRead(&(label->Z_Auto_Rescale),sizeof(int),1,fp,"int");

   return label;
}

int openFile(FILE **fp,char * filename,int index) {
   char buf[100];
   int type;
   if(index == 0 || index == 1) {
      sprintf(buf,"%s.bxg",filename);
      if((*fp = fopen(buf,"r"))==NULL) {
         sprintf(buf,"%s_1.bxg",filename);
         if((*fp = fopen(buf,"r"))==NULL) {
            fprintf(stderr,"Error, cannot find any file of the form %s\n",filename);
            exit(1);
         }
      }
      XGRead(&type,sizeof(int),1,*fp,"int");
      return type;
   }
   
   sprintf(buf,"%s_%d.bxg",filename,index);
   if((*fp = fopen(buf,"r"))==NULL) {
      *fp = 0;
      return -1;
   }
   XGRead(&type,sizeof(int),1,*fp,"int");
   return type;
}


plot::plot(char *_filename,int _xloc,int _yloc) {
   FILE *fp;
   filename = strdup(_filename);
   xloc = _xloc;
   yloc = _yloc;
   openFile(&fp,filename,0);  // 
   
   label = ReadLabel(fp);
   ReadStructures(structures,filename);
   fclose(fp);
}

One_D_plot::One_D_plot(char *filename,int xloc,int yloc):plot(filename,xloc,yloc) {
   FILE *fp;
   int i,j,count,type;
   LabelType locallabel;
   List<List<One_D_plot_data> > tmp;
   current = new ListIter< List<One_D_plot_data> >(graphdata);
   openFile(&fp,filename,0);
   count = getCount(filename);
   rewind(fp);
   //  for(i=1;openFile(&fp,filename,i)!=-1;i++) {
   for(i=0;i<count;i++) {
      SCALAR the_time;
      XGRead(&type,sizeof(int),1,fp,"int");
      locallabel=ReadLabel(fp);
      XGRead(&(the_time),sizeof(SCALAR),1,fp,SCALAR_CHAR);
      maxTime = max(maxTime,the_time);
      minTime = min(minTime,the_time);
      List<One_D_plot_data> *in_data = new List<One_D_plot_data>;
      do {
         One_D_plot_data *dat= new One_D_plot_data();
         int Skip = 1;
         XGRead(&(dat->n),sizeof(int),1,fp,"int");
         XGRead(&(dat->color),sizeof(int),1,fp,"int");
         if(dat->n == -1) {  // the exit condition 
            tmp.add(in_data);  //add this list of data to the graph list
            delete dat;  // not needed
            break;  // go to the next file
         }
         if(i==1)  { // have to build a current_data list
            One_D_plot_data *dat2 =new One_D_plot_data();
            dat2->time = the_time;
            dat2->x = (SCALAR *)calloc(MAX_LIN, sizeof(SCALAR));
            dat2->y = (SCALAR *)calloc(MAX_LIN, sizeof(SCALAR));
            dat2->color = dat->color;
            current_data.add(dat2);
         }
         dat->time = the_time;
         
         if(dat->n > MAX_LIN) {
            // we need to contract the dataset to fit into our
            // memory buffer.  We also must read the entire dataset.
            // what we do is stick dat->n/MAX_LIN items directly
            // into the same array location, the last one wins.
            static int warnflag = 0;
            Skip = dat->n / MAX_LIN+1;
            
            if(Skip < 2) Skip = 2;
            //		  printf("\n dat->n %d, Skip %d ",dat->n, Skip);
            if(!warnflag) { warnflag = 1;
                            printf("Warning, some datasets are too large.  Threading them. (%d)\n",Skip);
                         }
         }

         dat->x = (SCALAR *)calloc(max(1,(dat->n+Skip)/Skip) , sizeof(SCALAR));
         dat->y = (SCALAR *)calloc(max(1,(dat->n+Skip)/Skip) , sizeof(SCALAR));
         
         for(j=0;j<dat->n;j++) {
            XGRead(dat->x +j/Skip,sizeof(SCALAR),1,fp,SCALAR_CHAR);
            XGRead(dat->y +j/Skip,sizeof(SCALAR),1,fp,SCALAR_CHAR);
         }
         dat->n = dat->n / Skip;
         in_data->add(dat);
      } while (!feof(fp));  // actually a dummy, reading should stop earlier
   }
   fclose(fp);
   // reverse the list
   graphdata = tmp;

}

void One_D_plot::updatePlot(double time) {
   if(time == minTime) current->restart();

   List<One_D_plot_data> *thisdata = current->current();
   while(!current->Done()&& current->current()->head->data->time <= time) 
      {
         thisdata= current->current();
         (*current)++;
      }
   if(thisdata) {
      ListIter<One_D_plot_data> walk(*thisdata);
      ListIter<One_D_plot_data> walk2(current_data);
      for(walk.restart(),walk2.restart();!walk.Done();walk++,walk2++) {
         One_D_plot_data * thisgraph = walk.current();
         One_D_plot_data * destgraph = walk2.current();
         destgraph->n = thisgraph->n;
         memcpy(destgraph->x,thisgraph->x,destgraph->n * sizeof(SCALAR));
         memcpy(destgraph->y,thisgraph->y,destgraph->n * sizeof(SCALAR));
      }
   }
}




ScatterPlot::ScatterPlot(char *filename,int xloc,int yloc):One_D_plot(filename,xloc,yloc) {
   XGSet2D("linlin",label->X_Label,label->Y_Label,"open",xloc,yloc,label->X_Scale,
           label->Y_Scale,label->X_Auto_Rescale,label->Y_Auto_Rescale,
           label->X_Min,label->X_Max,label->Y_Min,label->Y_Max);
   ListIter<One_D_plot_data> walk(current_data);
   for(walk.restart();!walk.Done();walk++)
      XGScat2D(walk.current()->x,walk.current()->y,&(walk.current()->n),walk.current()->color);
   ListIter<StructureData> walk2(structures);
   for(walk2.restart();!walk2.Done();walk2++)
      XGStructureArray(walk2.current()->n,(STRUCT_FILL) walk2.current()->fillFlag,
                       walk2.current()->lineColor,
                       walk2.current()->fillColor,walk2.current()->points);
}

LinePlot::LinePlot(char *filename,int xloc, int yloc):One_D_plot(filename,xloc,yloc) {
   XGSet2D("linlin",label->X_Label,label->Y_Label,"open",xloc,yloc,label->X_Scale,
           label->Y_Scale,label->X_Auto_Rescale,label->Y_Auto_Rescale,
           label->X_Min,label->X_Max,label->Y_Min,label->Y_Max);
   ListIter<One_D_plot_data> walk(current_data);
   for(walk.restart();!walk.Done();walk++)
      XGCurve(walk.current()->x,walk.current()->y,&(walk.current()->n),walk.current()->color);
   ListIter<StructureData> walk2(structures);
   for(walk2.restart();!walk2.Done();walk2++)
      XGStructureArray(walk2.current()->n,(STRUCT_FILL) walk2.current()->fillFlag,
                       walk2.current()->lineColor,
                       walk2.current()->fillColor,walk2.current()->points);
}

SurfacePlot::SurfacePlot(char *filename,int xloc,int yloc) : plot(filename,xloc,yloc) {
   FILE *fp;
   SCALAR the_time;
   List<SurfacePlotData> tmp;
   int count,type;
   LabelType llabel;



   x=0;y=0;
   //  for(int i=1;openFile(&fp,filename,i)!=-1;i++) {
   openFile(&fp,filename,0);
   count = getCount(filename);
   rewind(fp);
   for(int c=0;c<count;c++) {
      //	 SurfacePlotData * thisgraph = new SurfacePlotData;
      SCALAR PlotEnergy = 0;
      XGRead(&type,sizeof(int),1,fp,"int");
      llabel=ReadLabel(fp);
      XGRead(&the_time,sizeof(SCALAR),1,fp,SCALAR_CHAR);
      maxTime = max(maxTime,the_time);
      minTime = min(minTime,the_time);
      XGRead(&n,sizeof(int),1,fp,"int");
      XGRead(&m,sizeof(int),1,fp,"int");
      if(!x) {
         x = (SCALAR *)malloc(m * sizeof(SCALAR));
         y = (SCALAR *)malloc(n * sizeof(SCALAR));
         z = (SCALAR **) malloc(m * sizeof(SCALAR *));
         for(int j=0;j<m;j++) {
            z[j] = (SCALAR *)calloc(n, sizeof(SCALAR));
            //	  memset(z[j],0,n * sizeof(SCALAR));
         }

      }
      // read the x and y axes
      XGRead(x,sizeof(SCALAR),m,fp,SCALAR_CHAR);
      XGRead(y,sizeof(SCALAR),n,fp,SCALAR_CHAR);

      
      //	 thisgraph->z = (SCALAR **) malloc(m * sizeof(SCALAR *));
      for(int j=0;j<m;j++) {
         //		  z[j] = (SCALAR *)malloc (n * sizeof(SCALAR));
         XGRead(z[j],sizeof(SCALAR),n,fp,SCALAR_CHAR);
      }
      //	 tmp.add(thisgraph);

      // Do some data reduction
      { 
         int ii,jj;
         for(ii=0;ii<m;ii++)
            for(jj=0;jj<n;jj++) {
               PlotEnergy += z[ii][jj] * z[ii][jj];
            }
         // Now normalize it.
         PlotEnergy *= (label->X_Max - label->X_Min) * (label->Y_Max - label->Y_Min);
         PlotEnergy *= 1.0/(m * n);
         printf("t= %g Energy= %g\n",the_time,PlotEnergy);  
      }

      //OK, now we work on the data we just read:  do we write it? 
      if(c % TIME_SKIP == 0){

         // open and check the file 
         FILE *fw = fopen("out.bxg","a"); if(!fp) { printf("Output file not writeable."),exit(1); };
         int i,j;
         SCALAR xscale = 1;
         SCALAR yscale = 1;
         SCALAR zscale = 1;
         SCALAR xw,yw,zw;
         int nl,ml;
         nl = n / Y_SKIP;
         ml = m / X_SKIP;
         
         XGWrite(&type,sizeof(int),1,fw,"int");
         XGWriteLabel(llabel,fw);
         XGWrite(&(the_time),sizeof(SCALAR),1,fw,SCALAR_CHAR);

         XGWrite(&nl,sizeof(int),1,fw,"int");
         XGWrite(&ml,sizeof(int),1,fw,"int");

         for (i=0;i<X_SKIP * ml;i+=X_SKIP) {
            xw = xscale * x[i];
            XGWrite(&xw,sizeof(SCALAR),1,fw,SCALAR_CHAR);
         }
         for (i=0;i<nl * Y_SKIP;i+=Y_SKIP) {
            yw = xscale * y[i];
            XGWrite(&yw,sizeof(SCALAR),1,fw,SCALAR_CHAR);
         }
         for (i=0;i<ml * X_SKIP;i+=X_SKIP) {
            for (j=0;j<nl * Y_SKIP;j+=Y_SKIP) {
               zw = zscale* z[i][j];
               XGWrite(&zw,sizeof(SCALAR),1,fw,SCALAR_CHAR);
            }
         }
         fclose(fw);
      }
      
      
   }
   fclose(fp);

   //  graphdata = tmp;  //this will reverse tmp
   //  current = new ListIter<SurfacePlotData>(graphdata);
   //  XGSet3D("linlinlin",label->X_Label,label->Y_Label,label->Z_Label,45.0,225.0,
   //			 "open",xloc,yloc,label->X_Scale,label->Y_Scale,label->Z_Scale,
   //			 label->X_Auto_Rescale,label->Y_Auto_Rescale,label->Z_Auto_Rescale,
   //			 label->X_Min,label->X_Max,label->Y_Min,label->Y_Max,label->Z_Min,label->Z_Max);
   //  XGSurf(x,y,z,&m,&n,1);
   
}

void SurfacePlot::updatePlot(double time) {

   if(time == minTime) current->restart();

   SurfacePlotData *thisdata= current->current();
   while(!current->Done() &&current->current()->time <= time) 
      {
         thisdata= current->current();
         (*current)++;
      }
   if(thisdata)
      for(int j=0;j<m;j++)
         memcpy(z[j],thisdata->z[j],n * sizeof(SCALAR));
}



VectorPlot::VectorPlot(char *filename,int xloc,int yloc) : plot(filename,xloc,yloc) {
   FILE *fp;
   SCALAR the_time;
   List<VectorPlotData> tmp;
   int count,type;
   x=0;y=0;

   //  for(int i=1;openFile(&fp,filename,i)!=-1;i++) {
   openFile(&fp,filename,0);
   count = getCount(filename);
   rewind(fp);
   for(int i=0;i<count;i++) {
      VectorPlotData * thisgraph = new VectorPlotData;
      XGRead(&type,sizeof(int),1,fp,"int");
      ReadLabel(fp);
      XGRead(&(thisgraph->time),sizeof(SCALAR),1,fp,SCALAR_CHAR);
      maxTime = max(maxTime,thisgraph->time);
      minTime = min(minTime,thisgraph->time);
      XGRead(&n,sizeof(int),1,fp,"int");
      XGRead(&m,sizeof(int),1,fp,"int");
      if(!x) {
         x = (SCALAR *)malloc(m * sizeof(SCALAR));
         y = (SCALAR *)malloc(n * sizeof(SCALAR));
         z = (SCALAR **) malloc(m * sizeof(SCALAR *));
         w = (SCALAR **) malloc(m * sizeof(SCALAR *));
         for(int j=0;j<m;j++) {
            z[j] = (SCALAR *)calloc(n, sizeof(SCALAR));
            w[j] = (SCALAR *)calloc(n, sizeof(SCALAR));
         }

      }
      // read the x and y axes
      XGRead(x,sizeof(SCALAR),m,fp,SCALAR_CHAR);
      XGRead(y,sizeof(SCALAR),n,fp,SCALAR_CHAR);

      
      thisgraph->z = (SCALAR **) malloc(m * sizeof(SCALAR *));
      thisgraph->w = (SCALAR **) malloc(m * sizeof(SCALAR *));
      for(int j=0;j<m;j++) {
         thisgraph->w[j] = (SCALAR *)malloc (n * sizeof(SCALAR));
         thisgraph->z[j] = (SCALAR *)malloc (n * sizeof(SCALAR));
         XGRead(thisgraph->w[j],sizeof(SCALAR),n,fp,SCALAR_CHAR);
         XGRead(thisgraph->z[j],sizeof(SCALAR),n,fp,SCALAR_CHAR);
      }
      tmp.add(thisgraph);
   }
   fclose(fp);

   graphdata = tmp;  //this will reverse tmp
   current = new ListIter<VectorPlotData>(graphdata);
   XGSetVec("vecvec",label->X_Label,label->Y_Label,label->Z_Label,
            "open",xloc,yloc,label->X_Scale,label->Y_Scale,
            label->X_Auto_Rescale,label->Y_Auto_Rescale,
            label->X_Min,label->X_Max,label->Y_Min,label->Y_Max);
   XGVector(x,y,w,z,&m,&n,1);
   ListIter<StructureData> walk2(structures);
   for(walk2.restart();!walk2.Done();walk2++)
      XGStructureArray(walk2.current()->n,(STRUCT_FILL) walk2.current()->fillFlag,
                       walk2.current()->lineColor,
                       walk2.current()->fillColor,walk2.current()->points);
}

void VectorPlot::updatePlot(double time) {

   if(time == minTime) current->restart();

   VectorPlotData *thisdata= current->current();
   while(!current->Done() &&current->current()->time <= time) 
      {
         thisdata= current->current();
         (*current)++;
      }
   if(thisdata)
      for(int j=0;j<m;j++) {
         memcpy(z[j],thisdata->z[j],n * sizeof(SCALAR));
         memcpy(w[j],thisdata->w[j],n * sizeof(SCALAR));
      }
}

List<plot> *thePlots;

int main(int argc, char **argv) {
   FILE *inputfile;
   FILE *tmp;
   thePlots = new List<plot>;
   char line[512];
   char filename[512];
   char lastline[512];
   int xloc,yloc;
   XGInit(argc,argv,&theTime);
   if((inputfile=fopen(theInputFile,"r"))==NULL) {
      fprintf(stderr,"Cannot open inputfile.  Exiting.");
      exit(1);
   }
   theTime = 0;
   maxTime = 0;
   minTime = 1e9;
   fgets(line,511,inputfile);
#ifdef XG_SCALAR_DOUBLE
   sscanf(line,"%lf %d %d %d %d",&thetimestep,&MAX_LIN, &TIME_SKIP, &X_SKIP, &Y_SKIP);
#else
   sscanf(line,"%f %d %d %d %d",&thetimestep,&MAX_LIN, &TIME_SKIP, &X_SKIP, &Y_SKIP);
#endif
   if(MAX_LIN <= 0) MAX_LIN = 12000;
   
   while(!feof(inputfile)) {
      fgets(line,511,inputfile);
      sscanf(line,"%s %d %d",filename,&xloc,&yloc);
      //	 sscanf(line,"%d %d",&xloc,&yloc);
      xloc = max(xloc,1); xloc = min(700,xloc);
      yloc = max(yloc,1); yloc = min(700,yloc);
      if(!strcmp(filename,"END")) break;
      switch(openFile(&tmp, filename,0)) 
         {
          case LINE_PLOT:
             thePlots->add(new LinePlot(filename,xloc,yloc));
             break;
           case SCATTER_PLOT:
              thePlots->add(new ScatterPlot(filename,xloc,yloc));
             break;
           case SURFACE_PLOT:
              thePlots->add(new SurfacePlot(filename,xloc,yloc));
             break;
           case VECTOR_PLOT:
              thePlots->add(new VectorPlot(filename,xloc,yloc));
             break;
           default:
              fprintf(stderr,"Unsupported plot type in file %s\n",filename);
          }
      fclose(tmp);
   }
   theTime = minTime;

   ListIter<plot> walk(*thePlots);
   //  for(walk.restart();!walk.Done();walk++) {
   ////	 walk.current()->updatePlot(theTime);
   //  }
   printf("\nStart time: %g  End time: %g\n",minTime,maxTime);
   //  XGStart();
   return 0;
}

void XGMainLoop(){

   //  cause the graphs to cycle
   if(theTime > maxTime) theTime = minTime;

   ListIter<plot> walk(*thePlots);
   for(walk.restart();!walk.Done();walk++) {
      walk.current()->updatePlot(theTime);
   } 
   theTime +=thetimestep;

}

void Dump(const char *) {}
void Quit() {}
