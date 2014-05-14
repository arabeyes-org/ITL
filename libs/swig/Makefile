#--
# $Id: Makefile 10738 2006-09-21 16:39:50Z thamer $
#--

CFLAGS+=$(shell perl -e 'use Config; print $$Config{ccflags};')

all: perl-demo

perl-demo: libitl.i
	swig -perl5 libitl.i
	$(CC) -c libitl_wrap.c $(CFLAGS) -I/usr/lib/perl/5.8.4/CORE/
	$(LD) -shared -G ../build/libitl.so libitl_wrap.o -o libitl_perl.so
clean:
	rm -f libitl_perl.pm libitl_wrap.c libitl_perl.so *.o
