/* Joe Gorse, 2016
 * iir filter
 * BSD 2-clause license.
 */

/*
 * This is a spartan IIR implementation.
 * It does no checking, no dynamic allocation, and is without sequins or frills.
 * I built it for simplicity, speed, and fun.
 *
 * We use the Transposed Direct-Form II (for the EE's who may care) because it
 * uses less memory and MATLAB/Octave call it filter().
 *
 * Conceptual overview:
 * x(k) input at time k
 * y(k) output
 * y(k) = a * y(k-1) + b * x(k) where the coefficients a(1:N) and b(0:M)
 * a() coefficients are associated with the denominator of a transfer function
 * b() coefficients, the numerator and FIR filter equivalent when a(:) = 0
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>

typedef double FILTER_TYPE_DATA;

/* Bring your own coefficients and filter state buffers. */
/* If N<M or M<N, pad the shorter vector with zeros to match length. */
typedef struct {
   // char name[32];      // Just in case you use these things
   size_t         taps;  // How many taps we have, max(N,M)
   FILTER_TYPE_DATA *a;  // Pointer to array of a(1:taps) coefficients, taps+1 long, index 0 not used
   FILTER_TYPE_DATA *b;  // Pointer to array of b(0:taps) coefficients, taps+1 long w/ index 0 coefficient
   FILTER_TYPE_DATA *z;  // Filter state (unit delay memory), taps long
} IIR_FILTER;

/* Give it a new input value, get one output value in return. */
FILTER_TYPE_DATA iir_filter (FILTER_TYPE_DATA new_x_value, IIR_FILTER *iir) {
   FILTER_TYPE_DATA yk = 0;
   yk = new_x_value * iir->b[0] + iir->z[0];
   // Now that we have what we came for, wait to calculate the rest of state
   for (int i = 1; i < iir->taps; i++)
     iir->z[i-1] = iir->z[i] + new_x_value * iir->b[i] - yk * iir->a[i]; // Sign change on a()
   // Calculate the last (outermost) delay value, minding taps+1 length
   iir->z[iir->taps-1] = new_x_value * iir->b[iir->taps] - yk * iir->a[iir->taps];
   return yk;
}

int main () {
  // Filter init
  FILTER_TYPE_DATA iir_filter_coef_b[] = {0.184523494307847,0,-0.184523494307847};
  FILTER_TYPE_DATA iir_filter_coef_a[] = {1,-1.59697023454539,0.630953011384306};
  FILTER_TYPE_DATA iir_history_z[]     = { 0,0 }; // Note: this is the order of the system and
                                                  // shorter than coefficients by 1
  IIR_FILTER a;
  a.taps = sizeof(iir_history_z)/sizeof(FILTER_TYPE_DATA);
  a.z    = iir_history_z;
  a.b    = iir_filter_coef_b;
  a.a    = iir_filter_coef_a;

  // Simulated data
  int16_t input_data[] = { 0,1,0,0,0, 0,0,0,0,0 }; // filter matches MATLAB/Octave, check
  // int16_t input_data[] = { 0,1,1,1,1,   1,1,1,1,1 };
  // int16_t input_data[] = { 0,1,-1,1,-1, 1,-1,1,-1,1 };

  size_t input_data_length = sizeof(input_data)/sizeof(int16_t);

  for (int i = 0; i < input_data_length; i++) {
    printf("x(%d)=%d\tf(x)=%f\n", i, input_data[i],
      iir_filter(input_data[i], &a));
  }

  return 0;
}
