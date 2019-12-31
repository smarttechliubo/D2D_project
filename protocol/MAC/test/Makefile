
CFLAGS := 
CFLAGS += -DMAC_TEST

#for mac test

MAC_SRCS_TEST += $(wildcard ../src/*.c src/*.c ../../../common/src/log.c case/$(case).c)
#MAC_SRCS_TEST += $(wildcard ../src/*.c ../../../common/src/log.c ../../../platform/ocp_itti/task_define.c)
MAC_OBJS_TEST += $(MAC_SRCS_TEST:.c=.o)
MAC_DEPS_TEST += $(MAC_SRCS_TEST:.c=.d)

INCFLAGS += -I. 
INCFLAGS += -I../../../common/inc/ 
INCFLAGS += -I../../../platform/osp_lib/
INCFLAGS += -I../../../platform/ocp_itti/
INCFLAGS += -I../../../interface/ 
INCFLAGS += -I./../inc 
INCFLAGS += -I./inc/
#INCFLAGS = -I. -I../../../common/inc/ -I../../../platform/osp_lib/ -I../../../platform/ocp_itti -I../../../interface/ -I./../inc

-include $(MAC_DEPS_TEST)

MAC_LIB_TEST=libD2D_protocol_shared.so

mac_test:$(MAC_LIB_TEST)
	gcc $^ -o $@  -g $(MAC_LIB_TEST) -losp -lrt -ldl -lpthread
	cp ../../../platform/osp_lib/libosp.so /lib
#	cp $(MAC_LIB_TEST) /lib
	rm -rf  $(MAC_OBJS_TEST) $(MAC_DEPS_TEST)

$(MAC_OBJS_TEST):%.o:%.c
	@echo "Compiling MAC_OBJS_TEST"
	gcc $(CFLAGS) $(INCFLAGS) -c  -fPIC -g -Wall $< -o $@ -MD -MF $*.d -MP

$(MAC_LIB_TEST):$(MAC_OBJS_TEST)
	@echo "Compiling start MAC_LIB_TEST"
	gcc -shared -fPIC -o $(MAC_LIB_TEST) $^ -o $@  -g  -lrt
	rm -rf  $(MAC_OBJS_TEST) $(MAC_DEPS_TEST)

clean:
	@echo "Make clean"
	rm -rf  *.d *.o mac_test
	rm -rf  $(MAC_OBJS_TEST) $(MAC_DEPS_TEST) mac_test


.PHONY: clean