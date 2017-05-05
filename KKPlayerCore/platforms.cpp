#include "stdafx.h"
#include <stdio.h>
#ifdef WIN32
void WindowsLOGE(const char* format,...)
{
	int len=strlen(format);
	
	
	va_list ap;  
	va_start(ap,format);  
	len+= _vscprintf( format,ap)+1;
	char *buf=(char*)::malloc(len);
	vsprintf(buf,format,ap);  
	va_end(ap);  
	OutputDebugStringA(buf);  
	::free(buf);
}
#endif
#ifndef Platforms_H_
	#ifndef WIN32_KK
		#ifndef WIN32
			void Sleep(int ms)
			{
				usleep(ms*1000);
			}

		#endif



		
	/*		 void __cdecl LOGE(const char* _Format, ...) 
			{

				char abcd[1024];
				sprintf_s(abcd,1024,_Format,__VA_ARGS__);
			     ::OutputDebugStringA(abcd);
			}*/


      /* BOOL ResetEvent(HANDLE hEvent)
       {
	      return TRUE;
       }
	   BOOL SetEvent(HANDLE hEvent)
	   {
		   return TRUE;
	   }
	   DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds)
	   {
		   return 0;
	   }
	   HANDLE CreateEvent(int lpEventAttributes,BOOL bManualReset,BOOL bInitialState,void* lpName)
	   {
		   return NULL;
	   }
	   BOOL TerminateThread(HANDLE hThread,DWORD dwExitCode)
	   {
		  return TRUE;
	   }
	   BOOL CloseHandle(HANDLE hObject)
	   {
           return TRUE;
	   }
	   DWORD WaitForMultipleObjects(DWORD nCount,HANDLE *lpHandles,BOOL bWaitAll,DWORD dwMilliseconds)
	   {
		    return TRUE;
	   }*/
#ifndef WIN32
	   BOOL GetClientRect(HWND hWnd,RECT *lpRect)
	   {
            return TRUE;
	   } 
	   void assert(int i)
	   {

	   }
#endif
	  
#endif
#endif

char**  KKCommandLineToArgv(char* CmdLine,int* _argc)
{
        char** argv;
        char*  _argv;
        unsigned long   len;
        unsigned long   argc;
        char   a;
        unsigned long    i, j;

		
        unsigned char  in_QM;
        unsigned char  in_TEXT;
        unsigned char  in_SPACE;

        len = strlen(CmdLine);
        i = ((len+2)/2)*sizeof(void*) + sizeof(void*);

        //argv = (char**)GlobalAlloc(GMEM_FIXED,i + (len+2)*sizeof(char));
		argv = (char**)malloc(i + (len+2)*sizeof(char));
        _argv = (char*)(((unsigned char*)argv)+i);

        argc = 0;
        argv[argc] = _argv;
        in_QM = 0;
        in_TEXT = 0;
        in_SPACE = 1;
        i = 0;
        j = 0;

        while( a = CmdLine[i] ) {
            if(in_QM) {
                if(a == '\"') {
                    in_QM = 0;
                } else {
                    _argv[j] = a;
                    j++;
                }
            } else {
                switch(a) {
                case '\"':
                    in_QM = 1;
                    in_TEXT = 1;
                    if(in_SPACE) {
                        argv[argc] = _argv+j;
                        argc++;
                    }
                    in_SPACE = 0;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if(in_TEXT) {
                        _argv[j] = '\0';
                        j++;
                    }
                    in_TEXT = 0;
                    in_SPACE = 1;
                    break;
                default:
                    in_TEXT = 1;
                    if(in_SPACE) {
                        argv[argc] = _argv+j;
                        argc++;
                    }
                    _argv[j] = a;
                    j++;
                    in_SPACE = 0;
                    break;
                }
            }
            i++;
        }
        _argv[j] = '\0';
        argv[argc] = NULL;

        (*_argc) = argc;
        return argv;
}

