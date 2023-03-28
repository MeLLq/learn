#include "peer.hh"
#include "ss.hh"
class Gossip {
public:
  Gossip();
  void SetLocalPeer(PeerId id, ss::sstring local_addres);
  ss::lw_shared_ptr<Peer> GetLocalPeer();
  void AddPeer(std::string filepath);
  void SetLocalPayload(Payload payload);
  Payload GetLocalPayload();
  void ReadPayload(std::string filepath);
  void DelPeer(PeerId id);
  std::map<PeerId, ss::lw_shared_ptr<Peer>> GetPeers() const;
  void SetConfig(std::string filepath);
  void SendPayload();

private:
  ss::sstring _local_addres;
  ss::lw_shared_ptr<Peer> _local_peer;
  std::map<PeerId, ss::lw_shared_ptr<Peer>> _peers;
  ss::timer<ss::lowres_clock> _timer;
  std::map<PeerId, Payload> _config;
  std::string _filepath;
};
