all:
	g++  App.cpp   Iscs.cpp TrapCallBack.cpp  hwzx.cpp  RecvSnmp.cpp  UdpDeal.cpp  Socket.cpp DataSql.cpp Node.cpp Nodes.cpp   Zmq.cpp  Conf.cpp Song.cpp -lzmq  -lsnmp++ -lmysqlclient -lmodbus -std=c++11 -I../include  -I /usr/include/mysql/   -I /usr/local/include/modbus/  -L /usr/local/lib/  -D__UDPDEAL_ -D__SNMPDEAL_ -D__ISCS__   -o ../bin/App
clean:
	rm  -f ../bin/*
