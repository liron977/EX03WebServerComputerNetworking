#include "requestUtils.h"

string divideRequest(string& str, string param)
{
	string value;
	int findFirstOoccurrence ;

	findFirstOoccurrence = str.find(param);
	if (findFirstOoccurrence != string::npos) {
		value = str.substr(0, findFirstOoccurrence);
		str.erase(0, value.size() + param.size());
	}
	else{
	  throw exception("syntax wrong");
	}
		
	return value;
}

bool isCharDisplayInStr(string str,char param) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == param) {
			return true;
		}
	}
	return false;
}
string getResource(string& str) {
	if (isCharDisplayInStr(str,'?')) {
		return divideRequest(str, "?");
	}
	else {
		return divideRequest(str, " ");
	}
}
string getRequestType(string& str) {
	return divideRequest(str, " /");
}
string getLengParmeter(string& str) {
	 divideRequest(str, "=");
	return divideRequest(str, " ");
}
string getHttpVersion(string& str) {
	return divideRequest(str, "\r\n");
}
string getRequestBody(string& str) {
	return divideRequest(str, "\r\n\r\n");
}
string removeSpaceChar(string& str) {
	return divideRequest(str, " ");
}
string getFileName(string& str) {
	return divideRequest(str, " ");
}
void removeCharsFromRequest(string& str, string param) {
	divideRequest(str, param);
}
string getHttpVersionForGetAndHeadRequest(string& str) {
	//divideRequest(str, " ");
	return divideRequest(str, "\r");
}

