#ifndef _CMDS_H
#define _CMDS_H

#include <_attrs.h>

int echo(int argc, char **argv);
int layout(int argc, char **argv);
int pscreen(int argc, char **argv);
int pgdt(int argc, char **argv);
int clear(int argc, char **argv);
int date(int argc, char **argv);
__NORETURN int shutdown(int argc, char **argv);
__NORETURN int reboot(int argc, char **argv);
int halt(int argc, char **argv);
int pstack(int argc, char **argv);
int plogo(int argc, char **argv);
int help(int argc, char **argv);

#endif // _CMDS_H