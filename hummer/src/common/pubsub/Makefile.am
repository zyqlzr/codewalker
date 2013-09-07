AM_CPPFLAGS=-I${top_srcdir}/src/common \
		-I${top_srcdir}/include \
		-I${top_srcdir}/src/common/log \
		-I${top_srcdir}/src/common/commdef \
		-I${top_srcdir}/include/zookeeper \
		-I${top_srcdir}/include/log4cxx	

LDADD= ${top_srcdir}/lib/libzookeeper_mt.a \
	$(top_builddir)/src/common/log/liblog.a \
	$(top_builddir)/src/common/util/libcommdef.a
 
AM_LDFLAGS=-lpthread 

source_list= pbdef.hpp pbdef.cpp pbconf.hpp pbconf.cpp \
		rule.hpp rule.cpp pubsub.hpp pubsub.cpp \
		zutil.hpp zutil.cpp zoowatcher.hpp zoowatcher.cpp \
		topic.hpp topic.cpp topicwatcher.hpp topicwatcher.cpp \
		listener.hpp listenerfactory.hpp listenerfactory.cpp handler.hpp \
		nsrule.hpp nsrule.cpp

noinst_LIBRARIES=libpubsub.a
libpubsub_a_SOURCES=${source_list}
