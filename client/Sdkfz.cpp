#include <iostream>
#include "resource.h"
#include "socketSSR.h"
#include <windows.h>

#define DEFAULT_LEN 512

using namespace std;

BOOL CALLBACK f(HWND h,UINT m,WPARAM w,LPARAM l){
  switch(m){
    case WM_INITDIALOG:
      //SetWindowLong(h,GWL_EXSTYLE,GetWindowLong(h,GWL_EXSTYLE)|WS_EX_LAYERED);
      SetLayeredWindowAttributes(h,0,241,LWA_ALPHA);
      break;
    case WM_KEYDOWN:
      break;
    case WM_KEYUP:
      /*
      cout<<GetFocus()<<endl;
      cout<<GetDlgItem(h,0002)<<endl;
      cout<<w<<endl; 
      */
      if(w==13)if(GetFocus()==GetDlgItem(h,0002)){
        char buf[DEFAULT_LEN];
        WPARAM buflen=DEFAULT_LEN;
        SendMessage(GetDlgItem(h,0002),WM_GETTEXT,buflen-1,(LPARAM)buf);
        SendMessage(GetDlgItem(h,0002),WM_SETTEXT,0,(LPARAM)"");
        if(buf[strlen(buf)-1]=='\n')buf[strlen(buf)-1]=0;
        buf[buflen-1]=0;
        cout<<buf<<endl;
        cout<<"touch"<<endl;
        
        sendmsg(buf);
        
        return TRUE;
      }
      break;
    case WM_COMMAND:
      switch(LOWORD(w)){
        case 0002:
          break;
      }
      break;
    case WM_LBUTTONDOWN:
      PostMessage(h,WM_SYSCOMMAND,SC_MOVE|HTCAPTION,0);
      break;
    case WM_RBUTTONDOWN:
      SendMessage(GetDlgItem(h,0001),LB_ADDSTRING,0,(LPARAM)"mika");
      SendMessage(GetDlgItem(h,0001),WM_VSCROLL,SB_PAGEDOWN,0);
      break;
  }
  return FALSE; 
}
int main(){
  //DialogBox(NULL,MAKEINTRESOURCE(IDD_KANOJYO),NULL,(DLGPROC)f);
  MSG msg;
  BOOL bRet;
  HWND hWnd=fox=CreateDialog(NULL,MAKEINTRESOURCE(IDD_KANOJYO),NULL,(DLGPROC)f);
  ShowWindow(hWnd,SW_SHOW);
  
  thread(socketSSR).detach();
  
  while((bRet=GetMessage(&msg,NULL,0,0))!=0){
    if(bRet==-1)break;
    else if(!IsWindow(hWnd)||!IsDialogMessage(hWnd,&msg)){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }else if(msg.message==WM_KEYUP||msg.message==WM_KEYDOWN){
      SendMessage(hWnd,msg.message,msg.wParam,msg.lParam);
    }
  }
  cout<<"state "<<GetLastError()<<" quiting"<<endl;
  return 0;
}
