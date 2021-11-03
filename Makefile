vpath %.h inc
vpath %.c src
vpath %.so lib
vpath %.c cur

OBJDIR = lib
BINDIR = bin
SRCDIR = src
CURDIR = cur
INCDIR = inc

CFLAGS = -g

OBJ =  fdbcon.so prolib.so procur.so supcur.so invcur.so jadlib.so suplib.so invlib.so \
	doccur.so doclib.so catcur.so catlib.so stmupl.so stmuplib.so stmlib.so

program : $(OBJ)
	gcc $(CFLAGS) $(SRCDIR)/jadeft.c $(OBJDIR)/fdbcon.so $(OBJDIR)/prolib.so \
	$(OBJDIR)/procur.so $(OBJDIR)/supcur.so $(OBJDIR)/invcur.so $(OBJDIR)/jadlib.so $(OBJDIR)/suplib.so \
	$(OBJDIR)/invlib.so $(OBJDIR)/doccur.so $(OBJDIR)/doclib.so $(OBJDIR)/catcur.so $(OBJDIR)/catlib.so \
	$(OBJDIR)/stmupl.so $(OBJDIR)/stmuplib.so $(OBJDIR)/stmlib.so   \
	-o $(BINDIR)/jadeft -I/usr/include -lpq -lecpg -lform -lpanel -lcurses -I/include -lpq

fdbcon.so : fdbcon.c fdbcon.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

prolib.so : prolib.c prolib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

procur.so : procur.c procur.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

supcur.so : supcur.c supcur.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

jadlib.so : jadlib.c jadlib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

suplib.so : suplib.c suplib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

invcur.so : invcur.c invcur.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

invlib.so : invlib.c invlib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

doccur.so : doccur.c doccur.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

doclib.so : doclib.c doclib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

catcur.so : catcur.c catcur.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

catlib.so : catlib.c catlib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

stmupl.so : stmupl.c stmuplf.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

stmuplib.so : stmuplib.c stmuplibf.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

stmlib.so : stmlib.c stmlib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

clean:
	rm -f $(OBJDIR)/*.so $(BINDIR)/*


# vpath will search for the files with the stated suffix in the directories applied
# OBJ can be used to list all the object files that will be required to compile the program
# variables used for various directories lib for .so files, bin for binary files, src for .c files
# -I/usr/include -lecpg poiting to the postgres library file
# example :  gcc -c $(\CFLAGS) $\< -o $(\OBJDIR)/$\@    backslash just an escape char for comment
# the above is know as a recipe and is a rule to compile the .c to a .so
# $\< refers to the first directive which in the case of fdbcon.so is fdncon.c or .h files ignored
# $(\OBJDIR) is the target directory in this case lib where .so files will sit
# $\@ refers to the target filename
# clean is used to remove .so and binary files with command 'make clean'
# when starting a new line make sure to add the back slash


