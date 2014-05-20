#include "main.h"

int main(int argc, char *argv[])
{
  ilda *m;

  /*------------------------------------*/
  /* arguments */
  /*------------------------------------*/
  /* options */
  char input[256]  = "input.twd";
  char output[256] = "model";
  char load[256]   = "";
  int topic = 5;
  int seed  = (int)time(NULL);
  int iter  = 10;
  double radius = 1e-5;

  /* optarg */
  int opt = 0, opt_idx = 0;
  char *opts = "I:O:L:i:s:r:k:C";
  struct option long_opts[] = {
    {"input",     1, NULL, 'I'},
    {"output",    1, NULL, 'O'},
    {"load",      1, NULL, 'L'},
    {"seed",      1, NULL, 's'},
    {"iter",      1, NULL, 'i'},
    {"radius",    1, NULL, 'r'},
    {"topic",     1, NULL, 'k'},
    {0, 0, 0, 0}
  };

  /* parsing */
  while((opt = getopt_long(argc, argv, opts, long_opts, &opt_idx)) != EOF){
    switch(opt){
    case 'I': sprintf(input, "%s", optarg); break;
    case 'O': sprintf(output, "%s", optarg); break;
    case 'L': sprintf(load, "%s", optarg); break;
    case 'i': iter   = atoi(optarg); break;
    case 's': seed   = atoi(optarg); break;
    case 'k': topic  = atoi(optarg); break;
    case 'r': radius = atof(optarg); break;
    case 'h':
    default:
      printf("Usage : ./ilda [options]\n");
      printf("    --input,      -I [string] : input twd file name\n");
      printf("    --output,     -O [string] : output file name\n");
      printf("    --load,       -L [string] : load file name\n");
      printf("    --seed,       -s [int]    : random seed\n");
      printf("    --iter,       -i [int]    : # iteration\n");
      printf("    --radius,     -r [double] : convergence radius\n");
      printf("    --topic,      -k [int]    : # topics\n");
      exit(1);
    }
  }

  /*------------------------------------*/
  /* main */
  /*------------------------------------*/
  /* init */
  isrand((unsigned int)seed);
  m = ilda_new(input, topic);
  printf("#### setting ####\n");
  printf("Input     : %s\n", input);
  printf("Output    : %s\n", output);
  if(strlen(load) > 0)
    printf("Load      : %s.{al, bt, alp, btp}\n", load);
  printf("seed      : %d\n", seed);
  printf("# iter    : %d\n", iter);
  printf("radius    : %e\n", radius);
  printf("# docs    : %d\n", m->D);
  printf("# topics  : %d\n", m->K);
  printf("# words   : %d\n", m->V);

  /* load */
  if(strlen(load) > 0)
    ilda_import(m, load);

  /* learn */
  printf("\n#### learning ####\n");
  m->iter = iter;
  m->radius = radius;
  ilda_vb_main(m);

  /* export */
  ilda_export(m, output);

  /* free */
  ilda_free(m);

  return 0;
}
