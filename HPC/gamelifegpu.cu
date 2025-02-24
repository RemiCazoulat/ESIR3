
/*
 * Conway's Game of Life
 *
 * basic sequential version
 *
 * AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

float compute_time(time_t start,time_t end)
{
   return ((float)((int)end - (int)start))/CLOCKS_PER_SEC;
};


typedef struct
{
   size_t n;
   size_t m;
   bool* t;
}  torus_t;

// converting (x,y) indices into one unique z index
size_t torus_index(size_t n,size_t m,size_t x,size_t y,int dx,int dy)
{
   assert(n > 0UL);
   assert(m > 0UL);
   long i = x + dx;
   long j = y + dy;
   if (i < 0)  i = n + i;
   if (j < 0)  j = m + j;
   i = i%n;
   j = j%m;
   return (size_t) i*m + j;  // row by row organization of cells
};

// torus memory allocation
torus_t* torus_allocate(size_t n,size_t m)
{
   assert(n > 0UL);
   assert(m > 0UL);
   torus_t *torus = NULL;
   torus = (torus_t*)malloc(sizeof(torus_t));
   if (torus == NULL)  return NULL;
   torus->n = n;
   torus->m = m;
   torus->t = NULL;
   torus->t = (bool*)calloc(n*m,sizeof(bool));
   if (torus->t == NULL)
   {
      free(torus);
      return NULL;
   };
   return torus;
};

// writing a torus cell location
void torus_write(torus_t *torus,size_t x,size_t y,bool value)
{
   size_t z = torus_index(torus->n,torus->m,x,y,0,0);
   torus->t[z] = value;
};

// reading a torus cell location
bool torus_read(torus_t *torus,size_t x,size_t y,int dx,int dy)
{
   size_t z = torus_index(torus->n,torus->m,x,y,dx,dy);
   return torus->t[z];
};

// random generating a torus (includes allocation)
torus_t* torus_random(size_t n,size_t m,double p)
{
   assert(n > 0UL);
   assert(m > 0UL);
   assert(p >= 0.0 && p <= 1.0);
   torus_t *torus = torus_allocate(n,m);
   for (int z = 0; z < n*m; z++)
   {
      bool cell = false;
      if (rand()%10000/10000.0 < p)  cell = true;
      torus->t[z] = cell;
   };
   return torus;
};

// loading a torus model from a file (includes allocation)
torus_t* torus_load(char* filename,bool DOSformat)
{
   // if your text file is encoded in DOS format,
   // two chars appear at the end of each line
   short endofline = 1;
   if (DOSformat)  endofline++;

   // attempting to open the file
   FILE *input = fopen(filename,"r");
   if (input == NULL)  return NULL;

   // estimating the torus size
   fseek(input,0,SEEK_END);
   size_t filesize = ftell(input);
   rewind(input);
   char line[256];
   fgets(line,sizeof(line),input);
   size_t m = ftell(input) - endofline;
   size_t n = filesize/m;
   torus_t *torus = torus_allocate(n,m);
   rewind(input);

   // reading the file char by char
   char c;
   size_t z = 0;
   while (EOF != (c = fgetc(input)))
   {
      if (c != '\n')
      {
         bool cell = false;
         if (c == 'x')  cell = true;
         torus->t[z] = cell;
         z++;
      };
   };

   // ending
   fclose(input);
   return torus;
};

// counting the number of alive neighbours for (x,y)-cell
size_t torus_neighbours(torus_t *torus,size_t x,size_t y)
{
   size_t count = 0;
   for (int dx = -1; dx <= 1; dx++)
   {
      for (int dy = -1; dy <= 1; dy++)
      {
         if (dx != 0 || dy != 0)
         {
            if (torus_read(torus,x,y,dx,dy))  count++;
         };
      };
   };
   return count;
};

// creating the next generation
void torus_next(torus_t *src,torus_t *dst)
{
   bool cell;
   size_t nn;
   for (size_t x = 0; x < src->n; x++)
   {
      for (size_t y = 0; y < src->m; y++)
      {
         cell = torus_read(src,x,y,0,0);
         nn = torus_neighbours(src,x,y);
         if (cell)  // alive
         {
            if (nn < 2 || nn > 3)  cell = false;  // dies
         }
         else  // dead
         {
            if (nn == 3)  cell = true;  // gets born
         };
         torus_write(dst,x,y,cell);
      };
   };
};

// printing the torus
void torus_print(torus_t* torus)
{
   for (int x = 0; x < torus->n; x++)
   {
      for (int y = 0; y < torus->m; y++)
      {
         bool cell = torus_read(torus,x,y,0,0);
         if (cell)  printf("x");  else  printf(" ");
      };
      printf("\n");
   };
};

// freeing the torus
void torus_free(torus_t *torus)
{
   free(torus->t);
   free(torus);
};

// drawing a separation line on the screen
void draw_line(size_t n)
{
   assert(n > 0UL);
   for (size_t i = 0; i < n; i++)  printf("-");
   printf("\n");
};

// converting (x,y) indices into one unique z index
size_t bool_index(size_t x,size_t y,int dx,int dy,size_t n,size_t m)
{
   assert(n > 0UL);
   assert(m > 0UL);
   long i = x + dx;
   long j = y + dy;
   if (i < 0)  i = n + i;
   if (j < 0)  j = m + j;
   i = i%n;
   j = j%m;
   return (size_t) i*m + j;  // row by row organization of cells
};

// reading a torus cell location
bool bool_read(bool *torus,size_t x,size_t y,int dx,int dy, size_t n, size_t m)
{
   size_t z = bool_index(x,y,dx,dy,n,m);
   return torus[z];
};

// printing the torus
void bool_print(bool* torus, size_t n, size_t m)
{
   for (int x = 0; x < n; x++)
   {
      for (int y = 0; y < m; y++)
      {
         bool cell = bool_read(torus,x,y,0,0,n,m);
         if (cell)  printf("x");  else  printf(" ");
      };
      printf("\n");
   };
};

__device__ size_t torus_index(size_t x,size_t y,int dx,int dy, size_t n, size_t m)
{
   long i = x + dx;
   long j = y + dy;
   if (i < 0)  i = n + i;
   if (j < 0)  j = m + j;
   i = i%n;
   j = j%m;
   return (size_t) i * m + j;  // row by row organization of cells
};


__device__ bool torus_read(bool *torus,size_t x, size_t y, int dx, int dy, size_t n, size_t m)
{
   size_t z = torus_index(x, y, dx, dy, n, m);
   return torus[z];
};


__device__ size_t torus_neighbours(bool* torus, size_t x, size_t y, size_t n, size_t m) {
    size_t count = 0;
    for (int dx = -1; dx <= 1; dx++)
    {
       for (int dy = -1; dy <= 1; dy++)
       {
          if (dx != 0 || dy != 0)
          {
             if (torus_read(torus, x, y, dx, dy, n, m))  count++;
          };
       };
    };
    return count;
}

__device__ void torus_write(bool *t, size_t x, size_t y, bool value, size_t n, size_t m)
{
   size_t z = torus_index(x,y,0,0,n,m);
   t[z] = value;
};


__device__ void cell_next(bool* src, bool* dst, size_t x, size_t y, size_t n, size_t m) {
    bool cell;
    size_t nn;
    cell = torus_read(src, x, y, 0, 0, n, m);
    nn = torus_neighbours(src, x, y, n, m);
    if (cell)  // alive
    {
        if (nn < 2 || nn > 3)  cell = false;  // dies
    }
    else  // dead
    {
        if (nn == 3)  cell = true;  // gets born
    }
    torus_write(dst, x, y, cell, n, m);
}

__global__ void run_step(bool* t_src, bool* t_dst) {
    size_t n = blockDim.x * gridDim.x;
    size_t m = blockDim.y * gridDim.y;
    size_t idx = (blockIdx.x*blockDim.x) + threadIdx.x;
    size_t idy = (blockIdx.y*blockDim.y) + threadIdx.y;
    cell_next(t_src, t_dst, idx, idy, n, m);

}

void compare_results(bool* cpu, bool* gpu, size_t n, size_t m) {
    size_t ndiff = 0;
    for(size_t i = 0; i < n; i ++) {
        for(size_t j = 0; j < m; j ++) {
            if(cpu[i *m + j] != gpu[i *m + j]) ndiff++;
        }
    }
    if(ndiff) printf("there is %i differences between gpu and cpu\n", (int)ndiff);
    else printf("there is no differences between gpu and cpu\n");
}


int main(int argc,char *argv[])
{
    size_t nstep = 10;
    bool gpu_running = true;
    bool cpu_running = true;
    bool print_grids = false;
    size_t nblocksx = 32;
    size_t nblocksy = 32;
    size_t nthreadsx = 64;
    size_t nthreadsy = 64;
    dim3 nblocks( nblocksx, nblocksy);
    dim3 nthreads( nthreadsx, nthreadsy);
    size_t nx = nblocksx * nthreadsx; // n
    size_t ny = nblocksy * nthreadsy; // m
    size_t n = nx * ny;
    time_t startclock, endclock;
    float time;
    
    
    printf("grid size : %i x %i, %i cells \n",(int)nx, (int)ny, (int)n);

    
    torus_t *t_src = torus_random(nx, ny, 0.1);
    torus_t *t_dst = torus_allocate(nx, ny);
    bool* b_src = (bool*)calloc(n,sizeof(bool));

    if(print_grids) {
        printf("printing initial config : \n");
        torus_print(t_src);
    }
    ////////////////////
    // GPU ////
    ////////////////////
    if(!gpu_running) goto cpu;
    bool* t_gpu_src;
    bool* t_gpu_dst;
    cudaMalloc((void**)&t_gpu_src,n*sizeof(bool));
    cudaMalloc((void**)&t_gpu_dst,n*sizeof(bool));
    
    cudaMemcpy(t_gpu_src,t_src->t,n*sizeof(bool),cudaMemcpyHostToDevice);
    printf("Computing game in gpu ... ");
    startclock = clock();
    for(size_t i = 0; i < nstep; i ++) {
        run_step<<<nblocks, nthreads>>>(t_gpu_src, t_gpu_dst);
        cudaDeviceSynchronize();
        bool* tmp = t_gpu_src;
        t_gpu_src = t_gpu_dst;
        t_gpu_dst = tmp;
    }
    endclock = clock();
    time = compute_time(startclock,endclock);
    printf("done, in %7.5f s. \n", time);
    cudaMemcpy(b_src,t_gpu_src,n*sizeof(bool),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    if(print_grids) {
        printf("printing gpu game : \n");
        bool_print(b_src, nx, ny);
    }


    
    ////////////////////
    // CPU ////
    ////////////////////
    cpu:
    if(!cpu_running) goto end;
    torus_t *tmp;
    printf("Computing game in cpu ... ");
    startclock = clock();
    for (size_t i = 0; i < nstep; i++)
    {
        torus_next(t_src,t_dst);
        //torus_print(t_dst);
        //draw_line(t_dst->m);
        //usleep(SPEED);
        tmp = t_src;
        t_src = t_dst;
        t_dst = tmp;
    };
    endclock = clock();
    time = compute_time(startclock,endclock);
    printf("done, in %7.5f s. \n", time);
    
    if(print_grids) {
        printf("printing cpu game : \n");
        torus_print(t_src);
    }
    end:
    if(gpu_running && cpu_running) compare_results(t_src->t, b_src, nx, ny);

    
}