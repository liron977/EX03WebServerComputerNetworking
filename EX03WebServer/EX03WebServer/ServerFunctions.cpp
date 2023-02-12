#include "ServerFunctions.h"

void main() {
	WSAData wsaData;
	

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	if (INVALID_SOCKET == listenSocket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	sockaddr_in serverService;

	serverService.sin_family = AF_INET;

	serverService.sin_addr.s_addr = INADDR_ANY;

	serverService.sin_port = htons(TIME_PORT);

	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	addSocket(listenSocket, LISTEN);

	while (true)
	{
		time_t timeNow;
		time(&timeNow);
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			cout << "Time Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		checkTimeouts();

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(i);
					break;

				case RECEIVE:
					receiveMessage(i);
					break;
				}
			}
		}
		//checkTimeouts();
		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				switch (sockets[i].send)
				{
				case SEND:
					sendMessage(i);
					break;
				}
			}
		}
	}


	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
}
bool addSocket(SOCKET id, int what)
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].len = 0;
			socketsCount++;
			sockets[i].beginCountTimeout = (unsigned int)GetTickCount();

			return (true);
		}
	}
	return (false);
}

void removeSocket(int index)
{
	sockets[index].recv = EMPTY;
	sockets[index].send = EMPTY;
	socketsCount--;
	sockets[index].timer = 0;
	sockets[index].beginCountTimeout = 0;
	sockets[index].id = 0;
}

void acceptConnection(int index)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Time Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

	//
	// Set the socket to be in non-blocking mode.
	//
	unsigned long flag = 1;
	if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
	{
		cout << "Time Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	}

	if (addSocket(msgSocket, RECEIVE) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void receiveMessage(int index)
{
	SOCKET msgSocket = sockets[index].id;
	sockets[index].beginCountTimeout = (unsigned int)GetTickCount();
	int len = sockets[index].len;
	int bytesRecv = recv(msgSocket, &sockets[index].buffer[0], sizeof(sockets[index].buffer), 0);
	(&sockets[index].timer);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	else
	{
		sockets[index].buffer[bytesRecv] = '\0'; //add the null-terminating to make it a string
	//	cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets[index].buffer[len] << "\" message.\n";

		sockets[index].len += bytesRecv;
		sockets[index].send = SEND;
	}

}

void sendMessage(int index) {
	int bytesSent = 0;
	char sendBuff[1000];
	string msg, response, requestType;
	SOCKET msgSocket = sockets[index].id;
	msg = sockets[index].buffer;
	requestType = getRequestType(msg);

	if (!strcmp(requestType.c_str(), "OPTIONS")) {
		response = optionRequest(msg);
	}
	else if (!strcmp(requestType.c_str(), "GET")) {
		response = getAndHeadRequest(msg, "GET");
	}
	else if (!strcmp(requestType.c_str(), "HEAD")) {
		response = getAndHeadRequest(msg, "HEAD");
	}
	else if (!strcmp(requestType.c_str(), "POST")) {
		response = postRequst(msg);
	}
	else if (!strcmp(requestType.c_str(), "PUT")) {
		response = putRequest(msg);
	}
	else if (!strcmp(requestType.c_str(), "DELETE")) {
		response = deleteRequest(msg);
	}
	else if (!strcmp(requestType.c_str(), "TRACE")) {
		response = traceRequest(msg, sockets[index].buffer);
	}
	else {
		response = createUnsopertedResponse();
	}
	strcpy(sendBuff, response.c_str());
	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);

	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	cout<<"Time Server: Sent: "<<bytesSent<<"\\"<<strlen(sendBuff)<<" bytes of \""<<sendBuff<<"\" message.\n" << endl;
	//Sleep(3000);
	//closesocket(sockets[index].id);
	
	sockets[index].send = IDLE;
	sockets[index].beginCountTimeout = (unsigned int)GetTickCount();


}
string traceRequest(string request,string originalRequest) {
	string response, httpVersion;

	httpVersion = removeSpaceChar(request);
	httpVersion = getHttpVersion(request);

	return createResponseForTrace(httpVersion, originalRequest);
}
string optionRequest(string request) {
	string resource, langParmeter, httpVersion, path;
	bool isValid = false;
	resource = divideRequest(request, " ");
	if (resource.empty() || (isResourseIsValid(resource))) {
		isValid = true;
	
	}
	httpVersion = getHttpVersion(request);
	return createResponseForOptionsrequest(isValid, httpVersion);
}
bool isResourseIsValid(string resource) {

	if (!strcmp(resource.c_str(), "Website.html")) {
		return true;
	}
	else {
		return false;
	}
}
bool isLangParmeterIsValid(string langParmeter) {

	if (!strcmp(langParmeter.c_str(), "en") || (!strcmp(langParmeter.c_str(), "he"))
		|| (!strcmp(langParmeter.c_str(), "fr"))) {
		return true;
	}
	return false;
}
string getAndHeadRequest(string& msg, string requestType) {
	string resource, langParmeter, httpVersion, path;
	bool isValid = false;

	resource = getResource(msg);
	if (isResourseIsValid(resource)) {
		isValid = true;
		if (isCharDisplayInStr(msg, '=')) {
			langParmeter = getLengParmeter(msg);
			if (isLangParmeterIsValid(langParmeter)) {
				httpVersion = getHttpVersion(msg);
				resource = divideRequest(resource, ".html");
				path = "C:\\temp\\" + resource + "_" + langParmeter + ".html";
			}
			else {
				isValid = false;
				httpVersion = getHttpVersionForGetAndHeadRequest(msg);
				path = "C:\\temp\\Error404.html";
			}
		}
		else {
			path = "C:\\temp\\Website_en.html";
			httpVersion = getHttpVersionForGetAndHeadRequest(msg);
		}
	}
	else {
		isValid = false;
		removeSpaceChar(msg);
		httpVersion = getHttpVersionForGetAndHeadRequest(msg);
		path = "C:\\temp\\Error404.html";
	}
	return createResponseForGetOrHeadRequest(isValid, httpVersion, path, requestType);
}
string postRequst(string request) {
	string requestBody, response, httpVersion;

	httpVersion = removeSpaceChar(request);
	httpVersion = getHttpVersion(request);
	requestBody = getRequestBody(request);
	requestBody = request;
	cout << "The text in the POST request was: " << requestBody << endl;
	response = createResponseForPostRequest(httpVersion);

	return response;
}
string putRequest(string request) {
	string fileName,path, response, httpVersion,requestBody;

	fileName = getFileName(request);
	path = "C:\\temp\\" + fileName;
	httpVersion = divideRequest(request,"\r\n");
	divideRequest(request, "\r\n");
	getRequestBody(request);
	requestBody = request;
	response=createResponseForPutRequest(httpVersion, requestBody,path);
	
	
	return response;
}
string deleteRequest(string request) {
	string fileName, path, response, httpVersion, requestBody;

	fileName = getFileName(request);
	path = "C:\\temp\\" + fileName;
	httpVersion = divideRequest(request, "\r\n");
	divideRequest(request, "\r\n");
	response = createResponseForDeleteRequest(httpVersion, path);


	return response;
}
void checkTimeouts()
{

	time_t timeNow;
	time(&timeNow);
	string response;
	char sendBuff[1000];
	int bytesSent = 0;
	unsigned int stopTimeout = 0;
	


	for (int i = 1; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].send != EMPTY || sockets[i].recv != EMPTY)
		{
			stopTimeout = (unsigned int)GetTickCount();
			if (stopTimeout - sockets[i].beginCountTimeout > 120000) {
					cout << "Server : one of the connections closed,in socket : "<< sockets[i].id <<"\n" << endl;


					SOCKET msgSocket = sockets[i].id;
					string msg = sockets[i].buffer;
		
					response = createResponseForTimeout();
					strcpy(sendBuff, response.c_str());
					sendBuff[response.length() - 1] = 0;
					bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff),0);
					//cout<<"Time Server: Sent: "<<bytesSent<<"\\"<<strlen(sendBuff)<<" bytes of \""<<sendBuff<<"\" message.\n";	

					if (SOCKET_ERROR == bytesSent)
					{
						cout << "Server: Error at send(): " << WSAGetLastError() << endl;
						return;//todo
					}
					Sleep(1000);
				
					closesocket(sockets[i].id);
					removeSocket(i);
			}
			stopTimeout = 0;
		}
	}
}