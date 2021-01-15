#makefile


ZMQLIB              = /home/libdev/zmq/lib
HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/jwumq_pool
QUEUE               = $(MAIN)/queue
JWUMQ               = $(SRC_CODE)/jwumq
LOG                 = $(SRC_CODE)/jlog
MSG                 = $(SRC_CODE)/jwumq_protocol

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -Wl,-lz -lm -lpthread
LIBS = -lzmq -L$(ZMQLIB) -lglog -lprotobuf

SRCS = $(wildcard $(MAIN)/main.cpp  \
				  $(MAIN)/pool.cpp  \
				  $(MAIN)/pool_branch.cpp  \
				  $(MAIN)/pool_status.cpp  \
				  $(MAIN)/access_unit.cpp  \
				  $(MAIN)/queue_center.cpp  \
				  $(QUEUE)/queue_base.cpp  \
				  $(QUEUE)/queue_pipe.cpp  \
				  $(QUEUE)/queue_member.cpp  \
				  $(QUEUE)/queue_producer.cpp  \
				  $(JWUMQ)/jwumq_message.cpp \
				  $(JWUMQ)/jwumq_message.pb.cpp \
				  $(JWUMQ)/jwumq_base.cpp  \
				  $(JWUMQ)/jwumq_duplex_sub.cpp  \
				  $(JWUMQ)/jwumq_router_server.cpp \
				  $(MSG)/jwumq_msg.pb.cpp  \
					$(LOG)/jLog.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/jwumq_pool

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(QUEUE)/*.o $(LOG)/*.o $(MSG)/*.o $(JWUMQ)/*.o

