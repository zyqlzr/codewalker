AM_CPPFLAGS= -I/usr/local/include/thrift \
			-I${top_srcdir}/src/common/log \
			-I${top_srcdir}/src/common/commdef \
			-I${top_srcdir}/include \
			-I${top_srcdir}/include/log4cxx

LDADD= 
 
AM_LDFLAGS= 

source_list= hbase_constants.h hbase_constants.cpp Hbase.h Hbase.cpp \
			Hbase_server.skeleton.cpp hbase_types.h hbase_types.cpp  \
			hbasewrapper.hpp hbasewrapper.cpp operator.hpp \
			regionscanner.hpp regionscanner.cpp	

noinst_LIBRARIES=libhbaseclient.a

libhbaseclient_a_SOURCES=${source_list}

