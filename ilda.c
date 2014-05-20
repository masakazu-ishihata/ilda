#include "ilda.h"

/*----------------------------------------------------------------------------*/
/* new / free */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* new */
/*------------------------------------*/
ilda *ilda_new(char const *_file, int _K)
{
  ilda *_m;
  icsv *f;
  int d, l, v, c;

  /*------------------------------------*/
  /* initialize */
  /*------------------------------------*/
  _m = (ilda *)imalloc(sizeof(ilda), "ilda_new");

  /*------------------------------------*/
  /* load twd file */
  /*------------------------------------*/
  f = icsv_new_delimiter(_file, " ");

  /* # documents, vocabulary size */
  _m->D = icsv_num_line(f);
  _m->V = 0;
  _m->K = _K;

  /* load documents */
  _m->dl = (int *)imalloc(_m->D * sizeof(int), "ilda_new");
  _m->dv = (int **)imalloc(_m->D * sizeof(int *), "ilda_new");
  _m->dc = (int **)imalloc(_m->D * sizeof(int *), "ilda_new");
  for(d=0; d<_m->D; d++){
    _m->dl[d] = icsv_num_item(f, d);
    _m->dv[d] = (int *)imalloc(_m->dl[d] * sizeof(int), "ilda_new");
    _m->dc[d] = (int *)imalloc(_m->dl[d] * sizeof(int), "ilda_new");
    for(l=0; l<_m->dl[d]; l++){
      v = atoi( strtok(icsv_get(f, d, l), ":") ) - 1;
      c = atoi( strtok(NULL, ":") );
      _m->dv[d][l] = v;
      _m->dc[d][l] = c;
      if(_m->V < v) _m->V = v;
    }
  }
  _m->V++;
  icsv_free(f);

  /*------------------------------------*/
  /* init parameters */
  /*------------------------------------*/
  /* p(th ; al), p(ph ; bt) */
  _m->al = iprob_new_ary(_m->K, ALPHA);
  _m->bt = iprob_new_ary(_m->V, BETA);

  /* q(th ; al'), q(ph ; bt') */
  _m->alp = iprob_new_matrix(_m->D, _m->K, 0);
  _m->btp = iprob_new_matrix(_m->K, _m->V, 0);

  /* q(z ; th, ph) */
  _m->th = iprob_new_matrix(_m->D, _m->K, 0);
  _m->ph = iprob_new_matrix(_m->K, _m->V, 0);
  iprob_rand_matrix(_m->D, _m->K, _m->th);
  iprob_rand_matrix(_m->K, _m->V, _m->ph);


  /*------------------------------------*/
  /* set values */
  /*------------------------------------*/
  _m->iter = 0;
  _m->radius = 0;
  _m->t_fpi = 0;
  _m->t_vbestep = 0;
  _m->t_vbmstep = 0;
  _m->t_vfe = 0;

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

  /* parameters */
  iprob_free_matrix(_m->D, _m->th);
  iprob_free_matrix(_m->K, _m->ph);

  /* variational parameters */
  iprob_free_matrix(_m->D, _m->alp);
  iprob_free_matrix(_m->K, _m->btp);

  /* documents */
  for(d=0; d<_m->D; d++){
    free(_m->dv[d]);
    free(_m->dc[d]);
  }
  free(_m->dv);
  free(_m->dc);
  free(_m->dl);

  /* main */
  free(_m);
}

/*----------------------------------------------------------------------------*/
/* VB */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* main */
/*------------------------------------*/
prob ilda_vb_main(ilda *_m)
{
  int i;
  prob vfe0=0, vfe=0;

  /* initial */
  ilda_vb_estep(_m);
  ilda_vb_mstep(_m);
  vfe0 = ilda_vb_vfe(_m);
  if(!DEBUG) printf("    0, %.15Le, %Le, %.5e\n", vfe0, (prob)0, ilda_time(_m));

  /* learn */
  for(i=1; i<=_m->iter; i++){
    ilda_vb_fpi(_m);
    ilda_vb_estep(_m);
    ilda_vb_mstep(_m);
    vfe = ilda_vb_vfe(_m);
    if(!DEBUG) printf("%5d, %.15Le, %Le, %.5e\n", i, vfe, (vfe0-vfe)/vfe, ilda_time(_m));
    if(!DEBUG && (vfe0-vfe)/vfe < _m->radius) break;
    vfe0 = vfe;
  }
  return vfe0;
}

/*------------------------------------*/
/* E-step */
/*------------------------------------*/
void ilda_vb_estep(ilda *_m)
{
  int d, k, v, l, c;
  prob sum, ex;
  double t1, t2;

  t1 = itime_get();

  /* init : al'[d][k] = al[k], bt'[k][v] = bt[v] */
  iprob_init_matrix_by_ary(_m->D, _m->K, _m->alp, _m->al);
  iprob_init_matrix_by_ary(_m->K, _m->V, _m->btp, _m->bt);

  /* sum up */
  for(d=0; d<_m->D; d++){
    for(l=0; l<_m->dl[d]; l++){
      v = _m->dv[d][l];
      c = _m->dc[d][l];

      /* normalize */
      for(sum=0, k=0; k<_m->K; sum+=_m->th[d][k]*_m->ph[k][v], k++);

      /* expecations */
      for(k=0; k<_m->K; k++){
        ex = c * _m->th[d][k] * _m->ph[k][v] / sum;
        _m->alp[d][k] += ex;
        _m->btp[k][v] += ex;
      }
    }
  }

  t2 = itime_get();
  _m->t_vbestep += t2-t1;
}

/*------------------------------------*/
/* M-step */
/*------------------------------------*/
void ilda_vb_mstep(ilda *_m)
{
  int d, k, v;
  prob psi_alp0, psi_btp0;
  double t1, t2;

  t1 = itime_get();

  /* update th */
  for(d=0; d<_m->D; d++){
    psi_alp0 = psi( iprob_sum_ary(_m->K, _m->alp[d]) );
    for(k=0; k<_m->K; k++) _m->th[d][k] = exp( psi(_m->alp[d][k]) - psi_alp0 );
  }

  /* update ph */
  for(k=0; k<_m->K; k++){
    psi_btp0 = psi( iprob_sum_ary(_m->V, _m->btp[k]) );
    for(v=0; v<_m->V; v++) _m->ph[k][v] = exp( psi(_m->btp[k][v]) - psi_btp0 );
  }

  t2 = itime_get();
  _m->t_vbmstep += t2-t1;
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
  double t1, t2;

  t1 = itime_get();

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
    _m->al[k] = (A0 - 1 + nu * _m->al[k]) / (B0 + de);
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
    sum += (A0 - 1 + nu * _m->bt[v]) / (B0 + de);
  }
  for(v=0; v<_m->V; v++){
    _m->bt[v] = sum / (prob)_m->V;
  }

  t2 = itime_get();
  _m->t_fpi += t2-t1;
}

/*------------------------------------*/
/* Variationl Free energy */
/*------------------------------------*/
prob ilda_vb_vfe(ilda *_m)
{
  int d, k, v, l, c;
  prob p, lq=0, klth=0, klph=0;
  double t1, t2;

  t1 = itime_get();

  /* log q(d) */
  for(d=0; d<_m->D; d++){
    for(l=0; l<_m->dl[d]; l++){
      v = _m->dv[d][l];
      c = _m->dc[d][l];
      for(p=0, k=0; k<_m->K; k++) p += _m->th[d][k] * _m->ph[k][v];
      lq += c * logl(p);
    }
  }

  /* KL(al'[d] | al) */
  for(d=0; d<_m->D; klth+=irand_dir_kld(_m->K, _m->alp[d], _m->al), d++);

  /* KL(bt'[k] | bt) */
  for(k=0; k<_m->K; klph+=irand_dir_kld(_m->V, _m->btp[k], _m->bt), k++);

  t2 = itime_get();
  _m->t_vfe = t2-t1;

  if(DEBUG) printf("%Le %Le %Le %Le\n", lq, klth, klph, lq-klth-klph);

  return lq - klth - klph;
}

/*----------------------------------------------------------------------------*/
/* util */
/*----------------------------------------------------------------------------*/
double ilda_time(ilda *_m)
{
  return _m->t_fpi + _m->t_vbestep + _m->t_vbmstep + _m->t_vfe;
}

/*----------------------------------------------------------------------------*/
/* import, export, show */
/*----------------------------------------------------------------------------*/
/* import */
void ilda_import(ilda *_m, char const *_body)
{
  char file[256];
  FILE *fp;

  /* al */
  sprintf(file, "%s.al", _body);
  fp = ifopen(file, "r");
  ilda_import_al(_m, fp);
  fclose(fp);

  /* bt */
  sprintf(file, "%s.bt", _body);
  fp = ifopen(file, "r");
  ilda_import_bt(_m, fp);
  fclose(fp);

  /* alp */
  sprintf(file, "%s.alp", _body);
  fp = ifopen(file, "r");
  ilda_import_alp(_m, fp);
  fclose(fp);

  /* btp */
  sprintf(file, "%s.btp", _body);
  fp = ifopen(file, "r");
  ilda_import_btp(_m, fp);
  fclose(fp);

  /* vbmstep */
  ilda_vb_mstep(_m);
}
/* al, bt */
void ilda_import_al(ilda *_m, FILE *_fp)
{
  int k;
  prob al;
  while(fscanf(_fp, "%d, %Le\n", &k, &al) != EOF)
    _m->al[k] = al;
}
void ilda_import_bt(ilda *_m, FILE *_fp)
{
  int v;
  prob bt;
  while(fscanf(_fp, "%d, %Le\n", &v, &bt) != EOF)
    if(v < _m->V) _m->bt[v] = bt;
}
/* alp, btp */
void ilda_import_alp(ilda *_m, FILE *_fp)
{
  int d, k;
  prob alp;
  while(fscanf(_fp, "%d, %d, %Le\n", &d, &k, &alp) != EOF)
    _m->alp[d][k] = alp;
}
void ilda_import_btp(ilda *_m, FILE *_fp)
{
  int k, v;
  prob btp;
  while(fscanf(_fp, "%d, %d, %Le\n", &k, &v, &btp) != EOF)
    if(v < _m->V) _m->btp[k][v] = btp;
}
/* export */
void ilda_export(ilda *_m, char const *_body)
{
  char file[256];
  FILE *fp;

  /* al */
  sprintf(file, "%s.al", _body);
  fp = ifopen(file, "w");
  ilda_export_al(_m, fp);
  fclose(fp);

  /* bt */
  sprintf(file, "%s.bt", _body);
  fp = ifopen(file, "w");
  ilda_export_bt(_m, fp);
  fclose(fp);

  /* th */
  sprintf(file, "%s.th", _body);
  fp = ifopen(file, "w");
  ilda_export_th(_m, fp);
  fclose(fp);

  /* ph */
  sprintf(file, "%s.ph", _body);
  fp = ifopen(file, "w");
  ilda_export_ph(_m, fp);
  fclose(fp);

  /* alp */
  sprintf(file, "%s.alp", _body);
  fp = ifopen(file, "w");
  ilda_export_alp(_m, fp);
  fclose(fp);

  /* btp */
  sprintf(file, "%s.btp", _body);
  fp = ifopen(file, "w");
  ilda_export_btp(_m, fp);
  fclose(fp);

  /* topic */
  sprintf(file, "%s.topic", _body);
  fp = ifopen(file, "w");
  ilda_export_topic(_m, fp);
  fclose(fp);

}
/* al, bt */
void ilda_export_al(ilda *_m, FILE *_fp)
{
  int k;
  for(k=0; k<_m->K; k++) fprintf(_fp, "%d, %.15Le\n", k, _m->al[k]);
}
void ilda_export_bt(ilda *_m, FILE *_fp)
{
  int v;
  for(v=0; v<_m->V; v++) fprintf(_fp, "%d, %.15Le\n", v, _m->bt[v]);
}
/* th, ph, gm */
void ilda_export_th(ilda *_m, FILE *_fp)
{
  int d, k;
  for(d=0; d<_m->D; d++){
    for(k=0; k<_m->K; k++){
      fprintf(_fp, "%d, %d, %.15Le\n", d, k, _m->th[d][k]);
    }
  }
}
void ilda_export_ph(ilda *_m, FILE *_fp)
{
  int k, v;
  for(k=0; k<_m->K; k++){
    for(v=0; v<_m->V; v++){
      fprintf(_fp, "%d, %d, %.15Le\n", k, v, _m->ph[k][v]);
    }
  }
}
/* alp, btp */
void ilda_export_alp(ilda *_m, FILE *_fp)
{
  int d, k;
  for(d=0; d<_m->D; d++){
    for(k=0; k<_m->K; k++){
      fprintf(_fp, "%d, %d, %.15Le\n", d, k, _m->alp[d][k]);
    }
  }
}
void ilda_export_btp(ilda *_m, FILE *_fp)
{
  int k, v;
  for(k=0; k<_m->K; k++){
    for(v=0; v<_m->V; v++){
      fprintf(_fp, "%d, %d, %.15Le\n", k, v, _m->btp[k][v]);
    }
  }
}
/* topic */
void ilda_export_topic(ilda *_m, FILE *_fp)
{
  int d, k, v, l, c, max_k;
  prob p, max_p;

  for(d=0; d<_m->D; d++){
    for(l=0; l<_m->dl[d]; l++){
      v = _m->dv[d][l];
      c = _m->dc[d][l];

      /* estimate */
      max_k =  0;
      max_p = -1;
      for(k=0; k<_m->K; k++){
        p = _m->th[d][k] * _m->ph[k][v];
        if(max_p < p){
          max_k = k;
          max_p = p;
        }
      }

      /* show */
      fprintf(_fp, "%d, %d, %d, %d\n", d, v, c, max_k);
    }
  }
}

/*----------------------------------------------------------------------------*/
/* show */
/*----------------------------------------------------------------------------*/
void ilda_show(ilda *_m, FILE *_fp)
{
  ilda_show_hyperparameter(_m, _fp);
  ilda_show_parameter(_m, _fp);
  ilda_show_variationalparameter(_m, _fp);
  ilda_show_topic(_m, _fp);
}
/* hyper parameters */
void ilda_show_hyperparameter(ilda *_m, FILE *_fp)
{
  fprintf(_fp, "#### hyperparameters ####\n");
  fprintf(_fp, "# k, al[k]\n");
  ilda_export_al(_m, _fp);
  fprintf(_fp, "# v, bl[v]\n");
  ilda_export_bt(_m, _fp);
}
/* parameters */
void ilda_show_parameter(ilda *_m, FILE *_fp)
{
  fprintf(_fp, "#### parameters ####\n");
  fprintf(_fp, "# d, k, th[d][k]\n");
  ilda_export_th(_m, _fp);
  fprintf(_fp, "# k, v, ph[k][v]\n");
  ilda_export_ph(_m, _fp);
}
/* variational parameter */
void ilda_show_variationalparameter(ilda *_m, FILE *_fp)
{
  fprintf(_fp, "#### variational parameters ####\n");
  fprintf(_fp, "# d, k, al'[d][k]\n");
  ilda_export_alp(_m, _fp);
  fprintf(_fp, "# k, v, bt'[k][v]\n");
  ilda_export_btp(_m, _fp);
}
/* topic */
void ilda_show_topic(ilda *_m, FILE *_fp)
{
  fprintf(_fp, "#### expected topics ####\n");
  fprintf(_fp, "# d, v, c, extimated k\n");
  ilda_export_topic(_m, _fp);
}
