
#include "detectPrimes.h"
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <pthread.h>

struct InputData{
  int id;
  int nthreads;
};
//Initialize global vectors to be used
std::vector<int64_t> all_nums;
std::vector<int64_t> result;
//Initialize Mutex 
pthread_mutex_t mutex_lock;

// returns true if n is prime, otherwise returns false
// -----------------------------------------------------------------------------
// to get full credit for this assignment, you will need to adjust or even
// re-write the code in this function to make it multithreaded.
static bool is_prime(int64_t n)
{
  // handle trivial cases
  if (n < 2) return false;
  if (n <= 3) return true; // 2 and 3 are primes
  if (n % 2 == 0) return false; // handle multiples of 2
  if (n % 3 == 0) return false; // handle multiples of 3
  // try to divide n by every number 5 .. sqrt(n)
  int64_t i = 5;
  int64_t max = sqrt(n);
  while (i <= max) {
    if (n % i == 0) return false;
    if (n % (i + 2) == 0) return false;
    i += 6;
  }
  // didn't find any divisors, so it must be a prime
  return true;
}

// from the dividing work in threads example 
void * thread_function(void * arg){
  struct InputData * inputdata = ((struct InputData *)arg);
  int id = inputdata -> id;
  int nthreads = inputdata -> nthreads;
  //round robin assignment using thread id to divide the work 
  for(int i = id; i < (int)all_nums.size(); i += nthreads){
    if (is_prime(all_nums[i])){
      pthread_mutex_lock(&mutex_lock);
      result.push_back(all_nums[i]);
      pthread_mutex_unlock(&mutex_lock);
    } 
  }
  pthread_exit(NULL);

}
// This function takes a list of numbers in nums[] and returns only numbers that
// are primes.
//
// The parameter n_threads indicates how many threads should be created to speed
// up the computation.
std::vector<int64_t>
detect_primes(const std::vector<int64_t> & nums, int n_threads)
{
  all_nums = nums;
  pthread_t threadpool[n_threads];
  InputData inputdata[n_threads];
  pthread_mutex_init(&mutex_lock, NULL);

  //Initial structure variables 
  for(int i = 0 ; i < n_threads; i++){
    inputdata[i].id = i;
    inputdata[i].nthreads = n_threads;
  }
  //Create Threads
  for(int i = 0; i < n_threads; i++){
    pthread_create(&threadpool[i],NULL,thread_function,(void *) &inputdata[i]);
  }
  //Join Threads
  for(int i = 0; i < n_threads; i++){
    pthread_join(threadpool[i],NULL);
    
  }
  //destroy the mutex
  pthread_mutex_destroy(&mutex_lock);

  return result;
}
