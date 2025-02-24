
/* bits type (header file)
 *
 * These are strings of bits of variable length
 *
 * last update: September 29th, 2024
 *
 * AM
 */

//#include "general.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// bits_t type
struct bits
{
   size_t nbytes;
   short ignore;
   char *byte;
};
typedef struct bits bits_t;

// functions
bits_t* bits_allocate(size_t nbits);
size_t bits_actual_length(bits_t *bits);
bits_t* bits_zeros(size_t nbits);
bits_t* bits_ones(size_t nbits);
bits_t* bits_random(size_t nbits);
bits_t* bits_clone(bits_t *bits);
size_t bits_count(bits_t *bits,bool bvalue);
size_t bits_nzeros(bits_t *bits);
size_t bits_nones(bits_t *bits);
bool bits_equals(bits_t *b1,bits_t *b2);
void bits_print(bits_t *bits);
void bits_println(bits_t *bits);
void bits_free(bits_t *bits);
// >>>>>>> additionals functions : long - bits conversion >>>>>>>
bits_t* bits_from_long(size_t n, unsigned long value);
unsigned long bits_to_long(bits_t* bits);

