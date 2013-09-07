AM_CPPFLAGS=-I${top_srcdir}/src/common \
		-I${top_srcdir}/src/common/pubsub \
		-I${top_srcdir}/include \
		-I${top_srcdir}/src/common/log \
		-I${top_srcdir}/src/common/commdef \
		-I${top_srcdir}/src/common/protobuf \
		-I${top_srcdir}/include/zookeeper \
		-I${top_srcdir}/include/libxml \
		-I${top_srcdir}/include/log4cxx \
		-I${top_srcdir}/src/configserver \
		-I${top_srcdir}/src/core \
		-I/usr/local/include/thrift \
		-I${top_srcdir}/src/common/hbaseclient \
		-I${top_srcdir}/include/fastdfs \
		-I${top_srcdir}/include/lsd

LDADD= ${top_srcdir}/src/core/libcore.a \
	${top_srcdir}/src/common/hbaseclient/libhbaseclient.a \
	$(top_srcdir)/src/common/pubsub/libpubsub.a \
	$(top_srcdir)/src/configserver/libconfigserver.a \
	$(top_srcdir)/src/common/commdef/libcommdef.a  \
	$(top_srcdir)/src/common/hbaseclient/libhbaseclient.a \
	$(top_srcdir)/src/common/sketchhbase/libsketch.a \
	$(top_srcdir)/src/common/log/liblog.a \
	${top_srcdir}/src/common/protobuf/libpbmsg.a \
	$(top_srcdir)/lib/libxml2.a \
	$(top_srcdir)/lib/liblog4cxx.a \
	$(top_srcdir)/lib/libapr-1.a \
	$(top_srcdir)/lib/libaprutil-1.a \
	$(top_srcdir)/lib/libzookeeper_mt.a \
	$(top_srcdir)/lib/libprotobuf.a \
	$(top_srcdir)/lib/boost/libboost_system.a \
	$(top_srcdir)/lib/libfdfsclient.a \
	$(top_srcdir)/lib/libfastcommon.a \
	$(top_srcdir)/lib/libhiredis.a \
	$(top_srcdir)/lib/opencv/libopencv_core.so.2.4 \
	$(top_srcdir)/lib/opencv/libopencv_imgproc.so.2.4 \
	$(top_srcdir)/lib/opencv/libopencv_highgui.so.2.4 \
	$(top_srcdir)/lib/libLSD.a

AM_LDFLAGS=-lexpat -lz -pthread -lthrift -Wl,-Bsymbolic \
			-Wl,-rpath=/usr/local/lib \
			-Wl,-rpath=/home/zhengyang/running/demo_hummer/lib/opencv \
			-L/home/zhengyang/hummer/lib/opencv

source_list= test.hpp \
		test.cpp \
		hbasetest.cpp \
		hbasewrappertest.cpp \
		sketchtest.cpp \
		timecachedtest.cpp \
		testmain.cpp

bin_PROGRAMS=simpletest
simpletest_SOURCES=${source_list}

