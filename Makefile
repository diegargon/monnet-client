CC = gcc
CFLAGS  = -Wall -g  -std=gnu99
LFLAGS = 
LIBS = 
INCLUDES = 
BUILD_DIR = build
SRCS_DIR = src
SRCS = $(wildcard $(SRCS_DIR)/*.c)
HEADERS_DIR = src
HEADERS = $(wildcard $(HEADERS_DIR)/*.h)
OBJS = $(SRCS:.c=.o)
MAIN_PROG = monnet-client

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif


default: all

all: $(MAIN_PROG)
	@echo Doing $(MAIN_PROG)
	
$(MAIN_PROG): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(MAIN_PROG) $(OBJS) $(LFLAGS) $(LIBS)
	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@ $(LFLAGS) $(LIBS)
clean: 
	rm -f $(BUILD_DIR)/* $(SRCS_DIR)/*.o $(SRCS_DIR)/*~
install: all	

uninstall: