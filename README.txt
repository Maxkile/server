============================================================================================
This is simple "cloud server" which you can connect to with client.cpp.
Client binary should contain ip-adress of server(now it only works in LAN) as command line argument.
GUI is not provided, onlu CLI is. Architecture is based on sockets.
============================================================================================
Client forms graph on it's side,sends it to server which  makes topological sort over it(if possible) and sends result as a string.
There will also be created log file with useful information about server activity.
Makefile will assemble all sources in binaries.
P.S. To drop the server press "CNTR + C"
============================================================================================
Documentation can be found in Docs