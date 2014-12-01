#include "ilda_main.h"

/*------------------------------------*/
/* new */
/*------------------------------------*/
ilda *ilda_new(char const *_file, int _K)
{
  ilda *_m;
  icsv *f;
  int d, l, v, n;
  int *C, N;

  /*------------------------------------*/
  /* initialize */
  /*------------------------------------*/
  _m = (ilda *)malloc(sizeof(ilda));

  /*------------------------------------*/
  /* load file */
  /*------------------------------------*/
  f = icsv_new_delimiter(_file, " ");

  /* # documents, vocabulary size */
  _m->D = icsv_num_line(f);
  _m->V = 0;
  _m->K = _K;

  /* load documents */
  _m->L = (int *)malloc(_m->D * sizeof(int));
  _m->X = (int **)malloc(_m->D * sizeof(int *));
  for(d=0; d<_m->D; d++){
    _m->L[d] = icsv_num_item(f, d);
    _m->X[d] = (int *)malloc(_m->L[d] * sizeof(int));
    for(l=0; l<_m->L[d]; l++){
      v = atoi( icsv_get(f, d, l) ) - 1;
      _m->X[d][l] = v;
      if(_m->V < v) _m->V = v;
    }
  }
  _m->V++;
  icsv_free(f);

  /*------------------------------------*/
  /* BoW */
  /*------------------------------------*/
  C = (int *)malloc(_m->V * sizeof(int));
  _m->N = (int *)malloc(_m->D * sizeof(int));
  _m->U = (int **)malloc(_m->D * sizeof(int *));
  _m->C = (int **)malloc(_m->D * sizeof(int *));
  for(d=0; d<_m->D; d++){
    /* init count */
    for(v=0; v<_m->V; v++)
      C[v] = 0;

    /* count */
    for(l=0; l<_m->L[d]; l++)
      C[ _m->X[d][l] ]++;

    /* # uniq words */
    for(N=0, v=0; v<_m->V; v++)
      if(C[v] > 0)
        N++;

    /* BoW */
    _m->N[d] = N;
    _m->U[d] = (int *)malloc(N * sizeof(int));
    _m->C[d] = (int *)malloc(N * sizeof(int));
    for(n=0, v=0; v<_m->V; v++){
      if(C[v] > 0){
        _m->U[d][n] = v;
        _m->C[d][n] = C[v];
        n++;
      }
    }
  }
  free(C);

  /*------------------------------------*/
  /* init parameters */
  /*------------------------------------*/
  /* p(th ; al), p(ph ; bt) */
  _m->al = iprob_new_ary(_m->K, 0);
  _m->bt = iprob_new_ary(_m->V, 0);

  /* q(th ; al'), q(ph ; bt'), q(z ; th', ph') */
  _m->alp = iprob_new_matrix(_m->D, _m->K, 0);
  _m->btp = iprob_new_matrix(_m->K, _m->V, 0);
  _m->thp = iprob_new_matrix(_m->D, _m->K, 0);
  _m->php = iprob_new_matrix(_m->K, _m->V, 0);
  _m->gmp = (prob ***)malloc(_m->D * sizeof(prob **));
  for(d=0; d<_m->D; d++)
    _m->gmp[d] = iprob_new_matrix(_m->N[d], _m->K, 0);

  /*------------------------------------*/
  /* set values */
  /*------------------------------------*/
  _m->iter = 0;
  _m->radius = 0;
  _m->ALPHA = 1e-1;
  _m->BETA  = 1e-1;
  _m->A0    = 2.0;
  _m->B0    = 1.0;

  return _m;
}
/*------------------------------------*/
/* free */
/*------------------------------------*/
void ilda_free(ilda *_m)
{
  int d;

  /* hyperparameters */
  free(_m->al);
  free(_m->bt);

  /* variational parameters */
  iprob_free_matrix(_m->D, _m->alp);
  iprob_free_matrix(_m->K, _m->btp);
  iprob_free_matrix(_m->D, _m->thp);
  iprob_free_matrix(_m->K, _m->php);
  for(d=0; d<_m->D; d++)
    iprob_free_matrix(_m->N[d], _m->gmp[d]);
  free(_m->gmp);

  /* documents */
  for(d=0; d<_m->D; d++){
    free(_m->U[d]);
    free(_m->C[d]);
  }
  free(_m->U);
  free(_m->C);
  free(_m->N);
  free(_m->L);

  /* main */
  free(_m);
}

