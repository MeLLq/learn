#include "gossip.hh"
#include <seastar/core/app-template.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/thread.hh>

namespace bpo = boost::program_options;
using namespace std::chrono_literals;

int main(int ac, char **av) {
  Gossip gossip(0);
  ss::timer<ss::lowres_clock> timer;
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
      gossip.SetLocalPeer(id, ip_and_port);
      gossip.AddPeer(peers_path);
      gossip.SetConfig(config_path);
      auto conf = gossip.GetConfig();
      for (const auto &[id, payload] : conf.payload) {
        std::cout << "id " << id << "payload epoch " << payload.epoch
                  << "payload " << payload.blob << std::endl;
      }
      timer.arm_periodic(1s);
      timer.set_callback([&]() noexcept {
        auto &&config = app.configuration();
        auto payload_path = config["payload"].as<std::string>();
        gossip.ReadPayload(payload_path);
        auto payload1 = gossip.GetLocalPayload();
      });
      gossip.StartTimer();
    } catch (...) {
      std::cout << std::current_exception();
    }
  });
}