#makefile


ZMQLIB              = /home/libdev/zmq/lib
HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/libamtpa/server_access
JWUMQ               = $(SRC_CODE)/jwumq_external

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -ldl -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -shared -Wl,-lz -lm -lpthread -lprotobuf -lglog -lzmq -L$(ZMQLIB)


SRCS = $(wildcard $(MAIN)/libamtpsa.cpp \
					$(MAIN)/amtpsa.cpp \
					$(JWUMQ)/jwumq_base.cpp  \
					$(JWUMQ)/jwumq_dealer.cpp\
					$(JWUMQ)/jwumq_message.cpp \
					$(JWUMQ)/jwumq_external_message.pb.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libamtpsa.so.1.0.4

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS)  $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(JWUMQ)/*.o 


