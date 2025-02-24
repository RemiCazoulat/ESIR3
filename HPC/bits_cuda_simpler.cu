
/* bits type
 *
 * for simplicity, we only transfer the array of unsigned chars!
 *
 * AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

// bits_t type
struct bits
{
   size_t nbytes;
   short ignore;
   unsigned char *byte;
};
typedef struct bits bits_t;

// allocating memory for a bits_t instance
__host__ bits_t* bits_allocate(size_t nbits)
{
   bits_t *bits = NULL;
   bits = (bits_t*)malloc(sizeof(bits_t));
   if (bits == NULL)  return NULL;
   bits->nbytes = nbits/8;
   bits->ignore = 0;
   short remainder = nbits%8;
   if (remainder != 0)
   {
      bits->ignore = (short) (8 - remainder);
      bits->nbytes++;
   };
   bits->byte = NULL;
   bits->byte = (unsigned char*)calloc(bits->nbytes,sizeof(unsigned char));
   if (bits->byte == NULL)
   {
      free(bits);
      return NULL;
   }
   return bits;
};

// counting the number of actually used bits
__host__ size_t bits_actual_length(bits_t *bits)
{
   return 8*bits->nbytes - bits->ignore;
};

// printing a bits_t instance
__host__ void bits_print(bits_t *bits)
{
   // the list of bits
   printf("[");
   for (size_t k = 0; k < bits->nbytes; k++)
   {
      short b = 7;
      if (k == 0 && bits->ignore != 0)  b = b - bits->ignore;
      for (; b >= 0; b--)
      {
         printf("%1d",(bits->byte[k]>>b)&1);
      };
      if (k != bits->nbytes - 1)  printf("|");
   };
   printf("] ");

   // info about the bit string
   size_t actual_bits = bits_actual_length(bits);
   printf("(%lu;%lu)",actual_bits,bits->nbytes);
};

// printing a bits_t instance with new line
__host__ void bits_println(bits_t *bits)
{
   bits_print(bits);
   printf("\n");
};

// freeing the memory for a bits_t instance
__host__ void bits_free(bits_t *bits)
{
   free(bits->byte);
   free(bits);
};

// kernel for a random bit string generation with predefined number of zeros and ones
// -> the number of ones is set to the number of ones forming the thread id
__global__ void generate(size_t nbytes,unsigned char *array)
{
   size_t id = (blockIdx.x*blockDim.x) + threadIdx.x;
   size_t istart = id*nbytes;  // this thread's starting and ending elements in the array
   size_t iend = (id + 1)*nbytes;
   size_t k = 0;
   for (size_t i = istart; i < iend; i++)
   {
      array[i] = (unsigned char) (id >> k);
      k = k + 8;
   };
};


// main
int main(int argc,char *argv[])
{
   size_t nbits = 16;  // the length of the bit strings is fixed
   size_t warp_size = 32;
   size_t warps_per_block = 4;
   size_t nthreads = warp_size*warps_per_block;  // 128
   size_t nblocks = 512;
   size_t n = nblocks*nthreads;

   // welcome message
   fprintf(stderr,"Transfers of the bit strings stored in the bits_t types between RAM and GPU global memory\n");

   // memory allocation on the RAM
   fprintf(stderr,"Memory allocation on the RAM ... ");
   bits_t **cpu_side = (bits_t**)calloc(n,sizeof(bits_t*));
   for (size_t k = 0; k < n; k++)  cpu_side[k] = bits_allocate(nbits);
   fprintf(stderr,"done!\n");

   // allocation of one long array of unsigned chars on the global memory
   fprintf(stderr,"The memory allocation on the GPU ... ");
   unsigned char *gpu_side;
   cudaMalloc((void**)&gpu_side,(n*nbits)*sizeof(unsigned char));
   fprintf(stderr,"done!\n");

   // invoking the kernel (each thread works on its piece of array)
   size_t nbytes = nbits/8;
   fprintf(stderr,"Invoking the kernel for generating the bit strings ...");
   generate<<<nblocks,nthreads>>>(nbytes,gpu_side);
   cudaDeviceSynchronize();
   fprintf(stderr,"done!\n");

   // memory transfer back from GPU global memory
   fprintf(stderr,"Memory transfer of the 'unsigned char' array ... ");
   for (size_t k = 0; k < n; k++)  cudaMemcpy(cpu_side[k]->byte,&gpu_side[k*nbytes],nbytes*sizeof(unsigned char),cudaMemcpyDeviceToHost);
   fprintf(stderr,"done!\n");

   // visualization of the bits_t instances
   fprintf(stderr,"Visualization of the results ... ");
   for (size_t k = 0; k < n; k++)  bits_println(cpu_side[k]);

   // freeing memory
   for (size_t k = 0; k < n; k++)  bits_free(cpu_side[k]);
   free(cpu_side);
   cudaFree(gpu_side);

   // ending
   fprintf(stderr,"We are done!\n");
   return 0;
};

