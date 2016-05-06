#include "..\StdAfx.h"
#include "cchinesecode.h"
//#include <stdint.h>
using namespace std;
CChineseCode::CChineseCode()
{
}

void CChineseCode::UTF_8ToUnicode(wchar_t* pOut,char *pText)

{

   char* uchar = (char *)pOut;

   uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);

   uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

   return;

}



void CChineseCode::UnicodeToUTF_8(char* pOut,wchar_t* pText)

{

   // 注意 WCHAR高低字的顺序,低字节在前，高字节在后

   char* pchar = (char *)pText;



   pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));

   pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);

   pOut[2] = (0x80 | (pchar[0] & 0x3F));

   return;

}



void CChineseCode::UnicodeToGB2312(char* pOut,wchar_t uData)

{
	//StrHelper::WcharToChar(uData, pOut);
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);

	return;

}



void CChineseCode::Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer)

{

   ::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);

   return ;

}



void CChineseCode::GB2312ToUTF_8(string& pOut,char *pText, int pLen)

{

   char buf[4];

   int nLength = pLen* 3;

   char* rst = new char[nLength];



   memset(buf,0,4);

   memset(rst,0,nLength);



   int i = 0;

   int j = 0;

   while(i < pLen)

   {

           //如果是英文直接复制就可以

           if( *(pText + i) >= 0)

           {

                   rst[j++] = pText[i++];

           }

           else

           {

                   wchar_t pbuffer;

                   Gb2312ToUnicode(&pbuffer,pText+i);



                   UnicodeToUTF_8(buf,&pbuffer);



                   unsigned short int tmp = 0;

                   tmp = rst[j] = buf[0];

                   tmp = rst[j+1] = buf[1];

                   tmp = rst[j+2] = buf[2];



                   j += 3;

                   i += 2;

           }

   }

   rst[j] = ' ';



   //返回结果

   pOut = rst;

   delete []rst;



   return;

}


//UTF-8 GBK  
void CChineseCode::UTF_8ToGBK(std::string &pOut,char *pInUtf8) 
{  
	//return;
    int n = MultiByteToWideChar(CP_UTF8, 0,pInUtf8, -1, NULL, 0)+1;  
    WCHAR * wszGBK = new WCHAR[sizeof(WCHAR) * n];  
    memset(wszGBK, 0, sizeof(WCHAR) * n);  
    MultiByteToWideChar(CP_UTF8, 0,pInUtf8,-1, wszGBK, n);  
  
    n = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL)+1;  
    
    char * szGbk = (char*)malloc(n);
    memset(szGbk, 0,  n);  
    WideCharToMultiByte(CP_ACP,0, wszGBK, -1, szGbk, n, NULL, NULL);  
  
    delete[]wszGBK;  
    wszGBK = NULL;  
    pOut=szGbk;
    
	free(szGbk);
}  
string CChineseCode::GBK_ToUTF8(const std::string& strGBK)
{
	string strOutUTF8 = "";  
	WCHAR * str1;  
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	str1 = new WCHAR[n];  
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);  
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);  
	char * str2 = new char[n];  
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);  
	strOutUTF8 = str2;  
	delete[]str1;  
	str1 = NULL;  
	delete[]str2;  
	str2 = NULL;  
	return strOutUTF8;  
}
void CChineseCode::UTF_8ToGB2312(string &pOut, char *pText, int pLen)

{

	   char * newBuf = new char[pLen+1];

   char Ctemp[4];

   memset(Ctemp,0,4);
   memset(newBuf,0,pLen+1);


   int i =0;

   int j = 0;



   while(i < pLen)

   {

       if(pText[i] > 0)

       {

               newBuf[j++] = pText[i++];

       }

       else

       {

               WCHAR Wtemp;

               UTF_8ToUnicode(&Wtemp,pText + i);

               UnicodeToGB2312(Ctemp,Wtemp);

               newBuf[j] = Ctemp[0];

               newBuf[j + 1] = Ctemp[1];

               i += 3;

               j += 2;

       }

   }

   newBuf[j] = '\0';
   pOut = newBuf;
   
   free(newBuf);
   return;

}

unsigned int CChineseCode::UTF8StrToUnicode( const char* UTF8String, unsigned int UTF8StringLength, wchar_t* OutUnicodeString, unsigned int UnicodeStringBufferSize )
       {
           unsigned int  UTF8Index = 0;
           unsigned int  UniIndex = 0;

           while ( UTF8Index < UTF8StringLength )
           {
               unsigned char UTF8Char = UTF8String[UTF8Index];

               if ( UnicodeStringBufferSize != 0 && UniIndex >= UnicodeStringBufferSize )
                   break;

               if ((UTF8Char & 0x80) == 0)
               {
                   const unsigned int  cUTF8CharRequire = 1;

                   // UTF8字码不足
                   if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                       break;

                   if ( OutUnicodeString )
                   {
                       wchar_t& WideChar = OutUnicodeString[UniIndex];

                       WideChar = UTF8Char;
                   }

                   UTF8Index++;

               }
               else if((UTF8Char & 0xE0) == 0xC0)  ///< 110x-xxxx 10xx-xxxx
               {
                   const unsigned int  cUTF8CharRequire = 2;

                   // UTF8字码不足
                   if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                       break;

                   if ( OutUnicodeString )
                   {
                       wchar_t& WideChar = OutUnicodeString[UniIndex];
                       WideChar  = (UTF8String[UTF8Index + 0] & 0x3F) << 6;
                       WideChar |= (UTF8String[UTF8Index + 1] & 0x3F);
                   }

                   UTF8Index += cUTF8CharRequire;
               }
               else if((UTF8Char & 0xF0) == 0xE0)  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
               {
                   const unsigned int  cUTF8CharRequire = 3;

                   // UTF8字码不足
                   if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                       break;

                   if ( OutUnicodeString )
                   {
                       wchar_t& WideChar = OutUnicodeString[UniIndex];

                       WideChar  = (UTF8String[UTF8Index + 0] & 0x1F) << 12;
                       WideChar |= (UTF8String[UTF8Index + 1] & 0x3F) << 6;
                       WideChar |= (UTF8String[UTF8Index + 2] & 0x3F);
                   }


                   UTF8Index += cUTF8CharRequire;
               }
               else if((UTF8Char & 0xF8) == 0xF0)  ///< 1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx
               {
                   const unsigned int  cUTF8CharRequire = 4;

                   // UTF8字码不足
                   if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                       break;

                   if ( OutUnicodeString )
                   {
                       wchar_t& WideChar = OutUnicodeString[UniIndex];

                       WideChar  = (UTF8String[UTF8Index + 0] & 0x0F) << 18;
                       WideChar  = (UTF8String[UTF8Index + 1] & 0x3F) << 12;
                       WideChar |= (UTF8String[UTF8Index + 2] & 0x3F) << 6;
                       WideChar |= (UTF8String[UTF8Index + 3] & 0x3F);
                   }

                   UTF8Index += cUTF8CharRequire;
               }
               else ///< 1111-10xx 10xx-xxxx 10xx-xxxx 10xx-xxxx 10xx-xxxx
               {
                   const unsigned int  cUTF8CharRequire = 5;

                   // UTF8字码不足
                   if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                       break;

                  if ( OutUnicodeString )
                  {
                       wchar_t& WideChar = OutUnicodeString[UniIndex];

                      WideChar  = (UTF8String[UTF8Index + 0] & 0x07) << 24;
                      WideChar  = (UTF8String[UTF8Index + 1] & 0x3F) << 18;
                      WideChar  = (UTF8String[UTF8Index + 2] & 0x3F) << 12;
                      WideChar |= (UTF8String[UTF8Index + 3] & 0x3F) << 6;
                      WideChar |= (UTF8String[UTF8Index + 4] & 0x3F);
                  }

                  UTF8Index += cUTF8CharRequire;
              }


              UniIndex++;
          }

          return UniIndex;
      }
