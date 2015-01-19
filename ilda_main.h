#ifndef _ILDA_MAIN_H_
#define _ILDA_MAIN_H_

/*------------------------------------*/
/* include */
/*------------------------------------*/
#include <stdio.h>
#include <itime.h>
#include <irand.h>
#include <icsv.h>

/*------------------------------------*/
/* constants */
/*------------------------------------*/
#define DEBUG (0)

typedef unsigned int ui;

/*------------------------------------*/
/* ilda */
/*------------------------------------*/
typedef struct ILDA
{
  /* constants */
  ui D;   /* D = # documents */
  ui V;   /* V = # words */
  ui K;   /* K =# topics */

  /* document */
  ui TL;
  ui TN;
  ui *L;  /* L[d] = length of d-th doc = W[d][n] * C[d][n] */
  ui *N;  /* N[d] = # uniq words in d-th doc */
  ui **U; /* U[d][n] = n-th uniq word in d-th doc */
  ui **C; /* C[d][n] = count of U[d][n] */
  ui **X; /* X[d][l] = l-th word in d-th doc */

  /* hyper parameters */
  prob *al; /* al = R^K : th[d] ~ Dir(al) */
  prob *bt; /* bt = R^V : th[k] ~ Dir(bt) */

  /* variational parameters */
  prob **alp;  /* D * K : al'[d][k] */
  prob **btp;  /* K * V : bt'[k][v] */
  prob **thp;  /* D * K : th'[d][k] */
  prob **php;  /* K * V : ph'[k][v] */
  prob ***gmp; /* D * N * k : gm'[d][n][k] */

  /* learning setting */
  int iter;
  double radius;
  prob ALPHA;
  prob BETA;
  prob A0;
  prob B0;

  /* time */
  itime *T;
} ilda;

/*------------------------------------*/
/* new / free */
/*------------------------------------*/
ilda *ilda_new(char const *_file, ui _K);
void ilda_free(ilda *_m);

#endif
