#ifndef _PROLIB_H
#define _PROLIB_H

void providerInsert(int, char *);
void providerUpdate(int, int, char *);
void providerDelete(int);
void proTypeInsert(char *);
void proTypeUpdate(int, char *);
void proTypeDelete(int);
void proAccountInsert(int, int, char *, int, char *, int);
void proAccountUpdate(int, int, int, char *, int, char *, int);
void proAccountDelete(int);

#endif
