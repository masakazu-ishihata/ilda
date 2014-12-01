#include "ilda_io.h"

/*----------------------------------------------------------------------------*/
/* import */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* import all */
/*------------------------------------*/
void ilda_import(ilda *_m, char const *_body)
{
  char file[256];
  FILE *fp;

  /* al */
  sprintf(file, "%s.al", _body);
  fp = fopen(file, "r");
  ilda_import_al(_m, fp);
  fclose(fp);

  /* bt */
  sprintf(file, "%s.bt", _body);
  fp = fopen(file, "r");
  ilda_import_bt(_m, fp);
  fclose(fp);

  /* alp */
  sprintf(file, "%s.alp", _body);
  fp = fopen(file, "r");
  ilda_import_alp(_m, fp);
  fclose(fp);

  /* btp */
  sprintf(file, "%s.btp", _body);
  fp = fopen(file, "r");
  ilda_import_btp(_m, fp);
  fclose(fp);

  /* thp */
  sprintf(file, "%s.thp", _body);
  fp = fopen(file, "r");
  ilda_import_thp(_m, fp);
  fclose(fp);

  /* php */
  sprintf(file, "%s.php", _body);
  fp = fopen(file, "r");
  ilda_import_php(_m, fp);
  fclose(fp);
}
/*------------------------------------*/
/* al, bt */
/*------------------------------------*/
void ilda_import_al(ilda *_m, FILE *_fp)
{
  int k;
  prob al;
  for(k=0; k<_m->K; k++){
    if(fscanf(_fp, "%Le", &al) == EOF){
      printf("error @ ilda_import_al : invalid load\n");
      exit(1);
    }
    _m->al[k] = al;
  }
}
void ilda_import_bt(ilda *_m, FILE *_fp)
{
  int v;
  prob bt;
  for(v=0; v<_m->V; v++){
    if(fscanf(_fp, "%Le", &bt) == EOF){
      printf("error @ ilda_import_bt : invalid load\n");
      exit(1);
    }
    _m->bt[v] = bt;
  }
}
/*------------------------------------*/
/* alp, btp, thp, php */
/*------------------------------------*/
void ilda_import_alp(ilda *_m, FILE *_fp)
{
  int d, k;
  prob alp;
  for(d=0; d<_m->D; d++){
    for(k=0; k<_m->K; k++){
      if(fscanf(_fp, "%Le", &alp) == EOF){
        printf("error @ ilda_import_alp : invalid load\n");
        exit(1);
      }
      _m->alp[d][k] = alp;
    }
  }
}
void ilda_import_btp(ilda *_m, FILE *_fp)
{
  int k, v;
  prob btp;
  for(k=0; k<_m->K; k++){
    for(v=0; v<_m->V; v++){
      if(fscanf(_fp, "%Le", &btp) == EOF){
        printf("error @ ilda_import_btp : invalid load\n");
        exit(1);
      }
      _m->btp[k][v] = btp;
    }
  }
}
void ilda_import_thp(ilda *_m, FILE *_fp)
{
  int d, k;
  prob thp;
  for(d=0; d<_m->D; d++){
    for(k=0; k<_m->K; k++){
      if(fscanf(_fp, "%Le", &thp) == EOF){
        printf("error @ ilda_import_thp : invalid load\n");
        exit(1);
      }
      _m->thp[d][k] = thp;
    }
  }
}
void ilda_import_php(ilda *_m, FILE *_fp)
{
  int k, v;
  prob php;
  for(k=0; k<_m->K; k++){
    for(v=0; v<_m->V; v++){
      if(fscanf(_fp, "%Le", &php) == EOF){
        printf("error @ ilda_import_php : invalid load\n");
        exit(1);
      }
      _m->php[k][v] = php;
    }
  }
}

/*----------------------------------------------------------------------------*/
/* export */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* export all */
/*------------------------------------*/
void ilda_export(ilda *_m, char const *_body)
{
  char file[256];
  FILE *fp;

  /* al */
  sprintf(file, "%s.al", _body);
  fp = fopen(file, "w");
  ilda_export_al(_m, fp);
  fclose(fp);

  /* bt */
  sprintf(file, "%s.bt", _body);
  fp = fopen(file, "w");
  ilda_export_bt(_m, fp);
  fclose(fp);

  /* alp */
  sprintf(file, "%s.alp", _body);
  fp = fopen(file, "w");
  ilda_export_alp(_m, fp);
  fclose(fp);

  /* btp */
  sprintf(file, "%s.btp", _body);
  fp = fopen(file, "w");
  ilda_export_btp(_m, fp);
  fclose(fp);

  /* thp */
  sprintf(file, "%s.thp", _body);
  fp = fopen(file, "w");
  ilda_export_thp(_m, fp);
  fclose(fp);

  /* php */
  sprintf(file, "%s.php", _body);
  fp = fopen(file, "w");
  ilda_export_php(_m, fp);
  fclose(fp);

  /* gmp
  sprintf(file, "%s.gmp", _body);
  fp = fopen(file, "w");
  ilda_export_gmp(_m, fp);
  fclose(fp);
  */

  /* topic
  sprintf(file, "%s.topic", _body);
  fp = fopen(file, "w");
  ilda_export_topic(_m, fp);
  fclose(fp);
  */
}
/*------------------------------------*/
/* al, bt */
/*------------------------------------*/
void ilda_export_al(ilda *_m, FILE *_fp)
{
  int k;
  fprintf(_fp, "%.15Le", _m->al[0]);
  for(k=1; k<_m->K; k++)
    fprintf(_fp, " %.15Le", _m->al[k]);
  fprintf(_fp, "\n");
}
void ilda_export_bt(ilda *_m, FILE *_fp)
{
  int v;
  fprintf(_fp, "%.15Le", _m->bt[0]);
  for(v=0; v<_m->V; v++)
    fprintf(_fp, " %.15Le", _m->bt[v]);
  fprintf(_fp, "\n");
}
/*------------------------------------*/
/* alp, btp, thp */
/*------------------------------------*/
void ilda_export_alp(ilda *_m, FILE *_fp)
{
  int d, k;
  for(d=0; d<_m->D; d++){
    fprintf(_fp, "%.15Le", _m->alp[d][0]);
    for(k=1; k<_m->K; k++) fprintf(_fp, " %.15Le", _m->alp[d][k]);
    fprintf(_fp, "\n");
  }
}
void ilda_export_btp(ilda *_m, FILE *_fp)
{
  int k, v;
  for(k=0; k<_m->K; k++){
    fprintf(_fp, "%.15Le", _m->btp[k][0]);
    for(v=1; v<_m->V; v++) fprintf(_fp, " %.15Le", _m->btp[k][v]);
    fprintf(_fp, "\n");
  }
}
void ilda_export_thp(ilda *_m, FILE *_fp)
{
  int d, k;
  for(d=0; d<_m->D; d++){
    fprintf(_fp, "%.15Le", _m->thp[d][0]);
    for(k=1; k<_m->K; k++) fprintf(_fp, " %.15Le", _m->thp[d][k]);
    fprintf(_fp, "\n");
  }
}
void ilda_export_php(ilda *_m, FILE *_fp)
{
  int k, v;
  for(k=0; k<_m->K; k++){
    fprintf(_fp, "%.15Le", _m->php[k][0]);
    for(v=1; v<_m->V; v++) fprintf(_fp, " %.15Le", _m->php[k][v]);
    fprintf(_fp, "\n");
  }
}
void ilda_export_gmp(ilda *_m, FILE *_fp)
{
  int d, n, k;
  for(d=0; d<_m->D; d++){
    for(n=0; n<_m->N[d]; n++){
      fprintf(_fp, "%.15Le", _m->gmp[d][n][0]);
      for(k=1; k<_m->K; k++) fprintf(_fp, " %.15Le", _m->gmp[d][n][k]);
      fprintf(_fp, "\n");
    }
  }
}
/*------------------------------------*/
/* topic */
/*------------------------------------*/
void ilda_export_topic(ilda *_m, FILE *_fp)
{
  int d, k, v, max_k;
  prob p, max_p;

  for(d=0; d<_m->D; d++){
    for(v=0; v<_m->V; v++){
      /* estimate */
      max_k =  0;
      max_p = -1;
      for(k=0; k<_m->K; k++){
        p = _m->thp[d][k] * _m->php[k][v];
        if(max_p < p){
          max_k = k;
          max_p = p;
        }
      }

      /* show */
      fprintf(_fp, "%3d", max_k);
    }
    fprintf(_fp, "\n");
  }
}

/*----------------------------------------------------------------------------*/
/* show */
/*----------------------------------------------------------------------------*/
void ilda_show(ilda *_m, FILE *_fp)
{
  ilda_show_hyperparameter(_m, _fp);
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
/* variational parameter */
void ilda_show_variationalparameter(ilda *_m, FILE *_fp)
{
  fprintf(_fp, "#### variational parameters ####\n");
  fprintf(_fp, "# d, k, al'[d][k]\n");
  ilda_export_alp(_m, _fp);
  fprintf(_fp, "# k, v, bt'[k][v]\n");
  ilda_export_btp(_m, _fp);
  fprintf(_fp, "# d, k, th'[d][k]\n");
  ilda_export_thp(_m, _fp);
  fprintf(_fp, "# k, v, ph'[k][v]\n");
  ilda_export_php(_m, _fp);
}
/* topic */
void ilda_show_topic(ilda *_m, FILE *_fp)
{
  fprintf(_fp, "#### expected topics ####\n");
  fprintf(_fp, "# d, v, c, extimated k\n");
  ilda_export_topic(_m, _fp);
}
