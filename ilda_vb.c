#include "ilda_vb.h"

/*----------------------------------------------------------------------------*/
/* VB */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* main */
/*------------------------------------*/
prob ilda_vb_train(ilda *_m)
{
  int i;
  prob vfe0=0, vfe=0;
  double t = itime_get();
  /* 0-step */
  ilda_vb_init(_m);
  vfe0 = ilda_vb_vfe(_m);
  if(!DEBUG) printf("    0 %.15Le %Le %.5e\n", vfe0, (prob)0, itime_get()-t);

  /* learn */
  for(i=1; i<=_m->iter; i++){
    ilda_vb_estep(_m);
    ilda_vb_mstep(_m);
    ilda_vb_fpi(_m);
    vfe = ilda_vb_vfe(_m);
    if(!DEBUG) printf("%5d %.15Le %Le %.5e\n", i, vfe, (vfe0-vfe)/vfe, itime_get()-t);
    if((vfe0-vfe)/vfe < _m->radius) break;
    vfe0 = vfe;
  }

  return vfe0;
}
/*------------------------------------*/
/* initialize */
/*------------------------------------*/
void ilda_vb_init(ilda *_m)
{
  int d, v, k;

  /* init al & bt */
  for(k=0; k<_m->K; k++)
    _m->al[k] = _m->ALPHA;

  for(v=0; v<_m->V; v++)
    _m->bt[v] = _m->BETA;

  /* init al' & bt' */
  iprob_rand_matrix(_m->D, _m->K, _m->alp);
  iprob_rand_matrix(_m->K, _m->V, _m->btp);
  for(d=0; d<_m->D; d++)
    for(k=0; k<_m->K; k++)
      _m->alp[d][k] += _m->ALPHA;

  for(k=0; k<_m->K; k++)
    for(v=0; v<_m->V; v++)
      _m->btp[k][v] += _m->BETA;

  /* init th', ph' & gm' */
  ilda_vb_update_thp(_m);
  ilda_vb_update_php(_m);
  ilda_vb_update_gmp(_m);
}
/*------------------------------------*/
/* E-step : update al' & bt' */
/*------------------------------------*/
void ilda_vb_estep(ilda *_m)
{
  int d, k, n, v;
  prob e;

  /* init : bt'[k][v] = bt[v] */
  iprob_init_matrix_by_ary(_m->D, _m->K, _m->alp, _m->al);
  iprob_init_matrix_by_ary(_m->K, _m->V, _m->btp, _m->bt);

  /* sum up */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      for(k=0; k<_m->K; k++){
        v = _m->U[d][n];
        e = _m->C[d][n] * _m->gmp[d][n][k];
        _m->alp[d][k] += e;
        _m->btp[k][v] += e;
      }
    }
  }
}
/*------------------------------------*/
/* M-step : update th' & ph' */
/*------------------------------------*/
void ilda_vb_mstep(ilda *_m)
{
  ilda_vb_update_thp(_m);
  ilda_vb_update_php(_m);
  ilda_vb_update_gmp(_m);
}
/*------------------------------------*/
/* udpate al' */
/*------------------------------------*/
void ilda_vb_update_alp(ilda *_m)
{
  int d, k, n;

  /* sum up */
  for(d=0; d<_m->D; d++){
    for(k=0; k<_m->K; k++){
      _m->alp[d][k] = _m->al[k];
      for(n=0; n<_m->N[d]; n++)
        _m->alp[d][k] += _m->C[d][n] * _m->gmp[d][n][k];
    }
  }
}
/*------------------------------------*/
/* update bt' */
/*------------------------------------*/
void ilda_vb_update_btp(ilda *_m)
{
  int d, k, n, v, c;
  prob e;

  /* init : bt'[k][v] = bt[v] */
  iprob_init_matrix_by_ary(_m->K, _m->V, _m->btp, _m->bt);

  /* sum up */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      for(k=0; k<_m->K; k++){
        v = _m->U[d][n];
        c = _m->C[d][n];
        e = c * _m->gmp[d][n][k];
        _m->btp[k][v] += e;
      }
    }
  }
}
/*------------------------------------*/
/* update th' */
/*------------------------------------*/
void ilda_vb_update_thp(ilda *_m)
{
  int d, k;
  prob psi_alp0;
  for(d=0; d<_m->D; d++){
    psi_alp0 = psi( iprob_sum_ary(_m->K, _m->alp[d]) );
    for(k=0; k<_m->K; k++)
      _m->thp[d][k] = exp( psi(_m->alp[d][k]) - psi_alp0 );
  }
}
/*------------------------------------*/
/* update ph' */
/*------------------------------------*/
void ilda_vb_update_php(ilda *_m)
{
  int k, v;
  prob psi_btp0;
  for(k=0; k<_m->K; k++){
    psi_btp0 = psi( iprob_sum_ary(_m->V, _m->btp[k]) );
    for(v=0; v<_m->V; v++)
      _m->php[k][v] = exp( psi(_m->btp[k][v]) - psi_btp0 );
  }
}
/*------------------------------------*/
/* update gm' */
/*------------------------------------*/
void ilda_vb_update_gmp(ilda *_m)
{
  int d, k, n, v;
  prob sum;

  /* update gmp */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      v = _m->U[d][n];

      /* normalized const */
      sum = 0;
      for(k=0; k<_m->K; k++)
        sum += _m->thp[d][k] * _m->php[k][v];

      /* update */
      for(k=0; k<_m->K; k++)
        _m->gmp[d][n][k] = (_m->thp[d][k] * _m->php[k][v]) / sum;
    }
  }
}

/*------------------------------------*/
/* Fixed point iteration */
/*------------------------------------*/
void ilda_vb_fpi(ilda *_m)
{
  int d, k, v;
  prob de, nu;
  prob al0, alp0;
  prob bt0, btp0;
  prob sum;

  /*------------------------------------*/
  /* al */
  /*------------------------------------*/
  /* denominator */
  for(de=0, d=0; d<_m->D; d++){
    alp0 = iprob_sum_ary(_m->K, _m->alp[d]);
    al0  = iprob_sum_ary(_m->K, _m->al);
    de += psi(alp0) - psi(al0);
  }
  for(k=0; k<_m->K; k++){
    /* numerator */
    for(nu=0, d=0; d<_m->D; d++)
      nu += psi(_m->alp[d][k]) - psi(_m->al[k]);

    /* update */
    _m->al[k] = (_m->A0 - 1 + nu * _m->al[k]) / (_m->B0 + de);
  }

  /*------------------------------------*/
  /* bt */
  /*------------------------------------*/
  /* denominator */
  for(de=0, k=0; k<_m->K; k++){
    btp0 = iprob_sum_ary(_m->V, _m->btp[k]);
    bt0  = iprob_sum_ary(_m->V, _m->bt);
    de += psi(btp0) - psi(bt0);
  }
  sum = 0;
  for(v=0; v<_m->V; v++){
    /* numerator */
    for(nu=0, k=0; k<_m->K; k++)
      nu += psi(_m->btp[k][v]) - psi(_m->bt[v]);

    /* update */
    //    _m->bt[v] = (A0 - 1 + nu * _m->bt[v]) / (B0 + de);
    sum += (_m->A0 - 1 + nu * _m->bt[v]) / (_m->B0 + de);
  }
  for(v=0; v<_m->V; v++){
    _m->bt[v] = sum / (prob)_m->V;
  }
}

/*------------------------------------*/
/* Variationl Free energy */
/*------------------------------------*/
prob ilda_vb_vfe(ilda *_m)
{
  int d, k, c, n, v;
  prob p, lq=0, klth=0, klph=0;

  /* log q(d) */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      v = _m->U[d][n];
      c = _m->C[d][n];
      for(p=0, k=0; k<_m->K; k++) p += _m->thp[d][k] * _m->php[k][v];
      lq += c * logl(p);
    }
  }

  /* KL(al'[d] | al) */
  for(d=0; d<_m->D; klth+=irand_dir_kld(_m->K, _m->alp[d], _m->al), d++);

  /* KL(bt'[k] | bt) */
  for(k=0; k<_m->K; klph+=irand_dir_kld(_m->V, _m->btp[k], _m->bt), k++);

  if(DEBUG) printf("%Le %Le %Le %Le\n", lq, klth, klph, lq-klth-klph);

  return lq - klth - klph;
}


/*------------------------------------*/
/* test */
/*------------------------------------*/
prob ilda_vb_test(ilda *_m)
{
  prob vfe = ilda_vb_vfe(_m);
  prob lpp = ilda_vb_perplexity(_m);
  printf("%Le %Le\n", vfe, lpp);
  return lpp;
}

/*------------------------------------*/
/* perplexity */
/*------------------------------------*/
prob ilda_vb_perplexity(ilda *_m)
{
  int d, k, n, v, c;
  prob sum, lp = 0;

  /* th' \propto al' */
  for(d=0; d<_m->D; d++){
    iprob_copy_ary(_m->K, _m->thp[d], _m->alp[d]);
    iprob_normalize_ary(_m->K, _m->thp[d]);
  }

  /* ph' \propto bt' */
  for(k=0; k<_m->K; k++){
    iprob_copy_ary(_m->V, _m->php[k], _m->btp[k]);
    iprob_normalize_ary(_m->V, _m->php[k]);
  }

  /* perprexity */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      v = _m->U[d][n];
      c = _m->C[d][n];
      sum = 0;
      for(k=0; k<_m->K; k++)
        sum += _m->thp[d][k] * _m->php[k][v];
      lp += c * logl(sum);
    }
  }

  return lp;
}
