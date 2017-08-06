__DIR__	:= $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
$(info main source path: <$(__DIR__)> )
PROJROOT:= $(abspath $(__DIR__)/../../../)
$(info main source path: <$(PROJROOT)> )

LIBCURL	:= -lcurl
_LIBOPENSSL	:= -lyj_cipher -lssl -lcrypto -lz
LIBCLIENT	:= $(LIBCURL) -lthrift $(_LIBOPENSSL) -lboost_program_options -lpthread -lrt
M_T_SERVER__OBJS	:= $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/MTServer.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/mt_server_types.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/mt_server_constants.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/exception_types.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/exception_constants.o

.PHONY: clean target

target: idl2conf.cpp
	./generateDependencies4Idl2Conf.sh
	g++ idl2conf.cpp -o idl2conf -std=c++11 -I'/tmp/mstch/include/' -L'/tmp/mstch/build/src/' -lboost_program_options -lboost_regex -lmstch -g
	rm -rf /tmp/mstch	

clean:
	rm -rf idl2conf
	find ../supvr.conf.d -mindepth 1 -not -name 'supvr_event_callback.bin.supvr.conf' |xargs rm -f
