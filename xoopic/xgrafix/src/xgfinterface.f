*     FORTRAN Interface Code for Xgrafix
*****************************************************************
*     Subroutine XGINIT initializes the XGrafix package.
****************************************************************
      SUBROUTINE xginit(thetime)
*     Global Symbols:
        COMMON /labels/ labindex, plottype(100),
     +                  state(100), title(100),
     +                  xlabel(100), ylabel(100)
        INTEGER labindex
        CHARACTER*(80) plottype, state, title, xlabel, ylabel

*     Input Arguments:
        DOUBLE PRECISION thetime
        CHARACTER*(80) arg

*     Externals:

        EXTERNAL strlennb
        INTEGER strlennb
*        EXTERNAL iargc, getarg
*        INTEGER iargc


*     Local Symbols:
        INTEGER lenname, indx, k
        CHARACTER*(80) name1p, name2p, name3p, name4p, name5p,
     +                 name6p, name7p, name8p, name9p, name10p,
     +                 name11p,name12p,name13p,name14p,name15p,
     +                 name16p,names(16)
*     Initialize the label index counter.
      labindex = 0
      indx = iargc()

*     Convert the input FORTRAN strings to C strings.
      DO 10 k=0,indx
         CALL getarg (k, arg)
         lenname = STRLENNB(arg)
         names(k+1) =  arg(1:lenname) // char(0)
 10   CONTINUE

      DO 20 k = indx+1, 16
	names(k+1) = ' ' // char(0)
 20   CONTINUE

      name1p = names(1)
      name2p = names(2)
      name3p = names(3)
      name4p = names(4)
      name5p = names(5)
      name6p = names(6)
      name7p = names(7)
      name8p = names(8)
      name9p = names(9)
      name10p = names(10)
      name11p = names(11)
      name12p = names(12)
      name13p = names(13)
      name14p = names(14)
      name15p = names(15)
      name16p = names(16)

      CALL initxg(indx + 1,name1p,name2p,name3p,name4p,name5p,
     +  name6p,name7p,name8p,name9p,name10p,name11p,name12p,
     +  name13p,name14p,name15p,name16p,thetime)

      RETURN
      END
*
****************************************************************
*     Subroutine XGSTART refreshes the screen and
*     checks for events.
****************************************************************
      SUBROUTINE xgstart

      CALL startxg

      RETURN
      END


****************************************************************
*     Subroutine XGSETVEC sets up the information for the
*     initialization of a 2-D vector plot under XGrafix.
****************************************************************
      SUBROUTINE xgsetvec(pt,xl,yl,tl,ws,wulx,wuly,xscale,yscale,
     +                   xauto,yauto,xmin,xmax,ymin,ymax)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto
      DOUBLE PRECISION xmax, xmin, xscale, ymax, ymin, yscale
      CHARACTER*(*) pt, tl, ws, xl, yl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      ylabel(labindex)   = yl // char(0)
      
      CALL setvecxg(plottype(labindex),xlabel(labindex),
     +     ylabel(labindex),title(labindex),state(labindex),
     +     wulx,wuly,xscale,yscale,xauto,yauto,
     +     xmin,xmax,ymin,ymax)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGSETVECFLAG does the same thing as XGSETVEC
*     but also provides a flag that Xgrafix sets on or off
*     depending on whether the plot window is open or closed.
****************************************************************
      SUBROUTINE xgsetvecflag(pt,xl,yl,tl,ws,wulx,wuly,xscale,yscale,
     +                   xauto,yauto,xmin,xmax,ymin,ymax,openflag)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto, openflag
      DOUBLE PRECISION xmax, xmin, xscale, ymax, ymin, yscale
      CHARACTER*(*) pt, tl, ws, xl, yl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      ylabel(labindex)   = yl // char(0)
      
      CALL setvecxgflag(plottype(labindex),xlabel(labindex),
     +     ylabel(labindex),title(labindex),state(labindex),
     +     wulx,wuly,xscale,yscale,xauto,yauto,
     +     xmin,xmax,ymin,ymax,openflag)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGSET2D sets up the information for the
*     initialization of a 2-D (X-Y) line or scatter plot
*     under XGrafix.
****************************************************************
      SUBROUTINE xgset2d(pt,xl,tl,ws,wulx,wuly,xscale,yscale,
     +                   xauto,yauto,xmin,xmax,ymin,ymax)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto
      DOUBLE PRECISION xmax, xmin, xscale, ymax, ymin, yscale
      CHARACTER*(*) pt, tl, ws, xl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      
      CALL set2dxg(plottype(labindex),xlabel(labindex),
     +     title(labindex),state(labindex),
     +     wulx,wuly,xscale,yscale,xauto,yauto,
     +     xmin,xmax,ymin,ymax)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGSET2DFLAG does the same as XGSET2D but
*     also provides a flag that XGrafix turns on or off 
*     depending on whether the plot window is open or closed.
****************************************************************
      SUBROUTINE xgset2dflag(pt,xl,tl,ws,wulx,wuly,xscale,yscale,
     +                   xauto,yauto,xmin,xmax,ymin,ymax,openflag)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto,openflag
      DOUBLE PRECISION xmax, xmin, xscale, ymax, ymin, yscale
      CHARACTER*(*) pt, tl, ws, xl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      
      CALL set2dxgflag(plottype(labindex),xlabel(labindex),
     +     title(labindex),state(labindex),
     +     wulx,wuly,xscale,yscale,xauto,yauto,
     +     xmin,xmax,ymin,ymax,openflag)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGSET2DC sets up the information for the
*     initialization of a 2-D contour plot under XGrafix.
****************************************************************
      SUBROUTINE xgset2dc(pt,xl,yl,tl,ws,wulx,wuly,xscale,
     +     yscale,zscale,xauto,yauto,zauto,
     +     xmin,xmax,ymin,ymax,zmin,zmax)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto, zauto
      DOUBLE PRECISION xmax, xmin, xscale, ymax, ymin, yscale
      DOUBLE PRECISION zmax, zmin, zscale
      CHARACTER*(*) pt, tl, ws, xl, yl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      ylabel(labindex)   = yl // char(0)
      
      CALL set2dcxg(plottype(labindex),xlabel(labindex),
     +     ylabel(labindex),title(labindex),
     +     state(labindex),wulx,wuly,xscale,
     +     yscale,zscale,xauto,yauto,zauto,
     +     xmin,xmax,ymin,ymax,zmin,zmax)
      
      RETURN
      END
*     
*     
****************************************************************
*     Subroutine XGSET2DCFLAG does the same as XGSET2DC but
*     also has a flag which XGrafix turns on or off depending
*     on whether the plot window is open or closed.
****************************************************************
      SUBROUTINE xgset2dcflag(pt,xl,yl,tl,ws,wulx,wuly,xscale,
     +     yscale,zscale,xauto,yauto,zauto,
     +     xmin,xmax,ymin,ymax,zmin,zmax,openflag)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto, zauto,openflag
      DOUBLE PRECISION xmax, xmin, xscale, ymax, ymin, yscale
      DOUBLE PRECISION zmax, zmin, zscale
      CHARACTER*(*) pt, tl, ws, xl, yl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      ylabel(labindex)   = yl // char(0)
      
      CALL set2dcxgflag(plottype(labindex),xlabel(labindex),
     +     ylabel(labindex),title(labindex),
     +     state(labindex),wulx,wuly,xscale,
     +     yscale,zscale,xauto,yauto,zauto,
     +     xmin,xmax,ymin,ymax,zmin,zmax,openflag)
      
      RETURN
      END

*     
****************************************************************
*     Subroutine XGSET3D sets up the information for the
*     initialization of a 3-D surface or scatter plot
*     under XGrafix.
****************************************************************
      SUBROUTINE xgset3d(pt,xl,yl,tl,theta,phi,ws,wulx,wuly,
     +     xscale,yscale,zscale,xauto,yauto,zauto,
     +     xmin,xmax,ymin,ymax,zmin,zmax)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto, zauto
      DOUBLE PRECISION phi, theta, xmax, xmin, xscale
      DOUBLE PRECISION ymax, ymin, yscale, zmax, zmin, zscale
      CHARACTER*(*) pt, tl, ws, xl, yl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      ylabel(labindex)   = yl // char(0)
      
      CALL set3dxg(plottype(labindex),xlabel(labindex),
     +     ylabel(labindex),title(labindex),
     +     theta,phi,state(labindex),wulx,wuly,
     +     xscale,yscale,zscale,xauto,yauto,
     +     zauto,xmin,xmax,ymin,ymax,zmin,zmax)
      
      RETURN
      END
*     
*     
****************************************************************
*     Subroutine XGSET3DFLAG does the same as XGSET3D but 
*     also has a flag that XGrafix turns on or off depending
*     on whether the plot window is open or closed.
****************************************************************
      SUBROUTINE xgset3dflag(pt,xl,yl,tl,theta,phi,ws,wulx,wuly,
     +     xscale,yscale,zscale,xauto,yauto,zauto,
     +     xmin,xmax,ymin,ymax,zmin,zmax,openflag)
*     Global Symbols:
      COMMON /labels/ labindex, plottype(100),
     +     state(100), title(100),
     +     xlabel(100), ylabel(100)
      INTEGER labindex
      CHARACTER*(80) plottype, state, title, xlabel, ylabel
      
*     Input Arguments:
      INTEGER wulx, wuly, xauto, yauto, zauto,openflag
      DOUBLE PRECISION phi, theta, xmax, xmin, xscale
      DOUBLE PRECISION ymax, ymin, yscale, zmax, zmin, zscale
      CHARACTER*(*) pt, tl, ws, xl, yl
      
*     Convert the input FORTRAN strings to C strings.
      labindex           = labindex + 1
      IF (labindex .GT. 100) THEN
         WRITE(6,1000)
 1000    FORMAT("Number of initialized windows exceeds ",
     +        "established allocation!")
         STOP
      ENDIF
      plottype(labindex) = pt // char(0)
      state(labindex)    = ws // char(0)
      title(labindex)    = tl // char(0)
      xlabel(labindex)   = xl // char(0)
      ylabel(labindex)   = yl // char(0)
      
      CALL set3dxgflag(plottype(labindex),xlabel(labindex),
     +     ylabel(labindex),title(labindex),
     +     theta,phi,state(labindex),wulx,wuly,
     +     xscale,yscale,zscale,xauto,yauto,
     +     zauto,xmin,xmax,ymin,ymax,zmin,zmax,openflag)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGCURVE adds an X-Y line plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgcurve(xplot,yplot,nx,color)
*     Input Arguments:
      INTEGER color, nx
      DOUBLE PRECISION xplot(*), yplot(*)
      
      CALL curvexg(xplot,yplot,nx,color)

      RETURN
      END
*
****************************************************************
*     Subroutine XGSCAT2D adds an X-Y scatter plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgscat2d(xplot,yplot,nx,color)      
*     Input Arguments:
      INTEGER color, nx
      DOUBLE PRECISION xplot(*), yplot(*)
      
      CALL scat2dxg(xplot,yplot,nx,color)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGSCAT3D adds a 3-D scatter plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgscat3d(xplot,yplot,zplot,nx,color)
*     Input Arguments:
      INTEGER color, nx
      DOUBLE PRECISION zplot(*), xplot(*), yplot(*)
      
      CALL scat3dxg(xplot,yplot,zplot,nx,color)

      RETURN
      END
*
****************************************************************
*     Subroutine XGVECTOR adds a 2-D vector plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgvector(xplot,yplot,zplot,wplot,nx,ny,
     +     ndim1,ndim2,color)
*     Input Arguments:
      INTEGER color, nx, ny, ndim1, ndim2
      DOUBLE PRECISION zplot(ndim1,*), wplot(ndim1,*)
      DOUBLE PRECISION xplot(*), yplot(*)
      
      CALL vectorxg(xplot,yplot,zplot,wplot,nx,ny,ndim1,ndim2,
     +     color)
      
      RETURN
      END
*
****************************************************************
*     Subroutine XGSURF adds a 3-D surface plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgsurf(xplot,yplot,zplot,nx,ny,ndim1,ndim2,
     +     color)
*     Input Arguments:
      INTEGER color, nx, ny, ndim1, ndim2
      DOUBLE PRECISION zplot(ndim1,*), xplot(*), yplot(*)
      
      CALL surfxg(xplot,yplot,zplot,nx,ny,ndim1,ndim2,color)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGIRSURF adds a 3-D surface plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgirsurf(xplot,yplot,zplot,nx,ny,ndim1,ndim2,
     +     color)
*     Input Arguments:
      INTEGER color, nx, ny, ndim1, ndim2
      DOUBLE PRECISION zplot(ndim1,ndim2), xplot(ndim1,ndim2),
     +     yplot(ndim1,ndim2)
      
      CALL irsurfxg(xplot,yplot,zplot,nx,ny,ndim1,ndim2,color)
      
      RETURN
      END
*     
****************************************************************
*     Subroutine XGCONT adds a 2-D contour plot to the
*     previously initialized window under XGrafix.
****************************************************************
      SUBROUTINE xgcont(xplot,yplot,zplot,nx,ny,ndim1,ndim2,
     +     color)
*     Input Arguments:
      INTEGER color, nx, ny, ndim1, ndim2
      DOUBLE PRECISION zplot(ndim1,*), xplot(*), yplot(*)
      
      CALL contxg(xplot,yplot,zplot,nx,ny,ndim1,ndim2,color)
      
      RETURN
      END
*
     
****************************************************************
*     Subroutine XGSTRUCTURE ARRAY allows user to draw
*     structures.
****************************************************************
      SUBROUTINE xgstructurearray(numpoints,fillflag,linecolor,
     +                            fillcolor,structure)
*     Input Arguments:
      INTEGER numpoints,fillflag,linecolor,fillcolor
      DOUBLE PRECISION structure(*)
 
      CALL structurexgarray(numpoints,fillflag,linecolor,fillcolor,
     +     structure)
      
      RETURN
      END
*     

****************************************************************
*     Subroutine SETUPNEWINT,SETUPNEWREAL,SETUPNEWDOUBLE and
*     SETUPNEWCHAR allows the dynamic changing of integers,reals,
*     double precision variables, and characters respectively.
****************************************************************
      SUBROUTINE setupnewint(newint, name)
      
*     Input Arguments:
      INTEGER newint
      CHARACTER*(*) name

*     Local Symbols
      INTEGER lenname
      CHARACTER*(80) namep

*     Externals:
      EXTERNAL strlennb
      INTEGER strlennb

*     Convert the input FORTRAN string to a C string.
      lenname = STRLENNB(name)
      namep =  name(1:lenname) // char(0)

      call newintsetup(newint,namep)
      RETURN
      END

****************************************************************
      SUBROUTINE setupnewreal(newreal, name)
      
*     Input Arguments:
      REAL newreal
      CHARACTER*(*) name

*     Local Symbols
      INTEGER lenname
      CHARACTER*(80) namep

*     Externals:
      EXTERNAL strlennb
      INTEGER strlennb

*     Convert the input FORTRAN string to a C string.
      lenname = STRLENNB(name)
      namep =  name(1:lenname) // char(0)
      call newrealsetup(newreal,namep)
      RETURN
      END
*****************************************************************      
      SUBROUTINE setupnewdouble(newdouble, name)
      
*     Input Arguments:
      DOUBLE PRECISION newdouble
      CHARACTER*(*) name

*     Local Symbols
      INTEGER lenname
      CHARACTER*(80) namep

*     Externals:
      EXTERNAL strlennb
      INTEGER strlennb

*     Convert the input FORTRAN string to a C string.
      lenname = STRLENNB(name)
      namep =  name(1:lenname) // char(0)
      call newdoublesetup(newdouble,namep)
      RETURN
      END

*****************************************************************
      SUBROUTINE setupnewchar(newchar, name)
      
*     Input Arguments:
      CHARACTER newchar
      CHARACTER*(*) name

*     Local Symbols
      INTEGER lenname
      CHARACTER*(80) namep

*     Externals:
      EXTERNAL strlennb
      INTEGER strlennb

*     Convert the input FORTRAN string to a C string.
      lenname = STRLENNB(name)
      namep =  name(1:lenname) // char(0)
      call newcharsetup(newchar,namep)

      RETURN 
      END
*

****************************************************************
*     Function STRLENNB returns either the index of last
*     non-blank character in the string STR or 1.
****************************************************************
      FUNCTION strlennb(str)
*     Input Arguments:      
      INTEGER strlennb
      CHARACTER*(*) str
      
      strlennb = LEN(str)
      
 10   CONTINUE
      
      IF (str(strlennb:strlennb) .NE. " ") THEN
         RETURN
      ELSE
         strlennb = strlennb - 1
      ENDIF
      
      IF (strlennb .GT. 1) GOTO 10
      
      RETURN
      END
