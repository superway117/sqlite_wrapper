#include "db_cursor.h"
#include <tg_utility.h>

typedef struct
{
	DatabaseHandle* hdl;
	INT32 key_col;
}DBCursorPrivInfo;


#if 0
static BOOL  _db_cursor_delete_row(Cursor* thiz, INT32 row)
{
	DBCursorPrivInfo* db_cursor=(DBCursorPrivInfo*)thiz->priv_subclass ;
	return_val_if_fail(db_cursor &&&db_cursor->hdl& db_cursor->key_col>=0,FALSE);
	//db_delete(db_cursor->hdl, const CHAR * table_name, const DatabaseWhereClause * clause)
	return  cursor_delete_row(thiz,row);
}
#endif

static const CHAR* _db_cursor_get_string(const Cursor*  cursor,INT32 column_index)
{
	return_val_if_fail(cursor&&cursor->data_win,NULL);
	return  cursor_window_get_string_coerce(cursor->data_win,cursor->cur_row,column_index);
}
static const WCHAR* _db_cursor_get_string_16(const Cursor*  cursor,INT32 column_index)
{
	return_val_if_fail(cursor&&cursor->data_win,NULL);
	return  cursor_window_get_string_16_coerce(cursor->data_win,cursor->cur_row,column_index);
}

Cursor* db_cursor_create(INT32 col_num,BOOL read_only,DatabaseHandle* hdl) 
{
	  Cursor* cursor = cursor_create(col_num,read_only);
	  DBCursorPrivInfo* info=NULL;
	  return_val_if_fail(cursor, NULL);
	  cursor->priv_subclass = TG_CALLOC_V2(sizeof(DBCursorPrivInfo));
	  if(!cursor->priv_subclass )
	  {
	  	cursor_destroy(cursor);
		cursor=NULL;
	  }
	  //delete
	 // cursor->delete_row= _db_cursor_delete_row;
	  
	  //get string
	  cursor->get_string= _db_cursor_get_string;
	  cursor->get_string_16= _db_cursor_get_string_16;


	 //priv data
	  info =(DBCursorPrivInfo*)cursor->priv_subclass;
	  info->hdl = hdl;
	  info->key_col=-1;
	  
	  return cursor;
	 
}
#if 0
DatabaseHandle* db_cursor_get_db_handle(const Cursor* thiz)
{
	DBCursorPrivInfo* info=NULL;
	return_val_if_fail(thiz, NULL);
	DBCursorPrivInfo* info=(DBCursorPrivInfo*)thiz->priv_subclass ;
	return_val_if_fail(info, NULL);
	return info->hdl;
}

BOOL db_cursor_set_key_col(const Cursor* thiz,INT32 col)
{
	DBCursorPrivInfo* info=NULL;
	return_val_if_fail(thiz, FALSE);
	DBCursorPrivInfo* info=(DBCursorPrivInfo*)thiz->priv_subclass ;
	return_val_if_fail(info, FALSE);
	return_val_if_fail(col+1<=cursor_get_column_num(thiz),FALSE);
	info->key_col = col;
	return TRUE;
}
#endif