#pragma once
#include "peer.hh"

static ss::logger lgr("rpc_demo");

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, ss::sstring ip_addr) {
  if (!ip_addr.empty()) {
    /*myrpc.register_handler(TYPE_SEND_CONFIG, [this](Config input) {
      return ClientRequest(input);
      return ss::make_ready_future<ss::sstring>("yes");
    });*/
    _ip_addr = ip_addr;
    myrpc.set_logger(&lgr);
    _rpc_server = std::make_unique<ss::rpc::protocol<serializer>::server>(
        myrpc, ss::ipv4_addr{ip_addr});
    std::cout << ip_addr << std::endl;
    _rpc_client = std::make_unique<ss::rpc::protocol<serializer>::client>(
        myrpc, ss::ipv4_addr{ip_addr});
    _id = id;
  }
  _id = id;
}
Peer::~Peer() {
  if (_rpc_client != nullptr) {
    (void)_rpc_client->stop();
  }
  _rpc_client.release();
}
Payload Peer::GetPayload() { return _payload; }

PeerId Peer::GetId() { return _id; }

ss::sstring Peer::GetIpAddr() { return _ip_addr; }

std::unique_ptr<ss::rpc::protocol<serializer>::client> Peer::GetRpcClient() {
  return std::move(_rpc_client);
}
ss::future<ss::sstring> Peer::ClientRequest(Config input) {
  fmt::print("ono tipa SRABOTALO\n");
  return ss::make_ready_future<ss::sstring>("Hi, dada konechno");
}