#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#define DEFAULT_BUFLEN 512
#define LINKSMAX 4
using namespace std;
vector<int> pool;
void broadcast(const char *cnt){
  cout<<"send "<<cnt<<endl;
  for(int connection:pool){
    if(send(connection,cnt,strlen(cnt)+1,0)<0){
      cout<<"send to "<<connection<<" error"<<endl;
    }
  }
}
void sanae(int connection){
  if(send(connection,"sys:connect\n",13,0)<0){
    cout<<"send erro"<<endl;
  }
  do{
    char recvbuf[DEFAULT_BUFLEN];
    int recvlen=DEFAULT_BUFLEN;
    if(recv(connection,recvbuf,recvlen,0)<=0){
      cout<<"recv error"<<endl;
      if(errno!=EINTR){
        cout<<"connection over"<<endl;
        cout<<"links "<<pool.size();
        pool.erase(std::remove(pool.begin(),pool.end(),connection),pool.end());
        cout<<" to "<<pool.size()<<endl;
        stringstream ss;
        ss<<"sys:"<<connection<<" out";
        broadcast(ss.str().c_str());
        break;
      }
      cout<<"connection sound:"<<errno<<endl;
      continue;
    }
    recvbuf[recvlen]=0;
    cout<<this_thread::get_id()<<" recieve:"<<recvbuf<<endl;
    stringstream ss;
    ss<<connection<<":"<<recvbuf;
    broadcast(ss.str().c_str());
  }while(1);
  if(shutdown(connection,SHUT_RDWR)<0){
    cout<<"shutdown error"<<endl;
  }
  if(close(connection)<0){
    cout<<"close error"<<endl;
  }
}
void maki(){
  int listener=socket(PF_INET,SOCK_STREAM,0);
  if(listener<0){
    cout<<"socket error"<<endl;
    return;
  }
  int reuse=1;
  if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(reuse))<0){
    cout<<"setsockopt SO_REUSEADDR error"<<endl;
    return;
  }
  int port=141;
  struct sockaddr_in addr={
    sin_family:PF_INET,
    sin_port:(in_port_t)htons(port),
    sin_addr:{
      s_addr:htonl(INADDR_ANY)
    }
  };
  if(bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0){
    cout<<"bind error"<<endl;
    return;
  }
  if(listen(listener,2)<0){
    cout<<"listen error"<<endl;
    return;
  }
  while(1){
    struct sockaddr_storage client;
    unsigned addr_size=sizeof(client);
    int connection=accept(listener,(struct sockaddr *)&client,&addr_size);
	if(pool.size()>=LINKSMAX)connection=-1;
    cout<<"accept a connection"<<endl;
    if(connection<0){
      cout<<"accept error"<<endl;
      close(connection);
      continue;
    }
    thread(sanae,connection).detach();
    pool.push_back(connection);
  }
  cout<<"maki ending"<<endl;
}
int main(int argc,char *argv[]){
  maki();
  cout<<"maki ended,main ending"<<endl;
  return 0;
}
