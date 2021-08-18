build: client

clean:
	rm -f client
	rm -f client.o
	rm -f helpers.o
	rm -f buffer.o
	rm -f requests.o

client: client.o buffer.o helpers.o requests.o
	g++ -o client client.o buffer.o helpers.o requests.o

client.o: client.cpp
	g++ -c client.cpp

buffer.o: buffer.cpp
	g++ -c buffer.cpp

helpers.o: helpers.cpp
	g++ -c helpers.cpp

requests.o: requests.cpp
	g++ -c requests.cpp
