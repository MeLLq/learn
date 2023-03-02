#pragma once
#include "gossip.hh"

Gossip::Gossip(sstring local_adderss) : _local_peer(1, local_adderss) {}

Peer Gossip::AddPeer(PeerId peer_id, sstring ip_addr) {
  _peers[peer_id] = {peer_id, ip_addr};
}

void Gossip::SetLocalPayload(Payload payload) {
  _local_peer.SetPayload(payload);
}

void Gossip::DelPeer(PeerId id) {
  if (_peers.count(id) > 0) {
    _peers.erase(id);
  }
}
