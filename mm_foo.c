/*
 * Matrix Multiply.
 *
 * This is a simple matrix multiply program which will compute the product
 *
 *                C  = A * B
 *
 * A ,B and C are both square matrix. They are statically allocated and
 * initialized with constant number, so we can focuse on the parallelism.
 *
 */

#include <caliper/cali.h>

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#ifndef ORDER
#define ORDER 1000   // the order of the matrix
#endif
#define AVAL  3.0    // initial value of A
#define BVAL  5.0    // initial value of B
#define TOL   0.001  // tolerance used to check the result

#define N ORDER
#define P ORDER
#define M ORDER

double A[N][P];
double B[P][M];
double C[N][M];
double D[N][M];

// Initialize the matrices (uniform values to make an easier check)
void matrix_init(void) {
CALI_CXX_MARK_FUNCTION;
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
cali_set_int(thread_attr, omp_get_thread_num());

        int i, j;

        // A[N][P] -- Matrix A
        for (i=0; i<N; i++) {
                for (j=0; j<P; j++) {
                        A[i][j] = AVAL;
                }
        }

        // B[P][M] -- Matrix B
        for (i=0; i<P; i++) {
                for (j=0; j<M; j++) {
                        B[i][j] = BVAL;
                }
        }

        // C[N][M] -- result matrix for AB
        for (i=0; i<N; i++) {
                for (j=0; j<M; j++) {
                        C[i][j] = 0.0;
                }
        }
}


double foo1(int i, int j){
CALI_CXX_MARK_FUNCTION;
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
cali_set_int(thread_attr, omp_get_thread_num());
  D[i][j] += A[i][j] * B[i][j];
}


double foo2(int i){
CALI_CXX_MARK_FUNCTION;
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
cali_set_int(thread_attr, omp_get_thread_num());
  int k,j;
  for (j=0; j<M; j++){
  foo1(i,j);
  for (k=0; k<P; k++){
    C[i][j] += A[i][k] * B[k][j];
  }
  }
}

// The actual mulitplication function, totally naive
double matrix_multiply(void) {
CALI_CXX_MARK_FUNCTION;
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
cali_set_int(thread_attr, omp_get_thread_num());
        int i, j, k;
        double start, end;
        double B_T[M][P];
        /*
        for (i=0; i<P; i++){
                for (j=0; j<M; j++){
                        B_T[j][i] = B[i][j];
                }
        }
        */
        // timer for the start of the computation
        // Reorganize the data but do not start multiplying elements before 
        // the timer value is captured.
        start = omp_get_wtime();

        #pragma omp parallel for private(i,j,k)
        for (i=0; i<N; i++){
                //for (j=0; j<M; j++){
                        //for (k=0; k<P; k++){
                                // C[i][j] += A[i][k] * B_T[j][k];
                                // C[i][j] += A[i][k] * B[k][j];
                        //}
                        //foo1(i,j);
                        foo2(i);
                //}
        }

        // timer for the end of the computation
        end = omp_get_wtime();
        // return the amount of high resolution time spent
        return end - start;
}


// Function to check the result, relies on all values in each initial
// matrix being the same
int check_result(void) {
CALI_CXX_MARK_FUNCTION;
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
cali_set_int(thread_attr, omp_get_thread_num());
        int i, j;

        double e  = 0.0;
        double ee = 0.0;
        double v  = AVAL * BVAL * ORDER;

        for (i=0; i<N; i++) {
                for (j=0; j<M; j++) {
                        e = C[i][j] - v;
                        ee += e * e;
                }
        }

        if (ee > TOL) {
                return 0;
        } else {
                return 1;
        }
}

// main function
int main(int argc, char **argv) {
CALI_CXX_MARK_FUNCTION;
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
cali_set_int(thread_attr, omp_get_thread_num());
        int correct;
        int err = 0;
        double run_time;
        double mflops;

	int nt = omp_get_max_threads();
	printf("Availalbe threads =  %d \n", nt);

        // initialize the matrices
        matrix_init();
        // multiply and capture the runtime
        run_time = matrix_multiply();
        // verify that the result is sensible
        correct  = check_result();

        // Compute the number of mega flops
        mflops = (2.0 * N * P * M) / (1000000.0 * run_time);
        printf("Order %d multiplication in %f seconds \n", ORDER, run_time);
        printf("Order %d multiplication at %f mflops\n", ORDER, mflops);

        // Check results
        if (! correct) {
                fprintf(stderr,"\n Errors in multiplication");
                err = 1;
        } else {
               fprintf(stdout,"\n SUCCESS : results match\n");
        }
        return err;
}

