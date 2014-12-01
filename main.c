#include "main.h"

int main(int argc, char *argv[])
{
  ilda *m;

  /*------------------------------------*/
  /* arguments */
  /*------------------------------------*/
  /* options */
  char train[256]  = "input.twd";
  char test[256]   = "";
  char model[256] = "model";
  int K = 5;
  int seed  = (int)time(NULL);
  int iter  = 10;
  double radius = 1e-5;

  /* optarg */
  int opt = 0, opt_idx = 0;
  char *opts = "I:O:T:i:s:r:k:C";
  struct option long_opts[] = {
    {"input",     1, NULL, 'I'},
    {"train",     1, NULL, 'I'},
    {"model",     1, NULL, 'O'},
    {"test",      1, NULL, 'T'},
    {"seed",      1, NULL, 's'},
    {"iter",      1, NULL, 'i'},
    {"radius",    1, NULL, 'r'},
    {"topic",     1, NULL, 'k'},
    {0, 0, 0, 0}
  };

  /* parsing */
  while((opt = getopt_long(argc, argv, opts, long_opts, &opt_idx)) != EOF){
    switch(opt){
    case 'I': sprintf(train, "%s", optarg); break;
    case 'O': sprintf(model, "%s", optarg); break;
    case 'T': sprintf(test,  "%s", optarg); break;
    case 'i': iter   = atoi(optarg); break;
    case 's': seed   = atoi(optarg); break;
    case 'k': K      = atoi(optarg); break;
    case 'r': radius = atof(optarg); break;
    case 'h':
    default:
      printf("Usage : ./ilda [options]\n");
      printf("    --input,      -I [string] : train file name\n");
      printf("    --model,      -O [string] : model name\n");
      printf("    --test,       -T [string] : test file name\n");
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
  isrand((unsigned int)seed);

  /* new for train */
  m = ilda_new(train, K);
  m->iter = iter;
  m->radius = radius;

  /* setting */
  printf("#### Setting ####\n");
  printf("Model     : %s\n", model);
  printf("Train     : %s\n", train);
  if(strlen(test) > 0)
    printf("test      : %s\n", test);
  printf("seed      : %d\n", seed);
  printf("# iter    : %d\n", m->iter);
  printf("radius    : %e\n", m->radius);
  printf("# docs    : %d\n", m->D);
  printf("# topics  : %d\n", m->K);
  printf("# words   : %d\n", m->V);

  /* train */
  printf("\n#### Training ####\n");
  ilda_vb_train(m);
  ilda_export(m, model);
  printf("%Le\n", ilda_vb_perplexity(m));
  ilda_free(m);

  /* test */
  if(strlen(test) > 0){
    printf("\n#### Test ####\n");
    m = ilda_new(test, K);
    ilda_import(m, model);
    printf("%Le\n", ilda_vb_perplexity(m));
    ilda_free(m);
  }

  return 0;
}
