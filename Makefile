CC = g++
CFLAGS = -g -std=c++17 -Wall -Werror -pthread
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SERVER_PATH = src/server
CLIENT_PATH = src/client

rebuild: clean all

all: server client


# ---- CLIENT ---- #

client: cmain
	$(CC) $(CFLAGS) $(OBJDIR)/cmain.o $(CLIENT_PATH)/Client.cpp -o $(BINDIR)/twmailer-client


cmain: $(CLIENT_PATH)/main.cpp
	$(CC) $(CFLAGS) -c $(CLIENT_PATH)/main.cpp -o $(OBJDIR)/cmain.o


# ---- SERVER ---- #

server: smain Server ClientHandler Request
	$(CC) $(CFLAGS) -o $(BINDIR)/twmailer-server $(OBJDIR)/smain.o $(OBJDIR)/Server.o $(OBJDIR)/ClientHandler.o $(OBJDIR)/Request.o

Server:
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/Server.cpp -o $(OBJDIR)/Server.o

ClientHandler:
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/ClientHandler.cpp -o $(OBJDIR)/ClientHandler.o

Request:
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/Request.cpp -o $(OBJDIR)/Request.o

smain: $(SERVER_PATH)/main.cpp
	$(CC) $(CFLAGS) -c $(SERVER_PATH)/main.cpp -o $(OBJDIR)/smain.o


# ---- CLEAN ---- #

clean:
	clear
	rm -f bin/* obj/*
