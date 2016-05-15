#ifndef AVInfomanage_H_
#define AVInfomanage_H_
class CAVInfoManage
{
  public:
		 ~CAVInfoManage();
         void SetPath(char *Path) ;
		 static CAVInfoManage *GetInance();

		 void InitDb();
  private:
	     static CAVInfoManage *m_pInance;
	     CAVInfoManage();
	     void *m_pDb;
	     void *m_pDbOp;
		 char m_strDb[256];
};
#endif