#include "ilda_vb.h"

/*------------------------------------*/
/* main */
/*------------------------------------*/

/* training */
prob ilda_vb_train(ilda *_m)
{
  ui i;
  prob vfe0=0, vfe=0;

  /* 0-step */
  itime_start(_m->T); /* start */
  ilda_vb_init(_m);
  ilda_vb_estep(_m);
  ilda_vb_mstep(_m);
  ilda_vb_fpi(_m);
  vfe0 = ilda_vb_vfe(_m);
  itime_stop(_m->T);  /* stop */

  printf("    0 %.15Le %Le %.5e\n", vfe0, (prob)0, itime_total(_m->T));

  /* learn */
  for(i=1; i<=_m->iter; i++){
    itime_start(_m->T); /* start */
    ilda_vb_estep(_m);
    ilda_vb_mstep(_m);
    ilda_vb_fpi(_m);
    vfe = ilda_vb_vfe(_m);
    itime_stop(_m->T);  /* stop */
    printf("%5d %.15Le %Le %.5e\n", i, vfe, (vfe0-vfe)/vfe, itime_total(_m->T));
    if((vfe0-vfe)/vfe < _m->radius) break;
    vfe0 = vfe;
  }

  return vfe0;
}

/* initialize */
void ilda_vb_init(ilda *_m)
{
  /* init al & bt */
  iprob_init_ary(_m->K, _m->al, _m->ALPHA);
  iprob_init_ary(_m->V, _m->bt, _m->BETA);

  /* init al' & bt' */
  iprob_rand_matrix(_m->D, _m->K, _m->alp);
  iprob_rand_matrix(_m->K, _m->V, _m->btp);
  iprob_add_matrix(_m->D, _m->K, _m->alp, _m->ALPHA);
  iprob_add_matrix(_m->K, _m->V, _m->btp, _m->BETA);

  /* init th', ph' & gm' */
  ilda_vb_mstep(_m);
}

/* E-step : update al' & bt' */
void ilda_vb_estep(ilda *_m)
{
  ilda_vb_update_alp(_m);
  ilda_vb_update_btp(_m);
}

/* M-step : update th' & ph' */
void ilda_vb_mstep(ilda *_m)
{
  ilda_vb_update_thp(_m);
  ilda_vb_update_php(_m);
  ilda_vb_update_gmp(_m);
}

/* Fixed point iteration : udpate al & bt */
void ilda_vb_fpi(ilda *_m)
{
  ilda_vb_update_al(_m);
  ilda_vb_update_bt(_m);
}

/*------------------------------------*/
/* updates */
/*------------------------------------*/

/* al'[d][k] */
void ilda_vb_update_alp(ilda *_m)
{
  ui d, k, n;

  /* initialize */
  iprob_init_matrix_by_ary(_m->D, _m->K, _m->alp, _m->al);

  /* sum up */
  for(d=0; d<_m->D; d++)
    for(k=0; k<_m->K; k++)
      for(n=0; n<_m->N[d]; n++)
        _m->alp[d][k] += _m->C[d][n] * _m->gmp[d][n][k];
}

/* bt'[k][v] */
void ilda_vb_update_btp(ilda *_m)
{
  ui d, k, n;

  /* init : bt'[k][v] = bt[v] */
  iprob_init_matrix_by_ary(_m->K, _m->V, _m->btp, _m->bt);

  /* sum up */
  for(d=0; d<_m->D; d++)
    for(n=0; n<_m->N[d]; n++)
      for(k=0; k<_m->K; k++)
        _m->btp[k][ _m->U[d][n] ] += _m->C[d][n] * _m->gmp[d][n][k];
}

/* th'[d][k] */
void ilda_vb_update_thp(ilda *_m)
{
  ui d, k;
  prob psi0;

  for(d=0; d<_m->D; d++){
    psi0 = psi( iprob_sum_ary(_m->K, _m->alp[d]) );
    for(k=0; k<_m->K; k++)
      _m->thp[d][k] = exp( psi(_m->alp[d][k]) - psi0 );
  }
}

/* ph'[k][v] */
void ilda_vb_update_php(ilda *_m)
{
  ui k, v;
  prob psi0;

  for(k=0; k<_m->K; k++){
    psi0 = psi( iprob_sum_ary(_m->V, _m->btp[k]) );
    for(v=0; v<_m->V; v++)
      _m->php[k][v] = exp( psi(_m->btp[k][v]) - psi0 );
  }
}

/* gm'[d][n][k] */
void ilda_vb_update_gmp(ilda *_m)
{
  ui d, k, n, v;
  prob p;

  /* update gmp */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      v = _m->U[d][n];

      /* p = normalized constant */
      for(p=0, k=0; k<_m->K; k++)
        p += _m->thp[d][k] * _m->php[k][v];

      /* update */
      for(k=0; k<_m->K; k++)
        _m->gmp[d][n][k] = (_m->thp[d][k] * _m->php[k][v]) / p;
    }
  }
}

/* al[k] */
void ilda_vb_update_al(ilda *_m)
{
  ui d, k;
  prob de, nu, al0, psi0;

  /* denominator */
  al0  = iprob_sum_ary(_m->K, _m->al);
  psi0 = psi(al0);
  for(de=0, d=0; d<_m->D; d++)
    de += psi( iprob_sum_ary(_m->K, _m->alp[d]) ) - psi0;

  /* numerator */
  for(k=0; k<_m->K; k++){
    psi0 = psi(_m->al[k]);
    for(nu=0, d=0; d<_m->D; d++)
      nu += psi(_m->alp[d][k]) - psi0;

    /* update */
    _m->al[k] = (_m->A0 - 1 + nu * _m->al[k]) / (_m->B0 + de);
  }
}

/* bt[v] */
void ilda_vb_update_bt(ilda *_m)
{
  ui k, v;
  prob de=0, nu=0, bt0, psi0;

  /* denominator */
  bt0  = iprob_sum_ary(_m->V, _m->bt);
  psi0 = psi( bt0 );
  for(k=0; k<_m->K; k++)
    de += psi( iprob_sum_ary(_m->V, _m->btp[k]) ) - psi0;

  /* nu */
  psi0 = psi( _m->bt[0] );
  for(k=0; k<_m->K; k++)
    for(v=0; v<_m->V; v++)
      nu += psi(_m->btp[k][v]) - psi0;

  /* update */
  nu = _m->A0 - 1 + nu * bt0;
  de = _m->B0 + de * _m->V;
  bt0 = nu / de;
  for(v=0; v<_m->V; v++)
    _m->bt[v] = bt0 / (prob)_m->V;
}

/*------------------------------------*/
/* Variationl Free energy */
/*------------------------------------*/

/* VFE */
prob ilda_vb_vfe(ilda *_m)
{
  ui d, k, c, n, v;
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
  for(d=0; d<_m->D; d++)
    klth += irand_dir_kld(_m->K, _m->alp[d], _m->al);

  /* KL(bt'[k] | bt) */
  for(k=0; k<_m->K; k++)
    klph += irand_dir_kld(_m->V, _m->btp[k], _m->bt);

  return lq - klth - klph;
}

/* log likelihood */
prob ilda_vb_log_likelihood(ilda *_m)
{
  ui d, k, n, v, c;
  prob ll = 0, p;

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

  /* log marginal */
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      v = _m->U[d][n];
      c = _m->C[d][n];
      for(p=0, k=0; k<_m->K; k++)
        p += _m->thp[d][k] * _m->php[k][v];
      ll += c * logl(p);
    }
  }

  return ll;
}
