CC=gcc
CFLAGS=-c -g -Wall -Werror
AR=ar
ARFLAGS=rv
LIBNAME=libinfra.a

OBJECTS  :=       cfg_loader.o\
                  cfg_parser.o\
                  cfg_scanner.o\
                  circ_buffer.o\
                  hash.o\
                  io_event_driver.o\
                  log.o\
                  mem_tracker.o\
                  timer.o\
                  cfg_util.o\
                  rbtree.o

AUTO_GENERATED	:=	cfg_parser.c\
						cfg_parser.h\
						cfg_scanner.c


all: infra demo

infra: ${OBJECTS}
	@echo "  Building Library ${LIBNAME} ..."
	@$(foreach obj, $^, echo "    adding $(obj)";)
	@${AR} ${ARFLAGS} ${LIBNAME} $^ > /dev/null 2>&1

demo:
	@echo "	Building demo"
	@make -C demo

%.o: %.c
	@echo "  CC $<"
	@${CC} ${CFLAGS} $<

cfg_parser.c: cfg_parser.y
	@echo "  Building cfg_parser.c"
	@bison -d -o $@ $<

cfg_scanner.c: cfg_scanner.l
	@echo "  Building cfg_scanner.c"
	@flex -o $@ $<

clean:
	@echo "Cleaning up..."
	@rm -f ${OBJECTS} ${AUTO_GENERATED} ${LIBNAME}
	make -C demo clean
