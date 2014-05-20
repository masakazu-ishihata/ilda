#ifndef _ILDA_H_
#define _ILDA_H_

/*----------------------------------------------------------------------------*/
/* include */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include "irand.h"
#include "iutil.h"
#include "icsv.h"

#define A0 (2.0)
#define B0 (1.0)
#define ALPHA (1.0e-1)
#define BETA  (1.0e-1)
#define ALMOST_ZERO (1e-18)

#define DEBUG (0)

/*----------------------------------------------------------------------------*/
/* ilda */
/*----------------------------------------------------------------------------*/
typedef struct ILDA
{
  /* constants */
  int D; /* # documents */
  int V; /* # words */
  int K; /* # topics */

  /* documents */
  int *dl;   /* D : document length */
  int **dv;  /* D * dl[d] : word index */
  int **dc;  /* D * dl[d] : word count */

  /* hyper parameters */
  prob *al; /* K : th[d] ~ Dir(al) */
  prob *bt; /* V : th[k] ~ Dir(bt) */

  /* parameters */
  prob **th; /* D * K */
  prob **ph; /* K * V */

  /* variational parameters */
  prob **alp; /* D * K : al'[d][k] */
  prob **btp; /* K * V : bt'[k][v] */

  /* learning setting */
  int iter;
  double radius;

  /* time */
  double t_vbestep, t_vbmstep, t_fpi, t_vfe;
} ilda;

/*------------------------------------*/
/* new / free */
/*------------------------------------*/
ilda *ilda_new(char const *_file, int _K);
void ilda_free(ilda *_m);

/*------------------------------------*/
/* vb */
/*------------------------------------*/
prob ilda_vb_main(ilda *_m);
void ilda_vb_estep(ilda *_m);
void ilda_vb_mstep(ilda *_m);
void ilda_vb_fpi(ilda *_m);
prob ilda_vb_vfe(ilda *_m);

/*------------------------------------*/
/* util */
/*------------------------------------*/
double ilda_time(ilda *_m);

/*------------------------------------*/
/* export, show */
/*------------------------------------*/
/* import */
void ilda_import(ilda *_m, char const *_body);
void ilda_import_al(ilda *_m, FILE *_fp);
void ilda_import_bt(ilda *_m, FILE *_fp);
void ilda_import_alp(ilda *_m, FILE *_fp);
void ilda_import_btp(ilda *_m, FILE *_fp);

/* export */
void ilda_export(ilda *_m, char const *_body);
void ilda_export_al(ilda *_m, FILE *_fp);
void ilda_export_bt(ilda *_m, FILE *_fp);
void ilda_export_th(ilda *_m, FILE *_fp);
void ilda_export_ph(ilda *_m, FILE *_fp);
void ilda_export_gm(ilda *_m, FILE *_fp);
void ilda_export_alp(ilda *_m, FILE *_fp);
void ilda_export_btp(ilda *_m, FILE *_fp);
void ilda_export_topic(ilda *_m, FILE *_fp);

/* show */
void ilda_show(ilda *_m, FILE *_fp);
void ilda_show_hyperparameter(ilda *_m, FILE *_fp);
void ilda_show_parameter(ilda *_m, FILE *_fp);
void ilda_show_variationalparameter(ilda *_m, FILE *_fp);
void ilda_show_topic(ilda *_m, FILE *_fp);

#endif
