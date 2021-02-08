#makefile

ZMQLIB              = /home/libdev/zmq/lib
ZMQINCLUDE          = /home/libdev/zmq/include
HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/amtp_proxy
JWUMQ               = $(SRC_CODE)/jwumq
LOG                 = $(SRC_CODE)/jlog
CJSON               = $(INCLUDE)/cjson
CONFIG              = $(SRC_CODE)/config

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -Wl,-lz -lm -lpthread
LIBS = -lzmq -L$(ZMQLIB) -lglog -lprotobuf

SRCS = $(wildcard $(MAIN)/main.cpp  \
					$(MAIN)/proxy.cpp  \
					$(JWUMQ)/jwumq_base.cpp  \
					$(JWUMQ)/jwumq_proxy.cpp  \
					$(JWUMQ)/jwumq_message.cpp \
					$(JWUMQ)/jwumq_message.pb.cpp \
					$(LOG)/jLog.cpp\
					$(CJSON)/cJSON.cpp\
					$(CONFIG)/config.cpp\
					$(CONFIG)/proxy_config.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/amtp_proxy

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(JWUMQ)/*.o $(LOG)/*.o $(CJSON)/*.o $(CONFIG)/*.o
