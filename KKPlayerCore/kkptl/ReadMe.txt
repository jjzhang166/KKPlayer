主要用于kkPlayer的p2p的支持。文件最大支持4G
关于kkv文件格式的说明。模拟flv文件存储(flv文件比较简单)。
1.文件2进制格式：小端字节。
2.文件头 KKV +ver(int32)。              3+4=7
         FileSize：uint32;               4
		 FileMd5:  uint32+strMd5;        4+32
		 FileName: uint32+strName;        
3.tag
      TagLen(int32):      4
	  SegId:int32:        4
	  StartPos:int32      4
	  EndPos:int32        4
	  md5:int32+str;      36
	  DataLen:int32:      4  56+59=115；
	  data;
	  
	 
	 32768
	 32768
	 
	 3478791
	 3478791
	 
	 3465974
	 3465974
	 
	 3123834
	 
	 
	 id：79
	 SegPos:115  03
	 startpos：2588672
	 endpos：2621439
	 
	 05359d0f410cdfae5392644874e7f55a
	 //32883
	 
	 id:17
	 SegPos:32939 91
	 
	  
	  

