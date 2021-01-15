#makefile


ZMQLIB              = /home/libdev/zmq/lib
HOME                = ..
SRC_CODE            = $(HOME)/source
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/jwumq_factory
JWUMQ               = $(SRC_CODE)/jwumq
LOG                 = $(SRC_CODE)/jlog
MSG                 = $(SRC_CODE)/jwumq_protocol

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -Wl,-lz -lm -lpthread
LIBS = -lzmq -L$(ZMQLIB) -lglog -lprotobuf

SRCS = $(wildcard $(MAIN)/main.cpp  \
					$(MAIN)/factory.cpp  \
					$(MAIN)/pool_monitor.cpp  \
					$(MAIN)/factory_status.cpp  \
					$(MAIN)/access_unit.cpp  \
					$(JWUMQ)/jwumq_message.cpp \
					$(JWUMQ)/jwumq_message.pb.cpp \
					$(JWUMQ)/jwumq_base.cpp  \
					$(JWUMQ)/jwumq_duplex_pub.cpp \
					$(JWUMQ)/jwumq_router_server.cpp \
					$(MSG)/jwumq_msg.pb.cpp  \
					$(LOG)/jLog.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/jwumq_factory

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(LOG)/*.o $(MSG)/*.o $(JWUMQ)/*.o

