#pragma once
#include "gossip.hh"

// Gossip::Gossip(sstring local_adderss) :
// _local_peer(std::make_shared<Peer>(nullptr,local_adderss)) {}

Gossip::Gossip(sstring local_address) : _local_peer(nullptr) {
  _local_addres = local_address;
}

lw_shared_ptr<Peer> Gossip::AddPeer(PeerId peer_id, sstring ip_addr) {
  auto ptr = seastar::make_lw_shared<Peer>(peer_id, ip_addr);
  _peers[peer_id] = ptr;
  return ptr;
}

void Gossip::SetLocalPayload(Payload payload) {
  if (_local_peer == nullptr) {
    _local_peer = seastar::make_lw_shared<Peer>(1, _local_addres);
  }
  _local_peer.get()->SetPayload(payload);
}

void Gossip::DelPeer(PeerId id) { _peers.erase(id); }

std::map<PeerId, seastar::lw_shared_ptr<Peer>> Gossip::GetPeers() const {
  return _peers;
}
void Gossip::SendConfig(Config config) {
  _timer.arm(std::chrono::seconds(1));
  _timer.set_callback([this, config] mutable {
    auto rand = std::mt19937(100);
    int random_numer = rand() % _peers.size() - 1;
    _peers.at(random_numer)->SetPayload(_local_peer.get()->GetPayload());
  });
}
