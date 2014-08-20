
PROJ_DIR	:= /home/wws/desktop/tencent-train/
CGI_DIR 	:= /usr/lib/cgi-bin/
STATIC_DIR	:= /var/www/
SRC_DIR		:= $(PROJ_DIR)/src/
BIN_DIR		:= $(PROJ_DIR)/bin/

COMMON_O	:= $(BIN_DIR)/common.o


.PHONY : all everything clean test reset

all : 
	test

everything : $(BIN_DIR)txproj_server $(BIN_DIR)txproj_login $(BIN_DIR)txproj_list $(BIN_DIR)txproj_pay $(BIN_DIR)txproj_refund

$(BIN_DIR)txproj_server : $(SRC_DIR)server/txproj_server.c
	gcc -g -o $@ $^ -lmysqlclient

$(COMMON_O) : $(SRC_DIR)common/common.h $(SRC_DIR)common/common.cpp
	g++ g -o $@  -c $^ -lcgicc

$(BIN_DIR)txproj_login : txproj_login.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc

$(BIN_DIR)txproj_list : txproj_list.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc

$(BIN_DIR)txproj_pay : txproj_pay.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc

$(BIN_DIR)txproj_refund : txproj_refund.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc

ln : txproj_server txproj_login txproj_list
	[ -e $(STATIC_DIR)bootstrap-3.2.0-dist 		] && sudo /bin/rm 	$(STATIC_DIR)bootstrap-3.2.0-dist 		
    [ -e $(STATIC_DIR)txproj_index.html			] && sudo /bin/rm 	$(STATIC_DIR)txproj_index.html			
    [ -e $(STATIC_DIR)txproj_pay.html			] && sudo /bin/rm 	$(STATIC_DIR)txproj_pay.html			
    [ -e $(STATIC_DIR)txproj_refund.html		] && sudo /bin/rm 	$(STATIC_DIR)txproj_refund.html		
    [ -e $(CGI_DIR)txproj_server				] && sudo /bin/rm 	$(CGI_DIR)txproj_server				
    [ -e $(CGI_DIR)txproj_login					] && sudo /bin/rm 	$(CGI_DIR)txproj_login					
    [ -e $(CGI_DIR)txproj_list					] && sudo /bin/rm 	$(CGI_DIR)txproj_list					
    [ -e $(CGI_DIR)txproj_pay					] && sudo /bin/rm 	$(CGI_DIR)txproj_pay					
    [ -e $(CGI_DIR)txproj_refund				] && sudo /bin/rm 	$(CGI_DIR)txproj_refund				
	sudo ln -s $(SRC_DIR)demo/bootstrap-3.2.0-dist 		$(STATIC_DIR)
	sudo ln -s $(SRC_DIR)html/txproj_index.html			$(STATIC_DIR)
	sudo ln -s $(SRC_DIR)html/txproj_pay.html			$(STATIC_DIR)
	sudo ln -s $(SRC_DIR)html/txproj_refund.html		$(STATIC_DIR)
	sudo ln -s $(BIN_DIR)txproj_server					$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_login					$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_list					$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_pay						$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_refund					$(CGI_DIR)

test : everything
	./txproj_server &
	
clean : 
	sudo /bin/rm $(STATIC_DIR)txproj_*.html $(STATIC_DIR)bootstrap-3.2.0-dist
	sudo /bin/rm $(CGI_DIR)txproj_*
	find bin/ txproj_* -maxdepth 1 -perm 755 -type f -exec /bin/rm {} \;

reset :
	clean
	everything
	ln
	test
