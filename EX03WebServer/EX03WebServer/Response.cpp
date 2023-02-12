#include "Response.h"

string createResponseForTimeout() {
	string response;

	response =
		"HTTP/1.1 408 Request Timeout\r\n"
		"Date: " + getCurrentTime() + "\r\n"
		"Server: WebServer\r\n"
		"Connection: close\r\n\r\n\n";

	return response;
}

string getCurrentTime()
{
	time_t timer;
	time(&timer);
	struct tm  tstruct;
	tstruct = *localtime(&timer);
	char curTime[1000];
	strftime(curTime, sizeof(curTime), "%a, %d %b %Y %X GMT", &tstruct);
	string curTimeStr = curTime;
	return curTimeStr;


}




string createResponseForPostRequest(string httpVersion)
{
	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1")) || (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response = httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}
	response +=" 200 OK\r\n"
		"Server: Webserver\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + getCurrentTime() + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 38\r\n\r\n"
		"The POST request successfully received\n";

	return response;
}
string createResponseForGetOrHeadRequest(bool isResoursFound,string httpVersion, string fileName, string requestType) {

	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1"))|| (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response= httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}
	

	if (!isResoursFound) {
		response += " 404 Not Found\r\n";
	}
	else{
		response +=" 200 OK\r\n";
	}
	string data= getFileData(fileName);
    response+= "Date: " + getNowTime() + "\r\n"
		"Server: WebServer\r\n"
		"Last-Modified: " + getLastModified(fileName) + "\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " +to_string(data.length()) + "\r\n\r\n";
	if (!strcmp(requestType.c_str(),"HEAD")) {
	   return response;
	}
	return response + data;

}
string getFileData(string path)
{
	string data = "";

	ifstream file(path);
	if (file.is_open()) {
		stringstream inputStream;
		inputStream << file.rdbuf();
		data = inputStream.str();
		file.close();
	}
	return data;
}
bool isFileOpen(string path) {
	bool isOpen = false;

	ifstream file(path);
	if (file.is_open()) {
		isOpen = true;
		file.close();
	}

	return isOpen;
}
string getLastModified(string fileName)
{
	struct stat result;
	struct tm time;
	char lastModifed[100];

	if (stat(fileName.c_str(), &result) == 0)
	{
		time = *localtime(&result.st_mtime);
		char lastModifiedDate[100];
		strftime(lastModifed, sizeof(lastModifed), "%a, %d %b %Y %X GMT", &time);	
	}
	return lastModifed;
}
string createResponseForOptionsrequest(bool isResorceIsValid,string httpVersion) {
	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1")) || (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response = httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}
	if (!isResorceIsValid) {
		response += " 404 Not Found\r\n";
     }
	else{
		response += " 200 OK\r\n";
	}
	response+="Date: "+getCurrentTime() + "\r\nServer: WebServer\r\nConnection: Keep-Alive\r\n"
			"Content-Type: text/html\r\nContent-Length: 45\r\n"
			"Allow: HEAD, GET, POST, PUT, DELETE, OPTIONS, TRACE\r\n\r\n"
			"Allow: GET HEAD POST PUT DELETE TRACE OPTIONS\n";
	
	return response;
}
string createResponseForTrace(string httpVersion,string request) {
	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1")) || (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response = httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}

	response+=" 200 OK\r\nDate: " + getCurrentTime() + "\r\nServer: WebServer\r\nConnection: Keep - Alive\r\n"
		"Content-Type: message/http\r\n"
		"Content-Length: " + to_string(request.length() + 1) + "\r\n\r\n"
		+ request + "\r\n";

	return response;
}
string createResponseForPutRequest(string httpVersion, string requestBody,string path) {
	string response, fileData;
	ofstream file;
	bool isFileAlreadyOpen;

	isFileAlreadyOpen = isFileOpen(path);
	if (isFileAlreadyOpen) {
		response = badRrequestForPutRequest(httpVersion);
	}
	else {
		response = successResponseForRequest(httpVersion,"File was successfully created.","201 Created");
	}
	file.open(path);
	file.write(requestBody.c_str(),requestBody.length());
	file.close();

	return response;
}
string badRrequestForPutRequest(string httpVersion) {
	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1")) || (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response = httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}

	response+=" 204 No Content\r\n"
		"Date: " + getCurrentTime() + "\r\n"
		"Connection: Keep-Alive\r\n"
		"Server: WebServer\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 0\r\n\r\n\n";

	return response;
}
string successResponseForRequest(string httpVersion,string content,string status) {
	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1")) || (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response = httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}
		
	response+= " "+status+"\r\n"
		"Date: " + getCurrentTime() + "\r\n"
		"Connection: Keep-Alive\r\n"
		"Server: WebServer\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + to_string(content.length()) + "\r\n\r\n"
		+ content+ "\n";

	return response;
}

string createResponseForDeleteRequest(string httpVersion, string path) {
	string response, fileData;
	ofstream file;
	bool isFileAlreadyOpen;

	isFileAlreadyOpen = isFileOpen(path);
	if (isFileAlreadyOpen) {
		remove(path.c_str());
		response = successResponseForRequest(httpVersion, "File was successfully deleted.","200 OK");	
	}
	else {
		response = badRrequestForDeleteRequest(httpVersion);
	}

	return response;
}
string badRrequestForDeleteRequest(string httpVersion) {
	string response;
	if ((!strcmp(httpVersion.c_str(), "HTTP/1.1")) || (!strcmp(httpVersion.c_str(), "HTTP/1.0"))) {
		response = httpVersion;
	}
	else {
		response = "HTTP/1.1";
	}

	response += " 400 Bad Request\r\n"
		"Date: " + getCurrentTime() + "\r\n"
		"Connection: Keep-Alive\r\n"
		"Server: WebServer\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 23\r\n\r\n"
		"The file did not found.\n";

	return response;
}
string createUnsopertedResponse()
{
	string HTTPAppLayer = "HTTP/1.1 405 Method Not Allowed\r\n"
		"Server: Webserver\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + getCurrentTime() + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 85\r\n"
		"Allow: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\r\n\r\n"
		"Unsupported method,the allowed methods: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE.\n";

	return HTTPAppLayer;
}
string getNowTime() {
	char buf[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S GMT", &tm);
	return buf;
}