#include "gossip.hh"
#include "ss.hh"

namespace bpo = boost::program_options;

int main(int ac, char **av) {
  ss::app_template app;
  app.add_options()("id", bpo::value<uint64_t>(), "Local peer id")(
      "ip", bpo::value<std::string>(),
      "Local ip address")("port", bpo::value<uint16_t>(), "Local peer port")(
      "payload", bpo::value<ss::sstring>(), "Path to local peer payload file")(
      "config", bpo::value<ss::sstring>(), "Path to local peer config file")(
      "peers", bpo::value<ss::sstring>(), "Path to peers file");
  return app.run_deprecated(ac, av, [&] {
    auto &&config = app.configuration();
    uint64_t id = config["id"].as<PeerId>();
    std::string ip = config["ip"].as<std::string>();
    uint16_t port = config["port"].as<uint16_t>();
    // auto payload_path = config["payload"].as<ss::sstring>();
    // auto config_path = config["config"].as<ss::sstring>();
    // auto peers_path = config["peers"].as<ss::sstring>();
    std::string str = ip + ":" + std::to_string(port);
    // Gossip gossip("127.0.0.1:10001");
    //  std::cout << str << std::endl;
    // gossip.AddPeer(1, "127.0.0.1:10002");
    // gossip.SetLocalPayload({});

    Peer peer(1, str);
  });
}