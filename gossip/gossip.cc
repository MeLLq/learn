#pragma once
#include "gossip.hh"

Gossip::Gossip(sstring local_adderss) {
  peer_local_.SetPeerIpAddr(local_adderss);
}

Peer Gossip::AddPeer(int peer_id, sstring ip_addr) {
  peer_local_.SetPeerId(peer_id);
  peer_local_.SetPeerIpAddr(ip_addr);
}
void Gossip::SetLocalPayload(Payload payload) {
  peer_local_.SetPeerPayload(payload);
}
void Gossip::DelPeer(int host_id) {
  for (auto it = peers_.payload.begin(); it != peers_.payload.end(); ++it) {
    if ((*it) == host_id) {
      peers_.payload.erase(it);
      return;
    }
  }
}

void Peer::SetPeerPayload(Payload add_payload) {
  payload_.Epoch = add_payload.Epoch;
  payload_.Blob = std::move(add_payload.Blob);
}
Payload Peer::GetPeerPayload() { return payload_; }

int Peer::GetPeerId() { return peer_id_; }

sstring Peer::GetPeerIpAddr() { return peer_ip_addr_; }
void Peer::SetPeerId(int peer_id) { peer_id_ = peer_id; }
void Peer::SetPeerIpAddr(sstring peer_ip_addr) { peer_ip_addr_ = peer_ip_addr; }
void Peer::SetPeerClient(
    std::unique_ptr<rpc::protocol<serializer>::client> client) {
  client_ = std::make_unique<rpc::protocol<serializer>::client>(client);
}
