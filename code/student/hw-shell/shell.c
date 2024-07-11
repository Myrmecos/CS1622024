#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <dirent.h>

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

#define FOREGROUND_RUN 0
#define BACKGROUND_RUN 1

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens* tokens);
int cmd_help(struct tokens* tokens);
int cmd_pwd(struct tokens* tokens);
int cmd_cd(struct tokens* tokens);
int cmd_exec(struct tokens* tokens);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens* tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t* fun;
  char* cmd;
  char* doc;
} fun_desc_t;

/*-----------list of commands and their functions----------------*/

fun_desc_t cmd_table[] = {
    {cmd_help, "?", "show this help menu"},
    {cmd_exit, "exit", "exit the command shell"},
    {cmd_pwd, "pwd", "present working directory"},
    {cmd_cd, "cd", "change directory"},
    {cmd_exec, "/", "execute a program"}
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens* tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens* tokens) { exit(0); }

int cmd_pwd(unused struct tokens* tokens) {
  int pwd_buf_size = 64;
  char pwd_buffer[pwd_buf_size];
  getcwd(pwd_buffer, pwd_buf_size);
  printf("%s\n", pwd_buffer);
  return 1;
}

int cmd_cd(struct tokens* tokens) {
  char* dir_to_change = tokens_get_token(tokens, 1);
  if (dir_to_change == NULL) {
    return -1;
  }
  //printf("%s\n", dir_to_change);
  chdir(dir_to_change);
  return 1;
}

int search_for_first_occurrence(char* target_path) {
  char* env_var = getenv("PATH");
  char* path_left = malloc(sizeof(char) * 128);
  //printf("the environment variable: %s\n", env_var);
  char* path;
  for(path = strtok_r(env_var, ":", &path_left); path != NULL; path = strtok_r(NULL, ":", &path_left) ) {
    //printf("The parsed element is: %s\n", token);
    //determine if the executable is in the dir
    //if yes, complete target_path and return 1
    struct dirent* entry;
    DIR* dir = opendir(path);
    if (dir == NULL) {return -1;}
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, target_path) == 0) {
        //printf("%s\n", token);
        char* temp = malloc(sizeof(char) * 128);
        strcpy(temp, path);
        strcat(temp, "/");
        strcat(temp, target_path);
        strcpy(target_path, temp);
        free(temp);
        closedir(dir);
        //printf("%s\n", target_path);
        return 1;
      }
   }
   closedir(dir);
  }
  return -1;
}

int cmd_exec(struct tokens* tokens) {

  int run_background = FOREGROUND_RUN;

  //prepare argument array
  int tokens_len = tokens_get_length(tokens);
  char** args = malloc(sizeof(char*) * (tokens_len + 1));

  if (strcmp(tokens_get_token(tokens, tokens_len - 1), "&") == 0) {
    //printf("run in background\n");
    run_background = BACKGROUND_RUN;
    tokens_len --;
  }

  //printf("There are %d tokens\n", tokens_len);
  int num_of_pipes = 0;

  for (int i = 0; i < tokens_len; i++) {
    args[i] = tokens_get_token(tokens, i);
    if (strcmp(args[i], "|") == 0) {
      num_of_pipes += 1;
    }
  }

  //obtain an array of pipe positions
  int* start_point_arr = malloc(sizeof(int) * (num_of_pipes + 2));
  start_point_arr[0] = -1;
  start_point_arr[num_of_pipes + 1] = tokens_len;
  int cnt = 1;
  for (int i = 0; i < tokens_len; i++) {
    if(strcmp(args[i], "|") == 0) {
      start_point_arr[cnt] = i;
      cnt ++;
    }
  }
  args[tokens_len] = NULL;

  //prepare pipes
  int* fds = malloc(sizeof(int) * num_of_pipes * 2);
  int* pipe_arr = malloc(sizeof(int) * num_of_pipes);
  for (int i = 0; i < num_of_pipes; i++) {
    pipe_arr[i] = pipe(&(fds[i*2]));
  }

  int base = 0;
  pid_t* pids = malloc(sizeof(pid_t) * (num_of_pipes + 1));
  for (int i = 0; i < num_of_pipes + 1; i++) {
    pids[i] = fork();
    if (pids[i] == 0) {
      //child
      //get child pid;
      pids[i] = getpid();
      //get base pos
      base = start_point_arr[i] + 1;

      //prepare stdin and stdout
      if (i != 0) {
        dup2(fds[i*2-2], STDIN_FILENO);
      }
      if (i != num_of_pipes) { // i != number of process - 1
        dup2(fds[i*2+1], STDOUT_FILENO);
      }
      for (int j = 0; j < num_of_pipes*2; j++) {
        close(fds[j]);
      }

      //special cases: redirection
      //if (1 == 0) {
      if (tokens_len - base >= 3) { //avoid segfault
        if (strcmp(args[base + 1], "<") == 0) {
          freopen(args[base + 2], "r", stdin);
        } else if (strcmp(args[base + 1], ">") == 0) {
          freopen(args[base + 2], "w", stdout);
        }
      }
      if (tokens_len - base >= 5) { //avoid segfault
        if (strcmp(args[base + 3], "<") == 0) {
          freopen(args[base + 4], "r", stdin);
        } else if (strcmp(args[base + 3], ">") == 0) {
          freopen(args[base + 4], "w", stdout);
        }
      }

      //prepare args
      int argnum = (start_point_arr[i + 1] - start_point_arr[i] - 1 + 1);
      char** newarg = malloc(sizeof(char*) * argnum);
      for (int k = 0; k < argnum - 1; k++) {
          newarg[k] = args[base + k];
      }
      newarg[argnum - 1] = NULL;

      //prepare full path
      char* path_name = malloc(sizeof(char)*128);
      strcpy(path_name, args[0 + base]);
      if (path_name[0] != '/') {
        search_for_first_occurrence(path_name);
      }

      //execute
      //printf("The executable file: %s\n", path_name); //debugging
      execv(path_name, newarg);

      //free argument array
      free(args);
      free(newarg);
      fclose(stdin);
      fclose(stdout);

      //exit
      exit(0);

    } else if (pids[i] > 0) {
      //parent wait for the execution to end
      //wait(NULL);
    } else {
      exit(-1);
    }
  }

  for (int j = 0; j < num_of_pipes*2; j++) { // close all read and write ends for main process
    close(fds[j]);
  }

  if (run_background == FOREGROUND_RUN) {
    while (wait(NULL) != -1); // wait till all processes to finish
  }

  return 1;
}


/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

void signal_callback_handler(int signum) {
  printf("received SIGINT\n");
}

int main(unused int argc, unused char* argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  //set current process to foreground
  pid_t current_pgid = getpgrp();
    if (tcsetpgrp(STDIN_FILENO, current_pgid) == -1) {
      perror("tcsetpgrp");
      return 1;
  }//no reasons why put it here. It stalls the program if placed in child process below.


  /*wait for signal*/
  struct sigaction sa;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sigaction(SIGINT, &sa, NULL);

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens* tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {
      /* REPLACE this to run commands as programs. */
      //fprintf(stdout, "This shell doesn't know how to run programs.\n");
      pid_t cpid = fork();
      if (cpid == 0) {
        //printf("child's pid: %d; group pid: %d\n", getpid(), getpgrp());
        //printf("child: original gpid: %d\n", getpgrp());
        setpgrp();//testing
        //printf("child: changed gpid: %d\n", getpgrp());
        
        //execute command
        cmd_exec(tokens);
        
        exit(0); //don't forget to exit!
      } else if (cpid > 0) {
        /*printf("parent's pid: %d; group pid: %d\n", getpid(), getpgrp());
        printf("parent: child's original gpid: %d\n", getpgid(cpid));*/
        
        //set signal handler to not ignore
        sa.sa_handler = signal_callback_handler;
        sigaction(SIGINT, &sa, NULL);

        setpgid(cpid, cpid);//testing
        //printf("parent: child's new gpid: %d\n", getpgid(cpid)); 
        wait(NULL);

        //restore signal handler to ignore
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT, &sa, NULL);

      } else if (cpid < 0) {
        perror("fork");
        exit(1);
      }
        
    }
    

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
