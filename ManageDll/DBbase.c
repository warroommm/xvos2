#include "DBbase.h"

char *errmsg ="aaaa";
static sqlite3 * db = 0;  
int result = 0;  

sqlite3 * DBbase()
{
	  
 	result = sqlite3_open("D:\\testVos.db", &db); // 连接数据库
   	if ( result != SQLITE_OK )
    {
    	fprintf(stderr, "无法打开数据库: %s", sqlite3_errmsg(db));
  	}
	return db;
}


void CreateDB()
{
	result = sqlite3_exec( db, "create table if not exists MyTable_1( ID integer primary key autoincrement, name nvarchar(32) ,attribute nvarchar(32),parents nvarchar(32),picID integer,irecord integer)", NULL, NULL, &errmsg );
	if(result != SQLITE_OK )
	{
   		printf( "创建表失败，错误码:%d，错误原因:%s \n", result, errmsg );
	}
}

void InsertDB(char *named,char *attributed,char *parentsvalue,int picnum,int irecorded)
{
	char sql_insert[256]; 
	sprintf(sql_insert, "insert into MyTable_1(name,attribute,parents,picID,irecord)  values('%s','%s','%s','%d','%d');",named,attributed,parentsvalue,picnum,irecorded);
	result = sqlite3_exec( db, sql_insert, 0, 0, &errmsg );
	if(result != SQLITE_OK )
	{
    	printf( "插入记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
	}

}


void SelectDBTest()
{
	char **dbResult; //是 char ** 类型，两个*号 
	int nRow, nColumn; 
	result = sqlite3_get_table( db, "select * from MyTable_1", &dbResult, &nRow, &nColumn, &errmsg );
 	if( SQLITE_OK == result )
   {
		int i , j;
   		int index;
        //查询成功
        index = nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
        printf( "查到%d条记录\n", nRow );
 
        for(  i = 0; i < nRow ; i++ )
        {
             printf( "第 %d 条记录\n", i+1 );
             for( j = 0 ; j < nColumn; j++ )
             {
                  printf( "字段名:%s  > 字段值:%s\n",  dbResult[j], dbResult [index] );
                  ++index; // dbResult 的字段值是连续的，从第0索引到第 nColumn - 1索引都是字段名称，从第 nColumn 索引开始，后面都是字段值，它把一个二维的表（传统的行列表示法）用一个扁平的形式来表示
             }
             printf( "-------\n");
        }  
   }
}



int SelectDB()
{
	char **dbResult; //是 char ** 类型，两个*号 
	int nRow, nColumn; 
	int index=0;
	result = sqlite3_get_table( db, "select * from MyTable_1", &dbResult, &nRow, &nColumn, &errmsg );
 	if( SQLITE_OK == result )
   	{
       if(nRow>0)
	   {  
		  index =(nRow+1)*nColumn -1;
		  return atoi(dbResult[index]); 
	   }
	   else
		   return 1;
  	}
	return 0;
}


void UpdateDbTable(char *named,char *attributed)
{
	char sql_update[256];
	sprintf(sql_update, "update MyTable_1 set name = '%s'  where attribute = '%s';",named,attributed);
	result = sqlite3_exec( db, sql_update, 0, 0, &errmsg );
	if(result != SQLITE_OK )
	{
    	printf( "更新记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
	}
	
}

 
void DeleteDB(char *attributed)
{
	char sql_delete[256];
	sprintf(sql_delete, "delete from  MyTable_1 where  attribute like  \'%%%s%%\'",attributed);  
	result = sqlite3_exec( db, sql_delete, 0, 0, &errmsg );
	if(result != SQLITE_OK )
	{
    	printf( "删除记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
	}
}


void CloseDB()
{
	//关闭数据库
	sqlite3_close( db );
}

