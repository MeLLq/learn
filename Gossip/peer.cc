#pragma once
#include "peer.hh"
#include "ss.hh"

static ss::logger lgr("rpc_demo");
std::unique_ptr<ss::rpc::protocol<serializer>::client> Peer::_rpc_client;

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, ss::sstring ip_addr) : _id(id), _ip_addr(ip_addr) {
  myrpc.set_logger(&lgr);
  _rpc_client = std::make_unique<ss::rpc::protocol<serializer>::client>(
      myrpc, ss::ipv4_addr{ip_addr});
  std::cout << "client start" << std::endl;
}

Payload Peer::GetPayload() { return _payload; }

int Peer::GetId() { return _id; }

ss::sstring Peer::GetIpAddr() { return _ip_addr; }