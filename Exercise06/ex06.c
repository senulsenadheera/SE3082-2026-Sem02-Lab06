#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N     1000000
#define STRIP 8
#define REPS  200

static double A[N], B[N], C[N];

int main(int argc, char *argv[])
{
    long i, is;
    int  r;
    double tstart, tstop, tcalc, check;
    int nthreads = (argc > 1) ? atoi(argv[1]) : 4;

    omp_set_num_threads(nthreads);

    for (i = 0; i < N; i++) { A[i] = (double)i * 0.5; B[i] = 2.0; }

    /* WARM-UP: fault in every page of C before any timing happens */
    for (i = 0; i < N; i++) C[i] = A[i] * B[i];

    tstart = omp_get_wtime();
    for (r = 0; r < REPS; r++)
        for (i = 0; i < N; i++) C[i] = A[i] * B[i];
    tstop = omp_get_wtime();
    tcalc = (tstop - tstart) / REPS;
    printf("SERIAL          : threads =  1  time = %f s\n", tcalc);

    tstart = omp_get_wtime();
    for (r = 0; r < REPS; r++) {
        #pragma omp parallel for schedule(static)
        for (i = 0; i < N; i++) C[i] = A[i] * B[i];
    }
    tstop = omp_get_wtime();
    tcalc = (tstop - tstart) / REPS;
    printf("PARALLEL(plain) : threads = %2d  time = %f s\n", nthreads, tcalc);

    tstart = omp_get_wtime();
    for (r = 0; r < REPS; r++) {
        #pragma omp parallel for schedule(static) private(i)
        for (is = 0; is < N; is += STRIP) {
            long end = (is + STRIP < N) ? is + STRIP : N;
            #pragma omp simd
            for (i = is; i < end; i++)
                C[i] = A[i] * B[i];
        }
    }
    tstop = omp_get_wtime();
    tcalc = (tstop - tstart) / REPS;
    printf("STRIP MINED(%d)  : threads = %2d  time = %f s\n", STRIP, nthreads, tcalc);

    check = 0.0;
    for (i = 0; i < N; i++) check += fabs(C[i] - A[i] * B[i]);
    printf("verification error = %.1f\n", check);
    return 0;
}
