#include "peer.hh"
#include "ss.hh"

class Gossip {
public:
  Gossip(PeerId id);
  void SetLocalPeer(PeerId id_peer, ss::sstring local_addres);
  void AddPeer(std::string filepath);
  void SetLocalPayload(Payload &payload);
  Payload GetLocalPayload();
  void ReadPayload(std::string filepath);
  std::map<PeerId, ss::lw_shared_ptr<Peer>> GetPeers() const;
  Config GetConfig();
  void OnReceiveConfig(Config config);
  ss::rpc::protocol<serializer>::client *GetRandomPeer();
  void DelPeer(PeerId id);
  ss::future<ss::sstring> ClientRequest(ss::sstring input);
  void StartTimer();
  void SaveConfig(YAML::Node doc);

private:
  bool _initial_payload = false;
  std::mt19937 _gen;
  ss::sstring _local_addres;
  ss::lw_shared_ptr<Peer> _local_peer;
  std::map<PeerId, ss::lw_shared_ptr<Peer>> _peers;
  ss::timer<ss::lowres_clock> _timer;
  std::unique_ptr<ss::rpc::protocol<serializer>::server> _rpc_server = nullptr;
};
