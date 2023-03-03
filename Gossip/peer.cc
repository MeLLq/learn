#pragma once
#include "peer.hh"

void Peer::SetPayload(Payload add_payload) {
  _payload.epoch = add_payload.epoch;
  _payload.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, sstring ip_addr) : _id(id), _ip_addr(ip_addr) {}

Payload Peer::GetPayload() { return _payload; }

int Peer::GetId() { return _id; }

sstring Peer::GetIpAddr() { return _ip_addr; }

void Peer::SetClient(seastar::lw_shared_ptr<seastar::rpc::client> client) {
  _rpc_client = std::move(client);
}
