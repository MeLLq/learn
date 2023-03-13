#pragma once
#include "peer.hh"
#include "ss.hh"

static ss::rpc::protocol<serializer> myrpc(serializer{});
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

  /*
  _rpc_client = ss::rpc::protocol<serializer>::client>(
      myrpc, ss::ipv4_addr(ip_addr));*/
}

/*Peer::Peer(PeerId id, ss::sstring ip_addr) : _id(id), _ip_addr(ip_addr) {
  myrpc.set_logger(&loger1);
  _rpc_client = std::make_unique<ss::rpc::protocol<serializer>::client>(
   myrpc, ss::ipv4_addr{ip_addr, 10000});
  ss::socket_address sa = ss::socket_address(ss::ipv4_addr(ip_addr, 10002));
  ss::rpc::client_options options;
  _rpc_client = std::make_unique<ss::rpc::client>(loger1, nullptr, options,
 sa);
} */
Payload Peer::GetPayload() { return _payload; }

int Peer::GetId() { return _id; }

ss::sstring Peer::GetIpAddr() { return _ip_addr; }