#include <cmath>
#include <exception>
#include <iostream>
#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/loop.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/timer.hh>
#include <seastar/rpc/rpc.hh>
#include <seastar/util/log.hh>

using namespace seastar;

struct serializer {};

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
inline int32_t read(serializer, Input &input, rpc::type<int32_t>) {
  return read_arithmetic_type<int32_t>(input);
}
template <typename Input>
inline uint32_t read(serializer, Input &input, rpc::type<uint32_t>) {
  return read_arithmetic_type<uint32_t>(input);
}
template <typename Input>
inline uint64_t read(serializer, Input &input, rpc::type<uint64_t>) {
  return read_arithmetic_type<uint64_t>(input);
}
template <typename Input>
inline uint64_t read(serializer, Input &input, rpc::type<int64_t>) {
  return read_arithmetic_type<int64_t>(input);
}
template <typename Input>
inline double read(serializer, Input &input, rpc::type<double>) {
  return read_arithmetic_type<double>(input);
}

template <typename Output>
inline void write(serializer, Output &out, const sstring &v) {
  write_arithmetic_type(out, uint32_t(v.size()));
  out.write(v.c_str(), v.size());
}

template <typename Input>
inline sstring read(serializer, Input &in, rpc::type<sstring>) {
  auto size = read_arithmetic_type<uint32_t>(in);
  sstring ret = uninitialized_string(size);
  in.read(ret.data(), size);
  return ret;
}

namespace bpo = boost::program_options;
using namespace std::chrono_literals;
timer<lowres_clock> _stats_timer;

#define PING_TYPE 1

sstring pingHandler(sstring data) {
  fmt::print("this is {} \n", data);
  return sstring("Hi");
}

void pingResponseHandler(sstring data) { fmt::print("this is {}\n", data); }

int main(int ac, char **av) {
  app_template app;
  app.add_options()("port", bpo::value<uint16_t>()->default_value(12345),
                    "RPC server port")("server", bpo::value<std::string>(),
                                       "Server address");
  std::cout << "start ";
  rpc::protocol<serializer> myrpc(serializer{});
  static std::unique_ptr<rpc::protocol<serializer>::server> server;
  static std::unique_ptr<rpc::protocol<serializer>::client> client;

  static logger log("rpc_demo");
  myrpc.set_logger(&log);

  return app.run_deprecated(ac, av, [&] {
    auto &&config = app.configuration();
    uint16_t port = config["port"].as<uint16_t>();
    // clientside
    if (config.count("server")) {
      auto pingRPC = myrpc.make_client<sstring(sstring)>(PING_TYPE);
      std::cout << "client" << std::endl;
      client = std::make_unique<rpc::protocol<serializer>::client>(
          myrpc, ipv4_addr{config["server"].as<std::string>()});
      _stats_timer.arm_periodic(1s);
      _stats_timer.set_callback([pingRPC] mutable {
        pingRPC(*client, "ping")
            .then(pingResponseHandler)
            .handle_exception_type([](rpc::closed_error &err) {
              _stats_timer.cancel();
              engine().exit(0);
            });
      });

      // pingRPC(*client, "ping").then(pingResponseHandler);
    }
    // serverside
    else {
      myrpc.register_handler(PING_TYPE, pingHandler);
      std::cout << "server on port " << port << std::endl;
      rpc::resource_limits limits;
      limits.bloat_factor = 1;
      limits.basic_request_size = 0;
      limits.max_memory = 10'000'000;
      server = std::make_unique<rpc::protocol<serializer>::server>(
          myrpc, ipv4_addr{port}, limits);
    }
  });
}