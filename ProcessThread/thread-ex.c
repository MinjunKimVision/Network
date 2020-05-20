#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

#include <pthread.h>//threading

#define maxTh 100

//int othread_create(pthread_t *thread, const pthread_attr_t * attr,
// void *(start_routine)(void *), void *arg)

//int pthread_join(pthread_t th, void **thread_return);

void *exThreadFunc(void * no);
int sum = 0;

int main(int argc, char *argv[]){
	int result, number, i, rst;
	pthread_t ex_thread[maxTh];
	if(argc != 2){
		printf("usage : %s number\n", argv[0]);
	}
	number = atoi(argv[1]);
	for(i = 0; i <number ; i++){
		result = pthread_create(&ex_thread[i], NULL, exThreadFunc, (void *)&i);
		printf("%d Thread Id at Main = %ud \n", i+1, (int)ex_thread[i]);
		if(result < 0){
			perror("thread create error : \n");
			continue;
		}
		sleep(1);
	}
	for(i = 0; i < number; i++){
		pthread_join(ex_thread[i], (void *)&rst);
	}
	return 0;
}
void *exThreadFunc(void *no){
	int seq;
	pthread_t id;
	seq = *((int *) no);
	id = pthread_self();
	printf("%d Thread Id at exThreadFunc = %ud \n",seq+1,(int)id);
	srand(time(NULL));
	sleep(rand()%5);
	sum++;
	printf("%d<-------- Thread Ended with Sum = %d --------> \n",seq+1, sum);
}
//eof
