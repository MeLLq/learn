#pragma once
#include "gossip.hh"

// Gossip::Gossip(sstring local_adderss) :
// _local_peer(std::make_shared<Peer>(nullptr,local_adderss)) {}

Gossip::Gossip(sstring local_address)
    : _local_peer(std::make_shared<Peer>(local_address)) {}

Peer Gossip::AddPeer(PeerId peer_id, sstring ip_addr) {
  _peers[peer_id] = {peer_id, ip_addr};
}

void Gossip::SetLocalPayload(Payload payload) {
  _local_peer.get()->SetPayload(payload);
}

void Gossip::DelPeer(PeerId id) {
  if (_peers.count(id) > 0) {
    _peers.erase(id);
  }
}
