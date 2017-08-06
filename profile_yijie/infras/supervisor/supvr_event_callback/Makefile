__DIR__	:= $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
$(info main source path: <$(__DIR__)> )
PROJROOT:= $(abspath $(__DIR__)/../../../)
$(info main source path: <$(PROJROOT)> )

LIBCURL	:= -lcurl
_LIBOPENSSL	:= -lyj_cipher -lssl -lcrypto -lz
LIBCLIENT	:= $(LIBCURL) -lthrift $(_LIBOPENSSL) -lboost_program_options -lpthread -lrt
M_T_SERVER__OBJS	:= $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/MTServer.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/mt_server_types.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/mt_server_constants.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/exception_types.o $(PROJROOT)/rpc-thrift/src_gen/gen-cpp/exception_constants.o

.PHONY: clean target

target: supvr_event_callback.cpp $(PROJROOT)/share/lib/IpUtil.o $(M_T_SERVER__OBJS) 
	g++ -std=c++11 supvr_event_callback.cpp -o supvr_event_callback.bin $(PROJROOT)/share/lib/IpUtil.o $(M_T_SERVER__OBJS) $(LIBCLIENT) -lconfig_manager -g

clean:
	rm -rf supvr_event_callback.bin
