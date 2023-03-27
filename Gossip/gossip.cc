#pragma once
#include "gossip.hh"
#include "ss.hh"

Gossip::Gossip() { _timer.arm_periodic(std::chrono::seconds(1)); }

ss::lw_shared_ptr<Peer> Gossip::AddPeer(ss::lw_shared_ptr<Peer> local_peer) {
  _peers[local_peer->GetId()] = local_peer;
  return local_peer;
}
ss::lw_shared_ptr<Peer> Gossip::GetLocalPeer() { return _local_peer; }

void Gossip::SetLocalPeer(PeerId id, ss::sstring local_addres) {
  _local_addres = local_addres;
  _local_peer = ss::make_lw_shared<Peer>(id, local_addres);
}

void Gossip::SetLocalPayload(Payload payload) {
  _local_peer.get()->SetPayload(payload);
}

/*void Gossip::ReadPayload(std::string filepath) {
  YAML::Node payload_node;
  payload_node = YAML::LoadFile(filepath);
  Payload payload;
  payload.epoch = payload_node["epoch"].as<uint64_t>();
  payload.blob = payload_node["payload"].as<std::vector<char>>();

  if (_local_peer == nullptr) {
    _local_peer = ss::make_lw_shared<Peer>(1, _local_addres);
  }

  if (payload.epoch != _local_peer->GetPayload().epoch ||
      payload.blob != _local_peer->GetPayload().blob) {
    std::cout << "Updating local payload" << std::endl;
    SetLocalPayload(payload);
    std::cout << "end" << std::endl;
  }
}*/

void Gossip::SetConfig() {
  for (const auto &[id, peer] : _peers) {
    _config[id] = peer.get()->GetPayload();
  }
}

void Gossip::DelPeer(PeerId id) { _peers.erase(id); }

std::map<PeerId, seastar::lw_shared_ptr<Peer>> Gossip::GetPeers() const {
  return _peers;
}
void Gossip::SendPayload() {
  _timer.set_callback([this] {
    auto rand = std::mt19937(100);
    int random_number = rand() % (_peers.size() - 1);
    auto it = _peers.begin();
    for (int i = 0; i < random_number; i++) {
      ++it;
    }
    for (; it != _peers.end(); ++it) {
      if (it->second == _local_peer) {
        continue;
      }
      auto peer = it->second;
      auto payload = _local_peer->GetPayload();
      peer->SetPayload(payload);
    }
  });
}
