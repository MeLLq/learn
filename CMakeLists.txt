find_package (Seastar REQUIRED) 
find_package (Valgrind REQUIRED)
add_executable (star rpc_remake_demos.cc)
target_link_libraries (star Seastar::seastar)
