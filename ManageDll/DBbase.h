
#ifndef __DBBASE_H__
#define __DBBASE_H__
#endif
#include "sqlite3.h"
#include <ansi_c.h>   


typedef struct _TabData{
	char 	szName[256];
	char 	szID[256];
	char 	szPID[256];
	int 	level;
	int		tick;
}TabData;



sqlite3 * DBbase();
void CreateDB();
void InsertDB(char *named,char *attributed,char *parents,int picnum,int irecord); 
void DeleteDB(char *attributed);
int SelectDB();
void CloseDB();
void UpdateDbTable(char *named,char *attributed); 

void SelectDBTest();
