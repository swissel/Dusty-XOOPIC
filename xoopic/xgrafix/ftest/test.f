*     Fortran Test Code for XGrafix.

      PROGRAM test

*     Global Symbols:
      COMMON /test1/ pi,size,nx,ny,nx_irr,ny_irr,
     +       ndim1,ndim2,nirdim1,nirdim2,xg,yg,zg,wg,vg,
     +       yg1,yg2,x_irr,y_irr,z_irr,dx,dy,
     +       openflag1,openflag2,openflag3,openflag4,
     +       testint,oldtestint,testfloat,oldtestfloat
      REAL pi
      INTEGER size
      INTEGER nx,ny,nx_irr,ny_irr
      INTEGER ndim1,ndim2,nirdim1,nirdim2
      DOUBLE PRECISION xg(31),yg(31),zg(31,31),wg(31,31),vg(31,31)
      DOUBLE PRECISION yg1(31),yg2(31)
      DOUBLE PRECISION x_irr(50,40),y_irr(50,40),z_irr(50,40)
      DOUBLE PRECISION dx, dy
      INTEGER openflag1,openflag2,openflag3,openflag4
      INTEGER testint,oldtestint
      REAL testfloat,oldtestfloat

      COMMON /test2/ t,testdouble,oldtestdouble
      DOUBLE PRECISION t,testdouble,oldtestdouble

      COMMON /test3/ testchar,oldtestchar
      CHARACTER testchar,oldtestchar
      
      testint = 100
      testfloat = 101.0
      testdouble = 1.02D+02
      testchar = 'a'      
      pi = ABS(ACOS(-1.0))
      size = 21

      t = 0.0
      CALL setup
      CALL initwindows
      CALL initvars
      CALL xgstart
      STOP
      END
****************************************************************

      SUBROUTINE setup

*     Global Symbols:
      COMMON /test1/ pi,size,nx,ny,nx_irr,ny_irr,
     +       ndim1,ndim2,nirdim1,nirdim2,xg,yg,zg,wg,vg,
     +       yg1,yg2,x_irr,y_irr,z_irr,dx,dy,
     +       openflag1,openflag2,openflag3,openflag4,
     +       testint,oldtestint,testfloat,oldtestfloat
      REAL pi
      INTEGER size
      INTEGER nx,ny,nx_irr,ny_irr
      INTEGER ndim1,ndim2,nirdim1,nirdim2
      DOUBLE PRECISION xg(31),yg(31),zg(31,31),wg(31,31),vg(31,31)
      DOUBLE PRECISION yg1(31),yg2(31)
      DOUBLE PRECISION x_irr(50,40),y_irr(50,40),z_irr(50,40)
      DOUBLE PRECISION dx, dy
      INTEGER openflag1,openflag2,openflag3,openflag4
      INTEGER testint,oldtestint
      REAL testfloat,oldtestfloat

      COMMON /test2/ t,testdouble,oldtestdouble
      DOUBLE PRECISION t,testdouble,oldtestdouble

      COMMON /test3/ testchar,oldtestchar
      CHARACTER testchar, oldtestchar
      
*     Local Symbols:
      INTEGER i, j
      DOUBLE PRECISION r, theta

      oldtestint = testint
      oldtestfloat = testfloat
      oldtestdouble = testdouble
      oldtestchar = testchar

      nx = size
      ny = size
      ndim1 = 31
      ndim2 = 31
      dx = 2*pi / FLOAT(nx)
      dy = pi / FLOAT(ny)

*     The 2-d plot stuff

      DO 10 i = 1,nx
         xg(i) = (i - 1)*dx
         yg1(i) = COS(2*(i - 1)*dy)
         yg2(i) = 1e3*EXP((1-i)/5.0)
 10   CONTINUE

*     The regular 3-d plot stuff
     
      DO 30 i = 1,nx
         DO 20 j = 1,ny
            yg(j) = (j - 1) * dy
            zg(i,j) = SIN((i-1)*dx)*COS((j-1)*dy)
            wg(i,j) = 0.25*COS((i-1)*dx)
            vg(i,j) = 1e3*EXP((1-i)*dx)*COS(0.5*pi*(j-1)*dy)
 20      CONTINUE
 30   CONTINUE

*     The irregular 3-d plot stuff
      nx_irr= 31
      ny_irr= 31
      nirdim1 = 50
      nirdim2 = 40
      
      DO 50 i = 1,nx_irr
         DO 40 j = 1,ny_irr
            r = 1.0*(i-1)/(nx_irr-1.0)
            theta = (j-1)*2*pi/(ny_irr-1)
            x_irr(j,i) = r*COS(theta)
            y_irr(j,i) = r*SIN(theta)
            z_irr(j,i) = COS(0.5*pi*r)*( 1 + .5*r*COS(3*theta))
 40      CONTINUE
 50   CONTINUE

      RETURN
      END

*****************************************************************************

      SUBROUTINE initwindows

*     Global Symbols:
      COMMON /test1/ pi,size,nx,ny,nx_irr,ny_irr,
     +       ndim1,ndim2,nirdim1,nirdim2,xg,yg,zg,wg,vg,
     +       yg1,yg2,x_irr,y_irr,z_irr,dx,dy,
     +       openflag1,openflag2,openflag3,openflag4,
     +       testint,oldtestint,testfloat,oldtestfloat
      REAL pi
      INTEGER size
      INTEGER nx,ny,nx_irr,ny_irr
      INTEGER ndim1,ndim2,nirdim1,nirdim2
      DOUBLE PRECISION xg(31),yg(31),zg(31,31),wg(31,31),vg(31,31)
      DOUBLE PRECISION yg1(31),yg2(31)
      DOUBLE PRECISION x_irr(50,40),y_irr(50,40),z_irr(50,40)
      DOUBLE PRECISION dx, dy
      INTEGER openflag1,openflag2,openflag3,openflag4
      INTEGER testint,oldtestint
      REAL testfloat,oldtestfloat

      COMMON /test2/ t,testdouble,oldtestdouble
      DOUBLE PRECISION t,testdouble,oldtestdouble
      
*     Local Symbols
      INTEGER Tru,Fals 
      DOUBLE PRECISION struct1(8),struct2(8),struct3(8),struct4(8)
      data struct1 /1.0, 0.0, 1.0, 0.5, 0.5, 0.5, 0.0, 0.0/,
     + struct2 /0.0, -0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5/,
     + struct3 /-1.0, 0.5, 1.5, 1.0, 0.0, 0.0, 1.0, 0.5/,
     + struct4 /1.0, 0.0, 1.0, 0.5, 0.5, 0.5, 0.0, 0.0/ 
      
      TRU = 1
      FALS = 0

      CALL xginit(t)

* Initialize an XGrafix window for an X-Y line curve plot.
      CALL xgset2d('linlin','X Axis','X-Y Line Plot','closed',
     +     150,450,1.0D0,1.0D0,Fals,Fals,xg(1),xg(nx),-1.0D0,1.0D0)
      CALL xgcurve(xg,yg1,nx,0)

* Initialize an XGrafix window for an X-Y line curve plot with structures.
      CALL xgset2dflag('linlin','X Axis','X-Y Line Plot & Structures',
     + 'closed',150,150,1.0D0,1.0D0,Fals,Fals,
     + xg(1),xg(nx),-1.0D0,1.0D0,openflag1)
      CALL xgstructurearray(4, 1, 0, 2, struct1)
      CALL xgstructurearray(4, 0, 1, 3, struct2)
      CALL xgcurve(xg,yg1,nx,5)

* Initialize an XGrafix window for an X-Y semi-log plot.
      CALL xgset2d('linlog','X Axis','X-Y Semi-Log Plot','closed',
     +     350,450,1.0D0,1.0D0,Fals,Tru,xg(1),xg(nx),1.0D0,1.0D0)
      CALL xgcurve(xg,yg2,nx,0)
      
* Initialize an XGrafix window for an X-Y scatter plot.
      CALL xgset2d('linlin','X Axis','X-Y Scatter Plot','closed',
     +     250,550,1.0D0,1.0D0,Fals,Fals,xg(1),xg(nx),-1.0D0,1.0D0)
      CALL xgscat2d(xg,yg1,nx,3)

* Initialize an XGrafix window for a 2-D vector plot with structures.
      CALL xgsetvecflag('vecvec','X Axis','Y Axis',
     +'2D Vector Plot & Structures','closed',600,175,1.0D0,1.0D0,
     + Fals,Fals,xg(1),xg(nx),yg(1),yg(ny),openflag2)
      CALL xgstructurearray(4, 0, 1, 2, struct3)
      CALL xgstructurearray(4, 1, 0, 3, struct4)
      CALL xgvector(xg,yg,zg,wg,nx,ny,ndim1,ndim2,3)

* Initialize an XGrafix window for a 3-D surface plot.
      CALL xgset3dflag('linlinlin','X Axis','Y Axis','3D Surface Plot',
     +     45.0D0,45.0D0,'open',10,15,1.0D0,1.0D0,1.0D0,Fals,Fals,Tru,
     +     xg(1),xg(nx),yg(1),yg(ny),1.0D0,1.0D0,openflag3)
      CALL xgsurf(xg,yg,vg,nx,ny,ndim1,ndim2,3)
      
* Initialize an XGrafix window for a 3-D irregular surface plot.
      CALL xgset3d('linlinlin','X Axis','Y Axis',
     +'3D Irregular Surface Plot',45.0D0,225.0D0,'closed',100,175,
     + 1.0D0,1.0D0,1.0D0,Fals,Fals,Fals,-1.0D0,1.0D0,-1.0D0,1.0D0,
     + 0.0D0,1.05D0)
      CALL xgirsurf(x_irr,y_irr,z_irr,nx_irr,ny_irr,nirdim1,nirdim2,3)
      
* Initialize an XGrafix window for a 2-D contour plot.
      CALL xgset2dcflag('linlinlin','X Axis','Y Axis',
     +'2D Contour Plot','closed',100,100,1.0D0,1.0D0,1.0D0,Fals,Fals,
     + Tru,xg(1),xg(nx),yg(1),yg(ny-1),0.0D0,0.0D0,openflag4)
      CALL xgcont(xg,yg,z_irr,nx,ny,nirdim1,nirdim2,3)

      RETURN
      END

**************************************************************************    

      SUBROUTINE initvars()

*     Global Symbols:
      COMMON /test1/ pi,size,nx,ny,nx_irr,ny_irr,
     +       ndim1,ndim2,nirdim1,nirdim2,xg,yg,zg,wg,vg,
     +       yg1,yg2,x_irr,y_irr,z_irr,dx,dy,
     +       openflag1,openflag2,openflag3,openflag4,
     +       testint,oldtestint,testfloat,oldtestfloat
      REAL pi
      INTEGER size
      INTEGER nx,ny,nx_irr,ny_irr
      INTEGER ndim1,ndim2,nirdim1,nirdim2
      DOUBLE PRECISION xg(31),yg(31),zg(31,31),wg(31,31),vg(31,31)
      DOUBLE PRECISION yg1(31),yg2(31)
      DOUBLE PRECISION x_irr(50,40),y_irr(50,40),z_irr(50,40)
      DOUBLE PRECISION dx, dy
      INTEGER openflag1,openflag2,openflag3,openflag4
      INTEGER testint,oldtestint
      REAL testfloat,oldtestfloat

      COMMON /test2/ t,testdouble,oldtestdouble
      DOUBLE PRECISION t,testdouble,oldtestdouble

      COMMON /test3/ testchar,oldtestchar
      CHARACTER testchar,oldtestchar
      
      CALL setupnewint(testint,'Test Int')
      CALL setupnewreal(testfloat,'Test Float')
      CALL setupnewdouble(testdouble,'Test Double')
      CALL setupnewchar(testchar,'Test Char')

      RETURN
      END

************************************************************************

*     Do your physics here.
      SUBROUTINE xgmainloop

*     Global Symbols:
      COMMON /test1/ pi,size,nx,ny,nx_irr,ny_irr,
     +       ndim1,ndim2,nirdim1,nirdim2,xg,yg,zg,wg,vg,
     +       yg1,yg2,x_irr,y_irr,z_irr,dx,dy,
     +       openflag1,openflag2,openflag3,openflag4,
     +       testint,oldtestint,testfloat,oldtestfloat
      REAL pi
      INTEGER size
      INTEGER nx,ny,nx_irr,ny_irr
      INTEGER ndim1,ndim2,nirdim1,nirdim2
      DOUBLE PRECISION xg(31),yg(31),zg(31,31),wg(31,31),vg(31,31)
      DOUBLE PRECISION yg1(31),yg2(31)
      DOUBLE PRECISION x_irr(50,40),y_irr(50,40),z_irr(50,40)
      DOUBLE PRECISION dx, dy
      INTEGER openflag1,openflag2,openflag3,openflag4
      INTEGER testint,oldtestint
      REAL testfloat,oldtestfloat


      COMMON /test2/ t,testdouble,oldtestdouble
      DOUBLE PRECISION t,testdouble,oldtestdouble

      COMMON /test3/ testchar,oldtestchar
      CHARACTER testchar,oldtestchar
      
*     Local Symbols:
      INTEGER i, j
      DOUBLE PRECISION r,theta

        t = t + 1.0/8.0

        DO 100 i = 1,nx
           yg1(i) = COS(2*(i-1)*dy)*COS(pi*0.1*t)
 100    CONTINUE

        DO 110 j = 1,ny
           DO 120 i = 1,nx
              zg(i,j) = zg(i,j)*COS(0.01*t*pi)/COS(0.01*(t - 1)*pi)
              vg(i,j)=  1e3*EXP((1-i)*dx)*COS(t*0.5*pi*(j-1)*dy)
 120       CONTINUE
 110    CONTINUE
        
        DO 130 i = 1,nx_irr
           DO 140 j = 1,ny_irr
              r = 1.0*(i-1)/(nx_irr-1.0)
              theta = (j-1)*2*pi/(ny_irr -1)
              z_irr(j,i)= COS(0.5*pi*r)*(1 +.5*r*COS(3*theta+pi*t))
 140       CONTINUE
 130    CONTINUE
      
      IF (oldtestint .ne. testint) THEN
         print*, 'New testint is = ', testint
         oldtestint = testint
      ENDIF
      IF (oldtestfloat .ne. testfloat) THEN
         print*, 'New testfloat is = ', testfloat
         oldtestfloat = testfloat
      END IF
      IF (oldtestdouble .ne. testdouble) THEN
         print*, 'New testdouble is = ', testdouble
         oldtestdouble = testdouble
      END IF
      IF (oldtestchar .ne. testchar) THEN
         print*, 'New testchar is = ', testchar
         oldtestchar = testchar
      END IF

      RETURN
      END







