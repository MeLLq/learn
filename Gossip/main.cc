#include "gossip.hh"
int main(int ac, char **av) {
  app_template app;
  return app.run_deprecated(ac, av, [] {
    auto gossip = std::make_shared<Gossip>("127.0.0.1");
    for (int i = 0; i <= 100; ++i) {
      sstring addr = "127.0.0.1:" + std::to_string(10000 + i);
      gossip.get()->AddPeer(i, addr);
    }
    for (const auto &[id, ip_addr] : gossip.get()->GetPeers()) {
      std::cout << "client id:" << id << " |  IP:" << ip_addr.get()->GetIpAddr()
                << std::endl;
    }
  });
}