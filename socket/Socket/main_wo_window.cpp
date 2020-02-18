//	Федорчук Р.Б.
//--------------------------------------
// Программа для обмена сообщениями через сокет
//--------------------------------------
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "klovr.h"
#include "xstay.h"
#include "interface.h"
#include "common.h"
#include "mat.hpp"
#include "singleton.h"
#include <stdlib.h>
#include "socketcl.h"
#include "serveraddr.h"
using namespace std;
using std::string;

bool GetJsonIn (const char *in_filename, char* in_json);
void SaveJsonOut (const char *out_filename, char* out_json, int code);
const int MAXMESSAGE = 1024; //#define DEFAULT_BUFLEN 1024 in socketcl.h

USETOOLS;USESHELL;USETECH;USECNG; 

ASOPDMAIN("Обмін через сокет.");

int main (int argc, char** argv)
{
	char jsonOut[MAXMESSAGE];
	char jsonIn[MAXMESSAGE];
	jsonOut[0] = '\0';
	jsonIn[0] = '\0';
	int result = 1;
	string srvrIP = "10.0.8.101";
	string srvrPort = "9000";

	INITTOOLS();INITSHELL();INITTECH();INITCNG();
	if(argc == 3) {
		string fileNameIn = argv[1]; //файл с входящим сообщением
		string fileNameOut = argv[2]; //файл для выходящего сообщения
		StackReset();
		SetDateDelim ('.');
		Initiate();
		//создание экземпляра класса глобальных переменных
		Singleton &glb = Singleton::getInstance();
		OpenCreate(B_SvrAdr, RDWR|ANRDWR);
		if(Size(B_SvrAdr)) {
			SetBegin(B_SvrAdr);
			GetNext(B_SvrAdr);
			srvrIP = J_SRVIP;
			srvrPort = J_SRVPORT;
		}
		else {
			StrForm(J_SRVIP, 15, srvrIP.c_str());
			StrForm(J_SRVPORT, 5, srvrPort.c_str());
			Put(B_SvrAdr);
		}

		if(!StartProcSet(&glb.insCode,NULL,glb.insFio,NULL))
		{
			glb.rayon = 2608;
			glb.uzel = 2608;
		}
		else
		{
			glb.rayon = fGetTech ("район");
			glb.uzel = fGetTech ("узел");
		}

		char recBuf[MAXMESSAGE];
		char errorMsg[100];
		errorMsg[0] = '\0';

		if(GetJsonIn(fileNameIn.c_str(), jsonIn)) {

			Socket *Soc = new Socket(srvrIP, srvrPort);
			Soc->MakeSocket();
			Soc->SendData(jsonIn);
			Sleep(1000);
			Soc->ShutdownSocket();
			Soc->ReceiveData();
			if(!Soc->IsError()) {
				StrForm(recBuf, MAXMESSAGE, Soc->recvbufAll);
				result = 0;
				SaveJsonOut(fileNameOut.c_str(), recBuf, 0);
			}
			else {
				StrForm(errorMsg, 100, Soc->errMsg);
				SaveJsonOut(fileNameOut.c_str(), errorMsg, -1);
			}
			delete Soc;			
		}
		else {
			StrForm(errorMsg, 100, "Input File Error");
			SaveJsonOut(fileNameOut.c_str(), errorMsg, -1);
		}
	}
	if(B_SvrAdr->bs)
		Close(B_SvrAdr);
	Terminate();
	TERMTOOLS();TERMSHELL();TERMTECH();TERMCNG();
	return result; 
}

bool GetJsonIn (const char *in_filename, char* in_json) {
	bool result = false;
	char buf[MAXMESSAGE];
	string inMessage = "";
	inMessage.clear();
	StayFile fJsonIn;
	fJsonIn = FOpen (in_filename, RD|ANRD); 
	if (fJsonIn) {
		FSeek (fJsonIn, 0);
		while(FReadText (fJsonIn, buf, MAXMESSAGE) > 0) {
			inMessage.append(buf);
		}
		StrForm (in_json, MAXMESSAGE, "%s\r\n\0", inMessage.c_str());
		FClose (fJsonIn);
		result = true;
	}
	return result;
}

void SaveJsonOut (const char *out_filename, char* out_json, int code) {
	StayFile fJsonOut;
	int len = 0;
	int lenAns = 2;
	if(code < 0 || code > 9)
		lenAns = 3;
	char ansCode[4];
	ansCode[0] = '\0';
	StrForm(ansCode, lenAns, "%d|", code);
    fJsonOut=FCreate(out_filename, RDWR);
	if(fJsonOut) {
		len = StrLen(ansCode);
		FWrite(fJsonOut,ansCode,len); 
		len = StrLen(out_json);
		FWrite(fJsonOut,out_json,len); 
		if(code < 0)
			FWrite(fJsonOut,"|",1); 
	}
    FClose(fJsonOut);
}