PLAT_CC = gcc
PLAT_CPP = g++
PLAT_CFLAGS = -I./http-parser-master -I./

SRCNAME = connector-main.c connector-utils.c ./http-parser-master/http_parser.c
OBJECTS = $(build)/$(SRCNAME:.c=.o)

adam: $(SRCNAME)
	mkdir -p ./build
	$(PLAT_CC) -o $@ $^ $(PLAT_CFLAGS) 
