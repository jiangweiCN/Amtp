#makefile

ZMQLIB              = /home/libdev/zmq/lib
ZMQINCLUDE          = /home/libdev/zmq/include
HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/jwumq_manager
LOG                 = $(SRC_CODE)/jlog
JWUMQ               = $(SRC_CODE)/jwumq
CONFIG              = $(SRC_CODE)/config
CJSON               = $(INCLUDE)/cjson

CXX = g++
CXX_OPTIONS = -Wall -O2 -std=c++14 -ldl -rdynamic -Wl,-rpath,./ 
CXX_LDOPTIONS = -Wl,-lm -lpthread
LIBS = -L. -lglog -lzmq -L$(ZMQLIB) -lprotobuf


SRCS = $(wildcard $(MAIN)/main.cpp \
				$(MAIN)/monitor.cpp \
				$(MAIN)/external_manage.cpp \
				$(MAIN)/internal_manage.cpp \
				$(CONFIG)/config.cpp\
				$(CONFIG)/monitor_config.cpp\
				$(JWUMQ)/jwumq_message.cpp \
				$(JWUMQ)/jwumq_message.pb.cpp \
				$(LOG)/jLog.cpp \
				$(CJSON)/cJSON.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/jwumq_monitor

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(CXX_LDOPTIONS) $(LIBS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(CONFIG)/*.o $(LOG)/*.o $(CJSON)/*.o $(JWUMQ)/*.o

