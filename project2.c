#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

#include "project2.h"

float code(float *arr, int *arr2){

  int flag = 0;
  int a = 0; 
  int g = 0; 
  int h = 0; 
  int i = 0;
  int j = 0;
  int k = 1;
  int l = 0; 
  int m = 0;
  int n = 0;
  int p = 0;
  int fcost1 = 0;
  int fcost2 = 0;
  int r[10] = {0};
  int s[10] = {0};
  int t[10] = {0};

  float q = 1;
  float qq = 0;
  float p1 = 1;
  float p2 = 1;
  float b = 0;

  /* i counts the number of basic terms */
  while (arr[i] <= 1) {
    i++; /* i总基本项个数 */
  }

  /* k is total possible combinations */
  while (j < i) {
    k*=2; j++;
  } //k为总的组合数

  struct {
    int b, L, R;
    float c;
  } A[k];
  
  for (j = 1; j < k; j++) {
    A[j].b = 0;
    A[j].L = 0;
    A[j].R = 0;

    h = k / 2;
    g = j;
    while (h > 0) {
      if(g >= h) {
        g %= h;
        s[m] = 1;
        m++;
      } else{
        s[m]=0;m++;
      }
      h /= 2;
    }

    for (n = 0; n < 10; n++) {
      if (s[n]==0) {
        continue;
      }

      l++;
      q*=arr[k-n-1];
    }//l为该j值下到基本项数，j从1到k

    qq=q;

    if (q > .5) {
      q = (1 - q);
    }
    /* the above if-statement replaces this: */
    /* q= (q > 0.5) ? (1 - q) : q; */

    A[j].c = l * arr2[0] + (l-1) *arr2[2] + l * arr2[5] + arr2[1] + arr2[3] * q + arr2[4] * qq;
    if (arr2[4] < (arr2[1] + arr2[3] * q + arr2[4] * qq)) {
      A[j].c = l * arr2[0] + (l-1) * arr2[2] + l * arr2[5] + arr2[4];
      A[j].b=1;
    }

    // printf("%.2f ",A[j].c);
    l=0;
    m=0;
    n=0;
  } /* end for (j = 1...) */

  for (j = 1; j < k - 1; j++) {
    h = k / 2;
    g = j;

    while (h > 0) {

      if (g >= h) {
        g%=h;
        r[m]=1;
        m++;
      } else {
        r[m]=0;
        m++;
      }

      h /= 2;
    }

    for (n = 0; n < 10; n++) {
      if(r[n]==0) {
        continue;
      }
      
      l++;
      q*=arr[k-n-1];
    }

    fcost2 = (l * arr2[0]) + ((l - 1) * arr2[2]) + l * (arr2[5] + arr2[1]);

    m=0;
    n=0;
    l=0;

    for(p=1; p < k - 1; p++) {
      h=k/2;
      g=p;

      while (h > 0) {
        if (g >= h) {
          g %= h;
          t[m]=1;
          m++;
        } else {
          t[m]=0;
          m++;
        }

        h /= 2;
      }

      m=0;
      n=0;
      l=0;

      for (n=0; n < 10; n++) {
        if (t[n] == 0) {
          continue;
        }
        l++;
        q *= arr[k-n-1];
      }

      fcost1 = (l * arr2[0])+ ((l - 1) * arr2[2]) + (l * arr2[5]) + (arr2[1]);

      // printf("l=%d,fcost1=%d ",l,fcost1);

      if (p1 > .5) {
        b = 1 - p1;
      } else {
        b = p1;
      }
      /* the above if replaces this: */
      /* b = (p1 > 0.5)?(1-p1):p1;*/

      for(a=0;a<i;a++) {
        if ((r[a] == 1) && (t[a] == 1)) {
          flag=1;
        }
        break;
      }

      if( flag == 1) {
        flag=0;
        continue;
      } else if ((p2 <= p1) && ((fcost2 - fcost1 + p2 * fcost1 - p1 * fcost2) < 0)) {
        continue;
      } else if (((p1 <= 0.5) && (p2 < p1)) && (fcost2 < fcost1)) {
        continue;
      } else if ((fcost1 + arr2[3] * b + p1 * A[j].c) < A[j + p].c) {
        A[j+p].c=fcost1+arr2[3]*b+p1*A[j].c;
        A[j+p].L=p;
        A[j+p].R=j;
      }
    }
  }

  m=0;
  n=0;
  l=0;
  printf("%d,%d,%d,%f", A[k-1].L, A[k-1].R, A[k-1].b, A[k-1].c);

  printf("for(i=0;i<LOOP;i++){\n");
  if((A[k-1].L == 0) && (A[k-1].b == 0)) {
    printf("if(f1(r1[i])");
    for(m = 2; m <= i; m++) {
      printf("&f%d(r%d[i])",m,m);
    }

    printf(")\n");
    printf("{answer[j++]=i;}\n}");
  }

  return A[k-1].c;

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

  /* x = code(arr[1],arr2); */
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
  /*   /\*x = code(arr[m],arr2);*\/ */
  /*   printf("\n\n\n\n\n"); */
  /*   printf("---------------------------------------------------\n"); */
  /*   printf("cost:%.2f\n", x); */
  /*   printf("===================================================\n"); */
  /* } */
  return 0;
}
