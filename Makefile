
.PHONY : all everything clean test reset

all : 
	test

everything : txproj_server txproj_login

txproj_server : txproj_server.c
	gcc -g -o $@ $^ -lmysqlclient

txproj_login : txproj_login.cpp
	g++ -g -o $@ $^ -lcgicc

ln : 
	sudo ln -s /home/wws/desktop/tencent-train/demo/bootstrap-3.2.0-dist /var/www/
	sudo ln -s /home/wws/desktop/tencent-train/txproj_*.html /var/www/

test : everything
	./_server &
	sleep 0.5

clean : 
	sudo /bin/rm /var/www/txproj_*.html /var/www/bootstrap-3.2.0-dist
	/bin/rm txproj_server txproj_login

reset :
	clean
	everything
	ln
	test
