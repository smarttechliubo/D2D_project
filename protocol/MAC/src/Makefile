MAC_SRCS += $(wildcard *.c ../test/src/*.c ../../../common/src/log.c)
MAC_OBJS += $(MAC_SRCS:.c=.o)
MAC_DEPS += $(MAC_SRCS:.c=.d)

INCFLAGS = -I. -I../../../common/inc/ -I../../../interface -I../inc/ -I../test/inc/

-include $(MAC_DEPS)

%.o:%.c
	@echo "Compiling obj"
	gcc $(INCFLAGS) -c -g -Wall $< -o $@ -MD -MF $*.d -MP

mac:$(MAC_OBJS)
	@echo "Compiling start"
	gcc $^ -o $@  -g  -lrt -ldl -lpthread

clean:
	rm -f  *.d *.o mac