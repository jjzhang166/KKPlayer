#include <vector>
#include <queue>
#include "FlvEncode.h"
#include "../KKLock.h"
#ifndef AV_FLV_H
#define AV_FLV_H
	typedef struct _S_Data_Info{
		void *pData;
		int Len;
		int AVType; //0 视频，1音频，2其它
	}S_Data_Info;
	class CAV_Flv
	{
		   public:
				CAV_Flv(void);
				~CAV_Flv(void);
				void CreateFlvHeadrInfo();
				int LocalRecord(bool IsVideo,unsigned char* avbuffer,unsigned int avbufferlen,int avpts);
			private:
				/******准备好的数据*****/
				std::queue<S_Data_Info*> m_VideoData_Queue;
				S_Data_Info* pTempCache;
				
				CKKLock m_AVQueueLock;
			
		
				FILE* m_pFlvFile;
				bool m_LocalAVFirst;
				bool m_LocalAVOk;
			
				int MetaDataPos;
				int MetaDataTagLen;
				KKMEDIA::METADATA MetaData;
				int m_OpenState;

				//前一个tag长度
				int m_PreTagLen;
				//重新计算
				int m_ReCalPreTagLen;
				//临时存储数据用
				std::vector<S_Data_Info*> m_Data_Vect;
				S_Data_Info* pVideoInfo;
				S_Data_Info* pAudioInfo;
				bool SPSFist;
	};
#endif