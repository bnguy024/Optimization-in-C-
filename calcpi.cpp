// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================
//
// count_pi() calculates the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.

// CPSC 457 winter 2021 
// 30009135, Brandon Nguyen 



#include "calcpi.h"
#include <stdio.h>
#include <math.h> 
#include <iostream>
#include <pthread.h> // pthread

int n_threads;  // Global Var for threads
int r;          // Global var for radius 

struct Task{                // struct to be used for thread 
  int start_row = 0, end_row = 0; 
  uint64_t partial_count = 0;  
}; 
uint64_t count_pixels(int r, int n_threads)
{
   double rsq = double(r) * r;
   uint64_t count = 0;
   for( double x = 1 ; x <= r ; x ++)
     for( double y = 0 ; y <= r ; y ++)
       if( x*x + y*y <= rsq) count ++;
   return count * 4 + 1;
 }

// Starting routine for threads, test the outter loop from the count_pixel function
void * test_threads(void * a){
  struct Task * task_range = ((struct Task*)a);
  uint64_t start = task_range -> start_row; 
  uint64_t end = task_range -> end_row; 
  uint64_t p_count = task_range -> partial_count;  
  int radius = r; 
  
  double rsq = double(radius) * radius;
  
  for(int num = start; num < end; num++)//parallize the outer loop 
     for( double y = 0 ; y <= r ; y ++)
      if( num*num + y*y <= rsq) p_count ++;
      
    // update the count after the thread is done
  task_range->partial_count = p_count;
  
  pthread_exit(NULL);
  
}

int main(int argc, char *argv[]) {
  r = atol(argv[1]); //access global var from command line
  n_threads = atol(argv[2]);

// delcare the threads and their params 
  pthread_t threadPool[n_threads];
  Task task[n_threads];
  uint16_t p_count = 0; 
  int workload = ceil(r/n_threads);

  // initialize task params, create the threads 
  for(int i = 0; i < n_threads; i++ )
  {

    task[i].start_row = workload * i  + 1 ;
    task[i].end_row = (workload * (i + 1)) + 1;
  
   //Check for when the last thread end_range is equal to r + 1 
    if(r % task[i].start_row != 0){    
         if(task[i].start_row  ==  n_threads || task[i].end_row >= n_threads * workload + 1 ){
           if(task[i].end_row != r + 1 ){
                task[i].end_row = r + 1;
           }
      }
    }
    pthread_create(&threadPool[i], NULL, test_threads, &task[i]);
  
  } 
   
  //wait for threads to finish and sum up all the counts 
  for(int i = 0; i < n_threads; i++){
    pthread_join(threadPool[i], NULL);
    p_count += task[i].partial_count;
  } 
  //apply the equation to sum up all quandrands of pie 
  p_count = p_count * 4 + 1;   
  //print statements for the pi and count 
  double pi = p_count / (double(r) * r);
  printf("Calculating PI with r=%d and n_threads=%d\n",r ,n_threads);
  printf("count: %d\n",p_count);
  printf("PI: %f\n",pi);

  return 0; 
}