
.PHONY : all everything clean test

everything : _server _client

_server : _server.c
	gcc -g -o $@ $^ -I/usr/include/mysql -lmysqlclient

_client : _client.c
	gcc -o $@ $^

test : everything
	./_server &
	sleep 0.5
	./_client
	pkill _server

all : everything
