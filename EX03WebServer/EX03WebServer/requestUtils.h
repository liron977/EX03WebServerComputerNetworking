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

 void removeCharsFromRequest(string& str, string param);
 string getRequestType(string& request);
 string divideRequest(string& str, string param);
 string getResource(string& str);
 string getRequestType(string& str);
 string getLengParmeter(string& str);
 string getHttpVersion(string& str);
 string getRequestBody(string& str);
 string removeSpaceChar(string& str);
 string getFileName(string& str);
 string getHttpVersionForGetAndHeadRequest(string& str);
 bool isCharDisplayInStr(string str, char param);