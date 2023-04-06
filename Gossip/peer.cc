#pragma once
#include "peer.hh"
#include "ss.hh"

static ss::logger lgr("rpc_demo");

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, ss::sstring ip_addr) {
  if (!ip_addr.empty()) {
    myrpc.set_logger(&lgr);
    _ip_addr = ip_addr;
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
void Peer::SetConfig(Config add_config) { _config = std::move(add_config); }