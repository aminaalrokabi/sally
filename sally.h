
#ifndef ns_ping_h
#define ns_ping_h

#include "agent.h"
#include "olsr/OLSR.h"
#include "tclcl.h"
#include "packet.h"
#include "address.h"
#include "ip.h"

class Sally : public OLSR {
 public:
	Sally(nsaddr_t);
	int command(int argc, const char*const* argv);
	void recv(Packet*, Handler*);
};

#endif
