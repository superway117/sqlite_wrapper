
#include "CUnit.h"
#include "Automated.h"

#include "../db_wrapper.h"
#include <common.h>
#include <tg_utility.h>
#include "../db_util.h"
//for open folder test
#include <tgfile.h>

//static const char *s_file_list_schema_str = "CREATE TABLE file_list_tb (ID INTEGER PRIMARY KEY, filename TEXT NOT NULL COLLATE defaultcollate);";
static const char *s_db_wrapper_test_file_list_schem = "CREATE TABLE file_list_tb (ID INTEGER PRIMARY KEY, filename TEXT NOT NULL,size INTEGER DEFAULT 0);";
static const char *s_db_wrapper_test_file_list_schem1 = "CREATE TABLE file_list_tb1 (ID INTEGER PRIMARY KEY,size DEFAULT 0, filename TEXT NOT NULL);";
static const char *  s_db_wrapper_test_file_list_table="file_list_tb";
static const char *  s_db_wrapper_test_file_list_table1="file_list_tb1";
static const char *  s_db_wrapper_test_file_list_name_field="filename";
static const char *  s_db_wrapper_test_file_list_size_field="size";

static const char *s_db_wrapper_test_stmt_file_list_schem = "CREATE TABLE file_list_stmt_tb (ID INTEGER PRIMARY KEY, filename TEXT NOT NULL,size INTEGER DEFAULT 0);";
static const char *  s_db_wrapper_test_stmt_file_list_table="file_list_stmt_tb";


static const char *s_db_wrapper_test_wchar_stmt_file_list_schem = "CREATE TABLE wchar_list_stmt_tb (ID INTEGER PRIMARY KEY, filename TEXT NOT NULL,size INTEGER DEFAULT 0);";
static const char *  s_db_wrapper_test_wchar_stmt_file_list_table="wchar_list_stmt_tb";
static int db_wrapper_test_suit_init(void)
{

    return 0;
}
static int db_wrapper_test_suit_clean(void)
{
//    tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void db_wrapper_open_folder_test1()  //this function is used to save origin file list
{
	INT32 ret = 0;
	INT32 db_ret=0;
	TG_FSDIRENTRY entry;
	TG_DIR* dir = tg_opendir("/storage_card");
	DatabaseHandle* hdl = NULL;
	ContentValues* values=NULL;
	INT32 tick1,tick2;
	return_if_fail(dir);
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
	{
		tg_closedir(dir);
		return;
	}


	db_ret=db_table_drop(hdl,s_db_wrapper_test_file_list_table1);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	db_ret=db_execSQL(hdl,s_db_wrapper_test_file_list_schem1);
	CU_ASSERT_EQUAL(db_ret,DB_OK);

	if(db_ret!=DB_OK)
	{
		tg_closedir(dir);
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	values = content_values_create();
	tick1=tg_os_GetTicks();
	while(ret==0)
	{
		INT32 filed_num=0;
		ret =  tg_readdir(dir, &entry);
		content_values_put_string(values,s_db_wrapper_test_file_list_name_field,entry.name);
		content_values_put_int(values,s_db_wrapper_test_file_list_size_field,entry.stat.file_size);
		CU_ASSERT_EQUAL(content_values_get_count(values,&filed_num),ContentValues_SUCC);
		CU_ASSERT_EQUAL(filed_num,2);
		if(ret==0)
		{

			db_ret=db_insert(hdl,s_db_wrapper_test_file_list_table1,values);
			CU_ASSERT_EQUAL(db_ret,DB_OK);
		}
	}
	tick2=tg_os_GetTicks();
	DB_PRINTF("db_wrapper_open_folder_test1 inset all use %d ticks",tick2-tick1);
	content_values_destroy(values);
	tg_closedir(dir);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_open_folder_test()
{
	INT32 ret = 0;
	INT32 db_ret=0;
	TG_FSDIRENTRY entry;
	TG_DIR* dir = tg_opendir("/storage_card");
	DatabaseHandle* hdl = NULL;
	ContentValues* values=NULL;
	INT32 tick1,tick2;
	return_if_fail(dir);
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
	{
		tg_closedir(dir);
		return;
	}
	db_ret=db_table_drop(hdl,s_db_wrapper_test_file_list_table);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	db_ret=db_execSQL(hdl,s_db_wrapper_test_file_list_schem);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
#if 0
	db_ret=db_table_drop(hdl,s_db_wrapper_test_file_list_table1);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	db_ret=db_execSQL(hdl,s_db_wrapper_test_file_list_schem1);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
#endif
	if(db_ret!=DB_OK)
	{
		tg_closedir(dir);
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	values = content_values_create();
	tick1=tg_os_GetTicks();
	while(ret==0)
	{
		INT32 filed_num=0;
		ret =  tg_readdir(dir, &entry);
		content_values_put_string(values,s_db_wrapper_test_file_list_name_field,entry.name);
		content_values_put_int(values,s_db_wrapper_test_file_list_size_field,entry.stat.file_size);
		CU_ASSERT_EQUAL(content_values_get_count(values,&filed_num),ContentValues_SUCC);
		CU_ASSERT_EQUAL(filed_num,2);
		if(ret==0)
		{
			db_ret=db_insert(hdl,s_db_wrapper_test_file_list_table,values);
			CU_ASSERT_EQUAL(db_ret,DB_OK);
			
		//	db_ret=db_insert(hdl,s_db_wrapper_test_file_list_table1,values);
		//	CU_ASSERT_EQUAL(db_ret,DB_OK);
		}
	}
	tick2=tg_os_GetTicks();
	DB_PRINTF("db_wrapper_open_folder_test inset all use %d ticks",tick2-tick1);
	content_values_destroy(values);
	tg_closedir(dir);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_file_list_delete_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* clause = NULL;
	INT32 old_count=0;
	INT32 new_count =0;
	INT32 deleted=0;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
	{
		
		return;
	}

//delete 1
	deleted=0;
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&old_count);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	clause = db_clause_create(DB_WHERE_CLAUSE,"filename BETWEEN 'mmiheap_0%' AND 'mmiheap_9%'",0);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	db_ret=db_delete(hdl,s_db_wrapper_test_file_list_table,clause);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret==DB_OK)
	{
		deleted = db_last_changes(hdl);
		DB_PRINTF("db_wrapper_delete_file_item_test delete %d file(s)",deleted);
		db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&new_count);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		CU_ASSERT_EQUAL(deleted+new_count,old_count);
	}
	db_clause_destroy(clause);

//delete 2
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&old_count);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	clause = db_clause_create(DB_WHERE_CLAUSE,"size>5000",0);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	db_ret=db_delete(hdl,s_db_wrapper_test_file_list_table,clause);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret==DB_OK)
	{
		deleted = db_last_changes(hdl);
		DB_PRINTF("db_wrapper_delete_file_item_test delete %d file(s)",deleted);
		db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&new_count);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		CU_ASSERT_EQUAL(deleted+new_count,old_count);
	}
	db_clause_destroy(clause);


//delete 3
	deleted=0;
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&old_count);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	clause = db_clause_create(DB_WHERE_CLAUSE,"size>=? and size<=?",2);
	db_clause_put_int(clause,0,3000);
	db_clause_put_int(clause,1,4000);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	
	db_ret=db_delete(hdl,s_db_wrapper_test_file_list_table,clause);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret==DB_OK)
	{
		deleted = db_last_changes(hdl);
		DB_PRINTF("db_wrapper_delete_file_item_test delete %d file(s)",deleted);
		db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&new_count);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		CU_ASSERT_EQUAL(deleted+new_count,old_count);
	}
	db_clause_destroy(clause);
	

	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_file_list_query_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* clause = NULL;
	INT32 row_num=0;
	Cursor* cursor = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	clause = db_clause_create(DB_WHERE_CLAUSE,"size>5000",0);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&row_num);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	
	cursor= db_query(hdl ,s_db_wrapper_test_file_list_table,"ID,filename,size",clause,"filename",NULL);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");
		DB_PRINTF("db_wrapper_query_file_list_test  traveser query data");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 size;
				CHAR* filename =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				filename= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( filename);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
				if(filename)
				{
			//		DB_PRINTF("--------------------Row:%d-------------------",i);
			//		DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
					//DB_PRINTF("name:%s",filename);
				}
				else
					DB_PRINTF("ROW:%d error,can not find filename",i);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}
	       else
	       {
	       	DB_PRINTF("db_wrapper_query_file_list_test  no data");
	       }
		
	   
	}

	db_clause_destroy(clause);
	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}
static void db_wrapper_file_list_query_v2_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* where_clause = NULL;

	INT32 row_num=0;
	Cursor* cursor = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	where_clause = db_where_clause_create("size>1",0);
	if(!where_clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&row_num);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
//	db_clause_put_int(where_clause,0,5000);
	cursor= db_query_v2(hdl ,s_db_wrapper_test_file_list_table,"ID,filename,size",where_clause,NULL,NULL,"size","3,50");
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");
		DB_PRINTF("db_wrapper_file_list_query_v2_test  traveser query data");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 size;
				CHAR* filename =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				filename= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( filename);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
				if(filename)
				{
			//		DB_PRINTF("--------------------Row:%d-------------------",i);
			//		DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
					//DB_PRINTF("name:%s",filename);
				}
				else
					DB_PRINTF("ROW:%d error,can not find filename",i);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}
	       else
	       {
	       	DB_PRINTF("db_wrapper_file_list_query_v2_test  no data");
	       }
		
	   
	}

	db_clause_destroy(where_clause);
	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}
static void db_wrapper_file_list_string_query_v2_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* where_clause = NULL;

	INT32 row_num=0;
	Cursor* cursor = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	where_clause = db_where_clause_create("filename = 'db_wrapper_test.db'",0);
	if(!where_clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&row_num);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
//	db_clause_put_int(where_clause,0,5000);
	cursor= db_query_v2(hdl ,s_db_wrapper_test_file_list_table,"ID,filename,size",where_clause,NULL,NULL,"size",NULL);
	db_ret = db_table_get_count_v2(hdl,s_db_wrapper_test_file_list_table,"filename = 'db_wrapper_test.db'",&row_num);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");
		DB_PRINTF("db_wrapper_file_list_query_v2_test  traveser query data");
		CU_ASSERT_EQUAL(read_num,row_num);
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 size;
				CHAR* filename =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				filename= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( filename);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
				if(filename)
				{
			//		DB_PRINTF("--------------------Row:%d-------------------",i);
			//		DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
					//DB_PRINTF("name:%s",filename);
				}
				else
					DB_PRINTF("ROW:%d error,can not find filename",i);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}
	       else
	       {
	       	DB_PRINTF("db_wrapper_file_list_query_v2_test  no data");
	       }
		
	   
	}

	db_clause_destroy(where_clause);
	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}


static void db_wrapper_file_list_raw_query_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* clause = NULL;
	INT32 row_num=0;
	Cursor* cursor = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	clause = db_clause_create(DB_WHERE_CLAUSE,"size>5000",0);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&row_num);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	
	cursor= db_raw_query(hdl ,"SELECT ID,filename,size FROM file_list_tb WHERE  filename BETWEEN 'mmiheap_0%' AND 'mmiheap_9%';");
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");
		DB_PRINTF("db_wrapper_query_file_list_test  traveser query data");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 size;
				CHAR* filename =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				filename= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( filename);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
				if(filename)
				{
				//	DB_PRINTF("--------------------Row:%d-------------------",i);
				//	DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
				}
				else
					DB_PRINTF("ROW:%d error,can not find filename",i);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}
	       else
	       {
	       	DB_PRINTF("db_wrapper_query_file_list_test  no data");
	       }
		
	   
	}

	db_clause_destroy(clause);
	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_file_list_update_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* clause = NULL;
	INT32 row_num=0;
	Cursor* cursor = NULL;
	ContentValues* values = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	clause = db_clause_create(DB_WHERE_CLAUSE,"filename BETWEEN 'mmiheap_0%' AND 'mmiheap_9%'",0);
	
	if(!clause )
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_file_list_table,&row_num);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	
	cursor= db_query(hdl ,s_db_wrapper_test_file_list_table,"ID,filename,size",clause,"filename",NULL);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CHAR new_name[50];
		values = content_values_create();
		
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");
		DB_PRINTF("db_wrapper_file_list_update_test  traveser query data");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 size;
				CHAR* filename =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				filename= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( filename);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
				if(filename)
				{
				//	DB_PRINTF("--------------------Row:%d-------------------",i);
				//	DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
					//DB_PRINTF("name:%s",filename);
				}
				else
					DB_PRINTF("ROW:%d error,can not find filename",i);
				i++;
				sprintf(new_name,"ID=%d",id);
				CU_ASSERT_EQUAL(content_values_put_string(values,"filename",new_name),ContentValues_SUCC);
				CU_ASSERT_EQUAL(db_clause_set_statement(clause,new_name),TRUE);
				CU_ASSERT_EQUAL(db_update(hdl,s_db_wrapper_test_file_list_table,values,clause),DB_OK);
				
			}while(cursor_move_to_next(cursor));
		}
	       else
	       {
	       	DB_PRINTF("db_wrapper_file_list_update_test  no data");
	       }
	   	content_values_destroy(values);
	}
	
	
	db_clause_destroy(clause);
	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_file_list_db_status_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	
	INT32 page_count=0;
	
	INT32 version=0;
	INT32 page_size=0;

	INT64 max_size = 0;
	
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	CU_ASSERT_EQUAL(db_set_version(hdl,1),DB_OK);
	CU_ASSERT_EQUAL(db_get_version(hdl,&version),DB_OK);
	CU_ASSERT_EQUAL(version,1);

	CU_ASSERT_EQUAL(db_set_page_size(hdl,1024),DB_OK);
	CU_ASSERT_EQUAL(db_get_page_size(hdl,&page_size),DB_OK);
	CU_ASSERT_EQUAL(page_size,1024);

	CU_ASSERT_EQUAL(db_set_max_page_count(hdl,100),DB_OK);
	CU_ASSERT_EQUAL(db_get_max_page_count(hdl,&page_count),DB_OK);
	CU_ASSERT_EQUAL(page_count,100);

	CU_ASSERT_EQUAL(db_set_maximum_size(hdl,110*1024),DB_OK);
	CU_ASSERT_EQUAL(db_get_maximum_size(hdl,&max_size),DB_OK);
	CU_ASSERT_EQUAL(max_size,110*1024);

       DB_PRINTF("db_wrapper_db_status_file_list_test mem highwater=%d",db_memory_highwater());
	DB_PRINTF("db_wrapper_db_status_file_list_test mem used=%d",db_memory_used());
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}
static CU_TestInfo db_wrapper_filebrowser_testcases[] =
{ 
    {"db_wrapper_open_folder_test1:", db_wrapper_open_folder_test1},
    {"db_wrapper_open_folder_test:", db_wrapper_open_folder_test},   //insert
    {"db_wrapper_file_list_query_test:", db_wrapper_file_list_query_test},  //query
    {"db_wrapper_file_list_query_v2_test:", db_wrapper_file_list_query_v2_test},  //query
    {"db_wrapper_file_list_string_query_v2_test:", db_wrapper_file_list_string_query_v2_test},
    {"db_wrapper_file_list_raw_query_test:", db_wrapper_file_list_raw_query_test},  //raw query
    {"db_wrapper_file_list_update_test",db_wrapper_file_list_update_test},  //update
    {"db_wrapper_file_list_delete_test:", db_wrapper_file_list_delete_test},  //delete
    {"db_wrapper_file_list_db_status_test:", db_wrapper_file_list_db_status_test},  //delete
    CU_TEST_INFO_NULL
};

static void db_wrapper_stmt_open_folder_test()
{
	INT32 ret = 0;
	INT32 db_ret=0;
	TG_FSDIRENTRY entry;
	TG_DIR* dir = tg_opendir("/storage_card");
	DatabaseHandle* hdl = NULL;
	ContentValues* values=NULL;
	INT32 tick1,tick2;
	DatabaseStatement* stmt = NULL;
	return_if_fail(dir);
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
	{
		tg_closedir(dir);
		return;
	}
	db_ret=db_table_drop(hdl,s_db_wrapper_test_stmt_file_list_table);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	db_ret=db_execSQL(hdl,s_db_wrapper_test_stmt_file_list_schem);
	CU_ASSERT_EQUAL(db_ret,DB_OK);

	if(db_ret!=DB_OK)
	{
		tg_closedir(dir);
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	values = content_values_create();
	stmt = db_stmt_build_insert_statement(hdl,s_db_wrapper_test_stmt_file_list_table,"filename,size",2);
	CU_ASSERT_EQUAL((stmt!=NULL),TRUE);
	if(!stmt)
	{
		tg_closedir(dir);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	tick1=tg_os_GetTicks(); 
	while(ret==0 && db_ret == DB_OK)
	{
		INT32 filed_num=0;
		ret =  tg_readdir(dir, &entry);
		content_values_put_string(values,s_db_wrapper_test_file_list_name_field,entry.name);
		content_values_put_int(values,s_db_wrapper_test_file_list_size_field,entry.stat.file_size);
		CU_ASSERT_EQUAL(content_values_get_count(values,&filed_num),ContentValues_SUCC);
		CU_ASSERT_EQUAL(filed_num,2);
		db_ret = db_stmt_bind_content_values( stmt,values);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		if(ret==0 && db_ret == DB_OK)
		{
			db_ret=db_stmt_step(stmt);
			CU_ASSERT_EQUAL(db_ret,DB_OK);
		}
	}
	tick2=tg_os_GetTicks();
	DB_PRINTF("db_wrapper_stmt_open_folder_test inset all use %d ticks",tick2-tick1);
	CU_ASSERT_EQUAL(db_stmt_finalize(stmt),DB_OK);
	content_values_destroy(values);
	tg_closedir(dir);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_stmt_file_list_delete_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* clause = NULL;
	INT32 old_count=0;
	INT32 new_count =0;
	INT32 deleted=0;
	DatabaseStatement* stmt = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
	{
		
		return;
	}

//delete 1
	deleted=0;
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&old_count);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	clause = db_clause_create(DB_WHERE_CLAUSE,"filename BETWEEN 'mmiheap_0%' AND 'mmiheap_9%'",0);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	stmt = db_stmt_build_delete_statement(hdl,s_db_wrapper_test_stmt_file_list_table,clause);
//	db_ret = db_stmt_bind_clause( stmt,clause);
	db_ret = db_stmt_step(stmt);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	CU_ASSERT_EQUAL(db_stmt_finalize(stmt),DB_OK);
	if(db_ret==DB_OK)
	{
		
		deleted = db_last_changes(hdl);
		DB_PRINTF("db_wrapper_stmt_file_list_delete_test delete %d file(s)",deleted);
		db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&new_count);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		CU_ASSERT_EQUAL(deleted+new_count,old_count);
	}
	db_clause_destroy(clause);


//delete 2
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&old_count);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	clause = db_clause_create(DB_WHERE_CLAUSE,"size>5000",0);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	stmt = db_stmt_build_delete_statement(hdl,s_db_wrapper_test_stmt_file_list_table,clause);
//	CU_ASSERT_EQUAL(db_stmt_bind_clause(stmt,clause),DB_OK);
	db_ret = db_stmt_step(stmt);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	CU_ASSERT_EQUAL(db_stmt_finalize(stmt),DB_OK);
	
	if(db_ret==DB_OK)
	{
		deleted = db_last_changes(hdl);
		DB_PRINTF("db_wrapper_stmt_file_list_delete_test delete %d file(s)",deleted);
		db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&new_count);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		CU_ASSERT_EQUAL(deleted+new_count,old_count);
	}
	db_clause_destroy(clause);


//delete 3
	deleted=0;
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&old_count);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	clause = db_clause_create(DB_WHERE_CLAUSE,"size>=? and size<=?",2);
	db_clause_put_int(clause,0,3000);
	db_clause_put_int(clause,1,4000);
	if(!clause)
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	stmt = db_stmt_build_delete_statement(hdl,s_db_wrapper_test_stmt_file_list_table,clause);
	CU_ASSERT_EQUAL(db_stmt_bind_clause(stmt,clause),DB_OK);
	db_ret = db_stmt_step(stmt);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	CU_ASSERT_EQUAL(db_stmt_finalize(stmt),DB_OK);
	
//	db_ret=db_delete(hdl,s_db_wrapper_test_stmt_file_list_table,clause);
	
	if(db_ret==DB_OK)
	{
		deleted = db_last_changes(hdl);
		DB_PRINTF("db_wrapper_stmt_file_list_delete_test delete %d file(s)",deleted);
		db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&new_count);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		CU_ASSERT_EQUAL(deleted+new_count,old_count);
	}
	db_clause_destroy(clause);
	

	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}



static void db_wrapper_stmt_file_list_update_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	DatabaseWhereClause* clause = NULL;
	INT32 row_num=0;
	Cursor* cursor = NULL;
	ContentValues* values = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	clause = db_clause_create(DB_WHERE_CLAUSE,"filename BETWEEN 'mmiheap_0%' AND 'mmiheap_9%'",0);
	
	if(!clause )
	{
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	db_ret = db_table_get_count(hdl,s_db_wrapper_test_stmt_file_list_table,&row_num);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	
	cursor= db_query(hdl ,s_db_wrapper_test_stmt_file_list_table,"ID,filename,size",clause,NULL,NULL);
	if(cursor && cursor_get_count(cursor)>0)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CHAR new_name[50];
		DatabaseStatement* stmt = NULL;
		values = content_values_create();
		
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");

		
		do
		{
			INT64 id;
			INT64 size;
			CHAR* filename =NULL;
			CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
			filename= cursor_get_string(cursor,1);
			CU_ASSERT_PTR_NOT_NULL( filename);
			CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
			if(filename)
			{
			//	DB_PRINTF("--------------------Row:%d-------------------",i);
			//	DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
				//DB_PRINTF("name:%s",filename);
			}
			else
				DB_PRINTF("db_wrapper_stmt_file_list_update_test ROW:%d error,can not find filename",i);
			i++;
			sprintf(new_name,"ID=%d",id);
			CU_ASSERT_EQUAL(content_values_put_string(values,"filename",new_name),ContentValues_SUCC);
			CU_ASSERT_EQUAL(db_clause_set_statement(clause,new_name),TRUE);
		//	CU_ASSERT_EQUAL(db_update(hdl,s_db_wrapper_test_file_list_table1,values,clause),DB_OK);
			if(!stmt)
			{
				stmt = db_stmt_build_update_statement(hdl,s_db_wrapper_test_stmt_file_list_table,values,clause);
				if(!stmt)
				{
					DB_PRINTF("db_wrapper_stmt_file_list_update_test create stmt fail");
					break;
				}
				db_ret =db_stmt_bind_content_and_clause_values(stmt,values,clause);
				CU_ASSERT_EQUAL(db_ret,DB_OK);
				db_ret = db_stmt_step(stmt);
				CU_ASSERT_EQUAL(db_ret,DB_OK);
				
				
			}
			
		}while(cursor_move_to_next(cursor));
	      
		CU_ASSERT_EQUAL(db_stmt_finalize(stmt),DB_OK);
	   	content_values_destroy(values);
	}
	
	
	db_clause_destroy(clause);
	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_stmt_file_list_db_status_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;
	
	INT32 page_count=0;
	
	INT32 version=0;
	INT32 page_size=0;

	INT64 max_size = 0;
	
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	CU_ASSERT_EQUAL(db_set_version(hdl,2),DB_OK);
	CU_ASSERT_EQUAL(db_get_version(hdl,&version),DB_OK);
	CU_ASSERT_EQUAL(version,2);

	CU_ASSERT_EQUAL(db_set_page_size(hdl,1024),DB_OK);
	CU_ASSERT_EQUAL(db_get_page_size(hdl,&page_size),DB_OK);
	CU_ASSERT_EQUAL(page_size,1024);

	CU_ASSERT_EQUAL(db_set_max_page_count(hdl,120),DB_OK);
	CU_ASSERT_EQUAL(db_get_max_page_count(hdl,&page_count),DB_OK);
	CU_ASSERT_EQUAL(page_count,120);

	CU_ASSERT_EQUAL(db_set_maximum_size(hdl,140*1024),DB_OK);
	CU_ASSERT_EQUAL(db_get_maximum_size(hdl,&max_size),DB_OK);
	CU_ASSERT_EQUAL(max_size,140*1024);

       DB_PRINTF("db_wrapper_db_status_file_list_test mem highwater=%d",db_memory_highwater());
	DB_PRINTF("db_wrapper_db_status_file_list_test mem used=%d",db_memory_used());
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_stmt_wchar_insert()
{
	INT32 ret = 0;
	INT32 db_ret=0;

	
	DatabaseHandle* hdl = NULL;
	ContentValues* values=NULL;
	INT32 tick1,tick2;
	DatabaseStatement* stmt = NULL;

	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	db_ret=db_table_drop(hdl,s_db_wrapper_test_wchar_stmt_file_list_table);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	db_ret=db_execSQL(hdl,s_db_wrapper_test_wchar_stmt_file_list_schem);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	
	values = content_values_create();
	stmt = db_stmt_build_insert_statement(hdl,s_db_wrapper_test_wchar_stmt_file_list_table,"filename,size",2);
	CU_ASSERT_EQUAL((stmt!=NULL),TRUE);
	if(!stmt)
	{

		content_values_destroy(values);
		CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
		return;
	}
	tick1=tg_os_GetTicks(); 
      //while(ret==0 && db_ret == DB_OK)
	{
		INT32 filed_num=0;

		content_values_put_string_16(values,s_db_wrapper_test_file_list_name_field,L"Ó¢·ÉÁè");
		content_values_put_int(values,s_db_wrapper_test_file_list_size_field,100);
		CU_ASSERT_EQUAL(content_values_get_count(values,&filed_num),ContentValues_SUCC);
		CU_ASSERT_EQUAL(filed_num,2);
		db_ret = db_stmt_bind_content_values( stmt,values);
		CU_ASSERT_EQUAL(db_ret,DB_OK);
		if(ret==0 && db_ret == DB_OK)
		{
			db_ret=db_stmt_step(stmt);
			CU_ASSERT_EQUAL(db_ret,DB_OK);
		}
	}
	tick2=tg_os_GetTicks();
	DB_PRINTF("db_wrapper_stmt_wchar_insert inset all use %d ticks",tick2-tick1);
	CU_ASSERT_EQUAL(db_stmt_finalize(stmt),DB_OK);
	content_values_destroy(values);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}

static void db_wrapper_stmt_wchar_query()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl = NULL;

	INT32 row_num=0;
	Cursor* cursor = NULL;
	db_ret=db_open("/storage_card/db_wrapper_test.db",&hdl);
	CU_ASSERT_EQUAL(db_ret,DB_OK);
	if(db_ret!=DB_OK)
		return;
	
	
	cursor= db_raw_query(hdl ,"SELECT ID,filename,size FROM wchar_list_stmt_tb;");
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"filename");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"size");
		DB_PRINTF("db_wrapper_query_file_list_test  traveser query data");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 size;
				WCHAR* filename =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				filename = cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL(filename);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,2,&size),TRUE);
				if(filename)
				{
				//	DB_PRINTF("--------------------Row:%d-------------------",i);
				//	DB_PRINTF4("ID:%d,Size:%d,Name:%s",(INT32)id,(INT32)size,filename);
				}
				else
					DB_PRINTF("ROW:%d error,can not find filename",i);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}
	       else
	       {
	       	DB_PRINTF("db_wrapper_stmt_wchar_query  no data");
	       }
		
	   
	}


	cursor_destroy( cursor);
	CU_ASSERT_EQUAL(db_close(hdl),DB_OK);
}
static void db_wrapper_multiopen_conflict_test()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl_1 = NULL;
       DatabaseHandle* hdl_2 = NULL;
	db_ret=db_open("/nvram/db_wrapper_test.db",&hdl_1);
	DB_PRINTF("db_wrapper_multiopen_conflict_test open 1 ret=%d",db_ret);
	db_ret=db_open("/nvram/db_wrapper_test.db",&hdl_2);
	DB_PRINTF("db_wrapper_multiopen_conflict_test open 2 ret=%d",db_ret);
	CU_ASSERT_EQUAL(db_close(hdl_1),DB_OK);
	CU_ASSERT_EQUAL(db_close(hdl_2),DB_OK);
}

static void db_wrapper_multiopen_conflict_test1()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl_1 = NULL;
       DatabaseHandle* hdl_2 = NULL;
	db_ret=db_open_v2("/nvram/db_wrapper_test.db",DB_OPEN_READONLY,&hdl_1);
	DB_PRINTF("db_wrapper_multiopen_conflict_test1 open 1 ret=%d",db_ret);
	db_ret=db_open_v2("/nvram/db_wrapper_test.db",DB_OPEN_READWRITE,&hdl_2);
	DB_PRINTF("db_wrapper_multiopen_conflict_test1 open 2 ret=%d",db_ret);

	CU_ASSERT_EQUAL(db_close(hdl_1),DB_OK);
	CU_ASSERT_EQUAL(db_close(hdl_2),DB_OK);
}

static void db_wrapper_multiopen_conflict_test2()
{

	INT32 db_ret=0;
	
	DatabaseHandle* hdl_1 = NULL;
       DatabaseHandle* hdl_2 = NULL;
	db_ret=db_open_v2("/nvram/db_wrapper_test.db",DB_OPEN_READWRITE,&hdl_1);
	DB_PRINTF("db_wrapper_multiopen_conflict_test1 open 1 ret=%d",db_ret);
	db_ret=db_open_v2("/nvram/db_wrapper_test.db",DB_OPEN_READWRITE,&hdl_2);
	DB_PRINTF("db_wrapper_multiopen_conflict_test1 open 2 ret=%d",db_ret);

	CU_ASSERT_EQUAL(db_close(hdl_1),DB_OK);
	CU_ASSERT_EQUAL(db_close(hdl_2),DB_OK);
}
static CU_TestInfo db_wrapper_stmt_filebrowser_testcases[] =
{ 
    {"db_wrapper_open_folder_test:", db_wrapper_stmt_open_folder_test},   //insert
    {"db_wrapper_stmt_file_list_update_test",db_wrapper_stmt_file_list_update_test},  //update
    {"db_wrapper_stmt_file_list_delete_test:", db_wrapper_stmt_file_list_delete_test},  //delete
    {"db_wrapper_file_list_db_status_test:", db_wrapper_stmt_file_list_db_status_test},  //status
    CU_TEST_INFO_NULL
};

static CU_TestInfo db_wrapper_stmt_wchar_testcases[] =
{ 
    {"db_wrapper_stmt_wchar_insert:", db_wrapper_stmt_wchar_insert},   //insert
    {"db_wrapper_stmt_wchar_query",db_wrapper_stmt_wchar_query},  //update
    CU_TEST_INFO_NULL
};
static CU_TestInfo db_wrapper_conflict_testcases[] =
{ 
    {"db_wrapper_multiopen_conflict_test:", db_wrapper_multiopen_conflict_test},   //
     {"db_wrapper_multiopen_conflict_test1:", db_wrapper_multiopen_conflict_test1},   //
     {"db_wrapper_multiopen_conflict_test2:", db_wrapper_multiopen_conflict_test2},   //
    CU_TEST_INFO_NULL
};
static CU_SuiteInfo db_wrapper_suites[] = {
    {"Testing db_wrapper_filebrowser_testcases:", NULL, NULL, db_wrapper_filebrowser_testcases},
     {"Testing db_wrapper_stmt_filebrowser_testcases:", NULL, NULL, db_wrapper_stmt_filebrowser_testcases},
	 {"Testing db_wrapper_stmt_wchar_testcases:", NULL, NULL, db_wrapper_stmt_wchar_testcases},
	 {"Testing db_wrapper_conflict_testcases:", NULL, db_wrapper_test_suit_clean, db_wrapper_conflict_testcases},
    CU_SUITE_INFO_NULL
};


static void db_wrapper_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(db_wrapper_suites))
    {
        return;
    }
}

void db_wrapper_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        db_wrapper_add_tests();
        CU_set_output_filename("db_wrapper_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}
