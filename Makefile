all:
	cmake -S . -B build -DCMAKE_MODULE_PATH=./seastar/cmake -DCMAKE_PREFIX_PATH="./seastar/build/release;./seastar/build/release/_cooking/installed"
	cmake --build build -j 17 --config Debug
run-server1:
	./build/star -c 1  --overprovisioned --id=1 --ip=127.0.0.1 --port=10001 --payload ./peer1/payload.yml --peers ./peers.yml --config ./config.yml
run-server2:
	./build/star -c 1  --overprovisioned --id=2 --ip=127.0.0.1 --port=10002 --payload ./peer2/payload.yml --peers ./peers.yml --config ./config.yml
run-server3:
	./build/star -c 1  --overprovisioned --id=3 --ip=127.0.0.1 --port=10003 --payload ./peer3/payload.yml --peers ./peers.yml --config ./config.yml
run-client:
	./build/star -c 1 --overprovisioned --server=0.0.0.0:10001
run-server:
	./build/star -c 1 --overprovisioned 