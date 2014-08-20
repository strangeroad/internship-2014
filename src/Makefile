
PROJ_DIR	:= /home/wws/desktop/tencent-train/
CGI_DIR 	:= /usr/lib/cgi-bin/
STATIC_DIR	:= /var/www/
SRC_DIR		:= $(PROJ_DIR)src/
BIN_DIR		:= $(SRC_DIR)bin/
COMMON_O	:= $(BIN_DIR)common.o


.PHONY : all everything clean test reset sql run

all : everything cleanweb ln run

everything : $(BIN_DIR)txproj_server $(BIN_DIR)txproj_login $(BIN_DIR)txproj_list $(BIN_DIR)txproj_pay $(BIN_DIR)txproj_refund

$(BIN_DIR)txproj_server : $(SRC_DIR)server/txproj_server.c
	[ -e $(BIN_DIR) ] || mkdir $(BIN_DIR)
	gcc -g -o $@ $^ -lmysqlclient

$(COMMON_O) : $(SRC_DIR)common/common.cpp $(SRC_DIR)common/common.h
	g++ -g -o $@ -c $< -lcgicc

$(BIN_DIR)txproj_login : $(SRC_DIR)cgi/txproj_login.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc -I$(SRC_DIR)common/

$(BIN_DIR)txproj_list : $(SRC_DIR)cgi/txproj_list.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc -I$(SRC_DIR)common/

$(BIN_DIR)txproj_pay : $(SRC_DIR)cgi/txproj_pay.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc -I$(SRC_DIR)common/

$(BIN_DIR)txproj_refund : $(SRC_DIR)cgi/txproj_refund.cpp $(COMMON_O)
	g++ -g -o $@ $^ -lcgicc -I$(SRC_DIR)common/

ln : everything   $(SRC_DIR)html/txproj_index.html   $(SRC_DIR)html/txproj_buyer.html   $(SRC_DIR)html/txproj_seller.html  
	sudo ln -s $(SRC_DIR)demo/html/bootstrap-3.2.0-dist	$(STATIC_DIR)
	sudo ln -s $(SRC_DIR)html/txproj_index.html			$(STATIC_DIR)
	sudo ln -s $(SRC_DIR)html/txproj_buyer.html			$(STATIC_DIR)
	sudo ln -s $(SRC_DIR)html/txproj_seller.html		$(STATIC_DIR)
	sudo ln -s $(BIN_DIR)txproj_server					$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_login					$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_list					$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_pay						$(CGI_DIR)
	sudo ln -s $(BIN_DIR)txproj_refund					$(CGI_DIR)

run : everything
	./bin/txproj_server

cleanweb :
	sudo /bin/rm $(CGI_DIR)txproj_* $(STATIC_DIR)txproj_*.html $(STATIC_DIR)bootstrap-3.2.0-dist

clean : cleanweb
	find bin/ -maxdepth 1 -perm 755 -type f -name txproj_* -exec /bin/rm {} \;
	/bin/rm $(COMMON_O)

sql : 
	mysql -upublic -ppublic public0 < init.sql

reset :
	clean
	everything
	ln
	test
