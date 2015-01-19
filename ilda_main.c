#include "ilda_main.h"

/*------------------------------------*/
/* new */
/*------------------------------------*/
ilda *ilda_new(char const *_file, ui _K)
{
  ilda *_m;
  icsv *f;
  ui d, l, v, n;
  ui *C, N;

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
  _m->TL = 0;
  _m->L = (ui *)malloc(_m->D * sizeof(ui));
  _m->X = (ui **)malloc(_m->D * sizeof(ui *));
  for(d=0; d<_m->D; d++){
    _m->TL += (_m->L[d] = icsv_num_item(f, d));
    _m->X[d] = (ui *)malloc(_m->L[d] * sizeof(ui));
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
  C = (ui *)malloc(_m->V * sizeof(ui));
  _m->TN = 0;
  _m->N = (ui *)malloc(_m->D * sizeof(ui));
  _m->U = (ui **)malloc(_m->D * sizeof(ui *));
  _m->C = (ui **)malloc(_m->D * sizeof(ui *));
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
    _m->TN += (_m->N[d] = N);
    _m->U[d] = (ui *)malloc(N * sizeof(ui));
    _m->C[d] = (ui *)malloc(N * sizeof(ui));
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
  /* hyper parameters */
  _m->al = iprob_new_ary(_m->K, 0);
  _m->bt = iprob_new_ary(_m->V, 0);

  /* variational parameters */
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
  _m->ALPHA = 1.0;
  _m->BETA  = 1.0;
  _m->A0    = 2.0;
  _m->B0    = 1.0;
  _m->T     = itime_new();

  return _m;
}
/*------------------------------------*/
/* free */
/*------------------------------------*/
void ilda_free(ilda *_m)
{
  ui d;

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
    free(_m->X[d]);
  }
  free(_m->U);
  free(_m->C);
  free(_m->X);
  free(_m->N);
  free(_m->L);

  /* time */
  itime_free(_m->T);

  /* main */
  free(_m);
}

