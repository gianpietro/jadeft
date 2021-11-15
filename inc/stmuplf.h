#ifndef _STMUPLF_H
#define _STMUPLF_H

#include "../inc/stmuplibf.h"

//void upLoadStatement();
struct statement * upLoadStatement();
char ** addAlias(); //struct statement *, int);
int resultRows();
void statementInsert(struct statement *);
char * fStmtName();

#endif
             
