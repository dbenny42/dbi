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
  /*float q;*/ /* defined by paper as 1 - sel_prod if sel_prod > .5; else
              sel_prod. */
  int no_branch;
  float best_cost;
  int left_child; /* TODO; revisit these; left_child & right_child might
                     need to be sets. */
  int right_child;
};

/* plan evaluation functions */
float compute_best_plan(int, struct sel_conf *, float []);x
float compute_logicaland_cost(struct subset_state *,
                              float sels[],
                              struct sel_conf *);
float compute_plan_fixed_cost(int, struct sel_conf *, float []);
float fixed_cost_helper(int, struct sel_conf *, float []);
float compute_nobranch_cost(int, float [], struct sel_conf *);

void init_subset_state(struct subset_state *);

/* auxiliary funs */
float sum_array(int, float []); /* fold the array into a sum; in real life
                                   we'd write a fold. */
float find_q(float); /* takes p, returns q */



/* bitmap operations */

/* takes the bitmap and the full set of filter values, places the filters
   associated with the set in the empty array.  Returns the number of
   elements in the set. the current selectivities (in the first float
   array are never materialized as part of a struct subset_state, they're
   developed from the bitmap in the subset_state, but not stored, to save
   space. */
int build_set(short, float [], float []);



/* parsing functions */
int parse_query_file(char *, float[][MAX_FILTERS]);
int parse_config_line(FILE *);
void parse_config_file(char *, struct sel_conf *);

#endif
