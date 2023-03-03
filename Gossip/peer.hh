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

class Peer {
public:
  Peer(PeerId id, sstring ip_addr);
  void SetPayload(Payload add_payload);
  Payload GetPayload();
  int GetId();
  sstring GetIpAddr();
  void SetClient(std::unique_ptr<rpc::protocol<serializer>::client> client);

private:
  Payload _payload;
  PeerId _id;
  sstring _ip_addr;
  std::unique_ptr<rpc::protocol<serializer>::client> _client;
};