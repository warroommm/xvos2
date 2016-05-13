#include "DBbase.h"

char *errmsg ="aaaa";
static sqlite3 * db = 0;  
int result = 0;  

sqlite3 * DBbase()
{
	  
 	result = sqlite3_open("D:\\testVos.db", &db); // �������ݿ�
   	if ( result != SQLITE_OK )
    {
    	fprintf(stderr, "�޷������ݿ�: %s", sqlite3_errmsg(db));
  	}
	return db;
}


void CreateDB()
{
	result = sqlite3_exec( db, "create table if not exists MyTable_1( ID integer primary key autoincrement, name nvarchar(32) ,attribute nvarchar(32),parents nvarchar(32),picID integer,irecord integer)", NULL, NULL, &errmsg );
	if(result != SQLITE_OK )
	{
   		printf( "������ʧ�ܣ�������:%d������ԭ��:%s \n", result, errmsg );
	}
}

void InsertDB(char *named,char *attributed,char *parentsvalue,int picnum,int irecorded)
{
	char sql_insert[256]; 
	sprintf(sql_insert, "insert into MyTable_1(name,attribute,parents,picID,irecord)  values('%s','%s','%s','%d','%d');",named,attributed,parentsvalue,picnum,irecorded);
	result = sqlite3_exec( db, sql_insert, 0, 0, &errmsg );
	if(result != SQLITE_OK )
	{
    	printf( "�����¼ʧ�ܣ�������:%d������ԭ��:%s \n", result, errmsg );
	}

}


void SelectDBTest()
{
	char **dbResult; //�� char ** ���ͣ�����*�� 
	int nRow, nColumn; 
	result = sqlite3_get_table( db, "select * from MyTable_1", &dbResult, &nRow, &nColumn, &errmsg );
 	if( SQLITE_OK == result )
   {
		int i , j;
   		int index;
        //��ѯ�ɹ�
        index = nColumn; //ǰ��˵�� dbResult ǰ���һ���������ֶ����ƣ��� nColumn ������ʼ��������������
        printf( "�鵽%d����¼\n", nRow );
 
        for(  i = 0; i < nRow ; i++ )
        {
             printf( "�� %d ����¼\n", i+1 );
             for( j = 0 ; j < nColumn; j++ )
             {
                  printf( "�ֶ���:%s  > �ֶ�ֵ:%s\n",  dbResult[j], dbResult [index] );
                  ++index; // dbResult ���ֶ�ֵ�������ģ��ӵ�0�������� nColumn - 1���������ֶ����ƣ��ӵ� nColumn ������ʼ�����涼���ֶ�ֵ������һ����ά�ı���ͳ�����б�ʾ������һ����ƽ����ʽ����ʾ
             }
             printf( "-------\n");
        }  
   }
}



int SelectDB()
{
	char **dbResult; //�� char ** ���ͣ�����*�� 
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
    	printf( "���¼�¼ʧ�ܣ�������:%d������ԭ��:%s \n", result, errmsg );
	}
	
}

 
void DeleteDB(char *attributed)
{
	char sql_delete[256];
	sprintf(sql_delete, "delete from  MyTable_1 where  attribute like  \'%%%s%%\'",attributed);  
	result = sqlite3_exec( db, sql_delete, 0, 0, &errmsg );
	if(result != SQLITE_OK )
	{
    	printf( "ɾ����¼ʧ�ܣ�������:%d������ԭ��:%s \n", result, errmsg );
	}
}


void CloseDB()
{
	//�ر����ݿ�
	sqlite3_close( db );
}

