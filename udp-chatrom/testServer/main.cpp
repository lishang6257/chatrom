// UDPServer.cpp

#include <stdio.h>
#include <WINSOCK2.H>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#define name_Length 50
#define BUF_SIZE	1024

using namespace std;

typedef struct
{
    char clientUID[name_Length];         //身份信息
    char passwd[name_Length] = "157uibfbcxd933218123ihn";
    struct sockaddr_in cliReceive;
    struct sockaddr_in cliSend;
    bool senderService =false;  //连接状态
    bool receiverService = false;
}Client;

vector<Client> cList;

namespace message{
    string getTime(){
        time_t timep;
        time (&timep);
        char tmp[64];
        strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
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
        return false;
    }
    bool getPasswd(char* mes,char* passwd){
        for(int i = 0;i < BUF_SIZE - 3*name_Length;i ++){
            passwd[i] = mes[i+name_Length*3];
            if(passwd[i] == '\0') return true;
            if(i == BUF_SIZE - 3*name_Length) return false;
        }
        return false;
    }
}

void ServerSenderToAll(const SOCKET sSocket,char* buf)
{
	// 向客户端发送数据
	int			len = sizeof(SOCKADDR),isend;
	for(auto c : cList){
        if(c.receiverService && c.senderService)
            isend = sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(c.cliReceive),len);
	}
}

bool ServerSendToOne(const SOCKET sSocket,char* buf,char* Receiver)
{
	// 向客户端发送数据
	int			len = sizeof(SOCKADDR);bool flag = false;
    for(auto c : cList){
        if((strcmp(Receiver,c.clientUID) == 0) && c.receiverService && c.senderService){
            sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(c.cliReceive),len);
            flag = true;
        }
	}
	return flag;
}

int getAdress(char* name)
{
	for(auto iter = cList.begin();iter < cList.end();iter ++){
        if((strcmp(name,(*iter).clientUID) == 0)) return (iter - cList.begin());
	}
	return -1;
}

int getNum()
{
    int num = 0;
    for(auto c : cList)
        if(c.senderService) num ++;
    return num;
}

bool userInList(char* name,char* passwd)
{
    ifstream iFile("userList.txt");
	string ss;
	while(getline(iFile,ss)){
        int v = ss.find(",");
        string clientID = ss.substr(0,v);
        string tpasswd = ss.substr(v+1,ss.length()-1);
        if(strcmp(clientID.c_str(),name) == 0){
            strcpy(passwd,tpasswd.c_str());
            iFile.close();
            return true;
        }
	}
	iFile.close();
	return false;
}

void addUserList(char* name,char* passwd)
{
//定义文件输出流
	ofstream oFile;
	//打开要输出的文件
	oFile.open("userList.txt", ios::app);    // 这样就很容易的输出一个需要的excel 文件
	oFile << name << "," << passwd << endl;
	oFile.close();
}

void ServerReceiver(const SOCKET sSocket)
{
    SOCKADDR_IN addrClient;
    int			len = sizeof(SOCKADDR);
    while(1){
        // 从客户端接收数据
        cout << "*********** online num : " << getNum() << " **************\n";

        char buf[BUF_SIZE];
        memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
        int iRecv = recvfrom(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&addrClient,&len);
        if(SOCKET_ERROR == iRecv){
            printf("recvfrom failed !/n");
            return ;
        }
        if (iRecv == 0) continue;

        char sender[name_Length],receiver[name_Length],mess[BUF_SIZE - 2*name_Length];
        if(!(message::getSender(buf,sender) && message::getMessage(buf,mess) && message::getReceiver(buf,receiver))){
            cout << "Error in message parsing \n";continue;
        }

        int senderID = getAdress(sender),receiverID = getAdress(receiver);

//        cout << userInList(sender,mess);

        if(strcmp(receiver,"SERVER") == 0){
            char SCommmand[name_Length];
            char TPasswd[name_Length],UPasswd[name_Length];
            Client c;
            strcpy(c.clientUID,sender);
            memset(SCommmand,0,sizeof(SCommmand));
            //向服务器的命令请求
            if(strcmp(mess,"UserRegister") == 0){
                if(userInList(sender,TPasswd)){//在用户表中
                    cout << sender << " : " << "sender has register\n";
                    strcpy(SCommmand,"UserHasRegister");
                }else{//不在用户表中,注册
                    strcpy(SCommmand,"UserRegisterSucceed");
                    message::getPasswd(buf,TPasswd);
                    addUserList(sender,TPasswd);
                    cout << sender << " : " << "sender register successfully\n";
                }
                memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
                message::messagesGenerate(buf,"SERVER",sender,SCommmand);
                sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(addrClient),len);
            }
            else if(strcmp(mess,"SenderLogin") == 0){
                message::getPasswd(buf,UPasswd);
                if(senderID == -1 || !cList[senderID].senderService){//客户机未登录||发送器未登录
                    if(userInList(sender,TPasswd)){//在用户表中,登陆许可
                        if(strcmp(TPasswd,UPasswd) == 0){//密码正确
                            if(senderID == -1){//不在用户表
                                c.cliSend = addrClient;
                                c.senderService = true;
                                cList.push_back(c);
                            }else {
                                cList[senderID].cliSend = addrClient;
                                cList[senderID].senderService = true;
                            }
                            cout << sender << " : " << "sender service status on\n";
                            strcpy(SCommmand,"PasswdRight");
                        }else{
                            cout << sender << " sender login failed : password error\n";
                            strcpy(SCommmand,"PasswdError");
                        }
                    }else{
                        cout << sender << " sender login failed : cannot find this user\n";
                        strcpy(SCommmand,"UserNotInList");
                    }
                }else if(cList[senderID].senderService){//发送器已登录
                    cout << sender << " sender login failed : cannot login twice\n";
                    strcpy(SCommmand,"UserHasLogin");
                }
                memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
                message::messagesGenerate(buf,"SERVER",sender,SCommmand);
                sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(addrClient),len);
            }
            else if(strcmp(mess,"ReceiverLogin") == 0){
                char TPasswd[name_Length],UPasswd[name_Length];
                message::getPasswd(buf,UPasswd);
                if(senderID == -1 || !cList[senderID].receiverService){//客户机未登录||接受器未登录
                    if(userInList(sender,TPasswd)){//在用户表中，登陆许可
                        if(strcmp(TPasswd,UPasswd) == 0){//密码正确
                            if(senderID == -1){//不在用户表
                                c.cliReceive = addrClient;
                                c.receiverService = true;
                                cList.push_back(c);
                            }else {
                                cList[senderID].cliReceive = addrClient;
                                cList[senderID].receiverService = true;
                            }
                            cout << sender << " : " << "receiver service status on\n";
                            strcpy(SCommmand,"PasswdRight");
                        }else{
                            cout << sender << " receiver login failed : password error\n";
                            strcpy(SCommmand,"PasswdError");
                        }
                    }
                }else if(cList[senderID].receiverService){//发送器已登录
                    cout << sender << " receiver login failed : cannot login twice\n";
                    strcpy(SCommmand,"UserHasLogin");
                }else{
                    cout << sender << " receiver login failed : cannot find this user\n";
                    strcpy(SCommmand,"UserNotInList");
                }
                memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
                message::messagesGenerate(buf,"SERVER",sender,SCommmand);
                sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(addrClient),len);
            }
            else if(senderID > -1 && strcmp(mess,"SenderExit") == 0){
                cList[senderID].senderService = false;
                if(!cList[senderID].receiverService && !cList[senderID].senderService)
                    cList.erase(cList.begin()+senderID);
                cout << sender << " : " << "sender service status off\n";
            }
            else if(senderID > -1 && strcmp(mess,"ReceiverExit") == 0){
                memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
                message::messagesGenerate(buf,"SERVER",sender,mess);
                sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(cList[senderID].cliReceive),len);
                cList[senderID].receiverService = false;
                if(!cList[senderID].receiverService && !cList[senderID].senderService)
                    cList.erase(cList.begin()+senderID);
                cout << sender << " : " << "receiver service status off\n";
            }
            else{
                cout << sender << " : " << "error command\n";
            }
        }
        else{//向其他用户发送消息
            char SCommmand[name_Length];
            struct sockaddr_in ssa,rrb;
            if(senderID == -1){//用户不存在
                strcpy(SCommmand,"UserNotLogin");
                memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
                message::messagesGenerate(buf,"SERVER",sender,SCommmand);
                sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(addrClient),len);
                cout << sender << " : sender is not on-line\n";continue;
            }
            ssa = cList[senderID].cliSend;
            cout << "-------------------------------\n";
            cout << "Sender[" << inet_ntoa(ssa.sin_addr) << " : " <<  ntohs(ssa.sin_port) << "] : " <<  sender << endl;
            if(strcmp(receiver,"ALL") == 0){
                cout << "Receiver"  << " : ALL"  << endl;
                ServerSenderToAll(sSocket,buf);
            }else if(receiverID == -1){
                strcpy(SCommmand,"ReceiverNotLogin");
                memset(buf, 0, sizeof(buf));		//初始化接收缓冲区
                message::messagesGenerate(buf,"SERVER",sender,SCommmand);
                sendto(sSocket,buf,BUF_SIZE,0,(SOCKADDR*)&(addrClient),len);
                cout << receiver << " :receiver is not on-line\n";
                cout << "-------------------------------\n";continue;
            }else{
                rrb = cList[receiverID].cliReceive;
                cout << "Receiver[" << inet_ntoa(rrb.sin_addr) << " : " <<  ntohs(rrb.sin_port) << "] : " <<  receiver << endl;
                ServerSendToOne(sSocket,buf,receiver);
            }
            cout << "Message : " << mess << endl;
            cout << "-------------------------------\n";
        }
    }
}


int main(void)
{
	WSADATA	wsd;
	int		nRet;

	// 初始化套接字动态库
	if(WSAStartup(MAKEWORD(2,2),&wsd) != 0)
	{
		printf("WSAStartup failed !/n");
		return 1;
	}

	SOCKET		sSocket = socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN addrSrv;


	// 设置服务器地址
//	ZeroMemory(buf,BUF_SIZE);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(5000);

	// 绑定套接字passwd
	nRet = bind(sSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if(SOCKET_ERROR == nRet)
    {
        printf("bind failed !/n");
        return -1;
    }

    ServerReceiver(sSocket);

	closesocket(sSocket);
	WSACleanup();
	return 0;
}
