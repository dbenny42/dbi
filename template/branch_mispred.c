/*
 ============================================================================
 Name        : branch_mispred.c
 Author      : John Demme
 Version     : Mar 21, 2011
 Description : Converted code to use perf_event instead of vTune

 Author      : Kyoho Satsumi
 Version     : Mar 27, 2010
 Description : Code for branch misprediction experiment
               This is the sample of four functions.
               You can edit this code for other number of functions as you need. 
 ============================================================================
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>

#define DLEN 3000
// INT_MAX = 2147483647 on w4112
#define LOOP 100000000

//perf counter syscall
int perf_event_open(struct perf_event_attr * hw,
		    pid_t pid, int cpu, int grp, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw, pid, cpu, grp, flags);
}

// function declaration
int createOffsets(int * off);
char * createData(double selectivity, char * dataSource);
double get_timestamp(double start_time);

// Setup info for perf_event
struct perf_event_attr attr[4];

// function implementation
int main(int argc, char * argv[]) {
  char t1[DLEN+1], t2[DLEN+1], t3[DLEN+1], t4[DLEN+1];
  double s1, s2, s3, s4;
  int i = 0, j = 0;
  //clock_t start, stop;
  double start, stop;
  FILE * fp;
  int *o1, *o2, *o3, *o4, *answer;

  uint64_t val1[4], val2[4];
  int fd[4], rc;

  attr[0].type = PERF_TYPE_HARDWARE;
  attr[0].config = PERF_COUNT_HW_CPU_CYCLES; /* generic PMU event*/
  attr[0].disabled = 0;
  fd[0] = perf_event_open(&attr[0], getpid(), -1, -1, 0);
  if (fd[0] < 0) {
    perror("Opening performance counter");
  }

  attr[1].type = PERF_TYPE_HARDWARE;
  attr[1].config = PERF_COUNT_HW_BRANCH_MISSES; /* generic PMU event*/
  attr[1].disabled = 0;
  fd[1] = perf_event_open(&attr[1], getpid(), -1, -1, 0);
  if (fd[1] < 0) {
    perror("Opening performance counter");
  }

  attr[2].type = PERF_TYPE_HARDWARE;
  attr[2].config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS; /* generic PMU event*/
  attr[2].disabled = 0;
  fd[2] = perf_event_open(&attr[2], getpid(), -1, -1, 0);
  if (fd[2] < 0) {
    perror("Opening performance counter");
  }

  attr[3].type = PERF_TYPE_HARDWARE;
  attr[3].config = PERF_COUNT_HW_INSTRUCTIONS; /* generic PMU event*/
  attr[3].disabled = 0;
  fd[3] = perf_event_open(&attr[3], getpid(), -1, -1, 0);
  if (fd[3] < 0) {
    perror("Opening performance counter");
  }

  // argument check
  if(argc != 5) {
    printf("argument error!\n");
    return -1;
  }
  
  // selectivity
  s1 = atof(argv[1]);
  s2 = atof(argv[2]);
  s3 = atof(argv[3]);
  s4 = atof(argv[4]);

  // allocate memory for offset, answer storage
  o1 = malloc(sizeof(int)*LOOP);
  o2 = malloc(sizeof(int)*LOOP);
  o3 = malloc(sizeof(int)*LOOP);
  o4 = malloc(sizeof(int)*LOOP);
  answer = malloc(sizeof(int)*LOOP);

  // create random seed
  unsigned int iseed = (unsigned int)time(NULL);
  srand(iseed);

  // create data for each function (size = DLEN)
  createData(s1, t1);
  createData(s2, t2);
  createData(s3, t3);
  createData(s4, t4);

  // create offset data for all functions (size = LOOP)
  createOffsets(o1);
  createOffsets(o2);
  createOffsets(o3);
  createOffsets(o4);

  // start
  printf("Loop start!\n");
  fflush(stdout);
  start = get_timestamp(0.0f);

  // Tell Linux to start counting events
  asm volatile ("nop;"); // pseudo-barrier
  rc = read(fd[0], &val1[0], sizeof(val1[0]));  assert(rc);
  rc = read(fd[1], &val1[1], sizeof(val1[1]));  assert(rc);
  rc = read(fd[2], &val1[2], sizeof(val1[2]));  assert(rc);
  rc = read(fd[3], &val1[3], sizeof(val1[3]));  assert(rc);
  asm volatile ("nop;"); // pseudo-barrier
  
  // actual branch code
  // you need to substitute this part for your own branch which you want to measure.

  for(i = 0; i < LOOP; i++) {
    if(t1[o1[i]] && (t2[o2[i]] && (t3[o3[i]] && t4[o4[i]]))) {
      answer[j++] = i;
    }
  }

  // Read the counter
  asm volatile("nop;"); // pseudo-barrier
  rc = read(fd[0], &val2[0], sizeof(val2[0]));  assert(rc);
  rc = read(fd[1], &val2[1], sizeof(val2[1]));  assert(rc);
  rc = read(fd[2], &val2[2], sizeof(val2[2]));  assert(rc);
  rc = read(fd[3], &val2[3], sizeof(val2[3]));  assert(rc);
  assert(rc);
  asm volatile ("nop;"); // pseudo-barrier

  // Close the counter
  close(fd[0]);
  close(fd[1]);
  close(fd[2]);
  close(fd[3]);

  stop = get_timestamp(start);

  printf("Loop stop!\n");
  printf("Elapsed time: %.9lf seconds\n", stop);
  printf("CPU Cycles:           %lu \n", val2[0] - val1[0]);
  printf("Instructions:         %lu \n", val2[3] - val1[3]);
  printf("IPC:                  %lf\n", ((double)val2[3] - val1[3]) / (val2[0] - val1[0]));
  printf("Branch misses:        %lu \n", val2[1] - val1[1]);
  printf("Branch instructions:  %lu \n", val2[2] - val1[2]);
  printf("Branch mispred. rate: %lf%%\n", 100.0*((double)val2[1] - val1[1]) / (val2[2] - val1[2]));
  printf("\n");
  printf("overall selectivity = %10.9f\n", (double)j / (double)LOOP);
  printf("theoretical selectivity = %10.9f\n", s1 * s2 * s3 * s4);

  // we need to use the answer just in case so that optimizer does not optimize too much.
  // i.e. optimizer might think that we don't need to calculate answers if we do not use them.
  fp = fopen("/dev/null", "w");
  for(i = 0; i < j; i++) {
     fprintf(fp, "%d ", answer[i]);
  }
  fclose(fp);

  // free offset memory
  free(o1);
  free(o2);
  free(o3);
  free(o4);
  // free answer memory
  free(answer);

  return 0;
}

/*
 get_timestamp
 utility function for getting timestamp
 */

double get_timestamp(double start_time) {
  struct timeval tp;
  // struct timeval *tp = (struct timeval*)malloc(sizeof(struct timeval*));
  if (gettimeofday(&tp, NULL) == 0) {
    return (double)tp.tv_sec + (double)tp.tv_usec * 1e-6 - start_time;
  } else {
    printf("Failed to get time stamp");
    return -1.0f;
  }
}

/* 
 createData
 This function create random value from 0 to 1, then if the value is less than selectivity,
 put 1, otherwise put 0. Fill 0 or 1 to the data array with given selectivity, size of DLEN. 
*/

char * createData(double selectivity, char * data) {
  int i;
  double r;

  for(i = 0; i < DLEN; i++) {
    r = ((double)rand() / ((double)(RAND_MAX) + (double)(1)));
    if(r < selectivity) {
      *data = 1;
    } else {
      *data = 0;
    }
    data++;
  }
  *data = '\0';
  return data;
}

/* 
 createOffsets
 This function create random value from 0 to DLEN - 1 (fit in the created data by createData function), 
 and store into the array of off[i]. 
*/

int createOffsets(int * off) {
  int i;
  for(i = 0; i < LOOP; i++) {
    double r = ((double)rand() / ((double)(RAND_MAX) + (double)(1)));
    off[i] = (int)(r * (double)DLEN);
  }
  return 0;
}
