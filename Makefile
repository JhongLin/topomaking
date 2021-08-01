all:algo topo_generator
algo:topo.cpp algo.cpp
	g++ -w -o algo topo.cpp algo.cpp
topo_generator:getPath.cpp topo_generator.cpp
	g++ -w -o topo_generator getPath.cpp topo_generator.cpp
clean:
	rm -f algo
	rm -f topo_generator
