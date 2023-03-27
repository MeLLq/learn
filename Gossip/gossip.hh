#include "peer.hh"
#include "ss.hh"
class Gossip {
public:
  Gossip();
  ss::lw_shared_ptr<Peer> AddPeer(ss::lw_shared_ptr<Peer> local_peer);
  void SetLocalPeer(PeerId id, ss::sstring local_addres);
  ss::lw_shared_ptr<Peer> GetLocalPeer();
  void SetLocalPayload(Payload payload);
  void ReadPayload(std::string filepath);
  void DelPeer(PeerId id);
  std::map<PeerId, ss::lw_shared_ptr<Peer>> GetPeers() const;
  void SetConfig();
  void SendPayload();

private:
  ss::sstring _local_addres;
  ss::lw_shared_ptr<Peer> _local_peer;
  std::map<PeerId, ss::lw_shared_ptr<Peer>> _peers;
  ss::timer<ss::lowres_clock> _timer;
  std::map<PeerId, Payload> _config;
  std::string filepath;
};
