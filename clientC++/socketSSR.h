#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include "Console.h"

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_BUFLEN 512

SOCKET Akari; 
HWND fox;

void sunrise(char *buf){
  SendMessage(GetDlgItem(fox,0001),LB_ADDSTRING,0,(LPARAM)buf);
  SendMessage(GetDlgItem(fox,0001),WM_VSCROLL,SB_PAGEDOWN,0);
}
void recvmsg(SOCKET &ClientSocket){
  char recvbuf[DEFAULT_BUFLEN];
  int recvbuflen=DEFAULT_BUFLEN;
  int iResult;
  do{
    iResult=recv(ClientSocket,recvbuf,recvbuflen,0);
    if(iResult>0){
      if(recvbuf[strlen(recvbuf)-1]=='\n')recvbuf[strlen(recvbuf)-1]=0;
      recvbuf[DEFAULT_BUFLEN-1]=0;
      sunrise(recvbuf);
      logW("Bytes recieved","ok",red);
      printf("Bytes recieved:%d\n",iResult);
      printf(":");
      cout<<blue;
      printf("%s\n",recvbuf);
      cout<<white;
    }
    if(iResult<=0){
      printf("recv failed:%d\n",WSAGetLastError());
    }
  }while(1);
}
void sendmsg(char *sendbuf){
  int iSendResult=SOCKET_ERROR;
  sendbuf[DEFAULT_BUFLEN-1]=0;
  do{
    if(iSendResult==SOCKET_ERROR){
      iSendResult=send(Akari,sendbuf,(int)(strlen(sendbuf)+1),0);
      if(iSendResult==SOCKET_ERROR){
        printf("send failed:%d\n",WSAGetLastError());
      }else{
        logW("Bytes send","ok",red);
        printf("Bytes sent:%d\n",iSendResult);
        printf(":");
        cout<<red;
        printf("%s\n",sendbuf);
        cout<<white;
        break;
      }
    }
  }while(1);
}
void asClient(struct addrinfo *result){
  struct addrinfo *ptr=NULL;
  SOCKET ConnectSocket=INVALID_SOCKET;
  int iResult;
  do{
    for(ptr=result;ptr!=NULL;ptr=ptr->ai_next){
      ConnectSocket=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
      if(ConnectSocket==INVALID_SOCKET){
        printf("Error at socket():%ld\n",WSAGetLastError());
        break;
      }
      logW("socket()","ok",yellow);
      iResult=connect(ConnectSocket,ptr->ai_addr,(int)ptr->ai_addrlen);
      if(iResult==SOCKET_ERROR){
        closesocket(ConnectSocket);
        ConnectSocket=INVALID_SOCKET;
        continue;
      }
      break;
    }
    if(ConnectSocket==INVALID_SOCKET){
      cout<<GetLastError()<<endl;
      logW("Unable to connect to server!","erro",yellow);
      logW("ReConnecting...","waiting",yellow);
      Sleep(500);
      continue;
    }
  }while(ConnectSocket==INVALID_SOCKET||iResult==SOCKET_ERROR);
  logW("connect()","ok",green);
  Akari=ConnectSocket;
  recvmsg(ConnectSocket);
  iResult=shutdown(ConnectSocket,SD_SEND);
  if(iResult==SOCKET_ERROR){
    printf("shutdown failed:%d\n",WSAGetLastError());
    closesocket(ConnectSocket);
    return;
  }
  logW("shutdown the connection","ok",green);
}
int socketSSR(){
  int iResult;
  WSADATA wsaData;
  iResult=WSAStartup(MAKEWORD(2,2),&wsaData);
  if(iResult!=0){
    printf("WSAStartup failed:%d\n",iResult);
    return 1;
  }
  logW("WSAStartup","ok",green);
  struct addrinfo *result=NULL,hints;
  ZeroMemory(&hints,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=IPPROTO_TCP;
  int flag=0;
  char conAddress[16]="127.0.0.1";
  char conPort[10]="141";
  iResult=getaddrinfo(conAddress,conPort,&hints,&result);
  if(iResult!=0){
    printf("getaddrinfo failed:%d\n",iResult);
    WSACleanup();
    return 1;
  }
  logW("getaddrinfo","ok",green);
  asClient(result);
  freeaddrinfo(result);
  WSACleanup();
  logW("closesocket()","ok",green);
  return 0;
}

