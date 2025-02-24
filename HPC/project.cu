// >>> Example of logs on a T4 gpu with 32 blocks in the grid and 64 threads per block >>>
/*
>>> Computing for the bits_t** array on gpu :
[bits_t a] init of gpu memory ... in 0.865703 s.
[bits_t a] calling first kernel ... in 0.000703 s.
[bits_t a] getting memory back on cpu ... in 1.648545 s.
first bits_t : 0 [00000000|00000000|00000000|00000000] (32;4)
...
last bits_t  : 65535 [00000000|00000000|11111111|11111111] (32;4)
[result] everything works well.

>>> Computing for the unsigned char* array on gpu :
[u char a] init of gpu memory ... in 0.000026 s.
[u char a] calling first kernel ... in 0.000082 s.
[u char a] getting memory back on cpu ... in 0.545371 s.
first bits_t : 0 [00000000|00000000|00000000|00000000] (32;4)
...
last bits_t  : 65535 [00000000|00000000|11111111|11111111] (32;4)
[result] everything works well.
 */
// >>>>>>> Interpretation >>>>>>>
// We can see on the logs that the init of the gpu memory is a lot longer with the bits_t** array than the char* array.
// It can be explained because the memory allocation is more complicated in the first case.

// Also, calling the kernel, even though it is doing the same operations, is longer with the bits_t** array.
// We can suppose that it is because the memory is more fragmented than the memory of a simple char* array, and accessing it could be longer.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

// >>>>>>> bits_t struct >>>>>>>
struct bits
{
    size_t nbytes;
    short ignore;
    unsigned char *byte;
};
typedef struct bits bits_t;

// >>>>>>> given functions >>>>>>>
__host__ float compute_time(time_t start,time_t end)
{
    return ((float)((int)end - (int)start))/CLOCKS_PER_SEC;
}

__host__ size_t bits_actual_length(bits_t *bits)
{
    return 8*bits->nbytes - bits->ignore;
}

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
        }
        if (k != bits->nbytes - 1)  printf("|");
    }
    printf("] ");

    // info about the bit string
    size_t actual_bits = bits_actual_length(bits);
    printf("(%zu;%zu)",actual_bits,bits->nbytes);
}

__host__ void bits_println(bits_t *bits)
{
    bits_print(bits);
    printf("\n");
}

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

// >>>>>>> project functions >>>>>>>

// >>> Host functions >>>

// converts a bits_t into an unsigned long
__host__ unsigned long bits_to_long(bits_t* bits){
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
};

// takes the first n bits of an unsigned long, and converts it into a bits_t
__host__ bits_t* bits_from_long(size_t n, unsigned long value){
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

__host__ void print_results(unsigned long* longs, size_t n, bits_t** bits_cpu) {
    /*
    if (see_bytes) {
        for (int k = 0; k < n; k++) {
            printf("%lu ", longs[k]);
            bits_println(bits_cpu[k]);
        }
    }
    */

    printf("first bits_t : ");printf("%lu ", longs[0]); bits_println(bits_cpu[0]);
    printf("...\n");
    printf("last bits_t  : ");printf("%lu ", longs[n - 1]); bits_println(bits_cpu[n - 1]);

    int result2 = 0;
    for(int k = 1; k < n; k ++) {
        if(longs[k - 1] != longs[k] - 1) result2 ++;
    }
    if(result2 > 0) printf("[result] there is %d/%zu problems. \n", result2, n);
    else printf("[result] everything works well. \n");
};

// >>> kernels >>>
/**
 * Here, it is assumed that we no longer need the n, to avoid having 2 times the same bits_t.
 * the unsigned long value is now the id of the thread.
 * We only need an array of bits_t pointer to store the result.
 */
__global__ void bits_from_long_gpu(bits_t** bits) {
    unsigned long id = (blockIdx.x * blockDim.x) + threadIdx.x;
    unsigned long value = id;
    for(long i = bits[id]->nbytes - 1; i >= 0; i --) {
        unsigned char byte = (unsigned char)value;
        bits[id]->byte[i] = byte;
        value = value >> 8;
    }
}

/**
 * Another kernel, that gets rid of the bits_t** array and replaces it by an unsigned char* array.
 * We this time need to have the nbytes argument, previsously stored in the bits_t struct.
 */
__global__ void chars_from_long_gpu(unsigned char* chars, size_t nbytes) {
    unsigned long id = (blockIdx.x * blockDim.x) + threadIdx.x;
    unsigned long value = id;
    for(long i = nbytes - 1; i >= 0; i --) {
        unsigned char byte = (unsigned char)value;
        chars[i + id * nbytes] = byte;
        value = value >> 8;
    }
}

// >>>>>>> main >>>>>>>
int main(int argc,char *argv[]) {
    size_t nblocks = 32 * 32;
    size_t nthreads = 64;
    size_t n = nblocks * nthreads;
    size_t nbytes = 4; // or 32 bits
    short ignore = 0;

    bits_t** bits_cpu; // array for cpu
    bits_t** bits_gpu; // array for the first kernel
    unsigned char * chars_gpu; // array for the second kernel
    unsigned long* longs; // array to verify the kernels

    time_t startclock, endclock;
    float time;


    // >>>>>>> init cpu arrays >>>>>>>
    bits_cpu = (bits_t**)calloc(n,sizeof(bits_t*));
    longs = (unsigned long*)calloc(n,sizeof(unsigned long));

    // >>>>>>> --------------------------------- >>>>>>>
    //    >>>>>>> First try : bits_t** array on gpu >>>>>>>
    // >>>>>>> --------------------------------- >>>>>>>

    // >>>>>>> init gpu array >>>>>>>
    printf(">>> Computing for the bits_t** array on gpu : \n");
    printf("[bits_t a] init of gpu memory ...");
    startclock = clock();

    bits_t** tmp = (bits_t**)calloc(n, sizeof(bits_t*));
    for(int k = 0; k < n; k ++) {
        // creating temporary bits_t on cpu
        bits_t* bits_tmp = bits_allocate((nbytes * 8) - ignore);
        // freeing bits_tmp->byte now to avoid loosing the pointer and make impossible the free.
        free(bits_tmp->byte);
        // allocating a memory in gpu for byte array and putting the gpu pointer in bits_tmp
        cudaMalloc((void**)&bits_tmp->byte, nbytes*sizeof(unsigned char));
        // allocating a memory space of the size of bits_t struct
        cudaMalloc((void**)&tmp[k], sizeof(bits_t));
        // copying bits_tmp (that already contains the byte array on gpu) on gpu
        cudaMemcpy(tmp[k], bits_tmp, sizeof(bits_t), cudaMemcpyHostToDevice);
        // freeing memory on cpu
        free(bits_tmp);
    }
    cudaMalloc((void**)&bits_gpu, n*sizeof(bits_t*));
    // copying every pointer of bits_t on gpu
    cudaMemcpy(bits_gpu, tmp, n*sizeof(bits_t*), cudaMemcpyHostToDevice);
    free(tmp);

    endclock = clock();
    time = compute_time(startclock, endclock);
    printf(" in %f s.\n", time);

    // >>>>>>> calling kernel >>>>>>>
    printf("[bits_t a] calling first kernel ...");
    startclock = clock();

    bits_from_long_gpu<<<nblocks, nthreads>>>(bits_gpu);
    cudaDeviceSynchronize();

    endclock = clock();
    time = compute_time(startclock, endclock);
    printf(" in %f s.\n", time);

    // >>>>>>> getting memory back >>>>>>>
    printf("[bits_t a] getting memory back on cpu ...");
    startclock = clock();

    cudaMemcpy(bits_cpu, bits_gpu, n*sizeof(bits_t*), cudaMemcpyDeviceToHost);
    for(int k = 0; k < n; k ++) {
        // copying back the bits_t
        bits_t* bits_tmp = bits_allocate(nbytes * 8 - ignore);
        cudaMemcpy(bits_tmp, bits_cpu[k], sizeof(bits_t), cudaMemcpyDeviceToHost);
        cudaFree(bits_cpu[k]); // free memory on gpu
        // copying back the byte array
        unsigned char* byte_tmp = (unsigned char*)malloc(nbytes * sizeof(unsigned char));
        cudaMemcpy(byte_tmp, bits_tmp->byte, nbytes*sizeof(unsigned char),cudaMemcpyDeviceToHost);
        cudaFree(bits_tmp->byte); // free memory on gpu
        bits_tmp->byte = byte_tmp;
        bits_cpu[k] = bits_tmp;
    }

    endclock = clock();
    time = compute_time(startclock, endclock);
    printf(" in %f s.\n", time);

    // >>>>>>> transforming bits_t into long >>>>>>>
    for(int k = 0; k < n; k ++) {
        longs[k] = bits_to_long(bits_cpu[k]);
    }

    // >>>>>>> printing the result >>>>>>>
    print_results(longs, n, bits_cpu);

    // >>>>>>> --------------------------------- >>>>>>>
    //    >>>>>>> second try : unsigned char* array >>>>>>>
    // >>>>>>> --------------------------------- >>>>>>>

    // >>>>>>> init gpu array >>>>>>>
    printf("\n>>> Computing for the unsigned char* array on gpu : \n");
    printf("[u char a] init of gpu memory ...");
    startclock = clock();

    cudaMalloc((void**)&chars_gpu,n*nbytes*sizeof(unsigned char));

    endclock = clock();
    time = compute_time(startclock, endclock);
    printf(" in %f s.\n", time);

    // >>>>>>> calling kernel >>>>>>>
    printf("[u char a] calling first kernel ...");
    startclock = clock();

    chars_from_long_gpu<<<nblocks, nthreads>>>(chars_gpu, nbytes);
    cudaDeviceSynchronize();

    endclock = clock();
    time = compute_time(startclock, endclock);
    printf(" in %f s.\n", time);

    // >>>>>>> passing chars into bits_t >>>>>>>
    printf("[u char a] getting memory back on cpu ...");
    startclock = clock();
    for(int k = 0; k < n; k ++) {
        // passing chars into bits_t
        cudaMemcpy(bits_cpu[k]->byte,&chars_gpu[k*nbytes],nbytes*sizeof(unsigned char),cudaMemcpyDeviceToHost);
    }
    endclock = clock();
    time = compute_time(startclock, endclock);
    printf(" in %f s.\n", time);

    // >>>>>>> transforming bits_t into long >>>>>>>
    for(int k = 0; k < n; k ++) {
        longs[k] = bits_to_long(bits_cpu[k]);
    }

    // >>>>>>> printing the result >>>>>>>
    print_results(longs, n, bits_cpu);

    // >>>>>>> freeing memory >>>>>>>
    cudaFree(bits_gpu);
    cudaFree(chars_gpu);
    for(int k = 0; k < n; k ++) {
        free(bits_cpu[k]->byte);
        free(bits_cpu[k]);
    }
    free(bits_cpu);
    free(longs);

}