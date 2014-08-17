
PROJ_DIR	:= /home/wws/desktop/tencent-train
CGI_DIR 	:= /usr/lib/cgi-bin
STATCI_DIR	:= /var/www

.PHONY : all everything clean test reset

all : 
	test

everything : txproj_server txproj_login txproj_list txproj_pay txproj_refund

txproj_server : txproj_server.c
	gcc -g -o $@ $^ -lmysqlclient

txproj_login : txproj_login.cpp common.h common.cpp
	g++ -g -o $@ $^ -lcgicc

txproj_list : txproj_list.cpp common.h common.cpp
	g++ -g -o $@ $^ -lcgicc

txproj_pay : txproj_pay.cpp common.h common.cpp
	g++ -g -o $@ $^ -lcgicc

txproj_refund : txproj_refund.cpp common.h common.cpp
	g++ -g -o $@ $^ -lcgicc

ln : txproj_server txproj_login txproj_list
	sudo ln -s $(PROJ_DIR)/demo/bootstrap-3.2.0-dist 	$(STATCI_DIR)/
	sudo ln -s $(PROJ_DIR)/txproj_*.html				$(STATCI_DIR)/
	sudo ln -s $(PROJ_DIR)/txproj_server				$(CGI_DIR)/cgi-bin/
	sudo ln -s $(PROJ_DIR)/txproj_login					$(CGI_DIR)/cgi-bin/
	sudo ln -s $(PROJ_DIR)/txproj_list					$(CGI_DIR)/cgi-bin/

test : everything
	./txproj_server &
	
clean : 
	sudo /bin/rm /var/www/txproj_*.html /var/www/bootstrap-3.2.0-dist
	sudo /bin/rm /usr/lib/cgi-bin/txproj_*
	/bin/rm $(find txproj_* -maxdepth 1 -perm 755 -type f)

reset :
	clean
	everything
	ln
	test
