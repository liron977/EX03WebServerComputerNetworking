#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "requestUtils.h"
#include "Response.h"

#include "windows.h"


struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	int sendSubType;	// Sending sub-type
	char buffer[1000];
	int len;
	time_t timer = 0;
	unsigned int beginCountTimeout;
};

const int TIME_PORT = 8080;
const int MAX_SOCKETS = 60;
const int EMPTY = 0;
#define IDLE 1
#define SEND 2
#define LISTEN 1
#define RECEIVE 2
const int SEND_TIME = 1;
const int SEND_SECONDS = 2;

bool addSocket(SOCKET id, int what);
void removeSocket(int index);
void acceptConnection(int index);
void receiveMessage(int index);
void sendMessage(int index);
bool isLangParmeterIsValid(string langParmeter);
bool isResourseIsValid(string resource);
string getAndHeadRequest(string& msg,string requestType);
void handleWaitingRecv(fd_set& waitRecv, int& nfd);
void handleWaitingSend(fd_set& waitSend, int& nfd);
string optionRequest(string request);
string postRequst(string request);
string traceRequest(string request, string originalRequest);
string putRequest(string request);
string deleteRequest(string request);
void checkTimeouts();


struct SocketState sockets[MAX_SOCKETS] = { 0 };
int socketsCount = 0;
