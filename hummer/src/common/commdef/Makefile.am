AM_CPPFLAGS=-I${top_srcdir}/src/common \
		-I${top_srcdir}/include \
		-I${top_srcdir}/include/log4cxx	 \
		-I${top_srcdir}/include/libxml \
		-I${top_srcdir}/src/common/log \
		-I${top_srcdir}/include/boost

LDADD= ${top_srcdir}/lib/libzookeeper_mt.a \
	${top_srcdir}/lib/libxml2.a \
	$(top_builddir)/src/common/log/liblog.a \
	$(top_srcdir)/lib/boost/libboost_system.a
 
AM_LDFLAGS=-lexpat -lz 

source_list= nodedes.hpp nodedes.cpp \
	xmlwrapper.hpp xmlwrapper.cpp clock.hpp clock.cpp \
	iopoller.hpp iopoller.cpp event.hpp event.cpp \
	epollimp.cpp pollerbase.hpp mutex.cpp mutex.hpp \
	thread.hpp thread.cpp signaler.hpp \
	signaler.cpp utildef.hpp utildef.cpp \
	batchq.hpp log.hpp log.cpp

noinst_LIBRARIES=libcommdef.a
libcommdef_a_SOURCES=${source_list}
