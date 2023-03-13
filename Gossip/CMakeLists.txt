cmake_minimum_required(VERSION 3.22)
project(Gossip)
add_library(libgossip PRIVATE peer.cc gossip.cc main.cc)
target_link_libraries(libgossip Seastar::seastar)


