
#include <sally/sally.h>


static class SALLYClass : public TclClass {
public:
	SALLYClass() : TclClass("Agent/SALLY") {}
	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);
		return new Sally((nsaddr_t)Address::instance().str2addr(argv[4]));
	}
} class_rtProtoSALLY;

Sally::Sally(nsaddr_t id) :	Agent(PT_OLSR), OLSR(id), AODV(id) {
}

void
Sally::recv(Packet* p, Handler* h) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);

	if(ch->ptype() == PT_AODV) {
	   ih->ttl_ -= 1;
	   recvAODV(p);
	   return;
	 }

	OLSR::recv(p, h);
}

int
Sally::command(int argc, const char*const* argv) {
	int olsr_result = OLSR::command(argc, argv);
	int aodv_result = AODV::command(argc, argv);
	if (olsr_result == TCL_ERROR && aodv_result == TCL_ERROR) {
		return TCL_ERROR;
	}
	return TCL_OK;
}


void
Sally::post_rtable_computation(Packet* p) {
	rt_resolve(p);
}

static void
olsr_mac_failed_callback(Packet *p, void *arg) {
  ((OLSR*)arg)->mac_failed(p);
}

void
Sally::forward_data(Packet* p) {
	struct hdr_cmn* ch	= HDR_CMN(p);
	struct hdr_ip* ih	= HDR_IP(p);

	if (ch->direction() == hdr_cmn::UP &&
		((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == ra_addr())) {
		OLSR::dmux_->recv(p, 0);
		return;
	}
	else {
		ch->direction()	= hdr_cmn::DOWN;
		ch->addr_type()	= NS_AF_INET;
		if ((u_int32_t)ih->daddr() == IP_BROADCAST)
			ch->next_hop()	= IP_BROADCAST;
		else {
			OLSR_rt_entry* entry = rtable_.lookup(ih->daddr());
			if (entry == NULL) {

				aodv_rt_entry *aodv_entry;
				aodv_entry = rtable.rt_lookup(ih->daddr());
			    if(aodv_entry == 0) {
			    	aodv_entry = rtable.rt_add(ih->daddr());
			    }
				if (aodv_entry == NULL) {
					drop(p, DROP_RTR_NO_ROUTE);
					return;
				}
				AODV::sendRequest(aodv_entry->rt_dst);
			}
			else {
				entry = rtable_.find_send_entry(entry);
				assert(entry != NULL);
				ch->next_hop() = entry->next_addr();
				if (use_mac()) {
					ch->xmit_failure_	= olsr_mac_failed_callback;
					ch->xmit_failure_data_	= (void*)this;
				}
			}
		}

		Scheduler::instance().schedule(target_, p, 0.0);
	}
}
