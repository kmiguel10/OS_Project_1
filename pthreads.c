#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

int sharedVariable = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;

void* simpleThread(void* threadIndex) {

	int num, val;

	int* indexPointer = (int *)threadIndex;
	int which = *indexPointer;	

	for (num = 0; num < 20; num++) {
	
		if (random() > RAND_MAX/2){
			usleep(500);
		}

		
		#ifdef PTHREAD_SYNC
		pthread_mutex_lock(&mutex);	
		#endif
		

		val = sharedVariable;
		printf("***thread %d sees value %d\n", which, val);
		sharedVariable = val + 1;

		
		#ifdef PTHREAD_SYNC
		pthread_mutex_unlock(&mutex);		
		#endif
		
	}


	pthread_barrier_wait(&barrier);
	
	val = sharedVariable;
  		
	printf("Thread %d sees final value %d\n", which, val);
	pthread_exit(NULL);
}

bool validateArgument(char* argument) {
	int i = 0;

	if (strcmp(argument, "") == true){
		return false;
	}

	while (argument[i] != '\0') {
		if (!isdigit(argument[i])){
			return false;	
		}
		i++;
	}
	return true;	
}

int main(int argc, char* argv[]) {
        
	int numberOfThreads = 0;

	if (argc > 1) {
		if (validateArgument(argv[1])) {
               		numberOfThreads = atoi(argv[1]);
        	}
	}
	
	int i;
	int createFailure;
	pthread_t threads[numberOfThreads];

	int indeces[numberOfThreads];
	
	if (pthread_barrier_init(&barrier, NULL, numberOfThreads)){
		printf("Barrier not created\n");
		return 1;
	}

	if (pthread_mutex_init(&mutex, NULL)) {
		printf("Mutex not created\n");
		return 1;
	}


	/*
 		This loop simply creates an array of integers from 0 to n where n is the number of threads.
		I'm using this array of integers to identify each thread. I pass this integer to the "simpleThread"
		method later on when I create each of the threads with pthread_create.
 	*/ 	
	for (i = 0; i < numberOfThreads; i++) {
		indeces[i] = i;
	} 

	for (i = 0; i < numberOfThreads; i++) {		
		createFailure = pthread_create(&threads[i], NULL, &simpleThread, &indeces[i]); 
		if (createFailure) { printf("Thread %d not created successfully\n", i); return 1;}
		else { printf("Thread %d created successfully\n", i);}
	}
	
	for (i = 0; i < numberOfThreads; i++) {
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);	
	pthread_barrier_destroy(&barrier);

	return 0;
}
