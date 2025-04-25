#include "calcpi.h"
#include <pthread.h>

//Initalize structure of variables to be used 
struct Task{
  int start_x;
  int end_x;
  uint64_t partial_count;
};
Task task[256];

double global_rsq;
int global_r;

//thread function used to parallelize the algorith for pi 
void * pi_calc(void * arg){ 
  struct  Task * task = (struct Task *)arg;
  // initial each variable 
  int start_x = task ->start_x;
  int end_x = task ->end_x;
  uint64_t partial_count = 0;

  for( double x = start_x + 1 ; x <= end_x ; x ++){
    for( double y = 0 ; y <= global_r ; y ++){
      if( x*x + y*y <=  global_rsq) partial_count ++;
    }
  }
  //Return partial_count
  task -> partial_count = partial_count;
  pthread_exit(NULL); 
}

uint64_t count_pixels(int r, int n_threads)
{
  double rsq = double(r) * r;
  uint64_t total_count = 0;

  //Initialize pthreads
  pthread_t threadpool[n_threads];

  //Divide the workload between each thread
  int div = r/n_threads;
  int mod = r % n_threads;
  int lastend = 0;

  for(int i = 0; i < n_threads; i++ ){
    task[i].start_x = lastend;
    global_rsq = rsq;
    global_r = r;
    if(i < mod){
      task[i].end_x = task[i].start_x + div + 1;
    }
    else{
      task[i].end_x = task[i].start_x + div;
    }
    lastend = task[i].end_x;
    pthread_create(&threadpool[i],NULL,pi_calc,(void *) &task[i]);
  }
 
  //Join the threads
  for(int i = 0; i < n_threads; i++){
    pthread_join(threadpool[i],NULL);
  }
  //Add the partial count from each thread to get the total 
  for(int i = 0; i < n_threads; i++ ){ 
    total_count += task[i].partial_count;
  }
  return total_count * 4 + 1;
}
