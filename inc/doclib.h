#ifndef _DOCLIB_H
#define _DOCLIB_H

#include "jaddef.h"

void docTypeInsert(char *);
void docTypeUpdate(int, char *);
void docTypeDelete(int);
void docImportInsert(int, char *, int, int, char *, char *, char *, int, int, char *);
void docImportUpdate(int, int, char[], int, int, char[], char[], char[], int, int, char[]);

#endif
