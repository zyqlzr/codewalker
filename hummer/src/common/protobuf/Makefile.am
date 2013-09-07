AM_CPPFLAGS=-I${top_srcdir}/src/common/protobuf

LDADD= $(top_srcdir)/lib/libprotobuf.a \
	$(top_srcdir)/lib/boost/libboost_system.a

AM_LDFLAGS=-lpthread -lz -lexpat

source_list= job.pb.h job.pb.cc \
				server.pb.h server.pb.cc \
				testcounter.pb.h testcounter.pb.cc \
				sketch.pb.h sketch.pb.cc

noinst_LIBRARIES=libpbmsg.a

libpbmsg_a_SOURCES=${source_list}
