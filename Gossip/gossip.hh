#include "peer.hh"
#include "ss.hh"

class Gossip {
public:
  Gossip(PeerId id);
  void SetPeer(PeerId id_peer, ss::sstring local_addres);
  void AddPeer(std::string filepath);
  void SetLocalPayload(Payload &payload);
  Payload GetLocalPayload();
  void ReadPayload(std::string filepath);
  std::map<PeerId, ss::lw_shared_ptr<Peer>> GetPeers() const;
  void SetConfig(std::string filepath);
  Config GetConfig();
  void SendConfig(Config config);
  std::unique_ptr<ss::rpc::protocol<serializer>::client> GetRandomPeer();
  void DelPeer(PeerId id);

private:
  ss::sstring _local_addres;
  ss::lw_shared_ptr<Peer> _local_peer;
  ss::lw_shared_ptr<Peer> _peer;
  std::map<PeerId, ss::lw_shared_ptr<Peer>> _peers;
  ss::timer<ss::lowres_clock> _timer;

  Config _config;
};
