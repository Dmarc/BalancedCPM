#include "cash.h"
#include <math.h>
#include <limits.h>
#include <float.h>
/**************************************************************************
** The following code is a slightly modied and mainly simplified version
** of the method of finding eigenvalues and eigenvectors, as provided by 
** the Meschach Library
** Athanasius Maree, Utrecht, the Netherlands, July 11, 2004
**************************************************************************/

/**************************************************************************
**
** Copyright (C) 1993 David E. Steward & Zbigniew Leyk, all rights reserved.
**
**			     Meschach Library
** 
** This Meschach Library is provided "as is" without any express 
** or implied warranty of any kind with respect to this software. 
** In particular the authors shall not be liable for any direct, 
** indirect, special, incidental or consequential damages arising 
** in any way from use of the software.
** 
** Everyone is granted permission to copy, modify and redistribute this
** Meschach Library, provided:
**  1.  All copies contain this copyright notice.
**  2.  All modified copies shall carry a notice stating who
**      made the last modification and the date of such modification.
**  3.  No charge is made for this software or works derived from it.  
**      This clause shall not be construed as constraining other software
**      distributed on the same medium as this software, nor is a
**      distribution fee considered a charge.
**
***************************************************************************/

#define	NEW(type) ((type *)calloc((size_t)1,(size_t)sizeof(type)))
#define	NEW_A(num,type)	((type *)calloc((size_t)(num),(size_t)sizeof(type)))

#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) > (b) ? (b) : (a))
#define	sgn(x)	( (x) >= 0 ? 1 : -1 )

Vec *trieig(Vec *,Vec *,Mat *);
Mat *Hfactor(Mat *,Vec *,Vec *);
double in_prod(Vec *,Vec *,int);
void mltadd(double *,double *,double,int);
Vec *hhvec(Vec *,int,double *,Vec *,double *);
Mat *hhtrcols(Mat *,int,int,Vec *,double);
Mat *hhtrrows(Mat *,int,int,Vec *,double);
Vec *hhtrvec(Vec *,double,int,Vec *,Vec *);
Mat *makeHQ(Mat *,Vec *,Vec *,Mat *);
void givens(double,double,double *,double *);
Vec *get_col(Mat *,int,Vec *);
Mat *set_col(Mat *,int,Vec *,int);
Mat *rot_cols(Mat *,int,int,double,double,Mat *);
Vec *vec_get(int);
Vec *vec_copy(Vec *,Vec *,int);
Mat *mat_copy(Mat *,Mat *,int,int);

static char *format = "%14.9g ";

/* mat_get -- gets an mxn matrix (in Mat form) by dynamic memory allocation */
Mat *mat_get(int m,int n)
{
  Mat *matrix;
  int i;
   
  if ((matrix=NEW(Mat)) == NULL ) {
    fprintf(stderr,"mat_get: error in memory allocation\n");
    exit(EXIT_FAILURE);
  }
  
  matrix->m = m;		
  matrix->n = matrix->max_n = n;
  matrix->max_m = m;	
  matrix->max_size = m*n;

  if ((matrix->base = NEW_A(m*n,double)) == NULL ) {
    fprintf(stderr,"mat_get: error in memory allocation\n");
    exit(EXIT_FAILURE);
  }
  
  if ((matrix->me = (double **)calloc(m,sizeof(double *))) == NULL ) {
    fprintf(stderr,"mat_get: error in memory allocation\n");
    exit(EXIT_FAILURE);
  }
  
  /* set up pointers */
  for ( i=0; i<m; i++ )
    matrix->me[i] = &(matrix->base[i*n]);
  
  return (matrix);
}

void vec_foutput(FILE *fp,Vec *x)
{
  int i, tmp;
     
  if ( x == NULL ) {  
    fprintf(fp,"Vector: NULL\n");   
    return;         
  }
  fprintf(fp,"Vector: dim: %d\n",x->dim);
  if ( x->ve == NULL ) {  
    fprintf(fp,"NULL\n");   
    return;         
  }
  for ( i=0, tmp=0; i<x->dim; i++, tmp++ ) {
    fprintf(fp,format,x->ve[i]);
    if ( tmp % 5 == 4 )   
      putc('\n',fp);
  }
  if ( tmp % 5 != 0 )        
    putc('\n',fp);
}

void mat_foutput(FILE *fp,Mat *a)
{
  int i, j, tmp;
     
  if ( a == NULL ) {  
    fprintf(fp,"Matrix: NULL\n");   
    return;         
  }
  fprintf(fp,"Matrix: %d by %d\n",a->m,a->n);
  if ( a->me == NULL ) {  
    fprintf(fp,"NULL\n");           
    return;         
  }
  for ( i=0; i<a->m; i++ )   /* for each row... */
    {
      fprintf(fp,"row %d: ",i);
      for ( j=0, tmp=2; j<a->n; j++, tmp++ )
	{             /* for each col in row... */
	  fprintf(fp,format,a->me[i][j]);
	  if ( ! (tmp % 5) )       
	    putc('\n',fp);
	}
      if ( tmp % 5 != 1 )   
	putc('\n',fp);
    }
}

/* mat_copy -- copies matrix into new area */
Mat *mat_copy(Mat *in,Mat *out,int i0,int j0)
{
  int i,j;

  if ( in==out )
    return out;
  if (out==NULL)
    out = mat_get(in->m,in->n);

  for ( i=i0; i < in->m; i++ )
    for ( j=j0; j < in->n; j++ )
      out->me[i][j] = in->me[i][j];
  
  return out;
}

/* hhtrvec -- apply Householder transformation to vector */
/* hh = Householder vector */
Vec *hhtrvec(Vec *hh,double beta,int i0,Vec *in,Vec *out) 
{
  double dscale;
  int i;
  
  dscale = beta*in_prod(hh,in,i0);
  out = vec_copy(in,out,0);
  for ( i=i0; i<in->dim; i++ )
    out->ve[i] = in->ve[i] - dscale*hh->ve[i];
  
  return out;
}

/* vec_copy -- copies vector into new area */
Vec *vec_copy(Vec *in,Vec *out,int i0)
{
  int i;
  if ( in==out )
    return (out);
  if ( out==NULL)
    out = vec_get(in->dim);
  
  for ( i=i0; i < in->dim; i++ )
    out->ve[i] = in->ve[i];
  
  return out;
}

/* in_prod -- inner product of two vectors from i0 downwards */
double in_prod(Vec *a,Vec *b,int i0)
{
  int i,limit;
  double sum=0.;
  double *a_v, *b_v;
  
  limit = min(a->dim,b->dim)-i0;
  a_v = &(a->ve[i0]);		
  b_v = &(b->ve[i0]);
  for ( i=0; i<limit; i++ )
    sum += (*a_v++)*(*b_v++);

  return (double)sum;
}

/* hhvec -- calulates Householder vector to eliminate all entries after the
   i0 entry of the vector vec. It is returned as out. */
Vec *hhvec(Vec *vec,int i0,double *beta,Vec *out,double *newval)
{
  double norm;
  
  out = vec_copy(vec,out,i0);
  norm = sqrt(in_prod(out,out,i0));
  if (norm < DBL_EPSILON)
    {
      *beta = 0.0;
      return (out);
    }
  *beta = 1.0/(norm * (norm+fabs(out->ve[i0])));
  if ( out->ve[i0] > 0.0 )
    *newval = -norm;
  else
    *newval = norm;
  out->ve[i0] -= *newval;
  
  return (out);
}

/* vec_get -- get a Vec of dimension 'size' */
Vec *vec_get(int size)
{
  Vec *vector;
  
  if ((vector=NEW(Vec)) == NULL) {
    fprintf(stderr,"vec_get: error in memory allocation\n");
    exit(EXIT_FAILURE);
  }
  
  vector->dim = vector->max_dim = size;
  if ((vector->ve=NEW_A(size,double)) == NULL ) {
    fprintf(stderr,"vec_get: error in memory allocation\n");
    exit(EXIT_FAILURE);
  }
  
  return vector;
}

/* get_col -- gets a specified column of a matrix and retruns it as a vector */
Vec *get_col(Mat *mat,int col,Vec *vec)
{
  int i;
  
  if ( vec==NULL)
    vec = vec_get(mat->m);
  
  for ( i=0; i<mat->m; i++ )
    vec->ve[i] = mat->me[i][col];
  
  return (vec);
}

/* mltadd -- scalar multiply and add */
void mltadd(double *dp1,double *dp2,double s,int len)
{
  int i;
    
  for ( i = 0; i < len; i++ )
    dp1[i] += s*dp2[i];
}

/* hhtrcols -- transform a matrix by a Householder vector by columns
   starting at row i0 from column j0 */
Mat *hhtrcols(Mat *M,int i0,int j0,Vec *hh,double beta)
{
  int	i;
  static Vec *w = NULL;
  
  if ( fabs(beta) < DBL_EPSILON ) 
    return M;
  
  if(w==NULL)
    w = vec_get(M->n);
  for ( i = 0; i < w->dim; i++ )
    w->ve[i] = 0.0;
  
  for ( i = i0; i < M->m; i++ )
    if ( !(fabs(hh->ve[i])<DBL_EPSILON))
      mltadd(&(w->ve[j0]),&(M->me[i][j0]),hh->ve[i],
	     (int)(M->n-j0));
  for ( i = i0; i < M->m; i++ )
    if ( !(fabs(hh->ve[i])<DBL_EPSILON))
      mltadd(&(M->me[i][j0]),&(w->ve[j0]),-beta*hh->ve[i],
	     (int)(M->n-j0));
  return M;
}

/* hhtrrows -- transform a matrix by a Householder vector by rows
   starting at row i0 from column j0 */
Mat *hhtrrows(Mat *M,int i0,int j0,Vec *hh,double beta)
{
  double ip, dscale;
  int i,j;

  if ( fabs(beta) < DBL_EPSILON )	
    return M;
  
  /* for each row ... */
  for ( i = i0; i < M->m; i++ )
    {	/* compute inner product */
      ip = 0.0;
      for ( j = j0; j < M->n; j++ )
	ip += M->me[i][j]*hh->ve[j];

      dscale = beta*ip;
      if ( fabs(dscale) < DBL_EPSILON)
	continue;

      /* do operation */
      mltadd(&(M->me[i][j0]),&(hh->ve[j0]),-dscale,
		 (int)(M->n-j0));
      /**************************************************
		for ( j = j0; j < M->n; j++ )
			M->me[i][j] -= dscale*hh->ve[j];
      **************************************************/
    }

  return M;
}

/* Hfactor -- compute Hessenberg factorisation in compact form */
Mat *Hfactor(Mat *A,Vec *diag,Vec *beta)
{
  static Vec *tmp1 = NULL;
  int k, limit;

  limit = A->m - 1;

  if(tmp1==NULL)
    tmp1 = vec_get(A->m);

  for ( k = 0; k < limit; k++ )
    {
      get_col(A,k,tmp1);
      hhvec(tmp1,k+1,&beta->ve[k],tmp1,&A->me[k+1][k]);
      diag->ve[k] = tmp1->ve[k+1];
      hhtrcols(A,k+1,k+1,tmp1,beta->ve[k]);
      hhtrrows(A,0  ,k+1,tmp1,beta->ve[k]);
    }
  
  return (A);
}

/* makeHQ -- construct the Hessenberg orthogonalising matrix Q;
   i.e. Hess M = Q.M.Q'	*/
Mat *makeHQ(Mat *H,Vec *diag,Vec *beta,Mat *Qout)
{
  int i, j, limit;
  static Vec *tmp1 = NULL, *tmp2 = NULL;

  limit = H->m - 1;
  if(Qout==NULL)
    Qout = mat_get(H->m,H->m);

  if(tmp1==NULL)
    tmp1 = vec_get(H->m);
  if(tmp2==NULL)
    tmp2 = vec_get(H->m);

  for ( i = 0; i < H->m; i++ )
    {
      /* tmp1 = i'th basis vector */
      for ( j = 0; j < H->m; j++ )
	tmp1->ve[j] = 0.0;
      tmp1->ve[i] = 1.0;
      
      /* apply H/h transforms in reverse order */
      for ( j = limit-1; j >= 0; j-- )
	{
	  get_col(H,j,tmp2);
	  tmp2->ve[j+1] = diag->ve[j];
	  hhtrvec(tmp2,beta->ve[j],j+1,tmp1,tmp1);
	}
      
      /* insert into Qout */
      set_col(Qout,i,tmp1,0);
    }
  
  return Qout;
}

/* set_col -- sets column of matrix to values given in vec */
Mat *set_col(Mat *mat,int col,Vec *vec,int i0)
{
  int i,lim;
  
  lim = min(mat->m,vec->dim);
  for ( i=i0; i<lim; i++ )
    mat->me[i][col] = vec->ve[i];
  
  return (mat);
}

/* givens -- returns c,s parameters for Givens rotation to
   eliminate y in the vector [ x y ]' */
void givens(double x,double y,double *c,double *s)
{
  double norm;
  
  norm = sqrt(x*x+y*y);
  if ( fabs(norm)<DBL_EPSILON)
    {	*c = 1.0;	*s = 0.0;	}	/* identity */
  else
    {	*c = x/norm;	*s = y/norm;	}
}

/* rot_cols -- postmultiply mat by givens rotation described by c,s */
Mat *rot_cols(Mat *mat,int i,int k,double c,double s,Mat *out)
{
  int j;
  double temp;
  
  out = mat_copy(mat,out,0,0);
  
  for ( j=0; j<mat->m; j++ )
    {
      temp = c*out->me[j][i] + s*out->me[j][k];
      out->me[j][k] = -s*out->me[j][i] + c*out->me[j][k];
      out->me[j][i] = temp; 
    }
  
  return out;
}

/* trieig -- finds eigenvalues of symmetric tridiagonal matrices;
   matrix represented by a pair of vectors a (diag entries)
   and b (sub- & super-diag entries);
   eigenvalues in a on return */
Vec *trieig(Vec *a,Vec *b,Mat *Q)
{
  int i, i_min, i_max, n, split;
  double *a_ve, *b_ve;
  double b_sqr, bk, ak1, bk1, ak2, bk2, z;
  double c, c2, cs, s, s2, d, mu;

  n = a->dim;
  a_ve = a->ve;		
  b_ve = b->ve;
  
  i_min = 0;
  while ( i_min < n )		/* outer while loop */
    {
      /* find i_max to suit;
	 submatrix i_min..i_max should be irreducible */
      i_max = n-1;
      for ( i = i_min; i < n-1; i++ )
	if ( fabs(b_ve[i]) < DBL_EPSILON) {	
	  i_max = i;	
	  break;	
	}
      if ( i_max <= i_min ) {
	i_min = i_max + 1;
	continue;	/* outer while loop */
      }
      
      /* repeatedly perform QR method until matrix splits */
      split = 0;
      while ( ! split )		/* inner while loop */
	{
	  
	  /* find Wilkinson shift */
	  d = (a_ve[i_max-1] - a_ve[i_max])/2;
	  b_sqr = b_ve[i_max-1]*b_ve[i_max-1];
	  mu = a_ve[i_max] - b_sqr/(d + sgn(d)*sqrt(d*d+b_sqr));
	  
	  /* initial Givens' rotation */
	  givens(a_ve[i_min]-mu,b_ve[i_min],&c,&s);
	  s = -s;
	  if ( fabs(c) < M_SQRT2 ) {	
	    c2 = c*c;	
	    s2 = 1-c2;	
	  }
	  else {	
	    s2 = s*s;	
	    c2 = 1-s2;	
	  }
	  cs = c*s;
	  ak1 = c2*a_ve[i_min]+s2*a_ve[i_min+1]-2*cs*b_ve[i_min];
	  bk1 = cs*(a_ve[i_min]-a_ve[i_min+1]) +
	    (c2-s2)*b_ve[i_min];
	  ak2 = s2*a_ve[i_min]+c2*a_ve[i_min+1]+2*cs*b_ve[i_min];
	  bk2 = ( i_min < i_max-1 ) ? c*b_ve[i_min+1] : 0.0;
	  z  = ( i_min < i_max-1 ) ? -s*b_ve[i_min+1] : 0.0;
	  a_ve[i_min] = ak1;
	  a_ve[i_min+1] = ak2;
	  b_ve[i_min] = bk1;
	  if ( i_min < i_max-1 )
	    b_ve[i_min+1] = bk2;
	  if ( Q )
	    rot_cols(Q,i_min,i_min+1,c,-s,Q);

	  for ( i = i_min+1; i < i_max; i++ )
	    {
	      /* get Givens' rotation for sub-block -- k == i-1 */
	      givens(b_ve[i-1],z,&c,&s);
	      s = -s;
	      
	      /* perform Givens' rotation on sub-block */
	      if ( fabs(c) < M_SQRT2 ) {	
		c2 = c*c;	
		s2 = 1-c2;	
	      }
	      else {	
		s2 = s*s;	
		c2 = 1-s2;	
	      }
	      cs = c*s;
	      bk  = c*b_ve[i-1] - s*z;
	      ak1 = c2*a_ve[i]+s2*a_ve[i+1]-2*cs*b_ve[i];
	      bk1 = cs*(a_ve[i]-a_ve[i+1]) +
		(c2-s2)*b_ve[i];
	      ak2 = s2*a_ve[i]+c2*a_ve[i+1]+2*cs*b_ve[i];
	      bk2 = ( i+1 < i_max ) ? c*b_ve[i+1] : 0.0;
	      z  = ( i+1 < i_max ) ? -s*b_ve[i+1] : 0.0;
	      a_ve[i] = ak1;	a_ve[i+1] = ak2;
	      b_ve[i] = bk1;
	      if ( i < i_max-1 )
		b_ve[i+1] = bk2;
	      if ( i > i_min )
		b_ve[i-1] = bk;
	      if ( Q )
		rot_cols(Q,i,i+1,c,-s,Q);
	    }
	  
	  /* test to see if matrix should be split */
	  for ( i = i_min; i < i_max; i++ )
	    if ( fabs(b_ve[i]) < DBL_EPSILON*
		 (fabs(a_ve[i])+fabs(a_ve[i+1])) ) {   
	      b_ve[i] = 0.0;	
	      split = 1;	
	    }
	}
    }
  
  return a;
}

/* symmeig -- computes eigenvalues of a dense symmetric matrix
	-- A **must** be symmetric on entry
	-- eigenvalues stored in out
	-- Q contains orthogonal matrix of eigenvectors
	-- returns vector of eigenvalues */
Vec *symmeig(Mat *A,Mat *Q,Vec *out)
{
  int i;
  static Mat *tmp = NULL;
  static Vec *b, *diag = NULL, *beta = NULL;
  static int allocated=0;
  
  if(!allocated) {
    allocated=1;
    b = vec_get(A->m - 1);
    diag = vec_get(A->m);
    beta = vec_get(A->m);
  }
  
  if(out==NULL)
    out = vec_get(A->m);
  tmp  = mat_copy(A,tmp,0,0);
  Hfactor(tmp,diag,beta);
  if (Q !=NULL)
    makeHQ(tmp,diag,beta,Q);
  
  for (i = 0; i < A->m - 1; i++)
    {
      out->ve[i] = tmp->me[i][i];
      b->ve[i] = tmp->me[i][i+1];
    }
  out->ve[i] = tmp->me[i][i];
  trieig(out,b,Q);

  return out;
}
