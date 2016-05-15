#ifndef CFileMgr_H
#define CFileMgr_H
class CFileMgr
{
     public:
		    CFileMgr();
		    //判断文件是否存在
		    bool IsExist(LPCTSTR Path);
			//如果目录不存在则会自动创建
			bool CreateDirectory(LPCTSTR Path);
};
#endif