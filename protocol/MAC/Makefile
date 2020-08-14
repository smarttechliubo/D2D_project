
CFLAGS := 
#CFLAGS += -DMAC_TEST

MAC_SRCS += $(wildcard src/*.c ../../common/src/log.c ../../platform/ocp_itti/task_define.c)
MAC_OBJS += $(MAC_SRCS:.c=.o)
MAC_DEPS += $(MAC_SRCS:.c=.d)

INCFLAGS += -I. 
INCFLAGS += -I../../common/inc/ 
INCFLAGS += -I../../platform/osp_lib/ 
INCFLAGS += -I../../platform/ocp_itti/ 
INCFLAGS += -I../../interface/ 
INCFLAGS += -Iinc/

-include $(MAC_DEPS)

MAC_LIB=libD2D_protocol_shared.so

mac:$(MAC_LIB)
	cp ../../platform/osp_lib/libosp.so /lib
	aarch64-linux-gnu-gcc $^ -o $@  -g $(MAC_LIB) -losp -lrt -ldl -lpthread
	cp $(MAC_LIB) /lib

$(MAC_OBJS):%.o:%.c
	@echo "Compiling obj"
	aarch64-linux-gnu-gcc $(CFLAGS) $(INCFLAGS) -c  -fPIC -g -Wall $< -o $@ -MD -MF $*.d -MP

$(MAC_LIB):$(MAC_OBJS)
	@echo "Compiling start"
	aarch64-linux-gnu-gcc -shared -fPIC -o $(MAC_LIB) $^ -o $@  -g
	rm -f  $(MAC_OBJS) $(MAC_DEPS)

clean:
	@echo "Make clean"
	rm -f  *.d *.o mac
	rm -f  $(MAC_OBJS) $(MAC_DEPS) mac


.PHONY: clean