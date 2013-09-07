AM_CPPFLAGS=-I${top_srcdir}/src/common \
		-I${top_srcdir}/include \
		-I${top_srcdir}/include/log4cxx	 \
		-I${top_srcdir}/include/libxml \
		-I${top_srcdir}/src/common/log \
		-I${top_srcdir}/src/common/pubsub \
		-I${top_srcdir}/src/common/commdef

LDADD= $(top_srcdir)/src/common/pubsub/libpubsub.a \
	$(top_srcdir)/lib/liblog4cxx.a \
	$(top_srcdir)/lib/libapr-1.a \
	$(top_srcdir)/lib/libaprutil-1.a \
	$(top_srcdir)/lib/libzookeeper_mt.a \
	$(top_srcdir)/src/common/log/liblog.a \
	$(top_srcdir)/src/common/commdef/libcommdef.a \
	$(top_srcdir)/lib/libxml2.a 

AM_LDFLAGS=-lpthread -lz -lexpat

source_list= confchecker.hpp confchecker.cpp \
		filenotifier.hpp filenotifier.cpp \
		pbconfig.hpp pbconfig.cpp \
		xmlloader.hpp xmlloader.cpp

exec_source_list = main.cpp

noinst_LIBRARIES=libconfigserver.a
bin_PROGRAMS = configureserver

libconfigserver_a_SOURCES=${source_list}
configureserver_SOURCES = ${source_list} ${exec_source_list}
