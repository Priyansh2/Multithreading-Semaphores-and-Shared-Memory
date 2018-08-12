#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

void print(char *username , char* Hostname , char *cwd)
{
  printf("<%s@%s:%s> ",username,Hostname,cwd);

}
char * createstr(char c)
{
  char * newstring;
  newstring = (char*)malloc(2*sizeof(char));
  *newstring=c;
  *(newstring+1) = '\0';


  return newstring;
}
int main()
{
  char Hostname[10000];
  char *username;
  char buff[10000],buff1[10000];
  char cmd[10000];
  char *slash,*cwd;
  char ch,ch1;
  ch='~';
  ch1='/';
  cwd=createstr(ch); 
  char *curr_dir;
  int flag=0,flag1=1,flag2=-1;
  int gotoloc=0,gotoprev=0;
  char **tokens = malloc(10000 * sizeof(char*));
  char *home,*prev_dir;
  char backup[10000];
  char dummy[10000];
  while(1)
  {
    username=getenv("USER");

    gethostname(Hostname, 10000);

    if(flag==0)
    {
      home = getcwd(buff,10000);
      strcpy(backup,home);
      flag=1;
    }
    if(gotoprev==1)
    {


      print(username,Hostname,prev_dir);
    }
    if(gotoloc==0 && gotoprev==0){

      print(username,Hostname,cwd);
    }
    if(gotoloc==1 && flag2==1)
    {
      char cwd1[]="~";
      print(username,Hostname,cwd1);
    }

    fgets(cmd,10000,stdin);

    int l = strlen(cmd);

    if(cmd[l-1] == '\n')
    {
      cmd[l-1] = '\0';
    }
    char *arg1;
    if(arg1 = strtok(cmd," \t\r\n"))
    {
      if(strcmp(arg1,"cd")==0)
      {
        char *arg = strtok(0,"\t\r\n");

        if(!arg)
        {

          if(strcmp(prev_dir,backup)<0)
          {
        //    printf("%s\n",prev_dir);
          
          gotoprev=-1;
          }
        
          chdir(backup);
          gotoloc=1;
          flag1=0;
          flag2=1;
          free(cwd);
          continue;
        }

        else
        {
          //          printf("yoyo\n");

          if(strcmp(arg1+3,"..")==0)
          { 
            gotoprev=1;
            chdir(arg);
            prev_dir = getcwd(buff1,10000);
            if(strcmp(prev_dir,backup)==0)
            {
              gotoprev=-1;
              gotoloc=1;
              flag1=0;
              free(cwd);
              flag2=1;
            }
            else if(strcmp(prev_dir,backup)<0)
            {
              //     gotoprev=1;
              flag2=0;       

            }
      //      else if(strcmp(prev_dir,backup)>0)
        //    {
          //       gotoloc=0;
          //       gotoprev=0;
          //  }

            continue;
          }

         /* else if(strcmp(arg1+3,".")==0)
          {
          }*/
          else
          {

            slash = createstr(ch1);
            strcat(slash,arg);//--> slash = "/arg"
     //       printf("%s\n",slash);
            struct stat check;
            curr_dir = getcwd(buff,10000);
            strcat(curr_dir,slash);


            if(stat(curr_dir,&check)==0 && S_ISDIR(check.st_mode))
            {   
              if(flag1==0)
              {

                cwd = createstr(ch);
                flag1=1;
              }
              strcat(cwd,slash);
       //       printf("%s\n",cwd);
              free(slash);
              chdir(arg);
              gotoloc=0;
              gotoprev=0;
              continue;
            }

            else{

              write(2,"No such file or directory\n",26);
              gotoprev=0;
              gotoloc=0;
              continue;
            }
          }
        }
      }
      if(strcmp(arg1,"exit")==0)
      {
        break;
      }
      else if(strcmp(arg1,"echo")==0)
      {
        char *arg = strtok(0,"\t\r\n");
        l = strlen(arg);
        arg[l] = '\0';
        printf("%s\n", arg);
      }
      else if(strcmp(arg1,"pwd")==0)
      {
        printf("%s\n",home);

      }
      else
      {
        int x=0;

        while (arg1 != NULL) 
        {
          tokens[x] = arg1;
          x++;



          arg1 = strtok(NULL," \t\r\n");
        }
        tokens[x] = NULL;


        pid_t pid, wpid;
        int status;

        pid = fork();
        if (pid == 0) // Child process
        {

          if (execvp(tokens[0], tokens) == -1) 
          {
            perror("Error");
          }

        } 
        else if (pid < 0) // Error forking
        {

          perror("Error");
        }
        else // Parent process
        {

          do {
            wpid = waitpid(pid, &status, WUNTRACED);
          } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

      }
    }
  }
  return 0;
}
