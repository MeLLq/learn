#include <iostream>
#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/loop.hh>
#include <seastar/core/reactor.hh>
#include <seastar/rpc/rpc.hh>

using namespace seastar;
using PeerId = uint64_t;
struct serializer {};

struct Payload {
  uint64_t epoch; // number of epoch
  std::vector<char> blob;
};

struct Config {
  std::vector<PeerId> payload;
};

class Gossip {
public:
  Gossip(sstring local_address);
  Peer AddPeer(int peer_id, sstring ip_addr);
  void SetLocalPayload(Payload payload);
  void DelPeer(PeerId peer_id);

private:
  Peer peer_local_;
  Config peers_;
};

class Peer {
public:
  void SetPeerPayload(Payload add_payload);
  Payload GetPeerPayload();
  int GetPeerId();
  sstring GetPeerIpAddr();
  void SetPeerId(int peer_id);
  void SetPeerIpAddr(sstring ip_addr);
  void SetPeerClient(std::unique_ptr<rpc::protocol<serializer>::client> client);

private:
  Payload payload_;
  PeerId peer_id_;
  sstring peer_ip_addr_;
  std::unique_ptr<rpc::protocol<serializer>::client> client_;
};
