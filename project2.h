#ifndef PROJECT2_H
#define PROJECT2_H

#define IN_BUFSIZE 1024 /* for reading lines from query and config
                           files. */
#define MAX_FILTERS 9 /* according to instructions */ 
#define MAX_RUNS 25 /* corresponds with lines in query file. */
#define SENTINEL -99.0

/* the configuration for the machine.  the terms' names correspond with
   the names given in the selconds paper. */
struct sel_conf {
  int r;
  int t;
  int l;
  int m;
  int a;
  int f;
};

struct subset_state {
  int num_basic_terms;
  float sel_prod;
  int no_branch;
  float best_cost;
  int left_child;
  int right_child;
};

float compute_best_plan(float *, int *);
int parse_query_file(char *, float[][MAX_FILTERS]);
int parse_config_line(FILE *);
void parse_config_file(char *, struct sel_conf *);

#endif
