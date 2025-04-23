# List of all targets
.PHONY: all
all: serverM serverA serverB client

# Clean up targets
.PHONY: clean
clean:
	rm -f serverM serverA serverB client

# Compile and link commands
serverM: serverM.cpp
	g++ -o serverM serverM.cpp

serverA: serverA.cpp
	g++ -o serverA serverA.cpp

serverB: serverB.cpp
	g++ -o serverB serverB.cpp

client: client.cpp
	g++ -o client client.cpp