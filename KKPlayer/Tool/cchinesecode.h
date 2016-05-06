#ifndef CCHINESECODE_H
#define CCHINESECODE_H
#include <string>
class CChineseCode{
public:
    CChineseCode();
public:
     
      static void GB2312ToUTF_8(std::string& pOut,char *pText, int pLen);//GB2312 转为 UTF-8
	  static void UTF_8ToGB2312(std::string &pOut, char *pText, int pLen);//UTF-8 转为 GB2312
	  static void  UTF_8ToGBK(std::string &pOut,char *pInUtf8) ; 
	  static std::string GBK_ToUTF8(const std::string& strGBK);
	  static  unsigned int UTF8StrToUnicode( const char* UTF8String, unsigned int UTF8StringLength, wchar_t* OutUnicodeString, unsigned int UnicodeStringBufferSize );

	  static void wcharTochar(const wchar_t *wchar, char *chr, int length)  
      {  
           WideCharToMultiByte( CP_ACP, 0, wchar, -1,  
           chr, length, NULL, NULL );  
      }  
	  static void charTowchar(const char *chr, wchar_t *wchar, int size)  
      {     
         MultiByteToWideChar( CP_ACP, 0, chr,  
         strlen(chr)+1, wchar, size/sizeof(wchar[0]) );  
      }
	  
private: 
	  
	  static void UnicodeToGB2312(char* pOut,wchar_t uData);  // 把Unicode 转换成 GB2312
      static void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);// GB2312 转换成　Unicode
	  static void UTF_8ToUnicode(wchar_t* pOut,char *pText);  // 把UTF-8转换成Unicode
	  static void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode 转换成UTF-8
};
#endif