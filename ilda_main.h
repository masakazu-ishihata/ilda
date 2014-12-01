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

/*------------------------------------*/
/* ilda */
/*------------------------------------*/
typedef struct ILDA
{
  /* constants */
  int D;   /* D = # documents */
  int V;   /* V = # words */
  int K;   /* K =# topics */

  /* document */
  int *L;  /* L[d] = length of d-th doc = W[d][n] * C[d][n] */
  int **X; /* X[d][l] = l-th word in d-th doc */

  /* BoW */
  int *N;  /* N[d] = # uniq words in d-th doc */
  int **U; /* U[d][n] = n-th uniq word in d-th doc */
  int **C; /* C[d][n] = count of U[d][n] */

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
} ilda;

/*------------------------------------*/
/* new / free */
/*------------------------------------*/
ilda *ilda_new(char const *_file, int _K);
void ilda_free(ilda *_m);

#endif
