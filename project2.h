#ifndef PROJECT2_H
#define PROJECT2_H

#define IN_BUFSIZE 1024 /* for reading lines from query and config
                           files. */
#define MAX_FILTERS 10 /* according to instructions */ 
#define MAX_RUNS 100 /* corresponds with lines in query file. */
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
  short left_child; /* short, because we're representing sets. */
  short right_child;


  /* the next set of fields are things that are not necessary, but we use
     frequently enough with such little use that it makes sense to store
     them. */
  float fixed_cost;
};

/* plan evaluation functions */
float compute_best_plan(int, struct sel_conf *, float []);
float compute_logicaland_cost(struct subset_state *,
                              float sels[],
                              struct sel_conf *);
float compute_fixed_cost(int, struct sel_conf *, float []);
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

int lemma_four_eight(struct subset_state [],
                     struct subset_state *,
                     struct subset_state *);
int lemma_four_nine(struct subset_state [],
                     struct subset_state *,
                     struct subset_state *);
int compare_cmetric(struct subset_state *,
                     struct subset_state *);
int compare_dmetric(struct subset_state *,
                     struct subset_state *);
int is_leaf(struct subset_state [], int);


/* parsing functions */
int parse_query_file(char *, int [], float[][MAX_FILTERS]);
int parse_config_line(FILE *);
void parse_config_file(char *, struct sel_conf *);


/* printing functions */
void print_plan(struct subset_state [], int);
void print_plan_loop(struct subset_state [], int);
void print_logical_term(short);

#endif
