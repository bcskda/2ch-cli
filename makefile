# Это можно трогать:
CONF_CURL_USERAGENT = "\"Linux\""
CONF_DEFAULT_EDITOR = "\"nano\""
CONF_DEFAULT_EMAIL  = "\"\""

CC = g++

LIBS = -lcurl -ljsoncpp -lncursesw -lcaca
OPTS = -std=c++11 -fPIC -Wno-pointer-arith
DEBUG_OPTS= -g -ggdb

CONFIG = -DCURL_UA=$(CONF_CURL_USERAGENT) \
         -DDEFAULT_EDITOR=$(CONF_DEFAULT_EDITOR) \
         -DDEFAULT_EMAIL=$(CONF_DEFAULT_EMAIL)

SRCDIR = ./src
OBJDIR = ./build
TARGETS = makaba.c parser.cpp external.cpp error.cpp 2ch-cli.cpp 
OBJECTS = makaba.o parser.o external.o error.o 2ch-cli.o 

all: default

default: prepare link strip

makaba.o:
	cd $(SRCDIR) && $(CC) ${CONFIG} ${OPTS} -c ../$(SRCDIR)/makaba.cpp   -o ../$(OBJDIR)/makaba.o
parser.o:
	cd $(SRCDIR) && $(CC) ${CONFIG} ${OPTS} -c ../$(SRCDIR)/parser.cpp   -o ../$(OBJDIR)/parser.o
external.o:
	cd $(SRCDIR) && $(CC) ${CONFIG} ${OPTS} -c ../$(SRCDIR)/external.cpp -o ../$(OBJDIR)/external.o
error.o:
	cd $(SRCDIR) && $(CC) ${CONFIG} ${OPTS} -c ../$(SRCDIR)/error.cpp    -o ../$(OBJDIR)/error.o
2ch-cli.o:
	cd $(SRCDIR) && $(CC) ${CONFIG} ${OPTS} -c ../$(SRCDIR)/2ch-cli.cpp  -o ../$(OBJDIR)/2ch-cli.o

link: ${OBJECTS}
	cd $(OBJDIR) && $(CC) ${OPTS} ${LIBS} ${OBJECTS} -o ../2ch-cli

strip:
	strip --strip-unneeded 2ch-cli

prepare:
	[ -d build ] || mkdir build
clean:
	rm -rf 2ch-cli build-log build
