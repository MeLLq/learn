#pragma once
#include "gossip.hh"

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
  write_arithmetic_type(out, config.payload.size);
  for (const auto &payload : config.payload) {
    write_arithmetic_type(out, payload.first);
    write_arithmetic_type(out, payload.second.blob.size);
    write(out, payload.first);
    write(out, payload.second.blob);
    write(out, payload.second.epoch);
  }
  write(out, config.payload);
}

template <typename Input>
inline Config read(serializer, Input &in, ss::rpc::type<Config>) {
  Config config;
  read_arithmetic_type(in, config.payload.size);
  for (const auto &payload : config.payload) {
    read_arithmetic_type(in, payload.first);
    read_arithmetic_type(in, payload.second.blob.size);
    read(in, payload.first);
    read(in, payload.second.blob);
    read(in, payload.second.epoch);
  }
  read(in, config.payload);
  return config;
}

Gossip::Gossip(PeerId id)
    : _local_peer(std::move(ss::make_lw_shared<Peer>(id))) {
  // auto send = myrpc.make_client<Config(Config)>(TYPE_SEND_CONFIG);
  //_timer.arm_periodic(std::chrono::seconds(1));
  //_timer.set_callback([&] {
  //   send(*GetRandomPeer(), GetConfig()).then([&](Config config) {
  //     SendConfig(config);
  //     return ss::make_ready_future<>();
  //   });
  // });
}

void Gossip::SetPeer(PeerId id_peer, ss::sstring local_addres) {
  _local_addres = local_addres;
  _peer = ss::make_lw_shared<Peer>(id_peer, local_addres);
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
void Gossip::SendConfig(Config config) {
  for (const auto &[id, payload] : config.payload) {
    std::cout << "id " << id << " payload epoch " << payload.epoch
              << " payload " << payload.blob << std::endl;
  }
}

std::unique_ptr<ss::rpc::protocol<serializer>::client> Gossip::GetRandomPeer() {
  std::vector<PeerId> ids;
  PeerId self_id = _peer.get()->GetId();
  for (const auto &peer : _peers) {
    if (peer.first != self_id)
      ids.push_back(peer.first);
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, ids.size() - 1);
  PeerId random_id = ids[distrib(gen)];
  return _peers[random_id]->GetRpcClient();
}

/*void Gossip::SetConfig() {
  for (const auto &[id, peer] : _peers) {
    _config[id] = peer.get()->GetPayload();
  }
}*/
