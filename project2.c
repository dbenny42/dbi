#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

#include "project2.h"

/*
 * where num_basic_terms is set from the number of filters gained from
 * this run...
 *
 *
 * sc is the machine configuration file parsed out by main, and sels is a
 * slice of the selectivities array--a single run's filters.
 *
 * in Shimao's code, this is set by incrementing i.
 */

float compute_best_plan(int num_basic_terms, struct sel_conf *sc, sels[NUM_FILTERS])
{
  int subset_idx;
  long num_subsets = 2 ** num_basic_terms;

  /* first, we set up an empty array of the subset state structs. */
  /* in Shimao's code, this is set by multiplying k again and again while
     incrementing j */
  /* 'plans' is the array that we'll use to store plans of increasing
     size. */
  struct subset_state plans[num_subsets];


  /* the low-order 9 bits (at most) of the basic_term_bm are used to
     indicate which basic terms are a part of a plan (of some size) stored
     in 'plans' */
  short basic_term_bm = 0; /* 16 bits is more than enough; we need a maximum of 9 */
  /* the integral representation of basic_term_bm becomes the index into
     plans where that particular subset's cost information is stored. */



  /* generate the cost of the (2^(num_basic_terms) - 1) &-only plans.  for
     each plan, check if the no-branch plan is more effective, then store
     that cost with the no-branch flag set if the cost is better. */ 

  /* we start the idx at 1 because we need non-empty subsets. */
  for (subset_idx = 1; subset_idx < num_subsets; subset_idx++) {
    /* look at page 12 for cost of evaluating &-plans... */
    float cost = compute_bw_and_cost();
    float no_branch_cost = compute_no_branch_cost();
    if (no_branch_cost < cost) {
      /* make appropriate adjustments */
    }
    
  }

  
    
} 


/* 
 * in: the filename and the empty array of selectivities (to be filled)
 * out: the number of lines that were read. 
 */
int parse_query_file(char *fname, float selectivities[][MAX_FILTERS]) {

  char *tok;
  char in_buffer[IN_BUFSIZE] = {0};
  int line_num = 0;
  int filter_num = 0;

  FILE *fp = fopen(fname,"r");
  if (!fp) {
    printf("error opening the query file.");
    printf("exiting on error: %s\n", strerror(errno));
    exit(errno);
  }

  while (fgets(in_buffer, IN_BUFSIZE, fp)) {
      /* do the first tokenization outside while because of need to
         specify buffer on first iteration */
    tok = strtok(in_buffer," ");
    selectivities[line_num][filter_num++] = atof(tok);

    while ((tok = strtok(NULL, " "))) {
      selectivities[line_num][filter_num++] = atof(tok);
    }


    filter_num = 0;
    memset(in_buffer, 0, IN_BUFSIZE); /* ensure in_buffer is null */

    line_num++;
    filter_num = 0;
  }
  fclose(fp);


  return line_num;
}


/* this fn is just defined to clean up the parse_config_file code. */
int parse_config_line(FILE *fp)
{
  /*char buf[10];*/ /* don't need much space for the line. */
  char buf[10];
  char *tok;

  if (!fgets(buf, sizeof(buf), fp)) {
    printf("fgets returned null; something is wrong with our assumption about the config file format.");
    exit(1); /* no point continuing if we were wrong about the format... */
  }
  printf("the line is %s\n", buf);
  tok = strtok(buf, "= ");
  tok = strtok(NULL, "= ");
  printf("the tok is %s\n", tok);
  return atoi(tok);
}

/*
 * in: the conf filename and the conf struct to be filled.
 * out: nothing returned; but the conf struct will be filled.
 */
void parse_config_file(char *fname, struct sel_conf *sc) {

  FILE *fp = fopen(fname,"r");

  if (!fp) {
    printf("error opening the query file.");
    printf("exiting on error: %s\n", strerror(errno));
    exit(errno);
  }

  /* all of this is based on a correctly formatted conf file */
  sc->r = parse_config_line(fp);
  sc->t = parse_config_line(fp);
  sc->l = parse_config_line(fp);
  sc->m = parse_config_line(fp);
  sc->a = parse_config_line(fp);
  sc->f = parse_config_line(fp);

  fclose(fp);
}

void init_array(float selectivities[][MAX_FILTERS])
{
  int i, j;
  for (i = 0; i < MAX_RUNS; i++) {
    for (j = 0; j < MAX_FILTERS; j++) {
      selectivities[i][j] = SENTINEL;
    }
  }
}

int main(int argc, char * argv[])
{
  int num_runs = 0;
  float selectivities[MAX_RUNS][MAX_FILTERS];
  init_array(selectivities);
  struct sel_conf sc;

  /* redundant check; with stage.sh's checks in place, but good practice
     if someone builds a different wrapper or usage. */
  if (argc != 3) {
    printf("usage: ./selconds <query_file> <config_file>\n");
    exit(1);
  }


  num_runs = parse_query_file(argv[1], selectivities); /* this will be the number of queries
                                    we're optimizing */
  parse_config_file(argv[2], &sc);

  /* test the parsers: */
  printf("the number of runs: %d\n", num_runs);
  printf("==================================================\n");
  int i, j;
  j = 0;
  for (i = 0; i < num_runs; i++) {
    while ((selectivities[i][j] != -99) && (j < MAX_FILTERS)) {
      printf("%.2f ", selectivities[i][j]);
      j++;
    }
    printf("\n");
    j = 0;
  }

  printf("==================================================\n");
  printf("conf r: %d\n", sc.r);
  printf("conf t: %d\n", sc.t);
  printf("conf l: %d\n", sc.l);
  printf("conf m: %d\n", sc.m);
  printf("conf a: %d\n", sc.a);
  printf("conf f: %d\n", sc.f);

  /* x = compute_best_plan(arr[1],arr2); */
  /* printf("%.2f\n",x); */
  /* printf("===================================================\n"); */

  /* for (m = 0; m < line; m++) { */
  /*   for(n = 0; n < 10; n++){ */
  /*     if (arr[m][n]>1) { */
  /*       break; */
  /*     } */
  /*     printf("%.2f ", arr[m][n]); */
  /*   } */

  /*   printf("\n---------------------------------------------------\n"); */
  /*   /\*x = compute_best_plan(arr[m],arr2);*\/ */
  /*   printf("\n\n\n\n\n"); */
  /*   printf("---------------------------------------------------\n"); */
  /*   printf("cost:%.2f\n", x); */
  /*   printf("===================================================\n"); */
  /* } */
  return 0;
}
