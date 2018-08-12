#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

char *ptr1,*ptr2;
char *ptr3;
struct proc_fields {
        int pid, pgid;
        char* name;
        int active;
};

typedef struct proc_fields proc_fields;

int num_jobs,shell, shell_pgid;
pid_t fgpid,my_pid, my_pgid;
proc_fields table[10000];






void print(char *username , char* hostname , char *cwd)
{
  printf("<%s@%s:%s> ",username,hostname,cwd);

}

void modify_cwd(char* cwd, char *home_dir) 
{
  int i, j;
  for(i = 0; cwd[i]==home_dir[i] && cwd[i]!='\0' && home_dir[i] != '\0'; i++);
  if(home_dir[i] == '\0') 
  {
    cwd[0] = '~';
    for(j = 1; cwd[i]!='\0'; j++) 
    {
      cwd[j] = cwd[i++];
    }
    cwd[j] = '\0';
  }
}

void other_cmds(char **tokens)
{
  pid_t  pid,wpid;
  int  status;
  //int i;
  pid = fork();
  if (pid < 0)  //error forking
    perror("Error creating child process");	

  else if (pid == 0) //child process
  {    

    if (execvp(*tokens, tokens) < 0) // executing the command  
      perror("Error command cant be executed");	
  }

  else 
  {    // for the parent:    
    do {
      
        wpid = waitpid(pid, &status, WUNTRACED);

    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  }
}





void parsing_other_cmds(char **other_cmd_str,char *token,char delim[])
{

  while(token!=NULL)
  {
    *other_cmd_str++ = token;
    token = strtok_r(NULL,delim,&ptr2);
  }
  *other_cmd_str = '\0';


}




int complete_parse_cmds(char* cmdline, char** cmds) 
{
        int num_cmds = 0;
        char* token = strtok(cmdline, ";");
        while(token!=NULL) 
        {
                cmds[num_cmds++] = token;
                token = strtok(NULL, ";");
        }
        return num_cmds;
}





int complete_parse_each_cmds(char* cmd, char** cmd_tokens) 
{
        int tok = 0;
        char* token = strtok(cmd," \t\n");
        while(token!=NULL) 
        {
                cmd_tokens[tok++] = token;
                //printf("%s\n",cmd_tokens[tok-1]);
                token = strtok(NULL," \t\n");
        }
        return tok;
}




void add_process(int pid, char* name) {
        table[num_jobs].pid = pid;
        table[num_jobs].name = strdup(name);
        table[num_jobs].active = 1;
        num_jobs++;
}

void rem_process(int pid) {
        int i;
        for(i = 0 ; i < num_jobs; i++) {
                if(table[i].pid == pid) {
                        table[i].active = 0;
                        break;
                }
        }
}


int bg_process(char** cmd_tokens,int is_bg)
{
 pid_t pid;
        pid = fork();
        if(pid < 0) {
                perror("Child Process not created\n");
                return -1; 
        }
        else if(pid==0) 
        { 
                setpgid(pid, pid);                               /* Assign pgid of process equal to its pid */
                
                if(is_bg == 0) 
                {
                tcsetpgrp(shell, getpid());        /* Assign terminal to this process if it is not background */
                }

                signal (SIGINT, SIG_DFL);                         /* Restore default signals in child process */
                signal (SIGQUIT, SIG_DFL);
                signal (SIGTSTP, SIG_DFL);
                signal (SIGTTIN, SIG_DFL);
                signal (SIGTTOU, SIG_DFL);
                signal (SIGCHLD, SIG_DFL);
                
               
                if(execvp(cmd_tokens[0], cmd_tokens) < 0) {
                        perror("Error executing command!\n");
                        _exit(-1);
                }
                _exit(0);
        }
        if(is_bg == 0) {
                tcsetpgrp(shell, pid);                              /* Make sure the parent also gives control to child */
                add_process(pid, cmd_tokens[0]);
                int status;
                fgpid = pid;
                waitpid(pid, &status, WUNTRACED);               /* Wait for this process, return even if it has stopped without trace */
                
                if(!WIFSTOPPED(status)) 
                rem_process(pid);         /* returns true if the child process was stopped by delivery of a signal */
         
                else 
                  {
                    fprintf(stderr, "\n%s with pid %d has stopped!\n", cmd_tokens[0], pid);
                }

                tcsetpgrp(shell, my_pgid);            /* Give control of terminal back to the executable */
        }
        else {

                printf("\[%d] %d\n", num_jobs, pid);
                add_process(pid, cmd_tokens[0]);
                return 0;
        }

}




int main()
{
  char hostname[10000];
  char *username;
  char home[10000];
  char cwd[10000];
  int i,j,k;

    
  int flag;
  char *token;//arg
  char *token1;
  char *cmd;
  int err_no;
  char *echo_content;
  char dummy[10000];
  char *str = (char*)malloc(10000*sizeof(char));
   char *str2 = (char*)malloc(10000*sizeof(char));
    char *str1 = (char*)malloc(10000*sizeof(char));

  char **other_cmd_str = (char**)malloc(10000*sizeof(char*));
  int brk_flag;
  char delim1[]=";";
  char delim2[]=" \t\n";
  char delim3[]="\0";
  brk_flag=0;
  username=getenv("USER");
  gethostname(hostname,10000);

  getcwd(home,10000);

  strcpy(cwd,home);
  modify_cwd(cwd,home); 

int is_bg;

shell = STDERR_FILENO;                         /* FD for stderr */

        num_jobs = 0;


        if(isatty(shell)) {                                                         /* test whether a stderr refers to a terminal */
                while(tcgetpgrp(shell) != (shell_pgid = getpgrp()))                  /* if it does, send signal to make process \
                                                                                 group or executable same as process group of stderr */
                        kill(shell_pgid, SIGTTIN);                             /* SIGTTIN sets terminal input for background processes */
        }

        signal (SIGINT, SIG_IGN);                                    /* To ignore Ctrl c */
        signal (SIGTTIN, SIG_IGN);                                   /* To ignore background processes */
        signal (SIGTTOU, SIG_IGN);

        my_pid = my_pgid = getpid();                                 /* Set pgid of executable same as pid */
        setpgid(my_pid, my_pgid);
        tcsetpgrp(shell, my_pgid);                                   /* Give control of stderr to executable's process group */
        



  while(1)
  {
    /*if(brk_flag==1)
    {
      break;
      continue;
    }*/

char *cmd_line = (char*)malloc(10000*sizeof(char));
int tokens;
char** cmd_tokens = malloc((sizeof(char)*1000)*1000);
    print(username,hostname,cwd);    

    fgets(cmd_line,10000,stdin);

    long long int l = strlen(cmd_line);
    cmd_line[l-1] = '\0';

    cmd = strtok_r(cmd_line,delim1,&ptr1);
 int is_bg=0;

 char** cmds = malloc((sizeof(char)*1000)*1000); // array of semi-colon separated commands

                //for(j = 0; j < 1000; j++) 
                //  cmds[j] = '\0';
 int num_cmds = complete_parse_cmds(cmd_line,cmds);
   for(i = 0; i < num_cmds; i++) 
   {

      
            char* cmd_copy = strdup(cmds[i]);

                       // char** cmd_tokens = malloc((sizeof(char)*1000)*1000); // array of command tokens
                       //for(j=0;j<1000;j++) 
                         // {
                           // cmd_tokens[j] = '\0';
                          //}
                                         tokens = complete_parse_each_cmds(strdup(cmds[i]),cmd_tokens);
                                        //cmd_tokens contain each command and tokens represent total number of commands
                                
                                      //  normal_cmd(tokens, cmd_tokens, cmd_copy);
                                
                  }
    // printf("%d\n",tokens );    

 //   printf("%s\n",cmd_tokens[tokens-1] );    

    while( cmd !=NULL )
    {
      /*if(brk_flag==1)
      { 
        break;
        continue;
      }*/

      token =strtok_r(cmd,delim2,&ptr2);

// token1 = token;

      if(token==NULL)


        break;

      if(tokens>0)
      {
token =strtok_r(cmd,delim2,&ptr2);
 printf("%s\n",token);
             if(strcmp(token,"&\0")==0)
            {
            token = strtok_r(NULL,delim2,&ptr2);
             //printf("%s\n",token1);
             if(token!=NULL)
            {

                      cmd_tokens[tokens-1]=NULL;
                        is_bg = 1;
                        int temp_var = bg_process(cmd_tokens,is_bg);        // for running background process
                continue;

            }
 
          else
        {

            perror("Error running bg process");
            }
            }

      }



      else if(strcmp(token,"exit\0")==0)
      {

        brk_flag=1;
        break;
        continue;
      }


      else if( strcmp(token,"cd\0") == 0)
      {
        token = strtok_r(NULL,delim2,&ptr2);
        if(token== NULL || strcmp(token, "~\0") == 0 || strcmp(token, "~/\0") == 0) 
        {                
          chdir(home);
          strcpy(cwd,home);
          modify_cwd(cwd,home);
        }
        else if(chdir(token) == 0) 
        {

          getcwd(cwd,10000);
          modify_cwd(cwd,home);

        }
        else {
          perror("Error executing cd commmand");
        }

      }

      else if( strcmp(token,"pwd\0") ==0)
      {
        getcwd(dummy,10000);

        printf("%s\n",dummy);
      }


      else if( strcmp(token,"echo\0") ==0)
      {

        token = strtok_r(NULL,delim3,&ptr2);
        strcpy(str,token);
        flag=0,i=0;

        while(flag!=1)
        {
          if(str[i]!=' ')
            flag = 1;
          i++;
          if(flag==1)
            break;

        }
        echo_content=&str[i-1];
        printf("%s\n",echo_content);	
      }
      else if(strcmp(token,"jobs\0")==0)
      {
          int var;
         for(var = 0; var < num_jobs ; var++) {
                if(table[var].active == 1) {
                        printf("[%d] %s [%d]\n", var, table[var].name, table[var].pid);
                }
        }

      }
      else if(strcmp(token,"killall\0")==0) 
      {
         int var1;
        for(var1 = 0 ; var1 < num_jobs ; var1++) {
                if(table[var1].active == 1) {
                        if(kill(table[var1].pid, SIGKILL) < 0)                           /* Kill all jobs */
                                perror("Error killing process!\n");
                }
        }


      }
       
      else if(strcmp(token,"kjob\0")==0)
      {
        //  token = strtok_r(NULL,delim2,&ptr2);
          if(tokens>0)
          {

            if(tokens != 3) 
        {
                fprintf(stderr, "Invalid usage of kjob!\n");
                continue;
        }
        int job_num = atoi(cmd_tokens[1]);
        if(table[job_num].active == 1) 
        {
                if(kill(table[job_num].pid, atoi(cmd_tokens[2])) < 0)                 /* For sending signal mentioned to job mentioned */
                        fprintf(stderr, "Signal not sent!\n");
        }
        else fprintf(stderr, "Job not found\n");    
          }            



      } 

   /*else if(tokens>0 && cmd_tokens[tokens]=="&\0") 
    {

printf("%s",cmd_tokens[tokens-1]);
             
                      cmd_tokens[tokens-1]=NULL;
                        is_bg = 1;
                        int temp_var = bg_process(cmd_tokens,is_bg);        // for running background process
                continue;
                }
*/
      else 
      {


        parsing_other_cmds(other_cmd_str,token,delim2);
        /*if(strcmp(other_cmd_str[0], "exit\0")==0)
        { 
          brk_flag=1;
          break;

        }*/
        //else{
           

        other_cmds(other_cmd_str);
        //}
      }

      cmd = strtok_r(NULL,delim1,&ptr1);
    }

   // if(brk_flag==1)
     // break;
if(cmds) free(cmds);
                if(cmd_line) free(cmd_line);
  }
  return 0;
}
