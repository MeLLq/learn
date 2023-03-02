#include "peer.hh"
struct Config {
  std::vector<PeerId> payload;
};

class Gossip {
public:
  Gossip(sstring local_address);
  Peer AddPeer(PeerId id, sstring ip_addr);
  void SetLocalPayload(Payload payload);
  void DelPeer(PeerId id);

private:
  Peer _local_peer;
  std::map<PeerId, Peer> _peers;
};
