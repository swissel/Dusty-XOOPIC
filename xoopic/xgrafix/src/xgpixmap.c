#include "xgrafixint.h"

void XG_Setup_Pixmap(WindowType theWindow)
{
  XRectangle      theClientArea;
  Tk_Window       tkwin = theWindow->tkwin;

  theClientArea.x = 0;
  theClientArea.y = 0;
  theClientArea.width = Tk_Width(tkwin);
  theClientArea.height = Tk_Height(tkwin);
  XSetClipRectangles(theDisplay, theWindow->xwingc, 0, 0, &theClientArea,
		     1, Unsorted);

  if (theWindow->pixmap_buffer) {
    XFreePixmap(theDisplay,theWindow->pixmap_buffer);
  }

  theWindow->pixmap_buffer = XCreatePixmap(theDisplay, Tk_WindowId(tkwin),
					   Tk_Width(tkwin),
					   Tk_Height(tkwin),
					   Tk_Depth(tkwin));

  if (!(theWindow->pixmap_buffer)) {
    puts("XG_Setup_Pixmap couldn't XCreatePixmap");
    exit(-1);
  }

  XSetForeground(theDisplay, theWindow->xwingc, GrayShade[3]);
  XFillRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 0, 0, Tk_Width(tkwin), Tk_Height(tkwin));

  XSetClipMask(theDisplay, theWindow->xwingc, None);
}

void XG_Copy_Pixmap(WindowType theWindow)
{
  XRectangle      theClientArea;
  Tk_Window       tkwin = theWindow->tkwin;

  theClientArea.x = 0;
  theClientArea.y = 0;
  theClientArea.width = Tk_Width(tkwin);
  theClientArea.height = Tk_Height(tkwin);

  XCopyArea(theDisplay, theWindow->pixmap_buffer, Tk_WindowId(tkwin), 
	    theWindow->xwingc, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 
	    0, 0);
  XFlush(theDisplay);
}






