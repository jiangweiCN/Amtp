#makefile


HOME                = ..
SRC_CODE            = $(HOME)/source
INCLUDE             = $(SRC_CODE)/include
OUTPUT_PATH         = $(HOME)/bin
OUTPUT_PATH_I368    = $(HOME)/bin_i386
MAIN                = $(SRC_CODE)/libamtpa/client_access
LOG                 = $(SRC_CODE)/jlog
JWUMQ               = $(SRC_CODE)/jwumq_external
LICENSE				= $(SRC_CODE)/license
MESSAGE				= $(MAIN)/message
SYSINFO				= $(MAIN)/sys_info
PROTOCOL			= $(SRC_CODE)/amtpa_protocol

CXX = ccache g++
CXX_OPTIONS = -g -Wall -Wno-unknown-pragmas -DOS_LINUX -O2 -std=c++14 -fPIC -ldl -rdynamic -Wl,-rpath,./ 
CXX_LDOPTIONS = -shared -Wl,-lz -lm -lpthread -ldl 
LIBS = -lprotobuf -lglog 


SRCS = $(wildcard $(MAIN)/libamtpca.cpp \
					$(MAIN)/amtpca.cpp \
					$(MAIN)/msg_event_objects.cpp \
					$(LOG)/jLog.cpp\
					$(LICENSE)/license_msg.pb.cpp\
					$(LICENSE)/libamtpa_license.cpp\
					$(JWUMQ)/jwumq_external_message.cpp \
					$(JWUMQ)/jwumq_external_message.pb.cpp \
					$(JWUMQ)/jwumq_queue.cpp \
					$(PROTOCOL)/amtpap_msg.pb.cpp\
					$(MESSAGE)/amtp_login.cpp\
					$(MESSAGE)/amtp_login_resp.cpp\
					$(MESSAGE)/amtp_logout.cpp\
					$(MESSAGE)/amtp_logout_resp.cpp\
					$(MESSAGE)/amtp_module_conf.cpp\
					$(MESSAGE)/amtp_module_conf_resp.cpp\
					$(MESSAGE)/amtp_config.cpp\
					$(MESSAGE)/amtp_config_resp.cpp\
					$(MESSAGE)/amtp_config_data.cpp\
					$(MESSAGE)/amtp_config_data_resp.cpp\
					$(MESSAGE)/amtp_config_notify.cpp\
					$(MESSAGE)/amtp_config_notify_resp.cpp\
					$(MESSAGE)/amtp_upload_file.cpp\
					$(MESSAGE)/amtp_upload_file_resp.cpp\
					$(MESSAGE)/amtp_upload_eof.cpp\
					$(MESSAGE)/amtp_upload_eof_resp.cpp\
					$(MESSAGE)/amtp_query_data.cpp\
					$(MESSAGE)/amtp_query_data_resp.cpp\
					$(MESSAGE)/amtp_upload_file_data.cpp\
					$(MESSAGE)/amtp_upload_file_data_resp.cpp)

OBJS= $(SRCS:%.cpp=%.o)

EXES= $(OUTPUT_PATH)/libamtpca.so.1.1.1

EXES_I368= $(OUTPUT_PATH_I368)/libamtpca.so.1.1.1


all:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

all_i386:$(OBJS)
	$(CXX) $(CXX_OPTIONS) -o $(EXES_I368) $(OBJS) $(LIBS) $(CXX_LDOPTIONS)

$(OBJS):%.o:%.cpp
	$(CXX) $(CXX_OPTIONS) -o $@ -c $<

clean:
	rm -f ./$(EXES) $(MAIN)/*.o $(JWUMQ)/*.o $(LOG)/*.o $(LICENSE)/*.o $(PROTOCOL)/*.o $(MESSAGE)/*.o

clean_i386:
	rm -f ./$(EXES_I368) $(MAIN)/*.o $(JWUMQ)/*.o $(LOG)/*.o $(LICENSE)/*.o $(PROTOCOL)/*.o $(MESSAGE)/*.o



