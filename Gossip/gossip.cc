#pragma once
#include "gossip.hh"

Gossip::Gossip(sstring local_adderss) : _local_peer(1, local_adderss) {}

Peer Gossip::AddPeer(PeerId peer_id, sstring ip_addr) {
  // peers_[peer_id] = Peer
}

void Gossip::SetLocalPayload(Payload payload) {
  _local_peer.SetPayload(payload);
}

void Gossip::DelPeer(int host_id) {
  for (auto it = _peers.payload.begin(); it != peers_.payload.end(); ++it) {
    if ((*it) == host_id) {
      peers_.payload.erase(it); //
      return;
    }
  }
}

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
