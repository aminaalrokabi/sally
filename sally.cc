
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
	OLSR::recv(p, h);
}

int
Sally::command(int argc, const char*const* argv) {
	return OLSR::command(argc, argv);
}

