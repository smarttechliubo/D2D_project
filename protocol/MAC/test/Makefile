
CFLAGS := 
#CFLAGS += -DMAC_TEST

MAC_SRCS += $(wildcard ../src/*.c src/*.c ../../../common/src/log.c)
MAC_OBJS += $(MAC_SRCS:.c=.o)
MAC_DEPS += $(MAC_SRCS:.c=.d)

INCFLAGS = -I. -I../../../common/inc/ -I../../../interface/ -I./../inc -I./inc/

-include $(MAC_DEPS)

%.o:%.c
	@echo "Compiling obj"
	gcc $(CFLAGS) $(INCFLAGS) -c -g -Wall $< -o $@ -MD -MF $*.d -MP

mac:$(MAC_OBJS)
	@echo "Compiling start"
	gcc $^ -o $@  -g  -lrt -ldl -lpthread
	rm -f  $(MAC_OBJS) $(MAC_DEPS)

clean:
	@echo "Make clean"
	rm -f  *.d *.o mac
	rm -f  $(MAC_OBJS) $(MAC_DEPS) mac