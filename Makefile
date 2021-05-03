vpath %.h inc
vpath %.c src
vpath %.so lib
vpath %.c cur

OBJDIR = lib
BINDIR = bin
SRCDIR = src
CURDIR = cur

CFLAGS = -g

OBJ =  fdbcon.so prolib.so procur.so

program : $(OBJ)
	gcc $(CFLAGS) $(SRCDIR)/jadeft.c $(OBJDIR)/fdbcon.so $(OBJDIR)/prolib.so \
	$(OBJDIR)/procur.so \
	-o $(BINDIR)/jadeft -I/usr/include -lecpg -lform -lcurses

fdbcon.so : fdbcon.c fdbcon.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

prolib.so : prolib.c prolib.h
	gcc -c $(CFLAGS) $< -o $(OBJDIR)/$@

procur.so : procur.c procur.h
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

