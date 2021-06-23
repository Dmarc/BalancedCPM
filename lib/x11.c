#include <stdio.h>
#include <stdlib.h>

#define VERBOSE 0

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "cash.h"

int cellinterface=BLACK;

static XImage *planeimage;
static char *imagedata;
static Display *display;
static char *server = NULL;
static Window window;
static GC windowGC;
static XEvent event;
static XSizeHints hint;
static XWindowAttributes attributes;
static XSetWindowAttributes setattributes;
static int screen,depth,class;
static Visual *visual;
static XVisualInfo visual_info, *visual_list;
static unsigned long white, black, foreground, background;
static XColor *colors;
static Colormap new_colormap;

static int pseudoCol8 = 1;
extern int nrow, ncol, scale, graphics;
extern int userCol[256][4];

int Beep(int i)
{
  if (graphics) XBell(display,i);
}

int Mouse()
{
  int mouse = 0;
  if (graphics) {
    while (XEventsQueued(display,QueuedAfterFlush) > 0) {
      XNextEvent(display, &event);
      switch (event.type) {
        case Expose:
          break;
        case ButtonPress:
          mouse = 1;
          switch(event.xbutton.button){
	    case Button1:
	      mouse = 1;
	      break;
	    case Button2:
	      mouse = 2;
	      break;
	    case Button3:
	      mouse = 3;
	      break;
	  }
          break;
        default:
          break;
      }
    }
  }
  return (mouse);
}

int Erase() 
{
  if (graphics) {
    XBell(display,0);
    printf("Click button to erase\n");
    do
      XNextEvent(display, &event);
    while (event.type != ButtonPress);
    XClearWindow(display,window);
  }
}

int CloseDisplay()
{ /* close workstation */
  if (graphics) {
    Erase();
    XFreeGC(display, windowGC);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
  }
}

void MakeColorMap()
{
  int i,colormap_size;

  colormap_size = DisplayCells(display, screen);
  if ((colors = (XColor *)calloc(colormap_size,sizeof(XColor))) == NULL) {
    fprintf(stderr, "No memory for setting up colormap\n");
    exit(1);
  }
  for (i=0; i < colormap_size; i++) {
    colors[i].pixel = i;
    colors[i].flags = DoRed | DoGreen | DoBlue;
  }
  XQueryColors(display,DefaultColormap(display,screen),
	       colors,colormap_size); 
  for (i=0; i <= 255; i++)
    if (userCol[i][0]) {
      if (i < colormap_size) {
	colors[i].red = 257*userCol[i][1];
	colors[i].green = 257*userCol[i][2];
	colors[i].blue = 257*userCol[i][3];
      } else fprintf(stderr,"Cannot set color %d, your screen has colormap size %d\n",i,colormap_size);
    }
  if (pseudoCol8) {
    new_colormap = XCreateColormap(display,RootWindow(display,screen),
				   visual,AllocAll);
    XStoreColors(display,new_colormap,colors,colormap_size);
    free(colors);
  }else{   
    new_colormap = XDefaultColormap(display,screen);
    for (i=0; i <= 255 && i < colormap_size; i++)
      XAllocColor(display,new_colormap,&colors[i]);
  }
}
 
int OpenDisplay(char title[],int h,int w)
{
 int i;
 int width, height;
 char **argv;
 if (graphics) {
   width = w*scale;
   height = h*scale;
   display = XOpenDisplay(server);
   if (!display) {
     printf("Failed to open display connection to %s\n",
       XDisplayName(server));
     graphics = 0;
     return(0);
   }
   screen = DefaultScreen(display);

   visual_info.screen = screen;
   visual_list = XGetVisualInfo(display,VisualScreenMask,&visual_info,&i);
   if (VERBOSE) printf("Number of supported visuals: %d\n",i);

   if (pseudoCol8 && XMatchVisualInfo(display,screen,8,PseudoColor,&visual_info)) {
     visual = visual_info.visual;
     depth = visual_info.depth;
     if (VERBOSE) printf("Got PseudoColor with Depth %d\n",depth);
   }else{
     pseudoCol8 = 0;
     visual = XDefaultVisual(display, screen);
     depth = XDefaultDepth(display, screen);
     if (VERBOSE) printf("Got default Visual with Depth %d\n",depth);
   }
   class = visual->class;
   
   black = BlackPixel(display, screen);
   white = WhitePixel(display, screen);
   if (class == GrayScale) {
     background = black;
     foreground = white;
   } else {
     background = white;
     foreground = black;
   }
   hint.x = 0; hint.y = 0;
   hint.width = width; hint.height = height;
   hint.flags = PPosition | PSize;
   
   MakeColorMap(); 
   setattributes.colormap = new_colormap; 
   setattributes.background_pixel = BlackPixel(display,screen);
   setattributes.border_pixel = WhitePixel(display,screen);
   window = XCreateWindow(display,
            DefaultRootWindow(display), hint.x, hint.y,
            hint.width, hint.height, 5, depth,
            InputOutput, visual,
            CWColormap | CWBackPixel | CWBorderPixel,
            &setattributes);
   
   XSetStandardProperties(display, window, title, title,
     None, argv, 0, &hint);
   XGetWindowAttributes(display,window,&attributes);
   XSelectInput(display, window, ButtonPressMask | ExposureMask);
   if (DoesBackingStore(DefaultScreenOfDisplay(display))) {
     setattributes.backing_store = WhenMapped;
     XChangeWindowAttributes(display,window,CWBackingStore,&setattributes);
   }
   windowGC = XCreateGC(display, window, 0, 0);
   XSetBackground(display, windowGC, background);
   XSetForeground(display, windowGC, foreground);

   XSelectInput(display, window, ButtonPressMask | ExposureMask);
   XMapRaised(display, window);
   XNextEvent(display, &event); 

   imagedata = (char *)calloc(width*height,depth);
   if (imagedata == NULL) printf("Error in memory allocation\n");
   planeimage = XCreateImage(display, visual, depth, ZPixmap,
		0, imagedata, ncol*scale, nrow*scale, 8, 0);
   return (depth);
 }
}

int PlaneDisplay(TYPE **a,int y,int x,TYPE c)
{
  int i,j,mi,mj,bj,width;
  long offset;
  int nc, nr;

  if (graphics) {
    nr = nrow;
    nc = ncol;
    width= nc*scale;
    for (mi=0; mi < scale; mi++)
      for (mj=0; mj < scale; mj++)
	for (i=1; i <= nr; i++) {
	  offset=((i-1)*scale+mi)*width;
	  if (pseudoCol8)
	    /*$dir force_vector*/
	    for (j=1; j <= nc; j++)
	      (planeimage->data)[offset+((j-1)*scale+mj)] = a[i][j]+c;
	  else
	    for (j=1; j <= nc; j++)
	      XPutPixel(planeimage,(j-1)*scale+mj,(i-1)*scale+mi,
			colors[a[i][j]+c].pixel);
	}
    XPutImage(display,window,windowGC,planeimage,0,0,x*scale,y*scale,
	      (unsigned int)(scale*ncol),(unsigned int)(scale*nrow));
  }
}

int CellPlaneDisplay(TYPE **a,TYPE **b,int y,int x,TYPE c)
{
  int i,j,mi,mj,bj,width;
  long offset;
  int nc, nr;

  if (graphics) {
	Boundaries(b);
    nr = nrow;
    nc = ncol;
    width= nc*scale;
    for (mi=0; mi < scale; mi++)
      for (mj=0; mj < scale; mj++)
	for (i=1; i <= nr; i++) {
	  offset=((i-1)*scale+mi)*width;
	  if (pseudoCol8)
	    /*$dir force_vector*/
	  for (j=1; j <= nc; j++){
		  (planeimage->data)[offset+((j-1)*scale+mj)] = a[i][j]+c;
		  if(mi==0 && mj==0){
			  if(b[i][j]!=b[i-1][j-1])
				  (planeimage->data)[offset+((j-1)*scale+mj)]=cellinterface;
		  }
		  if(mi==0){
			  if(b[i][j]!=b[i-1][j])
				  (planeimage->data)[offset+((j-1)*scale+mj)]=cellinterface;			  
		  }
		  if(mj==0){
			  if(b[i][j]!=b[i][j-1])
				  (planeimage->data)[offset+((j-1)*scale+mj)]=cellinterface;			  
		  }
	  }
	  else
	  for (j=1; j <= nc; j++){
		  XPutPixel(planeimage,(j-1)*scale+mj,(i-1)*scale+mi,colors[a[i][j]+c].pixel);
		  if(mi==0 && mj==0){
			  if(b[i][j]!=b[i-1][j-1])
				  XPutPixel(planeimage,(j-1)*scale+mj,(i-1)*scale+mi,colors[cellinterface].pixel);
		  }
		  if(mi==0){
			  if(b[i][j]!=b[i-1][j])
				  XPutPixel(planeimage,(j-1)*scale+mj,(i-1)*scale+mi,colors[cellinterface].pixel);
		  }
		  if(mj==0){
			  if(b[i][j]!=b[i][j-1])
				  XPutPixel(planeimage,(j-1)*scale+mj,(i-1)*scale+mi,colors[cellinterface].pixel);
		  }
	  }
	}
    XPutImage(display,window,windowGC,planeimage,0,0,x*scale,y*scale,
	      (unsigned int)(scale*ncol),(unsigned int)(scale*nrow));
  }
}

int RowDisplay(int *a,int y,int x,TYPE c)
{
  int j,mi,mj;
  long offset;
  int nc;
  if (graphics) {
    nc = ncol;
    for (mi=0; mi < scale; mi++) {
      offset=mi*nc*scale;
      for (mj=0; mj < scale; mj++)
	if (pseudoCol8)         
	  /*$dir force_vector*/
	  for (j=1; j <= nc; j++)
	    (planeimage->data)[offset+((j-1)*scale+mj)] = a[j]+c;
	else
	  for (j=1; j <= nc; j++)
	    XPutPixel(planeimage,(j-1)*scale+mj,offset,
		      colors[a[j]+c].pixel);
    }
    XPutImage(display,window,windowGC,planeimage,0,0,x*scale,y*scale,
	      (unsigned int)(scale*nc),(unsigned int)scale);
  }
}

int BlockDisplay(unsigned char *a,int nr,int nc,int y,int x,int c)
{
  XImage *blockimage;
  int i,j,mi,mj;
  int width,height;
  long offset1,offset2;
  if (graphics) {
    width =  nc*scale;
    height = nr*scale;
    blockimage = XCreateImage(display, visual, depth, ZPixmap,
      0, imagedata, width, height, 8, 0);
    for (mi=0; mi < scale; mi++)
    for (mj=0; mj < scale; mj++)
    for (i=0; i < nr; i++) {
      offset1 = (i*scale+mi)*width;
      offset2 = i*nc;
      if (pseudoCol8)
	/*$dir force_vector*/
	for (j=0; j < nc; j++)
	  (blockimage->data)[offset1+(j*scale+mj)] = a[offset2+j]+c;
      else
	for (j=0; j < nc; j++)
	  XPutPixel(blockimage,j*scale+mj,i*scale+mi,
		    colors[a[offset2+j]+c].pixel);
    }
    XPutImage(display,window,windowGC,blockimage,0,0,x*scale,y*scale,
	      (unsigned int)width,(unsigned int)height);
    XFree(blockimage);
  }
}

int BlockDisplayFast(unsigned char *a,int nr,int nc,int y,int x)
{
  XImage *blockimage;
  if (graphics) {
    if (pseudoCol8) {
      blockimage = XCreateImage(display, visual, depth, ZPixmap,
				0, a, nc, nr, 8, 0);
      XPutImage(display,window,windowGC,blockimage,0,0,x,y,
		blockimage->width,blockimage->height);
      XFree(blockimage);
    }
    else 
      BlockDisplay(a,nr,nc,y,x,0);
  }
}
