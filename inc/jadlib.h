#ifndef _JADLIB_H
#define _JADLIB_H

#include <form.h>
#include <libpq-fe.h>

PGconn * fdbcon();
char * trimWS(char *);
void keyNavigate(int ch, FORM * f);

#endif
