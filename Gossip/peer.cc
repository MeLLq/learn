#pragma once
#include "peer.hh"
#include "ss.hh"

static ss::logger lgr("rpc_demo");
static ss::rpc::protocol<serializer> myrpc(serializer{});
std::unique_ptr<ss::rpc::protocol<serializer>::client> Peer::_rpc_client;

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, ss::sstring ip_addr) : _id(id), _ip_addr(ip_addr) {
  myrpc.set_logger(&lgr);
  std::cout << ip_addr << std::endl;
  _rpc_client = std::make_unique<ss::rpc::protocol<serializer>::client>(
      myrpc, ss::ipv4_addr{ip_addr});
}

Payload Peer::GetPayload() { return _payload; }

PeerId Peer::GetId() { return _id; }

ss::sstring Peer::GetIpAddr() { return _ip_addr; }