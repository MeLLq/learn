#pragma once
#include "peer.hh"

bool Payload::operator==(const Payload &other) const {
  return epoch == other.epoch && blob == other.blob;
}

static ss::logger lgr("rpc_demo");

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, ss::sstring ip_addr) {
  if (!ip_addr.empty()) {
    _ip_addr = ip_addr;
    myrpc.set_logger(&lgr);
    // std::cout << ip_addr << std::endl;
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

ss::rpc::protocol<serializer>::client *Peer::GetRpcClient() {
  if (_rpc_client->error()) {
    _rpc_client = std::make_unique<ss::rpc::protocol<serializer>::client>(
        myrpc, ss::ipv4_addr{_ip_addr});
  }
  return _rpc_client.get();
}
