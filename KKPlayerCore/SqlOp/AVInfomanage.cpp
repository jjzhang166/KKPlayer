#include "../stdafx.h"
#include "AVInfomanage.h"
#include "SqliteOp.h"
CAVInfoManage::CAVInfoManage()
{
    m_pDbOp = new CSqliteOp();
	memset(m_strDb,0,256);
}
void CAVInfoManage::SetPath(char *Path)
{
	strcpy(m_strDb,Path);
}
CAVInfoManage::~CAVInfoManage()
{
    CSqliteOp *DbOp =(CSqliteOp *)m_pDbOp;
    delete DbOp;
}

CAVInfoManage *CAVInfoManage::GetInance()
{
	if(m_pInance==NULL)
	{
		m_pInance= new CAVInfoManage();
	}
	return m_pInance;
}
CAVInfoManage *CAVInfoManage::m_pInance=NULL;

void CAVInfoManage::InitDb()
{
  sqlite3* pDb;
  CSqliteOp *pDbOp =(CSqliteOp *)m_pDbOp;
  pDbOp->OpenDB(m_strDb,&pDb);
  m_pDb=pDb;

 
}