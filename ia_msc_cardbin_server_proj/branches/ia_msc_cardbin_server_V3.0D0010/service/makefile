###################################################################
# Vertion: 1.0
# Author:  edited by Hawkliu 2007-07-25 
# History: 
##################################################################
PATH_INC = ../include
PATH_SRC = ../service
PATH_BIN = ../bin
PATH_OBJ = ../obj
PATH_COM = ../comm
PATH_OPENSSL = /usr/local/ssl/include

# Setting for trpc lib
TRPC_HOME    = /usr/local/middle
LIB_TRPC     = $(TRPC_HOME)/lib/libtrpc.a
LIB_TRPC_SYS = $(TRPC_HOME)/lib/libtrpc_sys.a
LIB_TRPC_SRV = $(TRPC_HOME)/lib/libtrpc_server.a
LIB_TRPC_CLI = $(TRPC_HOME)/lib/libtrpc_client.a

# Setting for business lib
PATH_CFT         = /usr/local/cftlib
PATH_XML         = $(PATH_CFT)/tinyxml
LIB_XML          = $(PATH_CFT)/tinyxml/libtinyxml.a
PATH_XYZ         = $(PATH_CFT)/libxyz
LIB_XYZ	         = $(PATH_CFT)/libxyz/libxyz.a

PATH_QPAY_ENCRYPT= $(PATH_CFT)/qpayencryptclient
LIB_QPAY_ENCRYPT = $(PATH_QPAY_ENCRYPT)/libqpayencryptclient.a

PATH_QPAY_TOOL   = $(PATH_CFT)/qpaytools
LIB_QPAY_TOOL    = $(PATH_QPAY_TOOL)/libqpaytools.a

PATH_BILLNO      = $(PATH_CFT)/billnoapi
LIB_BILLNO       = $(PATH_BILLNO)/libbillnoapi.a

PATH_MSG_NOTIFY  = $(PATH_CFT)/msgnotify_client_v2.0
LIB_MSG_NOTIFY   = $(PATH_MSG_NOTIFY)/libmsgnotify_client.a

# Mysql library
MYSQL_HOME       = /usr/local/zftmysql
MYSQL_LIB        =-L$(MYSQL_HOME)/lib/mysql -lmysqlclient

# common static lib
LIB_SSL          = /usr/lib/libssl.a
LIB_CRYPTO       = /usr/lib/libcrypto.a
LIB_COMM         = $(PATH_COM)/libcommon.a

# Setting for compile
CC       := g++
CFLAGS   += -I$(PATH_XYZ) -I$(PATH_QPAY_TOOL) -I$(PATH_COM) -I$(PATH_INC) -I$(PATH_QPAY_ENCRYPT) \
            -I$(PATH_BILLNO) -I$(TRPC_HOME)/include -I$(PATH_XML) -I$(MYSQL_HOME)/include/mysql \
            -I$(PATH_OPENSSL) -I$(PATH_MSG_NOTIFY)
CFLAGS   +=  $(TENPAY_TEST_FLAG)            
            
#CFLAGS   += -g -O -DNDEBUG -Wall -fpic 
CFLAGS   += -g -DNDEBUG -Wall -fpic 
LIBS     += $(LIB_COMM) $(LIB_TRPC) $(LIB_TRPC_SYS) $(LIB_TRPC_SRV) $(LIB_TRPC_CLI) \
            $(LIB_QPAY_ENCRYPT)  $(LIB_BILLNO) $(LIB_SSL) $(LIB_CRYPTO) $(LIB_XML) \
            $(LIB_QPAY_TOOL) $(MYSQL_LIB) $(LIB_XYZ) $(LIB_MSG_NOTIFY)
LIBS     += $(TENPAY_TEST_LIBS)

# Target to create
SO   := $(PATH_BIN)/ia_msc_cardbin_service.so
OBJS := $(patsubst $(PATH_SRC)/%.cpp,$(PATH_OBJ)/%.o,$(wildcard $(PATH_SRC)/*.cpp))

$(SO) : $(LIB_COMM) $(OBJS)
	$(CC) -O -shared -lc -o $@ $(CFLAGS) $^ $(LIBS)

$(PATH_OBJ)/%.o :  $(PATH_SRC)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

# Create libcommon.a 
COMS := $(patsubst $(PATH_COM)/%.cpp,$(PATH_COM)/%.o,$(wildcard $(PATH_COM)/*.cpp))
$(LIB_COMM) : $(COMS)
	@ar rv $@ $^

$(PATH_COM)/%.o :  $(PATH_COM)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

# Tools
.PHONY : clean install
clean:
	rm -f $(COMS) $(LIB_COMM) $(PATH_COM)/*.d $(OBJS) $(SO) $(PATH_SRC)/*.d

install: 
	$(TRPC_HOME)/tools/install_so $(SO) ia_msc_cardbin_server 0 28001 

dev: 
	cp ../conf/ia_msc_cardbin_server ia_msc_cardbin_server.xml /usr/local/middle/ia_msc_cardbin_server/conf/
	$(TRPC_HOME)/tools/install_so $(SO) ia_msc_cardbin_server 0 28001 
	
