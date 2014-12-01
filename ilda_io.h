#ifndef _ILDA_IO_H_
#define _ILDA_IO_H_

/* include */
#include "ilda_main.h"

/* import */
void ilda_import(ilda *_m, char const *_body);
void ilda_import_al(ilda *_m, FILE *_fp);
void ilda_import_bt(ilda *_m, FILE *_fp);
void ilda_import_alp(ilda *_m, FILE *_fp);
void ilda_import_btp(ilda *_m, FILE *_fp);
void ilda_import_thp(ilda *_m, FILE *_fp);
void ilda_import_php(ilda *_m, FILE *_fp);

/* export */
void ilda_export(ilda *_m, char const *_body);
void ilda_export_al(ilda *_m, FILE *_fp);
void ilda_export_bt(ilda *_m, FILE *_fp);
void ilda_export_alp(ilda *_m, FILE *_fp);
void ilda_export_btp(ilda *_m, FILE *_fp);
void ilda_export_thp(ilda *_m, FILE *_fp);
void ilda_export_php(ilda *_m, FILE *_fp);
void ilda_export_gmp(ilda *_m, FILE *_fp);
void ilda_export_topic(ilda *_m, FILE *_fp);

/* show */
void ilda_show(ilda *_m, FILE *_fp);
void ilda_show_hyperparameter(ilda *_m, FILE *_fp);
void ilda_show_variationalparameter(ilda *_m, FILE *_fp);
void ilda_show_topic(ilda *_m, FILE *_fp);

#endif
