#pragma once
#include <string>

class CBase64
{
public:
CBase64();  
    ~CBase64();  
  
    /********************************************************* 
    * 函数说明：将输入数据进行base64编码 
    * 参数说明：[in]pIn      需要进行编码的数据 
                [in]uInLen  输入参数的字节数 
                [out]strOut 输出的进行base64编码之后的字符串 
    * 返回值  ：true处理成功,false失败 
    * 作  者  ：ChenLi 
    * 编写时间：2009-02-17 
    **********************************************************/  
    bool static Encode(const unsigned char *pIn, unsigned long uInLen, std::string& strOut);  
  
   
      
    /********************************************************* 
    * 函数说明：将输入数据进行base64解码 
    * 参数说明：[in]strIn        需要进行解码的数据 
                [out]pOut       输出解码之后的节数数据 
                [out]uOutLen    输出的解码之后的字节数长度 
    * 返回值  ：true处理成功,false失败 
    * 作  者  ：ChenLi 
    * 编写时间：2009-02-17 
    **********************************************************/  
    bool static Decode(const std::string& strIn, unsigned char *pOut, unsigned long *uOutLen) ;  
  
    /********************************************************* 
    * 函数说明：将输入数据进行base64解码 
    * 参数说明：[in]strIn        需要进行解码的数据 
                [out]pOut       输出解码之后的节数数据 
                [out]uOutLen    输出的解码之后的字节数长度 
    * 返回值  ：true处理成功,false失败 
    * 作  者  ：ChenLi 
    * 编写时间：2009-02-17 
    **********************************************************/  
   // bool static Decode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen) ;
private:
	 /********************************************************* 
    * 函数说明：将输入数据进行base64编码 
    * 参数说明：[in]pIn          需要进行编码的数据 
                [in]uInLen      输入参数的字节数 
                [out]pOut       输出的进行base64编码之后的字符串 
                [out]uOutLen    输出的进行base64编码之后的字符串长度 
    * 返回值  ：true处理成功,false失败 
    * 作  者  ：ChenLi 
    * 编写时间：2009-02-17 
    **********************************************************/  
    bool static Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);  
};

//void CreateGuid(CString &strguid)
//{
//	char buf[64] = {0};  
//	GUID guid;  
//	if (S_OK == ::CoCreateGuid(&guid))  
//	{  
//		_snprintf(buf, sizeof(buf)  
//			, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"  
//			, guid.Data1  
//			, guid.Data2  
//			, guid.Data3  
//			, guid.Data4[0], guid.Data4[1]  
//		, guid.Data4[2], guid.Data4[3]
//		, guid.Data4[4], guid.Data4[5]  
//		, guid.Data4[6], guid.Data4[7]  
//		);  
//	}
//	strguid.Format("%s",buf);
//}
