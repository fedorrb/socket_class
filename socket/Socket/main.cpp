//	Федорчук Р.Б.
//--------------------------------------
// Программа для обмена сообщениями через сокет
// формат вхідних повідомлень КОД|{"ПОЛЕ_1":"ЗНАЧЕННЯ","ПОЛЕ_2":"ЗНАЧЕННЯ",...,"ПОЛЕ_Н":"ЗНАЧЕННЯ"}
//--------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <exception>
#include "klovr.h"
#include "xstay.h"
#include "interface.h"
#include "common.h"
#include "mat.hpp"
#include "singleton.h"
#include <stdlib.h>
#include "socketcl.h"
#include "socketcl2.h"
#include "serveraddr.h"
#include "busyindc.h"//BUSY_FORM
using namespace std;
using std::string;
using std::vector;

vector<string> allFile;
vector<string> receivedLines;
StayEventProc BOSBusyForm; //BUSY_FORM
StayEventProc BOSWSetAddr;
bool GetJsonInAll(const char *in_filename, vector<string> & allFile);
void SaveJsonOut (const char *out_filename, char* out_json, int code);
void SaveJsonOutLines (const char *out_filename, int code, vector<string> & receivedLines);
int SaveLogFile (char* text, int direction);
int SaveLogFileAll (vector<string> & allFile, int direction);
void MoveFromLsToSok();
void DeleteSrvrAdr();
const int MAXMESSAGE = 131072; //#define DEFAULT_BUFLEN 1024 in socketcl.h
const unsigned long MAXLOGFILESIZE = 20000000; //20Mb
int GetNumPtk(vector<string> & allFile);
unsigned short GetSrvrDelay(int ptkCode);
//void SaveIniFile(vector<string> & lines);
//void SplitStringLine(const std::string& str, vector<string> & cont, const std::string& delims = "\\");

USETOOLS;USESHELL;USETECH;USECNG; 

ASOPDMAIN("Обмін через сокет.");

int main (int argc, char** argv)
{
	int result = 1;

	INITTOOLS();INITSHELL();INITTECH();INITCNG();
	//создание экземпляра класса глобальных переменных
	Singleton &glb = Singleton::getInstance();
	if(argc == 3) {
		StackReset();
		SetDateDelim ('.');
		Initiate();
		glb.pathAdmin = argv[0];
		glb.fileNameIn = argv[1]; //файл с входящим сообщением
		glb.fileNameOut = argv[2]; //файл для выходящего сообщения
		glb.vidpov = 1;

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

		Display (WndBusy, BOSBusyForm);//BUSY_FORM
		result = glb.vidpov;
	}
	else {
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
		glb.pathAdmin = argv[0];
		MoveFromLsToSok();
		Display (WndSetAddr, BOSWSetAddr);
	}
	Terminate();
	TERMTOOLS();TERMSHELL();TERMTECH();TERMCNG();
	return result; 
}

void MoveFromLsToSok() {
	if(!FFind("SOK", NULL))
		MakeDir("SOK");
	if(!FFind("SOK:SRVRADRS.DT", NULL))
	{
		if(FFind("LS:SRVRADRS.DT", NULL)) {
			FCopy("LS:SRVRADRS.DT","SOK:SRVRADRS.DT");
			FDelete("LS:SRVRADRS.DT");
		}
	}
	if(!FFind("SOK:SRVRADRS.BTV", NULL))
	{
		if(FFind("LS:SRVRADRS.BTV", NULL)) {
			FCopy("LS:SRVRADRS.BTV","SOK:SRVRADRS.BTV");
			FDelete("LS:SRVRADRS.BTV");
		}
	}
	if(!FFind("SOK:SOCKET_01.LOG", NULL))
	{
		if(FFind("LS:SOCKET_01.LOG", NULL)) {
			FCopy("LS:SOCKET_01.LOG","SOK:SOCKET_01.LOG");
			FDelete("LS:SOCKET_01.LOG");
		}
	}
}

void DeleteSrvrAdr() {
	if(FFind("LS:SRVRADRS.DT", NULL)) {
		FDelete("LS:SRVRADRS.DT");
	}
	if(FFind("LS:SRVRADRS.BTV", NULL)) {
		FDelete("LS:SRVRADRS.BTV");
	}
}
//возвращает содержимое входного файла в вектор
bool GetJsonInAll(const char *in_filename, vector<string> & allFile) {
	Singleton &glb = Singleton::getInstance();
	bool result = false;
	char buf[MAXMESSAGE];
	char bufNew[MAXMESSAGE];
	allFile.clear();
	StayFile fJsonIn;
	fJsonIn = FOpen (in_filename, RD|ANRD); 
	if (fJsonIn) {
		FSeek (fJsonIn, 0);
		while(FReadText (fJsonIn, buf, MAXMESSAGE) > 0) {
			StrForm(bufNew,26,"rajon:%d,insp:%d,code:", glb.rayon, glb.insCode);
			strcat(buf,"\r\n\1");
			strcat(bufNew, buf);
			allFile.push_back(bufNew);
		}
		//allFile.push_back("\r\n\1");
		FClose (fJsonIn);
		//StrForm (in_json, MAXMESSAGE, "%s", allFile[0].c_str());
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

void SaveJsonOutLines (const char *out_filename, int code, vector<string> & receivedLines) {
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
		int size_v = static_cast<int>(receivedLines.size());
		len = StrLen(ansCode);
		FWrite(fJsonOut,ansCode,len); 
		int i = 0;
		while(i < size_v) {
			FWrite(fJsonOut, receivedLines[i].c_str(), (int) strlen(receivedLines[i].c_str()));
			i++;
		}
		if(code < 0)
			FWrite(fJsonOut,"|",1); 
	}
    FClose(fJsonOut);
}

int SaveLogFile (char* text, int direction) {
	int result = 0;
	Singleton &glb = Singleton::getInstance();
	char buf[MAXMESSAGE];
	char buf_inout[4];
	char name_log_file[256];
	if(direction)
		StrCpy(buf_inout, "in");
	else
		StrCpy(buf_inout, "out");
	StayDate dtNow = GetSysDate();
	StayTime tmNow = GetSysTime();
	StrForm(buf, MAXMESSAGE, "\r\n%10v %5t %4u %3s %s", dtNow, tmNow, glb.rayon, buf_inout, text);
	StayFile logFile;
	int len = StrLen(buf);
	StrForm(name_log_file, 256, "SOK:socket%u.log", glb.insCode);
	try {
		if(FFind(name_log_file, NULL))
			logFile = FOpen(name_log_file, RDWR | ANRDWR);
		else
			logFile = FCreate(name_log_file, RDWR | ANRDWR);
	} catch (exception& e) {
		if(logFile)
			FClose(logFile);
		result = 1;
	}
	if(result)
		return result;
	if(logFile) {
		FSeek(logFile,FSize(logFile));
		FWrite(logFile, buf, len);
		FFlush(logFile);
		FClose(logFile);
	}
	return result;
}

int SaveLogFileAll (vector<string> & allFile, int direction) {
	int result = 0;
	Singleton &glb = Singleton::getInstance();
	char buf[MAXMESSAGE];
	char buf_inout[4];
	char name_log_file[256];
	if(direction)
		StrCpy(buf_inout, "in");
	else
		StrCpy(buf_inout, "out");
	StayDate dtNow = GetSysDate();
	StayTime tmNow = GetSysTime();
	int size_v = static_cast<int>(allFile.size());
	unsigned long fileSize = 0;
	StrForm(buf, MAXMESSAGE, "\r\n%10v %5t %4u %3s ", dtNow, tmNow, glb.rayon, buf_inout);
	//
	StayFile logFile;
	int len = StrLen(buf);
	StrForm(name_log_file, 256, "SOK:socket%u.log", glb.insCode);
	try {
		if(FFind(name_log_file, NULL)) {
			logFile = FOpen(name_log_file, RDWR | ANRD);
		} else {
			logFile = FCreate(name_log_file, RDWR | ANRD);
		}
	} catch (...){
		if(logFile)
			FClose(logFile);
		result = 1;
	}
	if(result)
		return result;
	//якщо лог файл більший за максимальний розмір ver.19
	fileSize = FSize(logFile);
	if(logFile && fileSize > MAXLOGFILESIZE) {
		FClose(logFile);
		char name2[L_tmpnam];
		if(std::tmpnam(name2)) {
			name2[0] = ':';
			std::string name1 = name2;
			std::string nameFull = "SOK" + name1 + "log";
			FCopy(name_log_file, nameFull.c_str());
			logFile = FCreate(name_log_file, RDWR | ANRD);
	    }
	}
	if(logFile) {
		FSeek(logFile,FSize(logFile));
		FWrite(logFile, buf, len);
		int i = 0;
		while(i < size_v && i < 50) {
			FWrite(logFile, allFile[i].c_str(), (int) strlen(allFile[i].c_str()));
			i++;
		}
		//for(int i = 0; i < size_v, i < 50; i++) {
			//FWrite(logFile, allFile[i].c_str(), (int) strlen(allFile[i].c_str()));
		//}
		if(size_v > 50) {
			StrForm(buf, MAXMESSAGE, "\r\nвсього надіслано (прийнято) %d строк", size_v);
			len = StrLen(buf);
			FWrite(logFile, buf, len);
		}

		FFlush(logFile);
		FClose(logFile);
	}
	return result;
}

//BUSY_FORM
int STAYPROC BOSBusyForm( StayEvent s, StayEvent id )
{
	char jsonOut[MAXMESSAGE];
	jsonOut[0] = '\0';
	string srvrIP = "10.0.5.155";
	string srvrPort = "1861";
	unsigned short srvrDL = 4; 
	unsigned short srvrAT = 10;
	//создание экземпляра класса глобальных переменных
	Singleton &glb = Singleton::getInstance();
	glb.socketType = 0;//тип сокета 23 - с задержками, 25 - без
	char errorMsg[100];
	errorMsg[0] = '\0';
	int fieldlen = 0;
	short ptkCode = 0;
	unsigned short pcPort = 1861;

	switch( s )
	{
		//case _Before:
	case _BeforeWindow:
		OpenCreate(B_SvrAdr, RDWR|ANRDWR);
		if(Size(B_SvrAdr)) {
			SetBegin(B_SvrAdr);
			GetNext(B_SvrAdr);
			srvrIP = J_SRVIP;
			srvrPort = J_SRVPORT;
			srvrDL = J_SRVDL;
			srvrAT = J_SRVAT;
			glb.socketType = J_SRVType;
			if(glb.socketType != 23 && glb.socketType != 25) {
				Close(B_SvrAdr);
				DeleteSrvrAdr();
				Create(B_SvrAdr, RDWR|ANRDWR);
				StrForm(J_SRVIP, 15, srvrIP.c_str());
				StrForm(J_SRVPORT, 5, srvrPort.c_str());
				J_SRVDL = srvrDL;
				J_SRVAT = srvrAT;
				J_SRVType = 23; //default
				glb.socketType = 23;
				Put(B_SvrAdr);
			}
		}
		else {
			StrForm(J_SRVIP, 15, srvrIP.c_str());
			StrForm(J_SRVPORT, 5, srvrPort.c_str());
			J_SRVDL = srvrDL;
			J_SRVAT = srvrAT;
			J_SRVType = 23; //default
			glb.socketType = 23;
			Put(B_SvrAdr);
		}

		if(GetJsonInAll(glb.fileNameIn.c_str(), allFile)) {
			for(std::vector<string>::iterator it = allFile.begin(); it != allFile.end(); ++it) {
				std::replace((*it).begin(), (*it).end(), '\\', '/');
			}
			Ldv(srvrPort.c_str());
			pcPort = Sti(0);
			receivedLines.clear();
			if(glb.socketType == 25) { //new
				Socket *Soc = new Socket(srvrIP, pcPort, srvrAT);
				Soc->NewSocket(allFile, receivedLines);
				SaveLogFileAll(allFile, 0);
				if(!Soc->IsError()) {
					glb.vidpov = 0;
					SaveJsonOutLines(glb.fileNameOut.c_str(), 0, receivedLines);
					SaveLogFileAll(receivedLines, 1);
				}
				else {
					StrForm(errorMsg, 100, Soc->errMsg);
					SaveJsonOut(glb.fileNameOut.c_str(), errorMsg, -1);
					SaveLogFile(errorMsg, 1);
				}
				delete Soc;
			} else { //old
				ptkCode = GetNumPtk(allFile);
				srvrDL = GetSrvrDelay(ptkCode);
				Socket23 *Soc = new Socket23(srvrIP, srvrPort, srvrAT);
				Soc->MakeSocket();
				if(Soc->SendData(allFile)) {
					Sleep(srvrDL * 1000);
					Soc->ShutdownSocket();	
					Soc->ReceiveData(receivedLines);
					SaveLogFileAll(allFile, 0);
				}
				else {
					Soc->ShutdownSocket();
					//todo closesocket(ConnectSocket);
				}
				if(!Soc->IsError()) {
					//StrForm(recBuf, MAXMESSAGE, Soc->recvbufAll);
					glb.vidpov = 0;
					SaveJsonOutLines(glb.fileNameOut.c_str(), 0, receivedLines);
					SaveLogFileAll(receivedLines, 1);
				}
				else {
					StrForm(errorMsg, 100, Soc->errMsg);
					SaveJsonOut(glb.fileNameOut.c_str(), errorMsg, -1);
					SaveLogFile(errorMsg, 1);
				}
				delete Soc;
			}
		}
		else {
			StrForm(errorMsg, 100, "Input File Error");
			SaveJsonOut(glb.fileNameOut.c_str(), errorMsg, -1);
			SaveLogFile(errorMsg, 0);
		}
		if(B_SvrAdr->bs)
			Close(B_SvrAdr);
		Exit(_Ok);
		break;
	}
	return 0;
}

int STAYPROC BOSWSetAddr( StayEvent s, StayEvent id )
{
	Singleton &glb = Singleton::getInstance();
	string srvrIP = "10.0.5.155";
	string srvrPort = "1861";
	char buf[MAXMESSAGE];
	unsigned short srvrDL = 4; 
	unsigned short srvrAT = 10;
	char recBuf[MAXMESSAGE];
	char errorMsg[100];
	errorMsg[0] = '\0';	
	char buf1[64];
	memset(buf1, 0, sizeof(buf1));
	StrForm(buf1,64,"rajon:%d,insp:%d,code:999|[{\"TEST\":\"TEST\"}]\r\n\1", glb.rayon, glb.insCode);
	//string strJsonIn = "999|[{\"TEST\":\"TEST\"}]\r\n\1";
	string strJsonIn = buf1;
	string strJsonOk = "{\"TEST\":\"OK\"}";
	int loc = 0;
	vector<string> textToSend;
	//vector<string> textIni;
	unsigned short pcPort = 1861;
	string tmpStr;
	stringstream ss;

	switch( s )
	{
	case _BeforeWindow:
		OpenCreate(B_SvrAdr, RDWR|ANRDWR);
		if(Size(B_SvrAdr)) {
			SetBegin(B_SvrAdr);
			GetNext(B_SvrAdr);
			srvrIP = J_SRVIP;
			srvrPort = J_SRVPORT;
			srvrDL = J_SRVDL;
			srvrAT = J_SRVAT;
			glb.socketType = J_SRVType;
			if(glb.socketType != 23 && glb.socketType != 25) {
				Close(B_SvrAdr);
				DeleteSrvrAdr();
				Create(B_SvrAdr, RDWR|ANRDWR);
				StrForm(J_SRVIP, 15, srvrIP.c_str());
				StrForm(J_SRVPORT, 5, srvrPort.c_str());
				J_SRVDL = srvrDL;
				J_SRVAT = srvrAT;
				J_SRVType = 23; //default
				glb.socketType = 23;
				Put(B_SvrAdr);
			}
		}
		else {
			Close(B_SvrAdr);
			DeleteSrvrAdr();
			Create(B_SvrAdr, RDWR|ANRDWR);
			StrForm(J_SRVIP, 15, srvrIP.c_str());
			StrForm(J_SRVPORT, 5, srvrPort.c_str());
			J_SRVDL = 2;
			J_SRVAT = 10;
			J_SRVType = 23; //default
			Put(B_SvrAdr);
		}
		if(glb.socketType == 23) {
			SetBit( &TypeMask, 0, 1 );
			SetBit( &TypeMask, 1, 0 );
			//SetField( WndSetAddr, RBUT1 );
		} else {
			SetBit( &TypeMask, 0, 0 );
			SetBit( &TypeMask, 1, 1 );
			//SetField( WndSetAddr, RBUT2 );
		}
		ShowWnd( NULL );
		break;
	case _Enter:
	case BUT1:
		if(GetBit(&TypeMask, 0)) {
			J_SRVType = 23;
			glb.socketType = 23;
		} else {
			J_SRVType = 25;
			glb.socketType = 25;
		}
		Modify(B_SvrAdr);
		/*
		textIni.clear();
		textIni.push_back(J_SRVIP);
		textIni.push_back(J_SRVPORT);
		ss << glb.rayon;
		tmpStr = ss.str();
		textIni.push_back(tmpStr);
		tmpStr.clear();
		ss.clear();
		ss.str(std::string());
		ss << glb.insCode;
		tmpStr = ss.str();
		textIni.push_back(tmpStr);
		SaveIniFile(textIni);
		*/
		if(B_SvrAdr->bs)
			Close(B_SvrAdr);
		StrForm(buf, MAXMESSAGE, "%s %s %D %D", J_SRVIP, J_SRVPORT, _J_SRVDL, _J_SRVAT);
		SaveLogFile(buf, 0);
		Exit(_Ok);
		break;
	case BUT2:
		if(GetBit(&TypeMask, 0)) {
			J_SRVType = 23;
			glb.socketType = 23;
		} else {
			J_SRVType = 25;
			glb.socketType = 25;
		}
		Modify(B_SvrAdr);
		receivedLines.clear();
		if(Size(B_SvrAdr)) {
			textToSend.clear();
			receivedLines.clear();
			SetBegin(B_SvrAdr);
			GetNext(B_SvrAdr);
			srvrIP = J_SRVIP;
			srvrPort = J_SRVPORT;
			srvrDL = J_SRVDL;
			srvrAT = J_SRVAT;
			textToSend.push_back(strJsonIn);
			srvrDL = 5;
			Ldv(srvrPort.c_str()); pcPort = Sti(0);
			if(glb.socketType == 25) {
				Socket *Soc = new Socket(srvrIP, pcPort, srvrAT);
				Soc->NewSocket(textToSend, receivedLines);
				if(!Soc->IsError()) {
					glb.vidpov = 0;
					std::transform(receivedLines[0].begin(), receivedLines[0].end(), receivedLines[0].begin(), ::toupper);
					loc = receivedLines[0].find(strJsonOk);
					if(loc != string::npos)
						MsgBox("Тест ОК", "Тест ОК");
					else
						MsgBox("Помилка", recBuf);
				}
				else {
					StrForm(errorMsg, 100, Soc->errMsg);
					SaveLogFile(errorMsg, 1);
					MsgBox("Помилка", errorMsg);
				}
				delete Soc;
			} else {
				srvrDL = 5;
				Socket23 *Soc = new Socket23(srvrIP, srvrPort, srvrAT);
				Soc->MakeSocket();
				Soc->SendData(textToSend);
				Sleep(srvrDL * 1000);
				Soc->ShutdownSocket();	
				Soc->ReceiveData(receivedLines);
				if(!Soc->IsError()) {
					glb.vidpov = 0;
					std::transform(receivedLines[0].begin(), receivedLines[0].end(), receivedLines[0].begin(), ::toupper);
					loc = receivedLines[0].find(strJsonOk);
					if(loc != string::npos)
						MsgBox("Тест ОК", "Тест ОК");
					else
						MsgBox("Помилка", recBuf);
				}
				else {
					StrForm(errorMsg, 100, Soc->errMsg);
					SaveLogFile(errorMsg, 1);
					MsgBox("Помилка", errorMsg);
				}
				delete Soc;
			}
		}
		else {
			StrForm(errorMsg, 100, "Не введено ір-адресу сервера!");
			MsgBox("Помилка", errorMsg);
		}
		break;
	case BUT3:
		break;
	}
	return 0;
}

int GetNumPtk(vector<string> & allFile) {
	int ptk = 0;
    std::size_t start, finish;
	string tmpStr = "";
	if(allFile.size()) {
		start = allFile[0].find(",code:");
		finish = allFile[0].find_first_of("|");
		if(start != std::string::npos &&
			finish != std::string::npos &&
			(start + 6) < finish) {
				tmpStr = allFile[0].substr((start + 6), finish - (start + 6));
				if(tmpStr.size()) {
					Ldv(tmpStr.c_str());
					ptk = Sti(0);
				}
		}
	}
	return ptk;
}

unsigned short GetSrvrDelay(int ptkCode) {
	unsigned short srvrDL = 10;
	switch(ptkCode) {
		case 10: case 11:
			srvrDL = 8;
			break;
		case 12:
			srvrDL = 12;
			break;
		case 21: case 22: case 23: case 24:
			srvrDL = 8;
			break;
		case 31:
			srvrDL = 8;
			break;
		case 582:
			srvrDL = 4;
			break;
		case 94:
			srvrDL = 6;
			break;
		case 61:
			srvrDL = 12;
			break;
		case 309:
			srvrDL = 15;
			break;
		case 577:
			srvrDL = 15;
			break;
		case 589:
			srvrDL = 15;
			break;
		case 563:
			srvrDL = 6;
			break;
		default:
			srvrDL = 10;
			break;
	}
	return srvrDL;
}
/*
void SplitStringLine(const std::string& str, vector<string> & cont, const std::string& delims)
{
    std::size_t current, previous = 0;
    current = str.find_first_of(delims);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find_first_of(delims, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
}
*/
/*
void SaveIniFile(vector<string> & lines) {
	Singleton &glb = Singleton::getInstance();
	vector<string> splitPath;
	splitPath.clear();
	SplitStringLine(glb.pathAdmin, splitPath);
	string pathIni;
	splitPath.pop_back();
	for (std::vector<string>::iterator it = splitPath.begin() ; it != splitPath.end(); ++it) {
		pathIni += *it;
		pathIni += "\\";
	}
	pathIni += "TCPClientW.ini";
	std::ofstream output_file(pathIni.c_str());
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(lines.begin(), lines.end(), output_iterator);
}
*/
