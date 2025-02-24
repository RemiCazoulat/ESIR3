#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include "bits_cuda_simpler.cu"

__host__ bits_t* bits_from_long(size_t n, unsigned long value){
    if(n == 0) return NULL;
    bits_t* bits = bits_allocate(n);
    printf("[DEBUG] n_bytes : %zu\n", bits->nbytes);
    //for(long i = (long)bits->nbytes - 1; i >= 0; i --) {
    for(long i = 0; i < (long)bits->nbytes - 1; i ++) {
        char byte = (char)value;
        bits->byte[i] = byte;
        value = value >> 8;
    }
    return bits;
}

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
}

// bits_from_long, but on GPU
__global__ void generate_ids(unsigned char* bits, size_t nbytes, unsigned long* debug) {
    unsigned long id = (blockIdx.x * blockDim.x) + threadIdx.x;
    debug[id] = id;
    unsigned long value = id;
    for(long i = nbytes - 1; i >= 0; i --) {
        unsigned char byte = (unsigned char)value;
        bits[i + id * nbytes] = byte;
        value = value >> 8;
    }
}

int main(int argc,char *argv[]) {

    size_t nblocks = 4;
    size_t nthreads = 32;
    time_t startclock, endclock;
    float time;

    size_t n = nblocks * nthreads;
    size_t nbytes = 2;
    short ignore = 0;
    bits_t** bits_cpu;
    unsigned char* bits_gpu;
    unsigned long* debug_cpu;
    unsigned long* debug_gpu;

    // init cpu array
    bits_cpu = (bits_t**)calloc(n,sizeof(bits_t*));

    for(int i = 0; i < n; i ++) {
        bits_cpu[i] = bits_allocate(nbytes * 8 - ignore);
    }
    debug_cpu = (unsigned long*)calloc(n, sizeof(unsigned long));
    // init gpu array
    cudaMalloc((void**)&bits_gpu,n*nbytes*sizeof(unsigned char));
    cudaMalloc((void**)&debug_gpu, n*sizeof(unsigned long));

    // calling kernel
    generate_ids<<<nblocks, nthreads>>>(bits_gpu, nbytes, debug_gpu);
    cudaDeviceSynchronize();
    // passing chars into bits_t and converting bits_t into unsigned long
    unsigned long* longs = (unsigned long*)calloc(n,sizeof(unsigned long));

    for(int i = 0; i < n; i ++) {
        // passing chars into bits_t
        cudaMemcpy(bits_cpu[i]->byte,&bits_gpu[i*nbytes],nbytes*sizeof(unsigned char),cudaMemcpyDeviceToHost);
        //converting bits_t into unsigned long
        longs[i] = bits_to_long(bits_cpu[i]);
    }
    cudaMemcpy(debug_cpu, debug_gpu, n*sizeof(unsigned long), cudaMemcpyDeviceToHost);

    // Verifying result
    // Visually

    for(int i = 0; i < n; i ++) {
        printf("%i, ",(int)debug_cpu[i]);
        printf("%i, ", (int)longs[i]);
        bits_println(bits_cpu[i]);
    }
    // Algo
    int result2 = 0;
    for(int i = 1; i < n; i ++) {
        if(longs[i - 1] != longs[i] - 1) result2 ++;
    }
    if(result2 > 0) printf("there is %i/%i problems", (int)result2, (int)n);
    else printf("There is no problems");
}