all:
	cmake -S . -B build -DCMAKE_MODULE_PATH=./seastar/cmake -DCMAKE_PREFIX_PATH="./seastar/build/release;./seastar/build/release/_cooking/installed"
	cmake --build build -j 17
run-server:
	sudo ./build/star -c 1 --overprovisioned
run-client:
	./build/star -c 1 --overprovisioned --server=0.0.0.0:12345