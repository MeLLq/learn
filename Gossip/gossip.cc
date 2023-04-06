#pragma once
#include "gossip.hh"
#include "ss.hh"

Gossip::Gossip(PeerId id)
    : _local_peer(std::move(ss::make_lw_shared<Peer>(id))) {
  _timer.arm_periodic(std::chrono::seconds(1));
  auto send = myrpc.make_client<Config(Config)>(GetRandomPeerId());
  _timer.set_callback([&] mutable {
    send(*GetPeer(), GetConfig()).then(SendConfig(GetConfig()));
  });
}

void Gossip::SetPeer(PeerId id_peer, ss::sstring local_addres) {
  _local_addres = local_addres;
  _peer = ss::make_lw_shared<Peer>(id_peer, local_addres);
}

std::unique_ptr<ss::rpc::protocol<serializer>::client> Gossip::GetPeer() {
  return _peer->GetRpcClient();
}

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
  std::vector<char> ready_payload(payload.begin(), payload.end());
  Payload new_payload{epoch, ready_payload};
  SetLocalPayload(new_payload);
}

void Gossip::SetLocalPayload(Payload &payload) {
  _local_peer->SetPayload(payload);
}

Payload Gossip::GetLocalPayload() { return _local_peer->GetPayload(); }

void Gossip::SetConfig(std::string filepath) {
  YAML::Node config1 = YAML::LoadFile(filepath);
  auto peers = config1["peers"];
  for (const auto &peer : peers) {
    PeerId id = peer["id"].as<PeerId>();
    uint64_t epoch = peer["epoch"].as<uint64_t>();
    std::string payload_str = peer["payload"].as<std::string>();
    std::vector<char> blob(payload_str.begin(), payload_str.end());
    _config.payload[id] = Payload(epoch, blob);
  }
}
Config Gossip::GetConfig() { return _config; }

void Gossip::DelPeer(PeerId id) { _peers.erase(id); }

std::map<PeerId, ss::lw_shared_ptr<Peer>> Gossip::GetPeers() const {
  return _peers;
}
void Gossip::SendConfig(Config config) { _peer->SetConfig(config); }

PeerId Gossip::GetRandomPeerId() {
  std::vector<PeerId> ids;
  for (const auto &peer : _peers) {
    ids.push_back(peer.first);
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, ids.size() - 1);
  PeerId random_id = ids[distrib(gen)];
  return random_id;
}

/*void Gossip::SetConfig() {
  for (const auto &[id, peer] : _peers) {
    _config[id] = peer.get()->GetPayload();
  }
}*/
/*void Gossip::SendConfig(Config config) {
  for (const auto &[id, payload] : config.payload) {
    std::cout << "id " << id << " payload epoch " << payload.epoch
              << " payload " << payload.blob << std::endl;
  }
}*/