CC = g++
CFLAGS = -g -std=c++17 -Wall -Werror -I /usr/local/include/gtest/ 
LIBS = -lldap -llber -pthread
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SERVER_PATH = src/server
CLIENT_PATH = src/client

rebuild: clean all

all: server client

client: ./bin/twmailer-client

server: ./bin/twmailer-server

# ---- CLIENT ---- #

./bin/twmailer-client: ./obj/cmain.o
	$(CC) $(CFLAGS) $(OBJDIR)/cmain.o $(CLIENT_PATH)/Client.cpp -o $(BINDIR)/twmailer-client


./obj/cmain.o: $(CLIENT_PATH)/main.cpp
	$(CC) $(CFLAGS) -c $(CLIENT_PATH)/main.cpp -o $(OBJDIR)/cmain.o


# ---- SERVER ---- #

./bin/twmailer-server: ./obj/smain.o ./obj/Server.o ./obj/ClientHandler.o ./obj/Request.o ./obj/Ldap.o
	$(CC) $(CFLAGS) -o $(BINDIR)/twmailer-server $(OBJDIR)/smain.o $(OBJDIR)/Server.o $(OBJDIR)/ClientHandler.o $(OBJDIR)/Request.o $(OBJDIR)/Ldap.o $(LIBS)

./obj/Server.o: $(SERVER_PATH)/Server.cpp
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/Server.cpp -o $(OBJDIR)/Server.o

./obj/ClientHandler.o: $(SERVER_PATH)/ClientHandler.cpp
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/ClientHandler.cpp -o $(OBJDIR)/ClientHandler.o

./obj/Request.o: $(SERVER_PATH)/Request.cpp
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/Request.cpp -o $(OBJDIR)/Request.o

./obj/smain.o: $(SERVER_PATH)/main.cpp
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/main.cpp -o $(OBJDIR)/smain.o

./obj/Ldap.o:
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/Ldap.cpp -o $(OBJDIR)/Ldap.o $(LIBS)


# ---- CLEAN ---- #

clean:
	clear
	rm -f bin/* obj/*
