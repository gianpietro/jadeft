#ifndef _SUPLIB_H
#define _SUPLIB_H

void supplierInsert(int, char *);

void supTypeInsert(char *);

void payPeriodInsert(char *);

void prtInsert(int, char *, char *, char *);

void supAccountInsert(int, char *, int, int, int, int, int, int, double, char *, char *, int);

void supplierUpdate(int, int, char *);

void supplierDelete(int);

void supTypeUpdate(int, char *);

void supTypeDelete(int);

void payPerUpdate(int, char *);

void payPerDelete(int);

void prtUpdate(int, int, char *, char *, char *);

void prtDelete(int);

void supAccountUpdate(int, int, char *, int, int, int, int, int, int, double, char *, char *, int );

void supAccountDelete(int );

#endif
