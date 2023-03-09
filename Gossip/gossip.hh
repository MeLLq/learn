#include "peer.hh"
#include "ss.hh"
class Gossip {
public:
  Gossip(ss::sstring local_address);
  ss::lw_shared_ptr<Peer> AddPeer(PeerId id, ss::sstring ip_addr);
  void SetLocalPayload(Payload payload);
  void DelPeer(PeerId id);
  std::map<PeerId, ss::lw_shared_ptr<Peer>> GetPeers() const;
  void SendConfig(Config config);

private:
  ss::sstring _local_addres;
  ss::lw_shared_ptr<Peer> _local_peer;
  std::map<PeerId, ss::lw_shared_ptr<Peer>> _peers;
  ss::timer<ss::lowres_clock> _timer;
};
