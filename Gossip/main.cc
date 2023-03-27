#include "gossip.hh"
#include "ss.hh"
#include <seastar/core/thread.hh>

namespace bpo = boost::program_options;

int main(int ac, char **av) {
  ss::app_template app;
  app.add_options()("id", bpo::value<uint64_t>(), "Local peer id")(
      "ip", bpo::value<std::string>(),
      "Local ip address")("port", bpo::value<uint16_t>(), "Local peer port")(
      "payload", bpo::value<std::string>(),
      "Path to local peer payload file") /*(
"config", bpo::value<std::string>(), "Path to local peer config file")(
"peers", bpo::value<std::string>(), "Path to peers file")*/
      ;
  return app.run_deprecated(ac, av, [&] {
    ss::timer<ss::lowres_clock> timer;
    auto &&config = app.configuration();
    uint64_t id = config["id"].as<PeerId>();
    std::string ip = config["ip"].as<std::string>();
    uint16_t port = config["port"].as<uint16_t>();
    auto payload_path = config["payload"].as<std::string>();
    // auto config_path = config["config"].as<std::string>();
    // auto peers_path = config["peers"].as<std::string>();
    std::string ip_and_port = ip + ":" + std::to_string(port);
    Gossip gossip;
    gossip.SetLocalPeer(id, ip_and_port);
    // gossip.AddPeer(gossip.GetLocalPeer());
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