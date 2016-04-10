/* Joe Gorse, 2016
 * FIR filter
 * BSD 2-clause license.
 */

/*
 * This is a spartan FIR implementation.
 * It does no checking, no dynamic allocation, and is without sequins.
 * I built it for simplicity, speed, and fun.
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>

typedef double fir_type_t;

/* Bring your own coefficients and data history buffers. */
typedef struct {
  // char name[32];      // Just in case you use these things
  size_t      taps;   // How many taps we have
  fir_type_t  *coef;  // Pointer to array of coefficients, taps long
  fir_type_t  *x;     // History of inputs, taps long
  size_t      ix;     // Iterator
} fir_filter_t;

/* Give it a new value, get one in return. */
fir_type_t fir_filter (fir_type_t new_value, fir_filter_t *fir) {
  fir_type_t sum = 0;
  fir->x[fir->ix] = new_value;
  fir->ix = (fir->ix + 1) % fir->taps;  // Never exceed taps for index

  for (int i=0;i<fir->taps;i++)
    sum += fir->x[(fir->ix + i) % fir->taps] * fir->coef[i];
  return sum;
}

int main () {
  // Filter init
  fir_type_t fir_filter_coef[] = { 0, .5, .75, .5, 0 };
  fir_type_t fir_history[]     = { 0,0,0,0,0 };
  fir_filter_t a;
  a.taps = sizeof(fir_filter_coef)/sizeof(fir_type_t);
  a.x    = fir_history;
  a.coef = fir_filter_coef;
  a.ix = 0;

  // Simulated data
  fir_type_t input_data[] = { 0,1,0,0,0, 0,0,0,0,0 }; // filter looks good, check
  // fir_type_t input_data[] = { 0,1,1,1,1, 1,1,1,1,1 };   // 1.75, check
  // double input_data[] = { 0,1,-1,1,-1, 1,-1,1,-1,1 };
  size_t input_data_length = sizeof(input_data)/sizeof(double);
  double out = -1;

  for (int i = 0; i < input_data_length; i++) {
    printf("x(%d)=%f\tf(x)=%f\n", i, input_data[i],
      fir_filter(input_data[i], &a));
  }

//  printf("fir->x[%d] = %f\n", 1, a.x[1]);
//  printf("fir->coef[%d] = %f\n", 2, a.coef[2]);

  return 0;
}
