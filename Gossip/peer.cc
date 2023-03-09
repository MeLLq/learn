#pragma once
#include "peer.hh"
#include "ss.hh"

ss::rpc::protocol<serializer> myrpc(serializer{});
ss::logger loger1("rpc_demo");

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, ss::sstring ip_addr) : _id(id), _ip_addr(ip_addr) {
  myrpc.set_logger(&loger1);
  _rpc_client = std::make_unique<ss::rpc::protocol<serializer>::client>(
      myrpc, ss::ipv4_addr{ip_addr, 10000});
}
Payload Peer::GetPayload() { return _payload; }

int Peer::GetId() { return _id; }

ss::sstring Peer::GetIpAddr() { return _ip_addr; }