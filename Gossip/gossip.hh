#include "peer.hh"
struct Config {
  std::vector<PeerId> payload;
};

class Gossip {
public:
  Gossip(sstring local_address);
  std::shared_ptr<Peer> AddPeer(PeerId id, sstring ip_addr);
  void SetLocalPayload(Payload payload);
  void DelPeer(PeerId id);
  std::map<PeerId, std::shared_ptr<Peer>> GetPeers() const;
  void SendConfig(Config config);

private:
  std::shared_ptr<Peer> _local_peer = nullptr;
  std::map<PeerId, std::shared_ptr<Peer>> _peers;
  timer<lowres_clock> _timer;
};
