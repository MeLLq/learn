#pragma once
#include "gossip.hh"
//
Gossip::Gossip(sstring local_adderss) {}
//
Peer Gossip::AddPeer(PeerId peer_id, sstring ip_addr) {
  peers_[peer_id] = Peer peer_local_.SetPeerIpAddr(ip_addr);
}
//
void Gossip::SetLocalPayload(Payload payload) {
  peer_local_.SetPeerPayload(payload);
}
//
void Gossip::DelPeer(int host_id) {
  for (auto it = peers_.payload.begin(); it != peers_.payload.end(); ++it) {
    if ((*it) == host_id) {
      peers_.payload.erase(it); //
      return;
    }
  }
}

void Peer::SetPeerPayload(Payload add_payload) {
  payload_.epoch = add_payload.epoch;
  payload_.blob = std::move(add_payload.blob);
}

Peer::Peer(PeerId id, sstring ip_addr) : id_(id), ip_addr_(ip_addr) {}

Payload Peer::GetPeerPayload() { return payload_; }

int Peer::GetPeerId() { return id_; }

sstring Peer::GetPeerIpAddr() { return ip_addr_; }

void Peer::SetPeerClient(
    std::unique_ptr<rpc::protocol<serializer>::client> client) {
  client_ = std::make_unique<rpc::protocol<serializer>::client>(client);
}
