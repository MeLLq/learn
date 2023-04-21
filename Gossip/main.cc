#include "gossip.hh"
#include <seastar/core/app-template.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/thread.hh>

// template <typename T, typename Output>
// inline void write_arithmetic_type(Output &out, T v) {
//   static_assert(std::is_arithmetic<T>::value, "must be arithmetic type");
//   return out.write(reinterpret_cast<const char *>(&v), sizeof(T));
// }
//
// template <typename T, typename Input> inline T read_arithmetic_type(Input
// &in) {
//   static_assert(std::is_arithmetic<T>::value, "must be arithmetic type");
//   T v;
//   in.read(reinterpret_cast<char *>(&v), sizeof(T));
//   return v;
// }
// template <typename Output>
// inline void write(serializer, Output &output, int32_t v) {
//   return write_arithmetic_type(output, v);
// }
// template <typename Output>
// inline void write(serializer, Output &output, uint32_t v) {
//   return write_arithmetic_type(output, v);
// }
// template <typename Output>
// inline void write(serializer, Output &output, int64_t v) {
//   return write_arithmetic_type(output, v);
// }
// template <typename Output>
// inline void write(serializer, Output &output, uint64_t v) {
//   return write_arithmetic_type(output, v);
// }
// template <typename Output>
// inline void write(serializer, Output &output, double v) {
//   return write_arithmetic_type(output, v);
// }
// template <typename Input>
// inline int32_t read(serializer, Input &input, ss::rpc::type<int32_t>) {
//   return read_arithmetic_type<int32_t>(input);
// }
// template <typename Input>
// inline uint32_t read(serializer, Input &input, ss::rpc::type<uint32_t>) {
//   return read_arithmetic_type<uint32_t>(input);
// }
// template <typename Input>
// inline uint64_t read(serializer, Input &input, ss::rpc::type<uint64_t>) {
//   return read_arithmetic_type<uint64_t>(input);
// }
// template <typename Input>
// inline uint64_t read(serializer, Input &input, ss::rpc::type<int64_t>) {
//   return read_arithmetic_type<int64_t>(input);
// }
// template <typename Input>
// inline double read(serializer, Input &input, ss::rpc::type<double>) {
//   return read_arithmetic_type<double>(input);
// }
//
// template <typename Output>
// inline void write(serializer, Output &out, const ss::sstring &v) {
//   write_arithmetic_type(out, uint32_t(v.size()));
//   out.write(v.c_str(), v.size());
// }
//
// template <typename Input>
// inline ss::sstring read(serializer, Input &in, ss::rpc::type<ss::sstring>) {
//   auto size = read_arithmetic_type<uint32_t>(in);
//   ss::sstring ret = ss::uninitialized_string(size);
//   in.read(ret.data(), size);
//   return ret;
// }
namespace bpo = boost::program_options;
using namespace std::chrono_literals;

int main(int ac, char **av) {
  Gossip gossip(0);
  // ss::timer<ss::lowres_clock> timer;
  ss::app_template app;
  app.add_options()("id", bpo::value<uint64_t>(), "Local peer id")(
      "ip", bpo::value<std::string>(),
      "Local ip address")("port", bpo::value<uint16_t>(), "Local peer port")(
      "payload", bpo::value<std::string>(), "Path to local peer payload file")(
      "config", bpo::value<std::string>(), "Path to local peer config file")(
      "peers", bpo::value<std::string>(), "Path to peers file");
  return app.run_deprecated(ac, av, [&] {
    try {
      auto &&config = app.configuration();
      uint64_t id = config["id"].as<PeerId>();
      std::string ip = config["ip"].as<std::string>();
      uint16_t port = config["port"].as<uint16_t>();
      auto payload_path = config["payload"].as<std::string>();
      auto config_path = config["config"].as<std::string>();
      auto peers_path = config["peers"].as<std::string>();
      std::string ip_and_port = ip + ":" + std::to_string(port);
      gossip.SetPeer(id, ip_and_port);
      gossip.AddPeer(peers_path);
      gossip.ReadPayload(payload_path);
      gossip.SetConfig(config_path);
      auto conf = gossip.GetConfig();
      for (const auto &[id, payload] : conf.payload) {
        std::cout << "id " << id << "payload epoch " << payload.epoch
                  << "payload " << payload.blob << std::endl;
      }
      gossip.TimerStart();

      /*timer.arm_periodic(1s);

      timer.set_callback([&]() noexcept {
        auto &&config = app.configuration();
        auto payload_path = config["payload"].as<std::string>();
        gossip.ReadPayload(payload_path);
        auto payload1 = gossip.GetLocalPayload();
        // std::cout << payload1.blob << std::endl;
      });*/
    } catch (...) {
      std::cout << std::current_exception();
    }

    //  gossip.ReadPayload(payload_path);
    /*if (!std::filesystem::exists(peers_path)) {
      std::ofstream create_file(peers_path);
      create_file.close();
    }
    std::ofstream ofout(peers_path, std::ofstream::in);
    std::ifstream ifout(peers_path);
    YAML::Node peer = YAML::Node(YAML::NodeType::Sequence);
    YAML::Node doc = YAML::Load(ifout);

    peer["id"] = id;
    peer["ip"] = ip;
    peer["port"] = port;
    doc["peers"].push_back(peer);
    ofout << doc << std::endl;
    ifout.close();
    ofout.close();

    if (!std::filesystem::exists(payload_path)) {
      std::string parse =
          payload_path.substr(payload_path.find_first_of('/') + 1);
      std::filesystem::create_directory(
          parse.substr(0, parse.find_first_of('/')));
      parse = parse.substr(parse.find_first_of('/') + 1);
      if (parse.find_first_of('/') == std::string::npos) {
        std::ofstream create_file1(payload_path);
        create_file1.close();
      }
    }
    std::ofstream ofout1(payload_path, std::ofstream::in);
    std::ifstream ifout1(payload_path);
    YAML::Node payload = YAML::Node(YAML::NodeType::Map);
    payload["epoch"] = 1;
    payload["payload"] = "hello form peer " + std::to_string(id);
    ofout1 << payload;
    ofout1.close();

    if (!std::filesystem::exists(config_path)) {
      std::ofstream create_file(config_path);
      create_file.close();
    }
    std::ofstream ofout2(config_path, std::ofstream::in);
    std::ifstream ifout2(config_path);
    YAML::Node cfg = YAML::Node(YAML::NodeType::Sequence);
    YAML::Node doc1 = YAML::Load(ifout2);
    cfg["id"] = peer["id"];
    cfg["epoch"] = payload["epoch"];
    cfg["payload"] = payload["payload"];
    doc1["peers"].push_back(cfg);
    ofout2 << doc1 << std::endl;
    ifout2.close();
    ofout2.close();*/
  });
}