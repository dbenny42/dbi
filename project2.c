#include<stdlib.h>
#include<stdio.h>
#include<string.h>


float code(float *arr,int *arr2){

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
  while(arr[i] <= 1) {
    i++; /* i is the total number of basic terms */
  }

  /* k is total possible combinations */
  while (j < i) {
    k*=2; j++;
  } //k is the total number of permutations

  struct {
    int b, L, R, flag;
    float c;
  } A[k];
  
  for (j = 1; j < k; j++) {
    A[j].b = 0;
    A[j].L = 0;
    A[j].R = 0;
    A[j].flag=0;

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
      if (s[n]==1) {
      l++;
      q*=arr[i-n];}
    }//l is the number of basic terms under the condition of j,where j ranges from 1 to k-1

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
      if(r[n]==1) {
      l++;
      p2*=arr[i-n];}
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
        if (t[n] == 1) {
        l++;
        p1 *= arr[i-n];
      }
      }

      fcost1 = (l * arr2[0])+ ((l - 1) * arr2[2]) + (l * arr2[5]) + (arr2[1]);
      l=0;

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
      } else if (((fcost1 + arr2[3] * b + p1 * A[j].c) < A[j + p].c)&&A[p].flag==1) {
        A[j+p].c=fcost1+arr2[3]*b+p1*A[j].c;
        A[j+p].L=p;
        A[j+p].R=j;
      }
      p1=1;
    }
    p2=1;
  }

  m=0;
  n=0;
  l=0;
  //printf("%d,%d,%d,%f", A[k-1].L, A[k-1].R, A[k-1].b, A[k-1].c);

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

int main(int argc, char * argv[]) {

  FILE * fp;

  int line = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int m;
  int n;

  char str[1024] = {0};
  int cost[10] = {0};
  float arr[10][10] = {0}, x;
  int arr2[6] = {0};

  fp = fopen(argv[1],"r");

  /* for each line of the file. */
  while(!feof(fp)){
    char *p;

    if (fgets(str,sizeof(str), fp) == NULL) {
      break;
    }

    line++;

    p = strtok(str," ");

    while(p){
      arr[i][j++]=atof(p);
      p=strtok(NULL," ");
    }

    while( j < 10) {
      arr[i][j++]=1.1;
    }
    i++;
    j=0;
  }

  /*for( m = 0; m < line; m++){
    for(n = 0; n < 10; n++){
      printf("%.2f ",arr[m][n]);
    }
    printf("\n");
  }*/
  fclose(fp;)

  fp = fopen(argv[2],"r");
  while(!feof(fp)){
    char *p;
    if(fgets(str,sizeof(str),fp)==NULL) break;
    p=strtok(str,"= ");
    while(p){
      p=strtok(NULL,"= ");
      arr2[k++]=atoi(p);
      p=strtok(NULL,"= ");
    }
  }

  /*for ( m=0; m < 6; m++) {
    printf("%d ",arr2[m]);
  }*/

  //puts("\n"); /* what is this here for?  why use puts? */

  fclose(fp);

  /* x=code(arr[1],arr2); */
  /* printf("%.2f\n",x); */

  printf("===================================================\n");

  for (m = 0; m < line; m++) {
    for(n = 0; n < 10; n++){
      if (arr[m][n]>1) {
        break;
      }
      printf("%.2f ", arr[m][n]);
    }

    printf("\n---------------------------------------------------\n");
    x = code(arr[m],arr2);
    printf("\n\n\n\n\n");
    printf("---------------------------------------------------\n");
    printf("cost:%.2f\n", x);
    printf("===================================================\n");
  }
  return 0;
}
