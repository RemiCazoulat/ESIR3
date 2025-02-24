
/* bits type (testing)
 *
 * These are strings of bits of variable length
 *
 * last update: September 29th, 2024
 *
 * AM
 */

#include "bits.h"

int main()
{
   printf("bits_t type\n");
   bits_t *zero = bits_zeros(30);
   bits_println(zero);
   printf("Number of 0's : %zu\n",bits_nzeros(zero));
   bits_t *one = bits_ones(20);
   bits_println(one);
   printf("Number of 1's : %zu\n",bits_nones(one));
   printf("These two bit strings are identical : ");
   bool answer = bits_equals(zero,one);
   if (answer) printf("YES!\n"); else printf("NO\n");
   bits_t *random = bits_random(40);
   bits_println(random);
   printf("Number of 0's : %zu\n",bits_nzeros(random));
   printf("Number of 1's : %zu\n",bits_nones(random));
   printf("Cloning ... ");
   bits_t *cloned = bits_clone(random);
   bits_println(cloned);
   printf("These two bit strings are identical : ");
   answer = bits_equals(cloned,random);
   if (answer) printf("YES!\n"); else printf("NO\n");

   // testing long - bits conversion
   printf("starting long - bit conversion \n");
   unsigned long value = 1 + 4 + 16 + 2048 + 4096;
   printf("value : %lu \n", value);
   bits_t* b_long = bits_from_long(2, value);
   bits_println(b_long);
   value = bits_to_long(b_long);
   printf("value : %lu\n", value);
   bits_free(b_long);
   // end

   bits_free(cloned);
   bits_free(random);
   bits_free(one);
   bits_free(zero);
}