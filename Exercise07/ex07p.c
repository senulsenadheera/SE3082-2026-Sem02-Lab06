#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NPOINTS 2000
#define MAXITER 2000

struct d_complex { double r; double i; };

/* c is now PASSED IN, not global -- otherwise every thread overwrites it */
int testpoint(struct d_complex c) {
  struct d_complex z;
  int iter;
  double temp;
  z = c;
  for (iter = 0; iter < MAXITER; iter++) {
    temp = (z.r*z.r) - (z.i*z.i) + c.r;
    z.i  = z.r*z.i*2 + c.i;
    z.r  = temp;
    if ((z.r*z.r + z.i*z.i) > 4.0) return 1;   /* outside */
  }
  return 0;                                    /* inside  */
}

int main(int argc, char *argv[]) {
  int i, j;
  int numoutside = 0;
  double area, error, eps = 1.0e-5;
  double t0, t1;
  int nthreads = (argc > 1) ? atoi(argv[1]) : 4;

  omp_set_num_threads(nthreads);
  t0 = omp_get_wtime();

  #pragma omp parallel private(i, j) reduction(+:numoutside)
  {
      int tid  = omp_get_thread_num();
      int nthr = omp_get_num_threads();
      struct d_complex c;

      /* manual cyclic distribution of the OUTER loop */
      for (i = tid; i < NPOINTS; i += nthr) {
          for (j = 0; j < NPOINTS; j++) {
              c.r = -2.0 + 2.5*(double)(i)/(double)(NPOINTS) + eps;
              c.i =        1.125*(double)(j)/(double)(NPOINTS) + eps;
              numoutside += testpoint(c);
          }
      }
  }

  t1 = omp_get_wtime();
  area = 2.0*2.5*1.125*(double)(NPOINTS*NPOINTS - numoutside)/(double)(NPOINTS*NPOINTS);
  error = area/(double)NPOINTS;
  printf("PARALLEL : threads = %2d  area = %12.8f +/- %12.8f  time = %f s\n",
         nthreads, area, error, t1-t0);
  return 0;
}
