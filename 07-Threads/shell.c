#include "shell.h"

#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include "threads.h"
#include "string.h"
#include "usart.h"
#include "asm.h"

/** Copy from rtenv+ **/
#define MAX_CMDNAME 19

/* Enumeration for command types. */
enum {
  CMD_FIBONACCI = 0,
  CMD_FIBONACCITHREAD,
  //CMD_LS,
  //CMD_CAT,
  //CMD_CD,
  //CMD_PWD,
  //CMD_VIEW,
  CMD_GREETING,
  CMD_EXIT,
  CMD_HELP,
  CMD_COUNT
} CMD_TYPE;

/* Structure for command handler. */
typedef struct {
  char cmd[MAX_CMDNAME + 1];
  void (*func)(int, char**);
} hcmd_entry;

const hcmd_entry cmd_data[CMD_COUNT] = {
  [CMD_FIBONACCI] = {.cmd = "fibonacci", .func = fibonacci_shell},
  [CMD_FIBONACCITHREAD] = {.cmd = "fibonacciThread", .func = fibonacciThread_shell},
  //[CMD_LS] = {.cmd = "fibonacciThread", .func = export_envvar},
  //[CMD_CAT] = {.cmd = "fibonacciThread", .func = export_envvar},
  //[CMD_CD] = {.cmd = "fibonacciThread", .func = export_envvar},
  //[CMD_PWD] = {.cmd = "fibonacciThread", .func = export_envvar},
  //[CMD_VIEW] = {.cmd = "fibonacciThread", .func = export_envvar},
  [CMD_GREETING] = {.cmd = "greeeting", .func = greeting_shell}, 
  [CMD_EXIT] = {.cmd = "exit", .func = exit_shell},
  [CMD_HELP] = {.cmd = "help", .func = help_shell},
};

void simple_shell()
{
  char buffer[100];
  char *argv[5];
  int argc, i;

  while(1){
    /* Get user input */
    *(argv[0]) = '\0';
    print_str("$ ");
    get_str(buffer, 99);

    /* Get arguments. */
    argv[0] = cmdtok(buffer);
    argc = 1;
    while(*buffer && argc < 5){
      argv[argc] = cmdtok(NULL);
      argc++;
    }

    /* Decode and execute command. */
    if(*(argv[0]) == '\0')
      continue;

    for(i = 0; i < CMD_COUNT; i++)
      if (strcmp(argv[0], cmd_data[i].cmd) == 0){
        cmd_data[i].func(argc, argv);
        break;
      }
    
    if(i == CMD_COUNT){
      print_str(argv[0]);
      print_str(":Command not found\n");
    }
  }

}

/* Privative function: Shell commands */
/* Homework7 */
void fibonacci_shell(int argc, char* argv[]){
  int result = fibonacci(str2int(argv[1]));
  print_str("fibonacci(");
  print_str(argv[1]);
  print_str(") = ");
  print_int(result);
  print_c('\n');
}

void fibonacciThread(void * userdata){
  int result = fibonacci(str2int(userdata));
  print_str("fibonacci(");
  print_str(userdata);
  print_str(") = ");
  print_int(result);
  print_c('\n');
}
void fibonacciThread_shell(int argc, char* argv[]){
  thread_create(fibonacciThread, (void *) argv[1]);
}

/* Homework8 /
void ls_shell(int argc, char* argv[]){

}

void cat_shell(int argc, char* argv[]){

}

void pwd_shell(int argc, char* argv[]){

}

void cd_shell(int argc, char* argv[]){

}

void view_shell(int argc, char* argv[]){

}*/

/* Other commands */
void greeting_shell(int argc, char* argv[]){
  print_str("Hello, i'm simple shell!\n");
}

void exit_shell(int argc, char* argv[]){
  print_str("Press \"Ctrl + a\" and then \"x\" to terminate qemu.\n");
}

void help_shell(int argc, char* argv[]){
  print_str("\tfibonacciThread %d\n");
  print_str("\tfibonacci %d\n");
  print_str("\thelp\n");
  print_str("\tgreeting\n");
  print_str("\texit\n");
}

