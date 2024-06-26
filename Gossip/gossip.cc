#pragma once
#include "gossip.hh"
#define TYPE_SEND_CONFIG 1
template <typename T, typename Output>
inline void write_arithmetic_type(Output &out, T v) {
  static_assert(std::is_arithmetic<T>::value, "must be arithmetic type");
  return out.write(reinterpret_cast<const char *>(&v), sizeof(T));
}

template <typename T, typename Input> inline T read_arithmetic_type(Input &in) {
  static_assert(std::is_arithmetic<T>::value, "must be arithmetic type");
  T v;
  in.read(reinterpret_cast<char *>(&v), sizeof(T));
  return v;
}
template <typename Output>
inline void write(serializer, Output &output, int32_t v) {
  return write_arithmetic_type(output, v);
}
template <typename Output>
inline void write(serializer, Output &output, uint32_t v) {
  return write_arithmetic_type(output, v);
}
template <typename Output>
inline void write(serializer, Output &output, int64_t v) {
  return write_arithmetic_type(output, v);
}
template <typename Output>
inline void write(serializer, Output &output, uint64_t v) {
  return write_arithmetic_type(output, v);
}
template <typename Output>
inline void write(serializer, Output &output, double v) {
  return write_arithmetic_type(output, v);
}
template <typename Input>
inline int32_t read(serializer, Input &input, ss::rpc::type<int32_t>) {
  return read_arithmetic_type<int32_t>(input);
}
template <typename Input>
inline uint32_t read(serializer, Input &input, ss::rpc::type<uint32_t>) {
  return read_arithmetic_type<uint32_t>(input);
}
template <typename Input>
inline uint64_t read(serializer, Input &input, ss::rpc::type<uint64_t>) {
  return read_arithmetic_type<uint64_t>(input);
}
template <typename Input>
inline uint64_t read(serializer, Input &input, ss::rpc::type<int64_t>) {
  return read_arithmetic_type<int64_t>(input);
}
template <typename Input>
inline double read(serializer, Input &input, ss::rpc::type<double>) {
  return read_arithmetic_type<double>(input);
}

template <typename Output>
inline void write(serializer, Output &out, const ss::sstring &v) {
  write_arithmetic_type(out, uint32_t(v.size()));
  out.write(v.c_str(), v.size());
}

template <typename Input>
inline ss::sstring read(serializer, Input &in, ss::rpc::type<ss::sstring>) {
  auto size = read_arithmetic_type<uint32_t>(in);
  ss::sstring ret = ss::uninitialized_string(size);
  in.read(ret.data(), size);
  return ret;
}

template <typename Output>
inline void write(serializer, Output &out, const Config &config) {
  write_arithmetic_type(out, config.payload.size());
  for (const auto &payload : config.payload) {
    write_arithmetic_type(out, payload.first);
    write_arithmetic_type(out, payload.second.blob.size());
    write_arithmetic_type(out, payload.second.epoch);
    out.write(payload.second.blob.data(), payload.second.blob.size());
  }
}

template <typename Input>
inline Config read(serializer, Input &in, ss::rpc::type<Config>) {
  Config config;
  auto payload_size = read_arithmetic_type<uint64_t>(in);
  for (auto i = 0; i < payload_size; i++) {
    auto Peer_Id = read_arithmetic_type<uint64_t>(in);
    auto blob_size = read_arithmetic_type<uint64_t>(in);
    auto epoch = read_arithmetic_type<uint64_t>(in);
    std::vector<char> blob(blob_size);
    in.read(blob.data(), blob_size);
    config.payload[Peer_Id] = Payload(epoch, blob);
  }
  return config;
}

Gossip::Gossip(PeerId id)
    : _local_peer(std::move(ss::make_lw_shared<Peer>(id))) {
  myrpc.register_handler(TYPE_SEND_CONFIG, [&](Config input) {
    OnReceiveConfig(input);
    return ss::sstring("hi");
  });
  auto send = myrpc.make_client<ss::sstring(Config)>(TYPE_SEND_CONFIG);
  _timer.set_callback([&, send] mutable {
    auto peer = GetRandomPeer();
    send(*peer, GetConfig())
        .then([](ss::sstring str) mutable { std::cout << str << std::endl; })
        .handle_exception_type([&](ss::rpc::closed_error &err) {
          std::cerr << "Произошло исключение " << err.what() << std::endl;
        });
  });
}
void Gossip::StartTimer() { _timer.arm_periodic(std::chrono::seconds(3)); }

void Gossip::SetLocalPeer(PeerId id_peer, ss::sstring local_addres) {
  _local_addres = local_addres;
  _local_peer = ss::make_lw_shared<Peer>(id_peer, local_addres);
  _rpc_server = std::make_unique<ss::rpc::protocol<serializer>::server>(
      myrpc, ss::ipv4_addr{local_addres});
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
  if (!_initial_payload) {
    auto peers = GetPeers();
    for (const auto &peer : peers) {
      Payload empty_payload{0, std::vector<char>()};
      peer.second->SetPayload(empty_payload);
    }
    _initial_payload = true;
  }
}

Payload Gossip::GetLocalPayload() { return _local_peer->GetPayload(); }

Config Gossip::GetConfig() {
  Config config;
  auto peers = GetPeers();
  for (const auto &peer : peers) {
    PeerId id = peer.first;
    if (id != _local_peer->GetId()) {
      Payload payload = peer.second->GetPayload();
      config.payload[id] = payload;
    } else {
      Payload payload = GetLocalPayload();
      config.payload[id] = payload;
    }
  }
  return config;
}

void Gossip::DelPeer(PeerId id) { _peers.erase(id); }

std::map<PeerId, ss::lw_shared_ptr<Peer>> Gossip::GetPeers() const {
  return _peers;
}
void Gossip::OnReceiveConfig(Config config) {
  auto peers = GetPeers();
  YAML::Node doc = YAML::LoadFile("./config.yml");
  bool config_changed = false;
  int index = 0;
  for (const auto &peer_node : doc["peers"]) {
    if (peer_node.IsMap()) {
      const auto &id_node = peer_node["id"];
      if (id_node && id_node.IsScalar()) {
        PeerId id = id_node.as<PeerId>();
        if (config.payload.count(id) > 0) {
          if (config.payload[id].epoch > GetConfig().payload[id].epoch) {
            Payload payload = config.payload[id];
            peers[id]->SetPayload(payload);
            doc["peers"][index]["epoch"] = static_cast<uint64_t>(payload.epoch);
            doc["peers"][index]["payload"] =
                std::string(payload.blob.begin(), payload.blob.end());
            config_changed = true;
            GetConfig();
          } else {
            std::cout << "Конфиг не изменился" << std::endl;
          }
        }
      }
    }
    ++index;
  }

  if (config_changed) {
    SaveConfig(doc);
  }

  for (const auto &[id, payload] : config.payload) {
    if (peers.count(id) > 0) {
      std::cout << "id " << id << " payload epoch " << payload.epoch
                << " payload " << payload.blob << std::endl;
    }
  }
}
void Gossip::SaveConfig(YAML::Node doc) {
  std::ofstream ofout1("./config.yml");
  ofout1 << doc;
  ofout1.close();
}
ss::rpc::protocol<serializer>::client *Gossip::GetRandomPeer() {
  std::vector<PeerId> ids;
  for (const auto &peer : _peers) {
    if (peer.first != _local_peer->GetId())
      ids.push_back(peer.first);
  }
  std::random_device rd;
  _gen.seed(rd());
  std::uniform_int_distribution<> distrib(0, ids.size() - 1);
  PeerId random_id = ids[distrib(_gen)];
  return _peers[random_id]->GetRpcClient();
}
