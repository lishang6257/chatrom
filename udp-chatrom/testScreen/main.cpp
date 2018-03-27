//////////////////////////////////////////////////////////////////////////
// UDPClient.cpp

#include <stdio.h>
#include <WINSOCK2.H>
#include <iostream>
#include <string>
#include <ctime>
#define BUF_SIZE	1024
#define name_Length 50

namespace message{
    std::string getTime(){
        time_t timep;
        time (&timep);
        char tmp[64];
        strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%sSocket",localtime(&timep) );
        return tmp;
    }
    bool messagesGenerate(char* mes,char* sender,char* receiver,char* message){
        int nameLen = strlen(sender),mesLen = strlen(message),recLen = strlen(receiver);
        if(nameLen > name_Length - 1) nameLen = name_Length - 1;
        if(recLen > name_Length - 1) recLen = name_Length - 1;
        if(mesLen > BUF_SIZE - 2*name_Length -1) mesLen = BUF_SIZE - 2*name_Length -1;
        int i;
        for(i = 0;i < nameLen;i ++) mes[i] = sender[i];
        mes[i] = '\0';
        for(i = 0;i < recLen;i ++) mes[i+name_Length] = receiver[i];
        mes[i+name_Length] = '\0';
        for(i = 0;i < mesLen;i ++) mes[i+2*name_Length] = message[i];
        mes[i+name_Length*2] = '\0';
        return true;
    }
    bool messagesGenerate(char* mes,char* sender,char* receiver,char* message,char * passwd){
        int nameLen = strlen(sender),mesLen = strlen(message),recLen = strlen(receiver),passLen = strlen(passwd);
        if(nameLen > name_Length - 1) nameLen = name_Length - 1;
        if(recLen > name_Length - 1) recLen = name_Length - 1;
        if(mesLen > BUF_SIZE - 2*name_Length -1) mesLen = BUF_SIZE - 2*name_Length -1;
        if(passLen > BUF_SIZE - 3*name_Length -1) passLen = BUF_SIZE - 3*name_Length -1;
        int i;
        for(i = 0;i < nameLen;i ++) mes[i] = sender[i];
        mes[i] = '\0';
        for(i = 0;i < recLen;i ++) mes[i+name_Length] = receiver[i];
        mes[i+name_Length] = '\0';
        for(i = 0;i < mesLen;i ++) mes[i+2*name_Length] = message[i];
        mes[i+name_Length*2] = '\0';
        for(i = 0;i < passLen;i ++) mes[i+3*name_Length] = passwd[i];
        mes[i+name_Length*3] = '\0';
        return true;
    }
    bool getSender(char* mes,char *sender){
        for(int i = 0;i < name_Length;i ++){
            sender[i] = mes[i];
            if(sender[i] == '\0') return true;
            if(i == name_Length) return false;
        }
        return true;
    }
    bool getReceiver(char* mes,char *receiver){
        for(int i = 0;i < name_Length;i ++){
            receiver[i] = mes[i+name_Length];
            if(receiver[i] == '\0') return true;
            if(i == name_Length) return false;
        }
        return true;
    }
    bool getMessage(char* mes,char* mess){
        for(int i = 0;i < BUF_SIZE - 2*name_Length;i ++){
            mess[i] = mes[i+name_Length*2];
            if(mess[i] == '\0') return true;
            if(i == BUF_SIZE - 2*name_Length) return false;
        }
    }
    bool getPasswd(char* mes,char* passwd){
        for(int i = 0;i < BUF_SIZE - 3*name_Length;i ++){
            passwd[i] = mes[i+name_Length*3];
            if(passwd[i] == '\0') return true;
            if(i == BUF_SIZE - 3*name_Length) return false;
        }
    }
}

bool login(SOCKET &sockClient,SOCKADDR_IN	&servAddr,int &nServAddLen,char* buf,char* clientUID,char* serIP,char* passwd,char* mess,char* sender)
{
    std::cout << "--------login----------\n";
    std::cout << "Input UID : ";
    gets(clientUID);
    std::cout << "Input passwd : ";
    gets(passwd);
    // 设置服务器地址
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.S_un.S_addr = inet_addr(serIP);
    servAddr.sin_port = htons(5000);
    nServAddLen = sizeof(servAddr);
    for(int errTime = 1; errTime <3;errTime++){
        message::messagesGenerate(buf,clientUID,"SERVER","ReceiverLogin",passwd);
        int iSend = sendto(sockClient,buf,BUF_SIZE,0,(struct sockaddr*)&servAddr,nServAddLen),iLen;
        if(iSend == SOCKET_ERROR){
            printf("recvfrom() failed:%d\n",WSAGetLastError());return false;
        }
        int nRet = recvfrom(sockClient,buf,BUF_SIZE,0,(struct sockaddr *)&servAddr,&nServAddLen);
        if(SOCKET_ERROR == nRet){
            std::cout <<"recvfrom failed : " << WSAGetLastError();return false;
        }
//        iSend = recvfrom(sockClient,buf,BUF_SIZE,0,(struct sockaddr*)&servAddr,&iLen);
        if(!(message::getSender(buf,sender) && message::getMessage(buf,mess))){
            std::cout << "message pass error";return false;
        }
        if((strcmp(sender,"SERVER") !=0) || (strcmp(mess,"PasswdError") == 0)){
            std::cout << "passwd error\nInput passwd : ";
            gets(passwd);
        }else if((strcmp(sender,"SERVER") !=0) || (strcmp(mess,"UserNotInList") == 0)){
            std::cout << "User not register\n";
            return false;
        }else if((strcmp(sender,"SERVER") !=0) || (strcmp(mess,"UserHasLogin") == 0)){
            std::cout << "User has login\n";
            return false;
        }else if((strcmp(sender,"SERVER") !=0) || (strcmp(mess,"PasswdRight") == 0)){
            std::cout << "login successfully\n";
            return true;
        }else return false;
    }
    std::cout << "Too much try,login refused\n";
    return false;
}

int main(void)
{
	WSADATA	wsd;
	SOCKET	sSocket;

	// 初始化套接字动态库
	if(WSAStartup(MAKEWORD(2,2),&wsd) != 0)
	{
		printf("WSAStartup failed !/n");
		return 1;
	}

	// 创建套接字
	sSocket = socket(AF_INET,SOCK_DGRAM,0);
	if(sSocket == INVALID_SOCKET)
	{
		printf("socket() failed, Error Code:%d\n",WSAGetLastError());
		WSACleanup();
		return 1;
	}

	char buf[BUF_SIZE];	// 接受数据
	SOCKADDR_IN	servAddr;		// 服务器套接字地址
	SOCKET		sockClient = socket(AF_INET,SOCK_DGRAM,0);
	int			nRet;
	ZeroMemory(buf,BUF_SIZE);

    char clientUID[name_Length],serIP[256],passwd[name_Length];
    std::cout << "Input IP : ";
    gets(serIP);
//    strcpy(serIP,"127.0.0.1");

	// 设置服务器地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr(serIP);
	servAddr.sin_port = htons(5000);

	// 向服务器发送数据
	int nServAddLen = sizeof(servAddr),errTime = 0;;
	char mess[BUF_SIZE - 2*name_Length],sender[name_Length];
    while(1){
        system("cls");
        while(!login(sockClient,servAddr,nServAddLen,buf,clientUID,serIP,passwd,mess,sender));

        while(1){
            ZeroMemory(buf,BUF_SIZE);
            nRet = recvfrom(sockClient,buf,BUF_SIZE,0,(struct sockaddr *)&servAddr,&nServAddLen);
            if(SOCKET_ERROR == nRet){
                std::cout <<"recvfrom failed : " << WSAGetLastError();break;
            }
            // 打印来自服务端发送来的数据
            char sender[name_Length],receiver[name_Length],mess[BUF_SIZE - 2*name_Length];int i;
            if(!(message::getSender(buf,sender) && message::getMessage(buf,mess) && message::getReceiver(buf,receiver))){
                std::cout << "message pass error";continue;
            }
            printf("-------------------------------\nSender : %s \nReceiver : %s\nMessage : %s\n-------------------------------\n",sender,receiver,mess);
            if((strcmp(sender,"SERVER") == 0) && strcmp(mess,"ReceiverExit") == 0) {
                std::cout << receiver << " : Receiver Exit successfully\n" ;break;
            }
        }
    }
	closesocket(sSocket);
	WSACleanup();
	return 0;
}
