
#include "db_clause.h"
#include <tg_utility.h>
extern char *tg_strdup( const char *src );
struct _DatabaseClause
{
  DatabaseClauseType type;
  CHAR* statement;
  CursorWindow* win;
};

void db_clause_destroy(DatabaseClause*  clause)
{
	return_if_fail(clause);
	TG_FREE(clause->statement);
	cursor_window_destroy(clause->win);
	TG_FREE(clause);
}

DatabaseClause* db_clause_create(DatabaseClauseType type,CHAR* statement,INT32 value_num)
{
	DatabaseClause* clause = NULL;
	return_val_if_fail(statement, NULL);
	clause = TG_CALLOC_V2(sizeof(DatabaseClause));
	return_val_if_fail(clause, NULL);
	clause->statement = tg_strdup(statement);
	if(!clause->statement )
	{
		TG_FREE(clause);
		return NULL;
	}
	if(value_num>0)
	{
		clause->win = cursor_window_create(value_num);
		if(!cursor_window_alloc_row(clause->win))
		{
			db_clause_destroy(clause);
			return NULL;
		}
	}
	return clause;
	
}

DatabaseClause* db_clause_duplicate(DatabaseClause* src)
{
	DatabaseClause* clause = NULL;
	return_val_if_fail(src, NULL);
	clause = TG_CALLOC_V2(sizeof(DatabaseClause));
	return_val_if_fail(clause, NULL);
	clause->statement = tg_strdup(src->statement);
	if(!clause->statement )
	{
		TG_FREE(clause);
		return NULL;
	}
	clause->win = cursor_window_duplicate(src->win);
	
	return clause;
	
}

DatabaseClause* db_where_clause_create(CHAR* statement,INT32 value_num)
{
	return db_clause_create(DB_WHERE_CLAUSE,statement,value_num);
}

DatabaseClause* db_having_clause_create(CHAR* statement,INT32 value_num)
{
	return db_clause_create(DB_HAVING_CLAUSE,statement,value_num);
}

const CHAR* db_clause_get_statement(DatabaseClause*  thiz)
{
	return_val_if_fail(thiz, NULL);
	return thiz->statement;
}
BOOL db_clause_set_statement(DatabaseClause*  thiz,const CHAR* statement)
{
	return_val_if_fail(thiz, FALSE);
	TG_FREE(thiz->statement);
	thiz->statement = tg_strdup(statement);
	return thiz->statement?TRUE:FALSE;
}

BOOL db_clause_append_where(DatabaseClause*  thiz,const CHAR* where_str)
{
	return_val_if_fail(thiz&&where_str, FALSE);
	if(!thiz->statement)
		thiz->statement = tg_strdup(where_str);
	else
	{
		INT32 len = strlen(thiz->statement)+strlen(where_str)+8;
		CHAR* new_statement = TG_CALLOC_V2(len);
		return_val_if_fail(new_statement, FALSE);
		sprintf(new_statement,"%s AND %s",thiz->statement,where_str);
		TG_FREE(thiz->statement);
		thiz->statement = new_statement;
	}
	return thiz->statement?TRUE:FALSE;
}

INT32 db_clause_get_data_count(DatabaseClause*  thiz)
{
	return_val_if_fail(thiz && thiz->win, 0);
	return cursor_window_get_col_num(thiz->win);
}

BOOL db_clause_get_data(DatabaseClause*  thiz,INT32 idx,UINT8* type,INT32* len,void** data)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_get_value(thiz->win,0,idx,type,data,len);
}

BOOL db_clause_put_int(DatabaseClause* thiz,INT32 col,INT64 value)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_put_int(thiz->win,col,value);
}
BOOL db_clause_put_double(DatabaseClause* thiz,INT32 col,double value)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_put_double(thiz->win,col,value);
}

BOOL db_clause_put_string(DatabaseClause* thiz,INT32 col,const CHAR* value)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_put_string(thiz->win,col,value);
}

BOOL db_clause_put_string_16(DatabaseClause* thiz,INT32 col,const WCHAR* value)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_put_string_16(thiz->win,col,value);
}
BOOL db_clause_put_blob(DatabaseClause* thiz,INT32 col,const void* value,INT32 len)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_put_blob(thiz->win,col,value,len);
}

BOOL db_clause_get_int(DatabaseClause* thiz,INT32 col,INT64* value)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_get_int(thiz->win,0,col,value);
}
BOOL db_clause_get_double(DatabaseClause* thiz,INT32 col,double* value)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_get_double(thiz->win,0,col,value);
}


const CHAR* db_clause_get_string(DatabaseClause* thiz,INT32 col)
{
	return_val_if_fail(thiz&&thiz->win, NULL);
	return cursor_window_get_string(thiz->win,0,col);
	
}


const WCHAR* db_clause_get_string_16(DatabaseClause* thiz,INT32 col)
{
	return_val_if_fail(thiz&&thiz->win, NULL);
	return cursor_window_get_string_16(thiz->win,0,col);
	
}
#if 0
BOOL db_clause_get_blob(DatabaseClause* thiz,INT32 row,INT32 col,void** value,INT32* len)
{
	return_val_if_fail(thiz&&thiz->win, FALSE);
	return cursor_window_get_blob(thiz->win,0,col,value,len);
}
#endif

const void* db_clause_get_blob(DatabaseClause* thiz,INT32 row,INT32 col,INT32* len)
{
	return_val_if_fail(thiz&&thiz->win, NULL);
	return cursor_window_get_blob(thiz->win,0,col,len);
	
}


