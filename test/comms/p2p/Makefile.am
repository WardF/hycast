AM_CPPFLAGS	= -I$(top_srcdir)/main \
                  -I$(top_srcdir)/main/misc \
                  -I$(top_srcdir)/main/net \
                  -I$(top_srcdir)/main/prod \
                  -I$(top_srcdir)/main/comms \
                  -I$(top_srcdir)/main/comms/mcast \
                  -I$(top_srcdir)/main/comms/p2p
LDADD		= $(top_builddir)/main/libhycast.la 
AM_LDFLAGS	= -lgtest -lsctp
CLEANFILES	= *.vg *.cg

check_PROGRAMS	= Channel_test \
                  ChunkChannel_test \
                  Peer_test \
                  PeerSet_test \
                  P2pMgr_test \
                  PeerAddrSet_test \
                  ChunkId2PeerAddrsMap_test
#                  Backlogger_test
#                  CancelDestroy_test

Channel_test_SOURCES		= Channel_test.cpp
ChunkChannel_test_SOURCES	= ChunkChannel_test.cpp
Peer_test_SOURCES		= Peer_test.cpp
PeerSet_test_SOURCES		= PeerSet_test.cpp
P2pMgr_test_SOURCES		= P2pMgr_test.cpp
PeerAddrSet_test_SOURCES	= PeerAddrSet_test.cpp
ChunkId2PeerAddrsMap_test_SOURCES	= ChunkId2PeerAddrsMap_test.cpp
#CancelDestroy_test_SOURCES     = CancelDestroy_test.cpp
#Backlogger_test_SOURCES	= Backlogger_test.cpp

TESTS	= $(check_PROGRAMS)

%.vg:	%
	$(TESTS_ENVIRONMENT) $(LIBTOOL) --mode=execute valgrind \
                --leak-check=full --show-reachable=yes ./$< >$@ 2>&1
	
%.cg:	%
	$(LIBTOOL) --mode=execute valgrind --tool=callgrind \
                --callgrind-out-file=$@ ./$<
	kcachegrind $@ &
