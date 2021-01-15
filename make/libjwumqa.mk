#makefile


ZMQLIB              = /home/libdev/zmq/lib
HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
MAIN                = $(SRC_CODE)/libjwumqa
JWUMQ               = $(SRC_CODE)/jwumq
MSG                 = $(SRC_CODE)/jwumq_protocol

CXX = g++
CXX_OPTIONS = -Wall -DOS_LINUX -O2 -std=c++14 -fPIC -rdynamic -Wl,-rpath,./
CXX_LDOPTIONS = -shared -Wl,-lz -lm -lpthread
LIBS = -lzmq -L$(ZMQLIB) -lglog -lprotobuf


SRCS = $(wildcard $(MAIN)/libjwumqa_factory.cpp \
				$(MAIN)/libjwumqa_connection.cpp \
				$(MAIN)/libjwumqa_session.cpp \
				$(MAIN)/libjwumqa_destination.cpp \
				$(MAIN)/libjwumqa_part_pipe.cpp \
				$(MAIN)/libjwumqa_part_member.cpp \
				$(MAIN)/libjwumqa_part_producer.cpp \
				$(MAIN)/libjwumqa_part_consumer.cpp \
				$(MAIN)/JTextMessageImpl.cpp \
				$(MAIN)/JBytesMessageImpl.cpp \
				$(MAIN)/factory_connection.cpp \
				$(MAIN)/pool_connection.cpp \
				$(MAIN)/libjwumqa_c.cpp \
				$(MSG)/jwumq_msg.pb.cpp  \
				$(JWUMQ)/jwumq_base.cpp \
				$(JWUMQ)/jwumq_message.cpp \
				$(JWUMQ)/jwumq_message.pb.cpp \
				$(JWUMQ)/jwumq_dealer.cpp )

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libjwumqa.so.2.1.0

all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(CXX_LDOPTIONS) $(LIBS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(JWUMQ)/*.o $(MSG)/*.o




