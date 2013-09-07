AM_CPPFLAGS=-I${top_srcdir}/src/common \
		-I${top_srcdir}/include \
		-I${top_srcdir}/include/log4cxx	 \
		-I${top_srcdir}/include/libxml \
		-I${top_srcdir}/src/common/log \
		-I${top_srcdir}/src/common/commdef \
		-I${top_srcdir}/src/common/hbaseclient \
		-I${top_srcdir}/src/common/pubsub \
		-I${top_srcdir}/src/common/protobuf \
		-I${top_srcdir}/src/processor \
		-I${top_srcdir}/include/boost \
		-I/usr/local/include/thrift


LDADD= $(top_srcdir)/src/common/pubsub/libpubsub.a \
	$(top_srcdir)/src/common/commdef/libcommdef.a  \
	${top_srcdir}/src/common/hbaseclient/libhbaseclient.a \
	${top_srcdir}/src/common/protobuf/libpbmsg.a \
	$(top_srcdir)/src/common/log/liblog.a \
	$(top_srcdir)/lib/libLSD.a \
	$(top_srcdir)/lib/libxml2.a \
	$(top_srcdir)/lib/liblog4cxx.a \
	$(top_srcdir)/lib/libapr-1.a \
	$(top_srcdir)/lib/libaprutil-1.a \
	$(top_srcdir)/lib/libzookeeper_mt.a \
	$(top_srcdir)/lib/libprotobuf.a \
	$(top_srcdir)/lib/boost/libboost_system.a


# -Wl,-rpath=/usr/local/lib
AM_LDFLAGS=-lexpat -lz -Wl,-rpath=/usr/local/lib -lthrift
#-Wl,-rpath=/usr/local/lib -lthrift


source_list= cmd.hpp cmdnotify.hpp cmdnotify.cpp \
			zkserver.hpp zkserver.cpp hummerctx.hpp \
			hummerctx.cpp serverbase.hpp serverbase.cpp \
			task.hpp task.cpp taskcontainer.hpp taskcontainer.cpp \
			cmdimp.hpp cmdimp.cpp job.hpp job.cpp \
			server.hpp server.cpp  taskhb.hpp taskhb.cpp \
			topology.hpp topology.cpp zkactor.hpp zkactor.cpp \
			worker.hpp worker.cpp master.hpp master.cpp \
			cluster.hpp cluster.cpp tasker.hpp tasker.cpp \
			scheduler.hpp scheduler.cpp processor.hpp \
			testtask.hpp testtask.cpp  \
			msgprocessor.hpp msgprocessor.cpp datacache.hpp datacache.cpp \
			timedwatcher.hpp timedwatcher.cpp

worker_source_list = workermain.cpp
master_source_list = mastermain.cpp

noinst_LIBRARIES=libcore.a
bin_PROGRAMS = worker master

libcore_a_SOURCES=${source_list}
worker_SOURCES = ${source_list} ${worker_source_list}
master_SOURCES = ${source_list} ${master_source_list}

