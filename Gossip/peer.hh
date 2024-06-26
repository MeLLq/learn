#include "ss.hh"
#include <fstream>
#include <iostream>
#include <random>
#include <seastar/core/future.hh>
#include <seastar/core/loop.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/timer.hh>
#include <seastar/rpc/rpc.hh>
#include <seastar/util/log.hh>
#include <yaml-cpp/yaml.h>

using PeerId = uint64_t;
struct serializer {};

static ss::rpc::protocol<serializer> myrpc(serializer{});
struct Payload {
  uint64_t epoch; // number of epoch
  std::vector<char> blob;
  bool operator==(const Payload &other) const;
};

struct Config {
  std::map<PeerId, Payload> payload;
};

class Peer {
public:
  Peer(PeerId id, ss::sstring ip_addr = "");
  ~Peer();
  void SetPayload(Payload add_payload);
  Payload GetPayload();
  PeerId GetId();
  ss::sstring GetIpAddr();
  ss::rpc::protocol<serializer>::client *GetRpcClient();

private:
  Payload _payload;
  PeerId _id;
  ss::sstring _ip_addr;
  std::unique_ptr<ss::rpc::protocol<serializer>::client> _rpc_client = nullptr;
};
