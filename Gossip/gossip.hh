#include "peer.hh"
#include "ss.hh"
#define TYPE_SEND_CONFIG 1
class Gossip {
public:
  Gossip(PeerId id);
  void SetLocalPeer(PeerId id_peer, ss::sstring local_addres);
  void AddPeer(std::string filepath);
  void SetLocalPayload(Payload &payload);
  Payload GetLocalPayload();
  void ReadPayload(std::string filepath);
  std::map<PeerId, ss::lw_shared_ptr<Peer>> GetPeers() const;
  void SetConfig(std::string filepath);
  Config GetConfig();
  void SendConfig(Config config);
  ss::rpc::protocol<serializer>::client *GetRandomPeer();
  void DelPeer(PeerId id);
  ss::future<ss::sstring> pingHandler(Config data);
  ss::future<ss::sstring> ClientRequest(ss::sstring input);
  void StartTimer();

private:
  ss::sstring _local_addres;
  ss::lw_shared_ptr<Peer> _local_peer;
  ss::lw_shared_ptr<Peer> _peer;
  std::map<PeerId, ss::lw_shared_ptr<Peer>> _peers;
  ss::timer<ss::lowres_clock> _timer;
  std::unique_ptr<ss::rpc::protocol<serializer>::server> _rpc_server = nullptr;
  Config _config;
};
