#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = lib.la

#######################################
# Build information for each library

# Library Sources
lib_la_SOURCES 	= Peer.cpp 		Peer.h \
		  PeerFactory.cpp	PeerFactory.h \
		  PeerSet.cpp		PeerSet.h \
		  ServerPool.cpp	ServerPool.h \
		  P2pMgr.cpp		P2pMgr.h \
		  Bookkeeper.cpp	Bookkeeper.h \
		  ChunkIdQueue.cpp	ChunkIdQueue.h

# Compiler options. Here we are adding the include directory to be searched for
# headers included in the source code.
AM_CPPFLAGS = -I$(srcdir)/.. \
	      -I$(srcdir)/../misc \
	      -I$(srcdir)/../inet \
	      -I$(srcdir)/../protocol \
	      -I$(srcdir)/../p2p \
	      -I$(srcdir)/../repository \
	      -I$(srcdir)/../node
	      
SUFFIXES: .i .cpp
.cpp.i:
	$(COMPILE) $(AM_CXXFLAGS) -E -o $@ $<
	      
