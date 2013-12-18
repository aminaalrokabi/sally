
#ifndef ns_ping_h
#define ns_ping_h

#include "agent.h"
#include "olsr/OLSR.h"
#include "aodv/aodv.h"

class Sally : public OLSR, public AODV {

 public:
	Sally(nsaddr_t);
	virtual int command(int argc, const char*const* argv);
	virtual void recv(Packet*, Handler*);
	virtual void post_rtable_computation(Packet *p);
	virtual void forward_data(Packet *p);
};

#endif
