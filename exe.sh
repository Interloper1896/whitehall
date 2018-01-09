#! /bin/sh
./server_white 8080&
./t7 localhost 8080 localhost 8081 jA&
./t7 localhost 8080 localhost 8082 jB&
./t7 localhost 8080 localhost 8083 jC&
./t7 localhost 8080 localhost 8084 jD&
