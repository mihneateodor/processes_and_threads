#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

sem_t sem_ex2_3_1,sem_ex2_3_2;			//pt problema 2.3
sem_t sem_ex2_4_b;						//pt problema 2.4 

sem_t *semEx2_5_1;
sem_t *semEx2_5_2;
sem_t *semEx2_5_3;
sem_t *semEx2_5_4;

int nrThreadsP7=0;
pthread_mutex_t lockP7Th11=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condTh11=PTHREAD_COND_INITIALIZER;

typedef struct TH_params{
	int id_proces;
	int id;
}TH_params;

void * fctThread3_2(void * param){				//ultimul thread care trebuie sa se termine T3.2
	TH_params * s=(TH_params *) param;
	sem_wait(semEx2_5_3);
	info(BEGIN,s->id_proces,s->id);
	info(END,s->id_proces,s->id);
	return NULL;
}

void * fctThread3_4(void * param){				//primul thread care trebuie sa se termine T3.4
	TH_params * s=(TH_params *) param;
	info(BEGIN,s->id_proces,s->id);
	info(END,s->id_proces,s->id);
	sem_post(semEx2_5_1);
	return NULL;
}

void * fctThread2_1(void *param){				//al doilea thread care trebuie sa se termine T2.1
	TH_params * s=(TH_params *) param;
	sem_wait(semEx2_5_2);
	info(BEGIN,s->id_proces,s->id);
	info(END,s->id_proces,s->id);
	sem_post(semEx2_5_4);
	return NULL;
}

void * fctThreadP7_TH11(void * param){
	TH_params *s=(TH_params *)param;
	sem_wait(&sem_ex2_4_b);
	
	pthread_mutex_lock(&lockP7Th11);
	nrThreadsP7++;
	info(BEGIN,s->id_proces,s->id);
	while(nrThreadsP7<5){
		pthread_cond_wait(&condTh11,&lockP7Th11);
	}
	info(END,s->id_proces,s->id);
	nrThreadsP7--;
	pthread_mutex_unlock(&lockP7Th11);
	
	sem_post(&sem_ex2_4_b);
	return NULL;
}

void* fctThreadP7(void *param){
	TH_params * s=(TH_params *)param;
	sem_wait(&sem_ex2_4_b);
	nrThreadsP7++;
	info(BEGIN,s->id_proces,s->id);
	pthread_mutex_lock(&lockP7Th11);
	info(END,s->id_proces,s->id);
	nrThreadsP7--;
	while(nrThreadsP7==5){
		pthread_cond_signal(&condTh11);
	}
	pthread_mutex_unlock(&lockP7Th11);
	sem_post(&sem_ex2_4_b);
	return NULL;
}

void * fctThread2_4(void *param){
	TH_params * s=(TH_params *)param;
	sem_wait(&sem_ex2_3_1);
	info(BEGIN,s->id_proces,s->id);
	info(END,s->id_proces,s->id);
	sem_post(&sem_ex2_3_2);
	return NULL;
}

void * fctThread2_3(void *param){
	TH_params * s=(TH_params *)param;
	info(BEGIN,s->id_proces,s->id);
	sem_post(&sem_ex2_3_1);
	sem_wait(&sem_ex2_3_2);
	info(END,s->id_proces,s->id);
	return NULL;
}

void * fctThreadBasic(void * param){
	TH_params * s=(TH_params *)param;
	info(BEGIN,s->id_proces,s->id);

	info(END,s->id_proces,s->id);
	return NULL;
}

void createProcesses(){
	int pid2,pid3,pid4,pid5,pid6,pid7; //pid1,
	
	pid2=fork();				
	if(pid2==-1){
		perror("Could NOT create P2.\n");
		return;
	}
	else if(pid2==0){							//procesul P2;
		semEx2_5_2=sem_open("sem1",O_CREAT,0644,0);
		semEx2_5_4=sem_open("sem2",O_CREAT,0644,0);
		info(BEGIN, 2, 0);
		pthread_t tid2[4];
		TH_params P2_threads[4];
		for(int i=0;i<4;i++){
			P2_threads[i].id=i+1;
			P2_threads[i].id_proces=2;
			if(i+1==1){
				if(pthread_create(&tid2[i],NULL,fctThread2_1,&P2_threads[i])!=0){
					perror("Could NOT create thread.\n");
					return;
				}
			}
			else if(i+1==3){
				if(pthread_create(&tid2[i],NULL,fctThread2_3,&P2_threads[i])!=0){
					perror("Could NOT create thread.\n");
					return;
				}
			}
			else if(i+1==4){
				if(pthread_create(&tid2[i],NULL,fctThread2_4,&P2_threads[i])!=0){
					perror("Could NOT create thread.\n");
					return;
				}
			}
			else if(pthread_create(&tid2[i],NULL,fctThreadBasic,&P2_threads[i])!=0){
				perror("Could NOT create thread.\n");
				return;
			}
		}
		for(int i=0;i<4;i++){
			if(pthread_join(tid2[i],NULL)!=0){
				perror("Could NOT join thread.\n");
				return;
			}
		}
		sem_unlink("sem1");
		sem_unlink("sem2");
		sem_destroy(&sem_ex2_3_1);
		sem_destroy(&sem_ex2_3_2);
		info(END, 2, 0);
	}
	else{										//procesul P1
		pid3=fork();
		if(pid3==-1){
			perror("Could NOT create P3.\n");
			return;
		}
		else if(pid3==0){						//procesul P3
			info(BEGIN, 3, 0);
			pid4=fork();
			if(pid4==-1){
				perror("Could NOT create P4.\n");
				return;
			}
			else if(pid4==0){					//procesul P4
				info(BEGIN,4,0);

				info(END,4,0);
			}
			else{								//procesul P3
				pid6=fork();
				if(pid6==-1){
					perror("Could NOT create P6.\n");
				}
				else if(pid6==0){				//procesul P6
					info(BEGIN, 6,0);

					info(END, 6,0);
				}
				else{							//procesul P3
					pid7=fork();
					if(pid7==-1){
						perror("Could NOT create P7.\n");
					}
					if(pid7==0){				//procesul P7
						info(BEGIN,7,0);
						sem_init(&sem_ex2_4_b,0,5);
						pthread_t tid7[50];
						TH_params P7_threads[50];
						for(int i=0;i<50;i++){
							P7_threads[i].id=i+1;
							P7_threads[i].id_proces=7;
							if(i+1==11){
								if(pthread_create(&tid7[i],NULL,fctThreadP7_TH11,&P7_threads[i])!=0){
									perror("Could NOT create thread.\n");
									return;
								}
							}
							else if(pthread_create(&tid7[i],NULL,fctThreadP7,&P7_threads[i])!=0){
								perror("Could NOT create thread.\n");
								return ;
							}
						}
						for(int i=0;i<50;i++){
							if(pthread_join(tid7[i],NULL)!=0){
								perror("Could NOT join thread.\n");
								return;
							}
						}
						sem_destroy(&sem_ex2_4_b);
						pthread_cond_destroy(&condTh11);
						pthread_mutex_destroy(&lockP7Th11);
						info(END, 7,0);	
					}
					else {						//procesul P3
						/////////////////////////////////////////////////////////////////
						semEx2_5_1 = sem_open("sem1",O_CREAT,0600,0);
						semEx2_5_3 = sem_open("sem2",O_CREAT,0600,0);
						pthread_t tid3[5];
						TH_params P3_threads[5];
						for(int i=0;i<5;i++){
							P3_threads[i].id=i+1;
							P3_threads[i].id_proces=3;
							if(i+1==2){
								if(pthread_create(&tid3[i],NULL,fctThread3_2,&P3_threads[i])!=0){
									perror("Could NOT create thread.\n");
									return;
								}
							}
							else if(i+1==4){
								if(pthread_create(&tid3[i],NULL,fctThread3_4,&P3_threads[i])!=0){
									perror("Could NOT create thread.\n");
									return;
								}
							}
							else if(pthread_create(&tid3[i],NULL,fctThreadBasic,&P3_threads[i])!=0){
								perror("Could NOT create thread.\n");
								return;
							}
						}
						for(int i=0;i<5;i++){
							if(pthread_join(tid3[i],NULL)!=0){
								perror("Could NOT join thread.\n");
								return;
							}
						}
						
						sem_close(semEx2_5_1);
						sem_close(semEx2_5_3);
						/////////////////////////////////////////////////////////////////
						waitpid(pid4,NULL,0);
						waitpid(pid6,NULL,0);
						waitpid(pid7,NULL,0);
						info(END, 3, 0);
					}
				}
			}
		}
		else{									//procesul P1
			pid5=fork();
			if(pid5==-1){
				perror("Could NOT create P5.\n");
				return;
			}
			else if(pid5==0){					//procesul P5
				info(BEGIN,5, 0);
				
				info(END,5,0);
			}
			else{								//procesul P1
				waitpid(pid5,NULL,0);
				waitpid(pid3,NULL,0);
				waitpid(pid2,NULL,0);
				info(END, 1, 0);
			}
		}
	}
}


int main(){
	init();

	info(BEGIN, 1, 0);

	createProcesses();
	


	return 0;
}
