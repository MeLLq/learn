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

void Peer::SetClient(
    std::unique_ptr<rpc::protocol<serializer>::client> client) {
  _client = std::make_unique<rpc::protocol<serializer>::client>(client);
}
