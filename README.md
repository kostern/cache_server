# cache_server
A simple cache server in c++

# To compile:
Server: g++ server.cpp -lboost_system -pthread
Client: g++ -c client.cpp cli_ex.cpp -lboost_system -pthread (then link to a main file) 

The server compilation throws an internal compiler error if your gcc version is too high, I had to downgrade to 6.x to get it to work

