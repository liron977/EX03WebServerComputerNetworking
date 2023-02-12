#include "requestUtils.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

string createResponseForTimeout();
string getCurrentTime();
string getLastModified(string fileName);
string getNowTime();
string getFileData(string path);
string createResponseForGetOrHeadRequest(bool isResoursFound,string httpVersion, string fileName, string requestType);
string createResponseForPostRequest(string httpVersion);
string createResponseForOptionsrequest(bool isResorceIsValid, string httpVersion);
string createResponseForTrace(string httpcVersion, string request);
string badRrequestForPutRequest(string httpVersion);
string successResponseForRequest(string httpVersion, string content, string status);
string createResponseForPutRequest(string httpVersion, string requestBody, string path);
bool isFileOpen(string path);
string badRrequestForDeleteRequest(string httpVersion);
string createResponseForDeleteRequest(string httpVersion, string path);
string createUnsopertedResponse();
