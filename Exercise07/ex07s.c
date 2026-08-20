#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NPOINTS 2000
#define MAXITER 2000

struct d_complex { double r; double i; };

struct d_complex c;
int numoutside = 0;

void testpoint(void) {
  struct d_complex z;
  int iter;
  double temp;
  z = c;
  for (iter = 0; iter < MAXITER; iter++) {
    temp = (z.r*z.r) - (z.i*z.i) + c.r;
    z.i  = z.r*z.i*2 + c.i;
    z.r  = temp;
    if ((z.r*z.r + z.i*z.i) > 4.0) { numoutside++; break; }
  }
}

int main() {
  int i, j;
  double area, error, eps = 1.0e-5;
  double t0, t1;

  t0 = omp_get_wtime();
  for (i = 0; i < NPOINTS; i++) {
    for (j = 0; j < NPOINTS; j++) {
      c.r = -2.0 + 2.5*(double)(i)/(double)(NPOINTS) + eps;
      c.i =        1.125*(double)(j)/(double)(NPOINTS) + eps;
      testpoint();
    }
  }
  t1 = omp_get_wtime();

  area = 2.0*2.5*1.125*(double)(NPOINTS*NPOINTS - numoutside)/(double)(NPOINTS*NPOINTS);
  error = area/(double)NPOINTS;
  printf("SERIAL   : threads =  1  area = %12.8f +/- %12.8f  time = %f s\n", area, error, t1-t0);
  return 0;
}
