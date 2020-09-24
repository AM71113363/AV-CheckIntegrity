#include <windows.h>   
#include <stdio.h>  
#include <stdlib.h>  
#include <commctrl.h>
#include "md5.h"

char vvvv[255];

#define MyCLASS "AntiVirus"

#define PBM_SETPOS	 (WM_USER+2) // = 1026
#define PBM_SETRANGE (WM_USER+1) //= 1025

#define YES     1
#define NO      0

HINSTANCE ins;
HWND hWnd;
HWND hBar;


UCHAR DropFile[MAX_PATH];
UCHAR IamWorking = NO;
ULONGLONG dFileSize = 0;
ULONGLONG dReadBytes = 0;

MD5_CTX MD5_Context;
UCHAR PartHash[8];

void CenterOnScreen(HWND hnd)
{
	RECT rcClient, rcDesktop;
	int nX, nY;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
    GetWindowRect(hnd, &rcClient);
	nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
	nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
	SetWindowPos(hnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void ErrorCode(unsigned char *fnc)
{
	LPVOID lpMsgBuf;
	unsigned char buffer[255];
	DWORD er = GetLastError();
	if(er == 0)
    {
        MessageBox(NULL,"ErroCode:[ 0 ]",fnc, MB_ICONINFORMATION|MB_SYSTEMMODAL|MB_OK);    
        return;
    }
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,er, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf, 0, NULL);

    sprintf(buffer,"  ErrorCode:[ %d ] %s\0",er,(UCHAR*)lpMsgBuf);
    LocalFree( lpMsgBuf );
    
    MessageBox(NULL,buffer,fnc, MB_ICONINFORMATION|MB_SYSTEMMODAL|MB_OK);
}

void CALLBACK pBarProc(HWND hwnd, UINT u, UINT_PTR t, DWORD w)
{
  static char temp[32];
  if(IamWorking==YES)
  {
      ULONGLONG nr = dReadBytes;
      nr *= 100;
      nr /= dFileSize;
      sprintf(temp, "%d%%", nr);
      SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)temp);
      SendMessage(hBar, PBM_SETPOS, (DWORD)nr, 0);
  }
}

void OpenThisFile()
{
	WIN32_FIND_DATA dataFile; 
    HANDLE hFile; FILE *fp;
    unsigned __int64 len;
	unsigned char buffer[1024];
	dReadBytes = 0;
    UINT i,iLen;
    SetWindowText(hWnd,"");
    hFile = FindFirstFile(DropFile,&dataFile);
    if(hFile == INVALID_HANDLE_VALUE)
    {
         MessageBox(hWnd,DropFile, "Failed to find the File !!!",MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
         return;
    } 
    FindClose(hFile);
    if(dataFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
         MessageBox(hWnd, "Must be a File not a Folder","Drag-Drop",MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
         return;
    }

    fp = fopen(DropFile, "rb");
	if(!fp) 
	{   
	     MessageBox(hWnd, "Failed to open File !!!","ERROR",MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
         return;	
    }    
    IamWorking=YES;

	dFileSize = MAXDWORD;
    dFileSize = dFileSize + 1;
	dFileSize= dFileSize * dataFile.nFileSizeHigh;
	dFileSize= dFileSize + dataFile.nFileSizeLow;

	MD5Init(&MD5_Context);
	SendMessage(hBar, PBM_SETPOS, (DWORD)0, 0);
    SetTimer(hWnd, 1000, 200, (TIMERPROC)pBarProc);
	
	while(1)
	{
		len = fread(buffer, 1, 1024, fp);
		if(len > 0)
		{
			dReadBytes += len;
			MD5Update(&MD5_Context, buffer, (UINT)len);	
		} 
		else
		{
		    UCHAR hash[16];
		    MD5Final(hash, &MD5_Context);
            sprintf(PartHash, "%02X%02X\0", hash[0],hash[1]);
            break;
        }
     }

	KillTimer(hWnd, 1000);
    SetWindowText(hWnd,PartHash);
    fclose(fp);
    IamWorking=NO;
    //verifyFile    
    iLen = strlen(DropFile);                
    for(i=iLen;i>0;i--)
    {
         if(DropFile[i] == '\\')
         break;                  
    }
    buffer[0]=DropFile[i+1];
    buffer[1]=DropFile[i+2];
    buffer[2]=DropFile[i+3];
    buffer[3]=DropFile[i+4];
    buffer[4]=DropFile[i+5];
    buffer[5]=0;
    
    if( (buffer[0] == PartHash[0]) &&
        (buffer[1] == PartHash[1]) &&
        (buffer[2] == PartHash[2]) &&
        (buffer[3] == PartHash[3]) &&
        (buffer[4] == '-'))
    {
        MessageBox(hWnd,DropFile, "File Is OK",MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
         return;
    } 
    //check if is infected or renamed or damaged
    if( (isalpha(buffer[0]) || isdigit(buffer[0]) ) &&
        (isalpha(buffer[1]) || isdigit(buffer[1]) ) &&
        (isalpha(buffer[2]) || isdigit(buffer[2]) ) &&
        (isalpha(buffer[3]) || isdigit(buffer[3]) ) &&
        buffer[4] == '-')
    {
        MessageBox(hWnd,DropFile, "File Infected or Renamed or Damaged!!!",MB_OK | MB_ICONWARNING  | MB_SYSTEMMODAL);
         return;
    }
    iLen =  MessageBox(hWnd,"Would You Like To Protect It?", "ACTION",MB_YESNO| MB_ICONINFORMATION | MB_SYSTEMMODAL);
    if(iLen ==IDYES	)
    {
        memset(vvvv,0,255);
        iLen = strlen(DropFile);
                      
        snprintf(vvvv, i,"%s\0",DropFile);
        sprintf(&vvvv[i],"\\%C%C%C%C-%s\0",
                      PartHash[0],
                      PartHash[1],
                      PartHash[2],
                      PartHash[3],
                      &DropFile[i+1]);
             
        if(MoveFile(DropFile,vvvv) == 0)
        {
             ErrorCode("Can Not Rename it!");                 
        }
        else
        {
             SetWindowText(hWnd,"OK");                 
        }
    }
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
             HBITMAP m; HWND s;
             hWnd = hwnd;
             InitCommonControls();
             m = LoadBitmap(ins,MAKEINTRESOURCE(8000));
             s = CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|SS_BITMAP,0,0,142,192,hwnd,(HMENU)0,ins,NULL);
             SendMessage(s, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)m);
			hBar = CreateWindowEx(0, "msctls_progress32", (LPSTR)NULL, 1 | WS_CHILD | WS_VISIBLE,
			44, 96, 58, 14, hwnd, NULL, ins, NULL);
			SendMessage(hBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            CenterOnScreen(hwnd);

		}
		break;
		case WM_DROPFILES: 
		{
			if(IamWorking==YES){ break; }
            
			memset(DropFile,0,MAX_PATH);
			DragQueryFile((HDROP)wParam, 0, DropFile, MAX_PATH);
			DragFinish((HDROP) wParam);
			CreateThread(0,0,(LPTHREAD_START_ROUTINE)OpenThisFile,0,0,0); 
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}




int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wc;
	HWND hwnd;
	ins=hinstance;

    wc.style = 0;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = NULL;
    wc.hIcon = LoadIcon(hinstance, "A");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = MyCLASS;
	if(!RegisterClass(&wc)) return FALSE;
	hwnd = CreateWindowEx(WS_EX_ACCEPTFILES|WS_EX_TOPMOST, MyCLASS, "AV",WS_VISIBLE |WS_SYSMENU,
         CW_USEDEFAULT, CW_USEDEFAULT, 148, 224, NULL, NULL, hinstance, NULL);
  if (!hwnd) return FALSE;
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

