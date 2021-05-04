#ifndef _PROCUR_H
#define _PROCUR_H

//#include <form.h>

char * trimWS(char *);
void providerWindow();
void providerTypeWindow();
//int proListWindow();
PGconn * fdbcon();
int proSelect();

#endif
