/*************************************
functions:修改文件名，只支持短文件名的修改
params:pfi指向信息集合的指针，old_name_path包括文件所在的路径，也支持修改文件目录比如
"/123.txt"或者"/a/b"这样的意思就是修改123.txt,或者修改文件目录b的名字,new_name就是要修改的文件或者目录，
必须"345.txt"就是把123.txt修改为345.txt 。由于支持修改通配符的，所以可能匹配到多个要修改的，这里的n就是要修改的第几个
is_file 是不是要修改的是文件，比如上边的/123.txt则is_file为1 /a/b is_file就为0
返回：成功则为0
*************************************/
#ifdef ZNFAT_MODIFY_NAME
UINT8 znFAT_Modify_Name(struct FileInfo *pfi,INT8 *old_name_path,INT8 *new_name,UINT32 n,UINT8 is_file)
{
	UINT8 result=0,flag=0;
	UINT32 sec_temp=0,Cur_Cluster=0,fn_pos=0,item=0;//fn_pos是文件名所在filename中的位置
	UINT32 iSec=0,iFDI=0;
	INT8 temp_filename[13];
	INT8 *filename;
	UINT8 fn_len=0,i=0,j=0,dot_pos=0,have_dot=0,findFile=0;
	struct FDIesInSEC *pitems;
	struct FDI * pitem;
	just_file=pfi;
	result=znFAT_Enter_Dir(old_name_path,&Cur_Cluster,&fn_pos); //获取路径最后一级目录的开始簇
	if(result) return result;
	filename=old_name_path+fn_pos;
	if(Check_Illegal_Char(filename) || Check_Illegal_Char(new_name)) return ERR_ILL_CHAR; //检查文件名中是否有非法字符，无论长名还是短名，或是通配名 
	if(!Is_WildFileName(filename)) //如果不是通配文件名，即确定名，则需要进行文件名合法性检测
	{
		if(Check_SFN_Illegal_Length(filename)) return ERR_SFN_ILL_LEN; //检查SFN是否符合8.3长度
		if(Check_SFN_Dot(filename)) return ERR_SFN_DOT; //检查SFN中.是否合法 
		if(Check_SFN_Special_Char(filename)) return ERR_SFN_SPEC_CHAR; //检查SFN中是否有特殊字符
		if(((UINT8)(-1))==Check_SFN_Illegal_Lower(filename)) return ERR_SFN_ILL_LOWER; //检查SFN中是否有非法的大小写
	}
	if(!Is_WildFileName(new_name)) //如果不是通配文件名，即确定名，则需要进行文件名合法性检测
	{
		if(Check_SFN_Illegal_Length(new_name)) return ERR_SFN_ILL_LEN; //检查SFN是否符合8.3长度
		if(Check_SFN_Dot(new_name)) return ERR_SFN_DOT; //检查SFN中.是否合法 
		if(Check_SFN_Special_Char(new_name)) return ERR_SFN_SPEC_CHAR; //检查SFN中是否有特殊字符
		if(((UINT8)(-1))==Check_SFN_Illegal_Lower(new_name)) return ERR_SFN_ILL_LOWER; //检查SFN中是否有非法的大小写
	}
	do
	{
		sec_temp=SOC(Cur_Cluster);
		for(iSec=0;iSec<(pInit_Args->SectorsPerClust);iSec++)
		{
			znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer);
			pitems=(struct FDIesInSEC *)znFAT_Buffer;
			for(iFDI=0;iFDI<NFDI_PER_SEC;iFDI++) //访问扇区中各文件目录项
				{
						pitem=&(pitems->FDIes[iFDI]); //指向一个文件目录项数据
						if((is_file?CHK_ATTR_FILE(pitem->Attributes):CHK_ATTR_DIR(pitem->Attributes)) 
							&& (0XE5!=pitem->Name[0]) && ('.'!=pitem->Name[0])) //依is_file检查属性，且没有被删除
																				//不是.与..
					{
						 To_File_Name((INT8 *)(pitem->Name),temp_filename); //将FDI中的文件名字段转为8.3文件名
						  if(!SFN_Match(filename,temp_filename)) //短文件名通配
						  {
						   if(n==item)
						   {
							Analyse_FDI(pfi,pitem); //解析匹配的文件目录项
							pfi->FDI_Sec=sec_temp+iSec; //文件目录项所在的扇区
							pfi->nFDI=(UINT8)iFDI; //文件目录项在扇区中的索引
							findFile=1;
						   } 
						   flag=1;
						  }
						 if(flag) {item++;flag=0;} 
						 if(!SFN_Match(new_name,temp_filename)) //匹配到了
						 {
							return ERR_FDI_ALREADY_EXISTING;
						 }
					}
				}
		}
	 Cur_Cluster=Get_Next_Cluster(Cur_Cluster); //获取下一簇
	}while(!IS_END_CLU(Cur_Cluster));
	//修改在这里改
	if(findFile==0) return ERR_NO_FILE;
	znFAT_Device_Read_Sector((unsigned long)pfi->FDI_Sec,znFAT_Buffer);//读取扇区
	pitems=(struct FDIesInSEC*)znFAT_Buffer;
	pitem=&(pitems->FDIes[pfi->nFDI]);
	//pitem->Name=;
	//pitem->Extension=;
	fn_len=(UINT8)StringLen(new_name);
	for(i=(UINT8)(fn_len-1);i>0;i--) //反向寻找. 第一个.是主文件与扩展名的分界
	{
	  if('.'==new_name[i]) 
	  {
	   dot_pos=i;
	   have_dot=1;
	   break;
	  }
	}
	 if(have_dot) //如果有点
	 {
		  //填入主文件名
		  for(i=0;i<dot_pos;i++)
		  {
		   (pitem->Name)[i]=(INT8)Lower2Up(pitem[i]); //转为大写
		  }
		  for(;i<8;i++)
		  {
		   (pitem->Name)[i]=' '; //不足8字节部分填入空格
		  }

		  //填入扩展名
		  for(i=(UINT8)(dot_pos+1);i<fn_len;i++)
		  {
		   (pitem->Extension)[j]=(UINT8)Lower2Up(pitem[i]); //转为大写
		   j++;
		  }
		  for(;j<3;j++)
		  {
		   (pitem->Extension)[j]=' '; //不足3字节部分填入空格
		  }
	 }
	  else //如果没有点
	 {
		  //填入主文件名
		  for(i=0;i<fn_len;i++)
		  {
		   (pfdi->Name)[i]=(UINT8)Lower2Up(pitem[i]); //转为大写
		  }
		  for(;i<8;i++)
		  {
		   (pfdi->Name)[i]=' '; //不足8字节部分填入空格
		  } 
		  
		  //填入扩展名
		  for(j=0;j<3;j++)
		  {
		   (pfdi->Extension)[j]=' '; //扩展名填入空格
		  }
	 }
	znFAT_Device_Write_Sector((unsigned long)pfi->FDI_Sec,znFAT_Buffer);//读取扇区
	return ERR_SUCC;
}
#endif

/*******************************************
功能:查询一个文件夹下边有多少个文件夹
形参:num是返回的文件数量，dirpath要查询的文件目录/a/b/c就是查询c目录下的文件数量
********************************************/
#ifdef ADDFAT_FIND_FILE_NUM
UINT8 addFAT_Find_File_Num(UINT32* num,INT8 * dirpath)
{
	UINT8 result;
	UINT32 Cur_Cluster=0,fn_pos=0,sec_temp=0;
	UINT32 iSec=0,iFDI=0;
	struct FDIesInSEC * pitems;
	*num=0;
	result=znFAT_Enter_Dir(dirpath,&Cur_Cluster,&fn_pos);
	if(result) return result;
	do
	{
		sec_temp=SOC(Cur_Cluster);
		for(iSec=0;i<(pInit_Args->SectorsPerClust);iSec++)
		{
			//read one cluster file num
			znFAT_Device_Read_Sector(sec_temp+(UINT32)iSec,znFAT_Buffer);
			pitems=(struct FDIesInSEC *)znFAT_Buffer;
			for(iFDI=0;i<NFDI_PER_SEC;iFDI++)
			{
				pitems=&(pitems->FDIes[iFDI]);
				if((pitems->Name[0]!=0xE5) && (CHK_ATTR_FILE(pitem->Attributes)) && ('.'!=pitem->Name[0]))
				{
					*num=*num+1;
				}else if(pitem->Name[0]==0x00)
				{
					break;    
				}	
			}
		}
		Cur_Cluster=Get_Next_Cluster(Cur_Cluster); 
	}
	while(!IS_END_CLU(Cur_Cluster));
	if(fn_pos==0)
	{
		*num=*num-3;
	}
	return ERR_SUCC;
}
#endif