
/* bits type
 *
 * These are strings of bits of variable length
 *
 * last update: September 29th, 2024
 *
 * AM
 */

#include "bits.h"

// allocating memory for a bits_t instance
bits_t* bits_allocate(size_t nbits)
{
   bits_t *bits = NULL;
   bits = (bits_t*)malloc(sizeof(bits_t));
   if (bits == NULL)  return NULL;
   bits->nbytes = nbits/8;
   bits->ignore = (short) (8 - nbits%8);
   if (bits->ignore != 0)  bits->nbytes++;
   bits->byte = NULL;
   bits->byte = (char*)calloc(bits->nbytes,sizeof(char));
   if (bits->byte == NULL)
   {
      free(bits);
      return NULL;
   }
   return bits;
}

// counting the number of actually used bits
size_t bits_actual_length(bits_t *bits)
{
   return 8*bits->nbytes - bits->ignore;
}

// constructor for a bits_t instance with all bits set to 0
bits_t* bits_zeros(size_t nbits)
{
   bits_t *bits = bits_allocate(nbits);
   if (bits == NULL)  return NULL;
   for (size_t k = 0; k < bits->nbytes; k++)  bits->byte[k] = 0;
   return bits;
}

// constructor for a bits_t instance with all bits set to 1
bits_t* bits_ones(size_t nbits)
{
   bits_t *bits = bits_allocate(nbits);
   if (bits == NULL)  return NULL;
   for (size_t k = 0; k < bits->nbytes; k++)  bits->byte[k] = (char)255;
   return bits;
}

// constructor for a bits_t instance with random bits
bits_t* bits_random(size_t nbits)
{
   bits_t *bits = bits_allocate(nbits);
   if (bits == NULL)  return NULL;
   for (size_t k = 0; k < bits->nbytes; k++)  bits->byte[k] = rand()%255;
   return bits;
}

// constructor for a bits_t instance cloned from another
bits_t* bits_clone(bits_t *bits)
{
   size_t nbits = bits_actual_length(bits);
   bits_t *cloned = bits_allocate(nbits);
   if (bits == NULL)  return NULL;
   for (size_t k = 0; k < bits->nbytes; k++)  cloned->byte[k] = bits->byte[k];
   return cloned;
}

// counting the number of bits having a given value
size_t bits_count(bits_t *bits, bool bvalue)
{
   size_t count = 0;
   for (size_t k = 0; k < bits->nbytes; k++)
   {
      short b = 7;
      char byte = bits->byte[k];
      if (k == 0 && bits->ignore != 0)  b = b - bits->ignore;
      for (; b >= 0; b--)
      {
         if ((byte&1) == bvalue)  count++;
         byte = byte >> 1;
      };
   };
   return count;
}

// counting the number of zeros
size_t bits_nzeros(bits_t *bits)
{
   return bits_count(bits,0);
}

// counting the number of ones
size_t bits_nones(bits_t *bits)
{
   return bits_count(bits,1);
}

// verifying equality between two bits_t instances
bool bits_equals(bits_t *b1,bits_t *b2)
{
   if (b1->nbytes != b2->nbytes)  return false;
   if (b1->ignore != b2->ignore)  return false;
   if (b1->ignore != 0)
   {
      char byte1 = b1->byte[0] << b1->ignore;
      char byte2 = b2->byte[0] << b2->ignore;
      if (byte1 != byte2)  return false;
   };
   for (size_t k = 1; k < b1->nbytes; k++)
   {
      if (b1->byte[k] != b2->byte[k])  return false;
   }
   return true;
}

// printing a bits_t instance
void bits_print(bits_t *bits)
{
   // the list of bits
   printf("[");
   for (size_t k = 0; k < bits->nbytes; k++)
   {
      short b = 7;
      if (k == b && bits->ignore != 0)  b = b - bits->ignore;
      for (; b >= 0; b--)
      {
         printf("%1d",(bits->byte[k]>>b)&1);
      }
      if (k != bits->nbytes - 1)  printf("|");
   }
   printf("] ");

   // info about the bit string
   size_t actual_bits = bits_actual_length(bits);
   printf("(%zu;%zu)",actual_bits,bits->nbytes);
}

// printing a bits_t instance with new line
void bits_println(bits_t *bits)
{
   bits_print(bits);
   printf("\n");
}

// freeing the memory for a bits_t instance
void bits_free(bits_t *bits)
{
   free(bits->byte);
   free(bits);
}

// >>>>>>> additionals functions : long - bits conversion >>>>>>>

// converting long into bits
bits_t* bits_from_long(size_t n, unsigned long value){
    if(n == 0) return NULL;
    bits_t* bits = bits_allocate(n);
    printf("[DEBUG] n_bytes : %zu\n", bits->nbytes);
    for(long i = (long)bits->nbytes - 1; i >= 0; i --) {
        char byte = (char)value;
        bits->byte[i] = byte;
        value = value >> 8;
    }
    return bits;
}

// converting bits into long
unsigned long bits_to_long(bits_t* bits){
    unsigned long final_long = 0;
    for(int i = 1; i < bits->nbytes; i ++) {
        unsigned long current_long = (long)bits->byte[i] << (bits->nbytes - 1 - i) * 8;
        final_long += current_long;
    }
    // shifting last byte 2 times to get rid of ignored information
    unsigned char last_char = (unsigned char)bits->byte[0];
    last_char = (unsigned char)(last_char << bits->ignore);
    last_char = (unsigned char)(last_char >> bits->ignore);
    unsigned long last_long = (unsigned long)last_char << (bits->nbytes - 1) * 8;
    final_long += last_long;
    return final_long;
}

