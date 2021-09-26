#ifndef _CATLIB_H
#define _CATLIB_H

void catInsert(char *);
void catUpdate(int, char *);
void catDelete(int );
void stmtLinkInsert(char *, int, char *);
void stmtLinkUpdate(int, char *, int, char *);
void stmtLinkDelete(int);

#endif
