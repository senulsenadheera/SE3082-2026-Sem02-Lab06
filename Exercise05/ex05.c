#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define CUTOFF 20

int fib_serial(int n) {
    int i, j;
    if (n < 2)
        return n;
    else {
        i = fib_serial(n-1);
        j = fib_serial(n-2);
        return i + j;
    }
}

int fib_task(int n) {
    int i, j;
    if (n < 2)
        return n;
    else {
        #pragma omp task shared(i)
        i = fib_task(n-1);

        #pragma omp task shared(j)
        j = fib_task(n-2);

        #pragma omp taskwait
        return i + j;
    }
}

int fib_cutoff(int n) {
    int i, j;
    if (n < 2)
        return n;
    if (n < CUTOFF)
        return fib_serial(n);

    #pragma omp task shared(i)
    i = fib_cutoff(n-1);

    #pragma omp task shared(j)
    j = fib_cutoff(n-2);

    #pragma omp taskwait
    return i + j;
}

int main(int argc, char *argv[])
{
    int n        = (argc > 1) ? atoi(argv[1]) : 35;
    int nthreads = (argc > 2) ? atoi(argv[2]) : 4;
    int result;
    double tstart, tstop, tcalc;

    omp_set_num_threads(nthreads);

    tstart = omp_get_wtime();
    result = fib_serial(n);
    tstop  = omp_get_wtime();
    tcalc  = tstop - tstart;
    printf("SERIAL        : fib(%d) = %d  threads =  1  time = %f s\n", n, result, tcalc);

    tstart = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        result = fib_task(n);
    }
    tstop = omp_get_wtime();
    tcalc = tstop - tstart;
    printf("TASK(nocut)   : fib(%d) = %d  threads = %2d  time = %f s\n", n, result, nthreads, tcalc);

    tstart = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        result = fib_cutoff(n);
    }
    tstop = omp_get_wtime();
    tcalc = tstop - tstart;
    printf("TASK(cutoff%d): fib(%d) = %d  threads = %2d  time = %f s\n", CUTOFF, n, result, nthreads, tcalc);

    return 0;
}
