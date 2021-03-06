#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<strings.h> /* required for the use of ffs */

#include "project2.h"

/*
 * where num_basic_terms is set from the number of filters gained from
 * this run...
 */
float compute_best_plan(int num_basic_terms, struct sel_conf *sc, float all_sels[])
{
  int i;
  for (i = 0; i < num_basic_terms; i++) {
    printf("%.5f ", all_sels[i]);
  }
  printf("\n");
  printf("------------------------------------------------------------------------------------------s\n");

  /* the low-order 9 bits (at most) of the subset_bm are used to
     indicate which basic terms are a part of a plan (of some size) stored
     in 'plans' */
  int subset_bm; /* 16 bits is more than enough; we need a maximum of 9 */

  /* the integral representation of basic_term_bm becomes the index into
     plans where that particular subset's cost information is stored. */
  long num_subsets = 1 << num_basic_terms; /* gives us 2 ^ num_basic_terms */

  /* 'plans' is the array that we'll use to store plans of increasing
     size. */
  struct subset_state plans[num_subsets];



  /* STAGE 1:

     Generate the cost of the (2^(num_basic_terms) - 1) &-only
     plans.  for each plan, check if the no-branch plan is more effective,
     then store that cost with the no-branch flag set if the cost is
     better. */ 

  /* we start the idx at 1 because we need non-empty subsets. */
  for (subset_bm = 1; subset_bm < num_subsets; subset_bm++) {
    /* build a subset state for this subset index; place it in the
       array. */
    struct subset_state ss;
    init_subset_state(&ss);

    float curr_sels[MAX_FILTERS] = {SENTINEL};

    /* build the associated struct subset_state */
    ss.num_basic_terms = build_set(subset_bm, curr_sels, all_sels);

    /* set the ss.sel_prod */
    int idx;
    for (idx = 0; idx < ss.num_basic_terms; idx++) {
      ss.sel_prod *= curr_sels[idx];
    }


    /* TODO: optimize the way all of these initial values are set; it's
       highly inefficient right now. */

    /* set the fixed cost */
    ss.fixed_cost = compute_fixed_cost(ss.num_basic_terms,
                                       sc,
                                       curr_sels);


    ss.best_cost = compute_logicaland_cost(&ss, curr_sels, sc);
    float no_branch_cost = compute_nobranch_cost(ss.num_basic_terms,
                                                 curr_sels,
                                                 sc);

    if (no_branch_cost < ss.best_cost) {
      /* make appropriate adjustments for no-branch */
      ss.best_cost = no_branch_cost;
      ss.no_branch = 1;
    }
        
    /* lastly, actually store the subset results. */
    plans[subset_bm] = ss;
  }


    
  /***************************************************
   ** Stage 2
   ***************************************************/
  short s1_idx;
  short s2_idx;
  for (s1_idx = 1; s1_idx < num_subsets; s1_idx++) {
    for (s2_idx = 1; s2_idx < num_subsets; s2_idx++) {

      /* continue only if the intersection is empty */
      if ((s2_idx & s1_idx) == 0) {

        /* TODO: more testing with the lemmas for correctness. */

        if (lemma_four_eight(plans, &plans[s1_idx], &plans[s2_idx])) {
          /* do nothing */
        } else if (lemma_four_nine(plans, &plans[s1_idx], &plans[s2_idx])) {
          /* do nothing */
        } else {

          /*fcost(s') + mq + pC*/
          float new_cost = ((plans[s2_idx].fixed_cost) +
                            (find_q(plans[s2_idx].sel_prod) * sc->m) +
                            (plans[s2_idx].sel_prod * plans[s1_idx].best_cost));

          short s1_union_s2 = s1_idx | s2_idx;

          if (new_cost < plans[s1_union_s2].best_cost) {
            /* replace A[s' U s].c with c */
            plans[s1_union_s2].best_cost = new_cost;
            /* replace A[s' U s].L with s'. */
            plans[s1_union_s2].left_child = s2_idx;
            /* replace A[s' U s].R with s. */
            plans[s1_union_s2].right_child = s1_idx;
          }
        }
      }
    }
  }

  /**************************************************
   ** end stage 2.
   **************************************************/

  /* NEXT STEP: */
  /* recursively print out the plan */

  print_plan(plans, num_subsets - 1);

  return plans[num_subsets - 1].best_cost;
}


void print_plan(struct subset_state plans[], int curr_subset)
{
  /* two cases: one with and one without children */
  /* no children is a special print case. */
  if (is_leaf(plans, curr_subset)) {
    if (plans[curr_subset].no_branch) {
      printf("/* no branch option */\n");
      printf("answer[j] = i;\n");
      printf("j += ");
      print_logical_term(plans, curr_subset);
      printf(";\n");
    } else {
      printf("if (");
      print_logical_term(plans, curr_subset);
      printf(") {\n");
      printf("\tanswer[j++] = i;\n");
      printf("}\n");
    }
  } else {

    /* now handle the case with children. */
    printf("if (");
    print_plan_loop(plans, curr_subset, 0);
  }
 

  /* wrap up. */
  printf("------------------------------------------------------------------------------------------p\n");
  printf("cost: %f\n", plans[curr_subset].best_cost);
  printf("==========================================================================================\n");
}

void print_plan_loop(struct subset_state plans[], int curr_subset, int curr_parens)
{
  /* printf("the current subset is %d\n", curr_subset);*/

  short left_child = plans[curr_subset].left_child;

  /* handle left side*/
  if (left_child) {
    /* recurse*/
    print_logical_term(plans, left_child);
  }
  

  /* handle right side.*/
  /* we don't need to check if the right side is valid, because we're
     guaranteed to have it */
  short right_child = plans[curr_subset].right_child;
  /* recurse*/
  if(!is_leaf(plans, right_child)) {
    printf(" && ");
    short right_grandchild = plans[right_child].right_child;
    if ((!is_leaf(plans, right_grandchild)) ||  /*next cond assumes IS leaf */
        (!plans[right_grandchild].no_branch)) {
      printf("(");
      curr_parens++;
    }
    print_plan_loop(plans, right_child, curr_parens);
  } else {
    /* is leaf */

    /* this is the last function call, then.  we have enough info to
       finish the print. */

    if (plans[right_child].no_branch) {
      while (curr_parens > 0) {
        printf(")");
        curr_parens--;
      }

      printf(") {\n");
      printf("\tanswer[j] += i;\n");
      printf("\tj = ");
      print_logical_term(plans, right_child);
      printf(";\n");
      printf("}\n");
    } else {
      printf(" && ");
      print_logical_term(plans, right_child);
      while (curr_parens > 0) {
        printf(")");
        curr_parens--;
      }

      printf(") {\n");
      printf("\tanswer[j++] = i;\n");
      printf("}\n");
    }
  }
}


void print_logical_term(struct subset_state plans[], short subset_bm)
{
  short bm_copy = subset_bm;
  if (plans[bm_copy].num_basic_terms > 1) {
    printf("(");
  }

  int curr_bit = 1;
  int needs_ampersand = 0;

  while (subset_bm) {
    if (subset_bm & 1) {
      if (!needs_ampersand) {
        printf("t%d[o%d[i]]", curr_bit, curr_bit);
        needs_ampersand = 1; /* so only the first term goes without */
      } else {
        printf(" & t%d[o%d[i]]", curr_bit, curr_bit);
      }
    }
    subset_bm >>= 1; /* get a new lowest-order bit. */
    curr_bit++;
  }

  if (plans[bm_copy].num_basic_terms > 1) {
    printf(")");
  }
}



int is_leaf(struct subset_state plans[], int curr_subset)
{
  return ((plans[curr_subset].left_child == 0) &&
          (plans[curr_subset].right_child == 0));
}


/* returns -1 if s1->cmet < s2->cmet */
/* returns 0 if s1->cmet == s2->cmet OR no ordering is possible */
/* returns 1 if s1->cmet > s2->cmet */
int compare_cmetric(struct subset_state *s1, struct subset_state *s2) {
  if (s1->sel_prod < s2->sel_prod) {
    if (((s1->sel_prod - 1) / s1->fixed_cost) <
        ((s2->sel_prod - 1) / s2->fixed_cost)) {
      return -1;
    } else { /* no ordering is possible */
      return 0;
    }
  } else if (s1->sel_prod > s2->sel_prod) {
    if (((s1->sel_prod - 1) / s1->fixed_cost) >
        ((s2->sel_prod - 1) / s2->fixed_cost)) {
      return 1;
    } else {
    return 0; /* equality of sel_prods means no ordering is possible OR equal */
    }
  } else {
    return 0;
  }
}

/* returns -1 if s1->dmet < s2->dmet */
/* returns 0 if s1->dmet == s2->dmet */
/* returns 1 if s1->dmet > s2->dmet */
int compare_dmetric(struct subset_state *s1, struct subset_state *s2) {
  if (s1->sel_prod < s2->sel_prod) {
    if(s1->fixed_cost < s2->fixed_cost) {
      return -1;
    } else { /* no ordering is possible */
      return 0;
    }
  } else if (s1->sel_prod > s2->sel_prod) {
    if(s1->fixed_cost < s2->fixed_cost) {
      return 1;
    } else {
      return 0; /* equality of sel_prods means no ordering is possible OR equal */
    }
  } else {
    return 0;
  }
}

/* returns 1 if the lemma applies, meaning 'do nothing', this plan is not
   optimal. */
/* passing plans should be okay, because it should be by reference */
int lemma_four_eight(struct subset_state plans[],
                     struct subset_state *s1,
                     struct subset_state *s2)
{
  /* first, get leftmost &-term of s1. */
  short curr_child = s1->left_child;

  /* as long as their is a non-empty left_child: */
  while (curr_child) {
    curr_child = (plans[curr_child].left_child);
  }

  /* s1_leftmost now points to the leftmost &-term of s1 */
  if ((compare_cmetric(&(plans[curr_child]), s2) == 1)) {
    /* s1 dominates s2 */
    return 1; /* the lemma applies. */
  }
  return 0; /* the lemma does not apply */
}

/* returns 1 if the lemma applies, meaning 'do nothing', this plan is not
   optimal. */
int lemma_four_nine(struct subset_state plans[],
                    struct subset_state *s1,
                    struct subset_state *s2)
{
  /* TODO: fix the way you're handling these left and right children. */
  short curr_child = s1->left_child;
  if (s2->sel_prod <= .5) {
    /* check the current &-term in s1 */
    if ((compare_dmetric(s1, s2) == 1)) {
      return 1; /* lemma applies */
    }

   /* check the left children &-terms in s1 */
    while (curr_child) {
      if ((compare_dmetric(&(plans[curr_child]), s2)) == 1) {
        return 1;
      }
      curr_child = plans[curr_child].left_child;
    }

    /* check the right children &-terms in s1 */
    curr_child = s1->right_child;
    while (curr_child) {
      if ((compare_dmetric(&(plans[curr_child]), s2)) == 1) {
        return 1;
      }
      curr_child = plans[curr_child].right_child;
    }
  }

  /* to make it this far is for the lemma to fail */
  return 0;
}



void init_subset_state(struct subset_state *ss)
{
  ss->num_basic_terms = 0;
  ss->sel_prod = 1;
  ss->no_branch = 0;
  ss->best_cost = 0;
  ss->left_child = 0;
  ss->right_child = 0;
}

/* bm is the actual set bitmap */
int build_set(short bm, float curr_sels[], float all_sels[])
{
  int bit_idx = 0;
  int curr_sels_idx = 0;
  while (bm) { /* so long as there are high bits in the bm */
    if (bm & 1) { /* if lowest bit is 1. */
      curr_sels[curr_sels_idx++] = all_sels[bit_idx];
    }
    bit_idx++;
    bm >>= 1; /* set up to look at next bit */
  }
  return curr_sels_idx; /* doubles as the number of high bits found */
}



/* begin auxiliaries */

float sum_array(int len, float values[])
{
  int i;
  float sum = 0;
  for (i = 0; i < len; i++) {
    sum += values[i];
  }
  return sum;
}

float find_q(float p)
{
  return ((p > .5) ? (1 - p) : p);
}


/* end auxiliaries; start cost fns. */



/* compute_fixed_cost and compute_nobranch_cost are essentially the
   same; both make use of fixed_cost_helper() */
float fixed_cost_helper(int num_basic_terms,
                        struct sel_conf *sc,
                        float sels[])
{
  return ((num_basic_terms * sc->r) +
          ((num_basic_terms - 1) * sc->l) +
          num_basic_terms * (sc->f));
          /* sum_array(num_basic_terms, sels));*/
}

/* where sels is the current group of selectivities */
float compute_fixed_cost(int num_basic_terms,
                              struct sel_conf *sc,
                              float sels[])
{
  return (fixed_cost_helper(num_basic_terms, sc, sels) +
          sc->t);
}

float compute_nobranch_cost(int num_basic_terms,
                            float sels[],
                            struct sel_conf *sc)
{
  return (fixed_cost_helper(num_basic_terms, sc, sels) +
          sc->a);
}

float compute_logicaland_cost(struct subset_state *ss,
                              float sels[],
                              struct sel_conf *sc)
{
  return (compute_fixed_cost(ss->num_basic_terms, sc, sels) +
          (sc->m * (find_q(ss->sel_prod))) +
          (ss->sel_prod * sc->a));
}

/* end cost fns */








/**************************************************
Here begins the parsing fns and main.
 **************************************************/

/* 
 * in: the filename and the empty array of selectivities (to be filled)
 * out: the number of lines that were read. 
 */
int parse_query_file(char *fname,
                     int num_filters[],
                     float selectivities[][MAX_FILTERS])
{
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
    num_filters[line_num] = filter_num;

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

  tok = strtok(buf, "= ");
  tok = strtok(NULL, "= ");

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
  int num_filters[MAX_RUNS] = {SENTINEL};
  float selectivities[MAX_RUNS][MAX_FILTERS];
  init_array(selectivities);
  struct sel_conf sc;

  /* redundant check; with stage.sh's checks in place, but good practice
     if someone builds a different wrapper or usage. */
  if (argc != 3) {
    printf("usage: ./selconds <query_file> <config_file>\n");
    exit(1);
  }

  /* this will be the number of queries we're optimizing */
  num_runs = parse_query_file(argv[1], num_filters, selectivities);
  parse_config_file(argv[2], &sc);

  /* perform the actual optimization. */
  int run_idx;
  printf("==========================================================================================\n");
  /*for (run_idx = 0; run_idx < num_runs; run_idx++) {*/
  for (run_idx = 0; run_idx < num_runs; run_idx++) {
    compute_best_plan(num_filters[run_idx], &sc, selectivities[run_idx]);
  }

  return 0;
}
