#include <strings.h>
#include <png.h>
#include "cash.h"

int nRow, nCol;
extern int userCol[256][4];
extern int scale,cellinterface;
static unsigned char *data;

static png_structp png_ptr;
static png_infop info_ptr;
static png_bytep ptr;
/*for Benoit: static*/ int nframes = 0;

static char dirname[512];

int OpenPNG(char name[],int nrow,int ncol)
{
  char command[512];
  nRow = nrow;
  nCol = ncol;
  strncpy(dirname,name,512);
  sprintf(command,"%s%s","mkdir -p ",dirname);
  system(command);
  data = (unsigned char *)calloc((size_t)(3*nrow*ncol),sizeof(unsigned char));
}

int OpenCellPNG(char name[],int nrow,int ncol)
{
  char command[512];
  nRow = nrow;
  nCol = ncol;
  strncpy(dirname,name,512);
  sprintf(command,"%s%s","mkdir -p ",dirname);
  system(command);
  data = (unsigned char *)calloc(scale*scale*3*nrow*ncol,sizeof(unsigned char));
}

/*
// identique à OpenPNG
int OpenRAW(char name[],int nrow,int ncol)
{
  char command[512];
  nRow = nrow;
  nCol = ncol;
  strncpy(dirname,name,512);
  sprintf(command,"%s%s","mkdir -p ",dirname);
  system(command);
  //data = (unsigned char *)calloc(scale*scale*3*nrow*ncol,sizeof(unsigned char));
  data = (unsigned char *)calloc(3*nrow*ncol,sizeof(unsigned char));
}
*/

int PlanePNG(TYPE **a, int c)
{
  int i,j;
  char name[512];
  FILE *fp;
  sprintf(name,"%s/%.5d.png",dirname,nframes);
  fp = fopen(name,"wb");
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
   (png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
  info_ptr = png_create_info_struct (png_ptr);
  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, nCol, nRow,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr,info_ptr);
  for (i=0; i < nRow; i++) {
    for (j=0; j < nCol; j++) {
      data[i*3*nCol + j*3] = userCol[c+a[i+1][j+1]][1];
      data[i*3*nCol + j*3 + 1] = userCol[c+a[i+1][j+1]][2];
      data[i*3*nCol + j*3 + 2] = userCol[c+a[i+1][j+1]][3];
    }
    ptr = data + i*3*nCol;
    png_write_rows(png_ptr, &ptr, 1);
  }
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
  fclose(fp);
  nframes++;
}





int CellPlanePNG(TYPE **a, TYPE **b, int c)
{
	int i,j,mi,mj;
	char name[512];
	FILE *fp;
	sprintf(name,"%s/%.5d.png",dirname,nframes);
	fp = fopen(name,"wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	(png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
	info_ptr = png_create_info_struct (png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, scale*nCol, scale*nRow,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	Boundaries(b);
	for (i=0; i < nRow; i++) {
		for (mi=0; mi < scale; mi++){
			for (j=0; j < nCol; j++) {
				for (mj=0; mj < scale; mj++){
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[c+a[i+1][j+1]][1];
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[c+a[i+1][j+1]][2];
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[c+a[i+1][j+1]][3];
					if(mi==0 && mj==0){
						if(b[i+1][j+1]!=b[i][j]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
					if(mi==0){
						if(b[i+1][j+1]!=b[i][j+1]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
					if(mj==0){
						if(b[i+1][j+1]!=b[i+1][j]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
				}
			}
			ptr = data + (scale*i+mi)*3*nCol;
			png_write_rows(png_ptr, &ptr, 1);
		}
	}
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
	fclose(fp);
	nframes++;
}

int CellPlanePNG2(char subdirname[], TYPE **a, TYPE **b, int c)
//identique à CellPlanePNG mais permet d'écrire les images dans des répertoires différents.
//On garde CellPlanePNG pour des raisons de rétrocompatibilité...
{
	int i,j,mi,mj;
	char name[512];
	FILE *fp;
	//sprintf(name,"%s/%s/%.5d.png",dirname,subdirname,nframes);
	sprintf(name,"%s/%.5d.png",subdirname,nframes);
	fp = fopen(name,"wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	(png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
	info_ptr = png_create_info_struct (png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, scale*nCol, scale*nRow,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	Boundaries(b);
	for (i=0; i < nRow; i++) {
		for (mi=0; mi < scale; mi++){
			for (j=0; j < nCol; j++) {
				for (mj=0; mj < scale; mj++){
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[c+a[i+1][j+1]][1];
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[c+a[i+1][j+1]][2];
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[c+a[i+1][j+1]][3];
					if(mi==0 && mj==0){
						if(b[i+1][j+1]!=b[i][j]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
					if(mi==0){
						if(b[i+1][j+1]!=b[i][j+1]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
					if(mj==0){
						if(b[i+1][j+1]!=b[i+1][j]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
				}
			}
			ptr = data + (scale*i+mi)*3*nCol;
			png_write_rows(png_ptr, &ptr, 1);
		}
	}
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
	fclose(fp);
	nframes++;
}

int CellPlaneRAW2(char subdirname[], TYPE **a, TYPE **b, int c)
//identique à CellPlanePNG2 mais sauve l'indice des bulles plutôt que leurs couleurs.
//l'indice est décomposé en 3 valeurs R,G,B. On peut donc faire 256^3 bulles différentes.
{
	int i,j,mi,mj;
	char name[512];
	FILE *fp;
	//sprintf(name,"%s/%s/%.5d.png",dirname,subdirname,nframes);
	sprintf(name,"%s/%.5d.png",subdirname,nframes);
	fp = fopen(name,"wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	(png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
	info_ptr = png_create_info_struct (png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, scale*nCol, scale*nRow,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	Boundaries(b);
	for (i=0; i < nRow; i++) {
		for (mi=0; mi < scale; mi++){
			for (j=0; j < nCol; j++) {
				for (mj=0; mj < scale; mj++){
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = c+a[i+1][j+1]%256;
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = c+(a[i+1][j+1]/256)%256;
					data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = c+(a[i+1][j+1]/65536)%256;
					if(mi==0 && mj==0){
						if(b[i+1][j+1]!=b[i][j]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
					if(mi==0){
						if(b[i+1][j+1]!=b[i][j+1]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
					if(mj==0){
						if(b[i+1][j+1]!=b[i+1][j]){
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3] = userCol[cellinterface][1];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 1] = userCol[cellinterface][2];
							data[(scale*i+mi)*3*nCol + (scale*j+mj)*3 + 2] = userCol[cellinterface][3];
						}
					}
				}
			}
			ptr = data + (scale*i+mi)*3*nCol;
			png_write_rows(png_ptr, &ptr, 1);
		}
	}
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
	fclose(fp);
//	nframes++;
}


int PlaneRAW2(char subdirname[], TYPE **a, TYPE **b, int c)
//identique à CellPlaneRAW2 mais les images brutes restent à l'échelle 1 et sans les bords des domaines.
{
	int i,j;
	char name[512];
	FILE *fp;
	//sprintf(name,"%s/%s/%.5d.png",dirname,subdirname,nframes);
	sprintf(name,"%s/%.5d.png",subdirname,nframes);
	fp = fopen(name,"wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	(png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
	info_ptr = png_create_info_struct (png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, nCol, nRow,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	Boundaries(b); //sans doute inutile...
	for (i=0; i < nRow; i++) {
			for (j=0; j < nCol; j++) {
					data[i*3*nCol + j*3] = c+a[i+1][j+1]%256;
					data[i*3*nCol + j*3 + 1] = c+(a[i+1][j+1]/256)%256;
					data[i*3*nCol + j*3 + 2] = c+(a[i+1][j+1]/65536)%256;					

			}
			ptr = data + i*3*nCol;
			png_write_rows(png_ptr, &ptr, 1);		
	}
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
	fclose(fp);
//	nframes++;
}

int BlockPNG(unsigned char *a,int nRow,int nCol,int c)
{
  int i,j,k;
  char name[512];
  FILE *fp;
  sprintf(name,"%s/%.5d.png",dirname,nframes);
  fp = fopen(name,"wb");
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
   (png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
  info_ptr = png_create_info_struct (png_ptr);
  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, nCol, nRow,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr,info_ptr);
  for (i=0; i < nRow; i++) {
    for (j=0; j < nCol; j++) {
      k = i*nCol + j;
      data[i*3*nCol + j*3] = userCol[c+a[k]][1];
      data[i*3*nCol + j*3 + 1] = userCol[c+a[k]][2];
      data[i*3*nCol + j*3 + 2] = userCol[c+a[k]][3];
    }
    ptr = data + i*3*nCol;
    png_write_rows(png_ptr, &ptr, 1);
  }
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
  fclose(fp);
  nframes++;
}

int ClosePNG()
{
  FILE *fp;
  char name[512];
  sprintf(name,"%s/mpeg.par",dirname);
  fp = fopen(name,"w");
  fprintf(fp,"MPEG-1 Sequence, 25 frames/sec \n");
  fprintf(fp,"%%.5d     /* name of source files */ \n");
  fflush(fp);
  fprintf(fp,"-         /* name of reconstructed images ('-': don't store) */ \n");
  fflush(fp);
  fprintf(fp,"-         /* name of intra quant matrix file     ('-': default matrix) */  \n");
  fprintf(fp,"-         /* name of non intra quant matrix file ('-': default matrix) */ \n");
  fprintf(fp,"/dev/null /* name of statistics file ('-': stdout ) */ \n");
  fprintf(fp,"2         /* input picture file format: 0=*.Y,*.U,*.V, 1=*.yuv, 2=*.ppm */  \n");
  fprintf(fp,"%d        /* number of frames */ \n",nframes);
  fprintf(fp,"0         /* number of first frame */ \n");
  fprintf(fp,"00:00:00:00 /* timecode of first frame */ \n");
  fprintf(fp,"12        /* N (# of frames in GOP) */ \n");
  fprintf(fp,"1         /* M (I/P frame distance) */ \n");
  fprintf(fp,"1         /* ISO/IEC 11172-2 stream */ \n");
  fprintf(fp,"0         /* 0:frame pictures, 1:field pictures */ \n");
  fprintf(fp,"%d        /* horizontal_size */ \n",nCol);
  fprintf(fp,"%d        /* vertical_size */ \n",nRow);
  fprintf(fp,"1         /* aspect_ratio_information 8=CCIR601 625 line, 9=CCIR601 525 line */ \n");
  fprintf(fp,"3         /* frame_rate_code 1=23.976, 2=24, 3=25, 4=29.97, 5=30 frames/sec. */ \n");
  fprintf(fp,"5000000.0 /* bit_rate (bits/s) */ \n");
  fprintf(fp,"20        /* vbv_buffer_size (in multiples of 16 kbit) */ \n");
  fprintf(fp,"0         /* low_delay  */ \n");
  fprintf(fp,"1         /* constrained_parameters_flag */ \n");
  fprintf(fp,"4         /* Profile ID: Simple = 5, Main = 4, SNR = 3, Spatial = 2, High = 1 */ \n");
  fprintf(fp,"8         /* Level ID:   Low = 10, Main = 8, High 1440 = 6, High = 4          */ \n");
  fprintf(fp,"1         /* progressive_sequence */ \n");
  fprintf(fp,"1         /* chroma_format: 1=4:2:0, 2=4:2:2, 3=4:4:4 */ \n");
  fprintf(fp,"1         /* video_format: 0=comp., 1=PAL, 2=NTSC, 3=SECAM, 4=MAC, 5=unspec. */ \n");
  fprintf(fp,"5         /* color_primaries */ \n");
  fprintf(fp,"5         /* transfer_characteristics */ \n");
  fprintf(fp,"5         /* matrix_coefficients */ \n");
  fprintf(fp,"%d       /* display_horizontal_size */ \n",nCol);
  fprintf(fp,"%d       /* display_vertical_size */ \n",nRow);
  fprintf(fp,"0         /* intra_dc_precision (0: 8 bit, 1: 9 bit, 2: 10 bit, 3: 11 bit */ \n");
  fprintf(fp,"0         /* top_field_first */ \n");
  fprintf(fp,"1 1 1     /* frame_pred_frame_dct (I P B) */ \n");
  fprintf(fp,"0 0 0     /* concealment_motion_vectors (I P B) */ \n");
  fprintf(fp,"0 0 0     /* q_scale_type  (I P B) */ \n");
  fprintf(fp,"0 0 0     /* intra_vlc_format (I P B)*/ \n");
  fprintf(fp,"0 0 0     /* alternate_scan (I P B) */ \n");
  fprintf(fp,"0         /* repeat_first_field */ \n");
  fprintf(fp,"1         /* progressive_frame */ \n");
  fprintf(fp,"0         /* P distance between complete intra slice refresh */ \n");
  fprintf(fp,"0         /* rate control: r (reaction parameter) */ \n");
  fprintf(fp,"0         /* rate control: avg_act (initial average activity) */ \n");
  fprintf(fp,"0         /* rate control: Xi (initial I frame global complexity measure) */ \n");
  fprintf(fp,"0         /* rate control: Xp (initial P frame global complexity measure) */ \n");
  fprintf(fp,"0         /* rate control: Xb (initial B frame global complexity measure) */ \n");
  fprintf(fp,"0         /* rate control: d0i (initial I frame virtual buffer fullness) */ \n");
  fprintf(fp,"0         /* rate control: d0p (initial P frame virtual buffer fullness) */ \n");
  fprintf(fp,"0         /* rate control: d0b (initial B frame virtual buffer fullness) */ \n");
  fprintf(fp,"2 2 11 11 /* P:  forw_hor_f_code forw_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 3  3  /* B1: forw_hor_f_code forw_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 7  7  /* B1: back_hor_f_code back_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 7  7  /* B2: forw_hor_f_code forw_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 3  3  /* B2: back_hor_f_code back_vert_f_code search_width/height */ \n");
  fprintf(fp," \n");
  fclose(fp);
}

int CloseCellPNG()
{
  FILE *fp;
  char name[512];
  sprintf(name,"%s/mpeg.par",dirname);
  fp = fopen(name,"w");
  fprintf(fp,"MPEG-1 Sequence, 25 frames/sec \n");
  fprintf(fp,"%%.5d     /* name of source files */ \n");
  fflush(fp);
  fprintf(fp,"-         /* name of reconstructed images ('-': don't store) */ \n");
  fflush(fp);
  fprintf(fp,"-         /* name of intra quant matrix file     ('-': default matrix) */  \n");
  fprintf(fp,"-         /* name of non intra quant matrix file ('-': default matrix) */ \n");
  fprintf(fp,"/dev/null /* name of statistics file ('-': stdout ) */ \n");
  fprintf(fp,"2         /* input picture file format: 0=*.Y,*.U,*.V, 1=*.yuv, 2=*.ppm */  \n");
  fprintf(fp,"%d        /* number of frames */ \n",nframes);
  fprintf(fp,"0         /* number of first frame */ \n");
  fprintf(fp,"00:00:00:00 /* timecode of first frame */ \n");
  fprintf(fp,"12        /* N (# of frames in GOP) */ \n");
  fprintf(fp,"1         /* M (I/P frame distance) */ \n");
  fprintf(fp,"1         /* ISO/IEC 11172-2 stream */ \n");
  fprintf(fp,"0         /* 0:frame pictures, 1:field pictures */ \n");
  fprintf(fp,"%d        /* horizontal_size */ \n",scale*nCol);
  fprintf(fp,"%d        /* vertical_size */ \n",scale*nRow);
  fprintf(fp,"1         /* aspect_ratio_information 8=CCIR601 625 line, 9=CCIR601 525 line */ \n");
  fprintf(fp,"3         /* frame_rate_code 1=23.976, 2=24, 3=25, 4=29.97, 5=30 frames/sec. */ \n");
  fprintf(fp,"5000000.0 /* bit_rate (bits/s) */ \n");
  fprintf(fp,"20        /* vbv_buffer_size (in multiples of 16 kbit) */ \n");
  fprintf(fp,"0         /* low_delay  */ \n");
  fprintf(fp,"1         /* constrained_parameters_flag */ \n");
  fprintf(fp,"4         /* Profile ID: Simple = 5, Main = 4, SNR = 3, Spatial = 2, High = 1 */ \n");
  fprintf(fp,"8         /* Level ID:   Low = 10, Main = 8, High 1440 = 6, High = 4          */ \n");
  fprintf(fp,"1         /* progressive_sequence */ \n");
  fprintf(fp,"1         /* chroma_format: 1=4:2:0, 2=4:2:2, 3=4:4:4 */ \n");
  fprintf(fp,"1         /* video_format: 0=comp., 1=PAL, 2=NTSC, 3=SECAM, 4=MAC, 5=unspec. */ \n");
  fprintf(fp,"5         /* color_primaries */ \n");
  fprintf(fp,"5         /* transfer_characteristics */ \n");
  fprintf(fp,"5         /* matrix_coefficients */ \n");
  fprintf(fp,"%d       /* display_horizontal_size */ \n",scale*nCol);
  fprintf(fp,"%d       /* display_vertical_size */ \n",scale*nRow);
  fprintf(fp,"0         /* intra_dc_precision (0: 8 bit, 1: 9 bit, 2: 10 bit, 3: 11 bit */ \n");
  fprintf(fp,"0         /* top_field_first */ \n");
  fprintf(fp,"1 1 1     /* frame_pred_frame_dct (I P B) */ \n");
  fprintf(fp,"0 0 0     /* concealment_motion_vectors (I P B) */ \n");
  fprintf(fp,"0 0 0     /* q_scale_type  (I P B) */ \n");
  fprintf(fp,"0 0 0     /* intra_vlc_format (I P B)*/ \n");
  fprintf(fp,"0 0 0     /* alternate_scan (I P B) */ \n");
  fprintf(fp,"0         /* repeat_first_field */ \n");
  fprintf(fp,"1         /* progressive_frame */ \n");
  fprintf(fp,"0         /* P distance between complete intra slice refresh */ \n");
  fprintf(fp,"0         /* rate control: r (reaction parameter) */ \n");
  fprintf(fp,"0         /* rate control: avg_act (initial average activity) */ \n");
  fprintf(fp,"0         /* rate control: Xi (initial I frame global complexity measure) */ \n");
  fprintf(fp,"0         /* rate control: Xp (initial P frame global complexity measure) */ \n");
  fprintf(fp,"0         /* rate control: Xb (initial B frame global complexity measure) */ \n");
  fprintf(fp,"0         /* rate control: d0i (initial I frame virtual buffer fullness) */ \n");
  fprintf(fp,"0         /* rate control: d0p (initial P frame virtual buffer fullness) */ \n");
  fprintf(fp,"0         /* rate control: d0b (initial B frame virtual buffer fullness) */ \n");
  fprintf(fp,"2 2 11 11 /* P:  forw_hor_f_code forw_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 3  3  /* B1: forw_hor_f_code forw_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 7  7  /* B1: back_hor_f_code back_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 7  7  /* B2: forw_hor_f_code forw_vert_f_code search_width/height */ \n");
  fprintf(fp,"1 1 3  3  /* B2: back_hor_f_code back_vert_f_code search_width/height */ \n");
  fprintf(fp," \n");
  fclose(fp);
}
