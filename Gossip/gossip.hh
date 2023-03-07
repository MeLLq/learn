#include "peer.hh"

class Gossip {
public:
  Gossip(sstring local_address);
  seastar::lw_shared_ptr<Peer> AddPeer(PeerId id, sstring ip_addr);
  void SetLocalPayload(Payload payload);
  void DelPeer(PeerId id);
  std::map<PeerId, seastar::lw_shared_ptr<Peer>> GetPeers() const;
  void SendConfig(Config config);

private:
  sstring _local_addres;
  seastar::lw_shared_ptr<Peer> _local_peer;
  std::map<PeerId, seastar::lw_shared_ptr<Peer>> _peers;
  timer<lowres_clock> _timer;
};
