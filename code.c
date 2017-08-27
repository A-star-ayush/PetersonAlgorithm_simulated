#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>


// To compile: gcc code.c -lpthread
// To execute: ./a.out

int N;			// user input
int* level;   	// the values ranges from 0 to N - 1
int* last_to_enter;

// N - 1 waiting rooms..
// processes travel form one waiting to the another
// until they have made through all the N - 1 waiting rooms

int check2(int id, int i){
	// checks if some other process is at a greater level than the current process
	int k;
	for(k=0;k<N;++k)
		if(k!=id && level[k]>=i)
			break;
	return k!=N;
}

void _lock_(int id){
	printf("Thread %d requested the lock\n", id);
	int i;
	for(i=0;i<N;++i){
		level[id] = i;
		last_to_enter[i] = id;
		while(last_to_enter[i]==id && check2(id, i));
	}
	printf("Thread %d obtained the lock\n", id);	
	// lock obtained
}

void _unlock_(int id){
	level[id] = -1;
	printf("Thread %d released the lock\n", id);
	// unlocked
}

void* on_thread_start(void* _arg){
	int id = (intptr_t)_arg;
	srand(time(NULL));

	// some work
	usleep(rand() / 1000);
	
	// critical section
	_lock_(id);
	printf("Thread %d is inside the critical seciton\n", id);
	_unlock_(id);
	
	// rest of the work
	usleep(rand() / 1000);

	pthread_exit(NULL);  // no value returned to the main thread
}

int main(int argc, char const *argv[])
{
	int i;
	printf("N: ");
	scanf("%d", &N);   // N - number of processes
	
	level = calloc(N, sizeof(int));
	last_to_enter = calloc(N-1, sizeof(int));
	// calloc was used to allocate the memory since it will also auto initialize
	// the array elements with 0

	pthread_t threads[N];

	for(i=0;i<N;++i){
		pthread_create(&threads[i], NULL, on_thread_start, (void*)(intptr_t)i);
		// each thread will call upon the function on_thread_start passing to it
		// it's id = i	
	}
	

	for(i=0;i<N;++i){
		pthread_join(threads[i], NULL);
		// waiting for threads to complete
	}

	free(level);
	free(last_to_enter);

	return 0;
}