#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void swap(int *x,int *y)
{
  int temp;

  temp  = *x;
  *x = *y;
  *y = temp;
}


void selectionsort(int arr[],int n)
{
  int i, j, index;

  for (i = 0; i < n-1; i++)
  {

    index = i;
    for (j = i+1; j < n; j++)
    {

      if (arr[j] < arr[index])
      {
        index = j;
      }
    }

    swap(&arr[index],&arr[i]);
  }
}


void merge(int a[], int h1, int l1, int h2)
{

  int count=h2-h1+1;
  int sorted[count];
  int i=h1, k=l1+1, m=0;
  int j;
  while (i<=l1 && k<=h2)
  {
    if (a[i]<a[k])
    {
      sorted[m]=a[i];
      m++;
      i++;

    }
    else if (a[k]<a[i])
    {
      sorted[m]=a[k];

      m++;
      k++;
    }


    else if (a[i]==a[k])
    {
      sorted[m]=a[i];
      m++;
      i++;
      sorted[m]=a[k];
      m++;
      k++;
    }
  }

  while (i<=l1)
  {

    sorted[m]=a[i];

    m++;
    i++;
  }

  while (k<=h2){


    sorted[m]=a[k];
    m++;
    k++;
  }

  j=0; 
  while(j<count)
  {

    a[h1] = sorted[j];
    j++;
    h1++;
  }
}

void mergesort(int a[], int l, int r)
{
  int i;
  int len=r-l+1;
  int status;


  if (len<=5)
  {
    selectionsort(a+l,len);
    return;
  }

  pid_t lpid,rpid;
  lpid = fork();
  if (lpid<0)
  {
    perror("Left Child Proc. not created\n");
    exit(-1);
  }
  else if (lpid==0)
  {
    mergesort(a,l,l+len/2-1);
    exit(0);
  }
  else
  {
    rpid = fork();
    if (rpid<0)
    {

      perror("Right Child Proc. not created\n");
      exit(-1);
    }
    else if(rpid==0)
    {
      mergesort(a,l+len/2,r);
      exit(0);
    }
  }



  // Wait for child processes to finish
  waitpid(lpid,&status,0);
  waitpid(rpid,&status,0);

  // Merge the sorted subarrays
  merge(a,l,l+len/2-1,r);
}


int issorted(int arr[], int len)
{
  if (len==1)
  {
    return 1;
  }

  int i;
  for (i=1; i<len; i++)
  {
    if (arr[i]<arr[i-1])
    {
      return 0;
    }
  }
  return 1;
}



int main()
{
  int shmid;
  key_t key = IPC_PRIVATE;
  int *arr;//shm_arr
  int len;
  int i,j,k;
  scanf("%d",&len); 

  // Calculate segment length
  size_t arr_size = sizeof(int)*len;

  // Create the segment.
  if ((shmid = shmget(key,arr_size, IPC_CREAT | 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  // Now we attach the segment to our data space.
  if ((arr = shmat(shmid, NULL, 0)) == (int *) -1)
  {
    perror("shmat");
    exit(1);
  }


  for (i=0;i<len;i++)
  {

    scanf("%d",&arr[i]);

  }
  // sort the created array
  mergesort(arr, 0, len-1);

  // check if array is sorted or not
  k = issorted(arr, len);

  if(k)
  {
    printf("sorting done\n");

    for(j=0;j<len;j++)
      printf("%d ",arr[j]);


  }

  else if(k==0)
  {
    printf("sorting not done\n");

  }

  printf("\n");

  // detach from the shared memory now that we are done using it. 
  if (shmdt(arr) == -1)
  {
    perror("shmdt");
    exit(1);
  }

  // delete the shared memory segment.
  if (shmctl(shmid, IPC_RMID, NULL) == -1)
  {
    perror("shmctl");
    exit(1);
  }

  return 0;
}
