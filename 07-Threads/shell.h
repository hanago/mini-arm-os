#ifndef __SHELL_H__
#define __SHELL_H__

void simple_shell();

/* Privative function: Shell commands */
/* Homework7 */
void fibonacci_shell(int argc, char* argv[]);
void fibonacciThread_shell(int argc, char* argv[]);

/* Homework8 */
// path
void ls_shell(int argc, char* argv[]);
void pwd_shell(int argc, char* argv[]);
void cd_shell(int argc, char* argv[]);
// text viewer
void view_shell(int argc, char* argv[]);

/* Other commands */
void greeting_shell(int argc, char* argv[]);
void exit_shell(int argc, char* argv[]);
void help_shell(int argc, char* argv[]);

#endif