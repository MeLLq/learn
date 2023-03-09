#include <iostream>
#include <random>
#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/loop.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/timer.hh>
#include <seastar/rpc/rpc.hh>
#include <seastar/util/log.hh>
#include "ss.hh"


using PeerId = uint64_t;
struct serializer {};

struct Payload {
  uint64_t epoch; // number of epoch
  std::vector<char> blob;
};
struct Config {
  std::vector<PeerId> payload;
};
class Peer {
public:
  Peer(PeerId id, ss::sstring ip_addr);
  void SetPayload(Payload add_payload);
  Payload GetPayload();
  int GetId();
  ss::sstring GetIpAddr();

private:
  Payload _payload;
  PeerId _id;
  ss::sstring _ip_addr;
  std::unique_ptr<seastar::rpc::client> _rpc_client;
};