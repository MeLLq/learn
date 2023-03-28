#pragma once
#include "gossip.hh"
#include "ss.hh"

Gossip::Gossip() { _timer.arm_periodic(std::chrono::seconds(1)); }

void Gossip::SetLocalPeer(PeerId id, ss::sstring local_addres) {
  _local_addres = local_addres;
  _local_peer = ss::make_lw_shared<Peer>(id, local_addres);
}

ss::lw_shared_ptr<Peer> Gossip::GetLocalPeer() { return _local_peer; }

void Gossip::AddPeer(std::string filepath) {
  YAML::Node config = YAML::LoadFile(filepath);
  auto peers = config["peers"];
  for (const auto &peer : peers) {
    PeerId id = peer["id"].as<PeerId>();
    std::string ip = peer["ip"].as<std::string>();
    uint16_t port = peer["port"].as<uint16_t>();
    std::string ip_and_port = ip + ":" + std::to_string(port);
    _peers[id] = ss::make_lw_shared<Peer>(id, ip_and_port);
  }
}
void Gossip::ReadPayload(std::string filepath) {
  YAML::Node config = YAML::LoadFile(filepath);
  uint64_t epoch = config["epoch"].as<uint64_t>();
  std::string payload = config["payload"].as<std::string>();
  Payload new_payload{epoch, std::vector<char>(payload.begin(), payload.end())};
  SetLocalPayload(new_payload);
}

void Gossip::SetLocalPayload(Payload payload) {
  _local_peer.get()->SetPayload(payload);
}

Payload Gossip::GetLocalPayload() { return _local_peer.get()->GetPayload(); }

void Gossip::SetConfig(std::string filepath) {
  YAML::Node config;
  for (const auto &[id, peer] : _peers) {
    YAML::Node peer_node;
    peer_node["id"] = id;
    peer_node["epoch"] = peer.get()->GetPayload().epoch;
    peer_node["payload"] = peer.get()->GetPayload().blob;
    config["peers"].push_back(peer_node);
  }
  std::ofstream fout(filepath);
  fout << config;
  fout.close();
}

void Gossip::DelPeer(PeerId id) { _peers.erase(id); }

std::map<PeerId, ss::lw_shared_ptr<Peer>> Gossip::GetPeers() const {
  return _peers;
}
/*void Gossip::SetConfig() {
  for (const auto &[id, peer] : _peers) {
    _config[id] = peer.get()->GetPayload();
  }
}*/