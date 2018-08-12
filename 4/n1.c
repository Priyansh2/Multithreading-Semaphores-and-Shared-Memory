#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include<signal.h>
sem_t writer[1000];
sem_t reader[1000];
pthread_t rid[1000],wid;
int n,k,buffer[1000],l=-1,r=0,readcount[1000],j,bs,rp[1000],b[1000];
pthread_mutex_t count_mutex;

void *writers(void *arg)
{
	int stat=0;
	for(j=0;j<k;j++)
	{
		while(buffer[j%bs]!=0 && rp[j%bs]!=n);
		sem_wait(&writer[j%bs]);
		stat++;
		buffer[j%bs]=rand()%100+1;
		printf("writer wrote %d at position %d\n",buffer[j%bs],j%bs);
		readcount[j%bs]=0;
		rp[j%bs]=0;
		r++;
		sem_post(&writer[j%bs]);
	}
}

void *readers(void *arg)
{
	int id,stat=0;
	id=*(int*)arg;
	id++;
	int i;
	for(i=0;i<k;i++)
	{
		while(r<=i);
		sem_wait(&reader[i%bs]);
		readcount[i%bs]++;	
		if(readcount[i%bs]==1)
		{
			stat++;
//			printf("%d\n",stat);
			sem_wait(&writer[i%bs]);
		}
		sem_post(&reader[i%bs]);
	
		printf("reader %d read %d from position %d\n",id,buffer[i%bs],i%bs);


		sem_wait(&reader[i%bs]);
		rp[i%bs]++;
		if(readcount[i%bs]==n)
		{
			stat++;
//			printf("%d\n",stat);
			sem_post(&writer[i%bs]);
		}
		sem_post(&reader[i%bs]);
	}
}
int main()
{
	int i;

	printf("Enter buffer size\n");
	scanf("%d",&bs);

	for(i=0;i<bs;i++)
	{
		sem_init(&reader[i],0,1);
		sem_init(&writer[i],0,1);
		readcount[i]=0;
		rp[i]=0;
	}

	printf("Enter value of n\n");
	scanf("%d",&n);

	printf("Enter how many times writer has to write\n");
	scanf("%d",&k);



	for(i=0;i<n;i++)
	{
		b[i]=i;
		pthread_create(&rid[i],NULL,readers,(void*)&b[i]);
	}
	pthread_create(&wid,NULL,writers,NULL);

	for(i=0;i<n;i++)
		pthread_join(rid[i],NULL);
	pthread_join(wid,NULL);
	return 0;
}
