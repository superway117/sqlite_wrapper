
#include "db_wrapper.h"
#include <sqlite3.h>
#include "db_util.h"
#include "db_cursor.h"
#include <printbuf.h>
#include <json_object.h>
#include <tg_utility.h>

#define DB_GET_PERFORMANCE 1

#define DB_MAX_QUERY_ROWS_ONE_TIME   5000
#define DB_MAX_HEAP_SIZE_FOR_ONE_DB   1024

struct _DatabaseHandle
{
    sqlite3* sqlite;
};

struct _DatabaseStatement
{
    sqlite3_stmt * sqlite_stmt;
};
static INT32 _db_error_code_convert(INT32 sqlite_err)
{
    if (SQLITE_DONE==sqlite_err)
        return DB_OK;
    return sqlite_err;
}

static INT32 _db_print_explain_query_plan(sqlite3* sqlite,const CHAR* zSql);
static INT32 _db_print_explain(sqlite3* sqlite,const CHAR* zSql);
static void _db_analyze(sqlite3* sqlite);



INT32 db_open(const CHAR* path,DatabaseHandle** hdl_ouput)
{
    DatabaseHandle* hdl = TG_CALLOC_V2(sizeof(DatabaseHandle));
    INT32 sqlite_err;
    return_val_if_fail(hdl,DB_NOMEM);
    sqlite_err =    sqlite3_open(path,(sqlite3 **)&hdl->sqlite);
    if (DB_OK != sqlite_err)
    {
        TG_FREE(hdl);
    }
    else
    {
        *hdl_ouput = hdl;
        sqlite3_soft_heap_limit(DB_MAX_HEAP_SIZE_FOR_ONE_DB);
    }
    return _db_error_code_convert(sqlite_err);
}


INT32 db_open_v2(const CHAR* path, INT32 flags,DatabaseHandle** hdl_ouput)
{
    DatabaseHandle* hdl = TG_CALLOC_V2(sizeof(DatabaseHandle));
    INT32 sqlite_err;
    return_val_if_fail(hdl,DB_NOMEM);
    sqlite_err =    sqlite3_open_v2(path,(sqlite3 **)&hdl->sqlite,flags,NULL);
    if (DB_OK != sqlite_err)
    {
        TG_FREE(hdl);
    }
    else
    {
        *hdl_ouput = hdl;
        sqlite3_soft_heap_limit(DB_MAX_HEAP_SIZE_FOR_ONE_DB);
    }

    return _db_error_code_convert(sqlite_err);
}

INT32 db_close(DatabaseHandle* hdl)
{
    INT32 sqlite_err;
    return_val_if_fail((hdl && hdl->sqlite),DB_ERROR);
    sqlite_err=sqlite3_close((sqlite3 *)hdl->sqlite);
    if (sqlite_err==DB_OK)
        TG_FREE(hdl);
    return _db_error_code_convert(sqlite_err);
}

INT32 _db_execSQL_impl(sqlite3* sqlite,const CHAR* sql_str,DB_CALLBACK callback,void* arg)
{
    CHAR* errmsg = NULL;
    INT32 sqlite_err;
    return_val_if_fail((sqlite),SQLITE_OK);
    sqlite_err= sqlite3_exec(sqlite,sql_str,(sqlite3_callback)callback,arg,&errmsg);
    if (SQLITE_OK!=sqlite_err)
    {
        DB_PRINTF("_db_execSQL_impl failure-error:%d; msg:%s\n",sqlite_err,errmsg);
        sqlite3_free(errmsg);

    }
    return sqlite_err;

}

INT32 db_execSQL_v2(DatabaseHandle* hdl,const CHAR* sql_str,DB_CALLBACK callback,void* arg)
{

    return_val_if_fail((hdl && hdl->sqlite),DB_ERROR);
    return _db_error_code_convert(_db_execSQL_impl(hdl->sqlite,sql_str,callback,arg));


}

INT32 db_execSQL(DatabaseHandle* hdl,const CHAR* sql_str)
{
    return_val_if_fail((hdl && hdl->sqlite),DB_ERROR);
    return _db_error_code_convert(_db_execSQL_impl(hdl->sqlite,sql_str,NULL,NULL));
}


static INT32 _db_simple_execSQL_for_int(void* arg,INT32 count ,char** values, char** columns)
{
    INT32* output_int = (INT32*)arg;
    DB_ASSERT(count > 0);
    *output_int = atoi(values[0]);
    return 0;
}
INT32 db_simple_execSQL_for_int(DatabaseHandle* hdl,const CHAR* sql_str,INT32* output_int)
{
    return db_execSQL_v2(hdl,sql_str,_db_simple_execSQL_for_int,(void*)output_int);
}

BOOL db_table_is_exist(DatabaseHandle* hdl,const CHAR* table_name)

{
    INT32 err = DB_OK;

    CHAR* sql_str = NULL;
    INT32 count=0;
    return_val_if_fail((hdl && hdl->sqlite && table_name),DB_ERROR);
    sql_str = (char*)TG_CALLOC_V2(100+strlen(table_name));
    return_val_if_fail(sql_str,DB_NOMEM);

    sprintf(sql_str,"SELECT count(*) FROM sqlite_master WHERE type='table' and name='%s';",table_name);
    err = db_simple_execSQL_for_int(hdl,sql_str,&count);
    TG_FREE(sql_str);
    return count!=0;
}


static INT32 _db_drop_impl(DatabaseHandle* hdl,const CHAR* tb_type,const CHAR* table_name)
{
    INT32 sqlite_err = DB_OK;
    CHAR* erro_msg = NULL;
    CHAR* sql_str = NULL;
    return_val_if_fail((hdl && hdl->sqlite && table_name),DB_ERROR);
    sql_str = (char*)TG_CALLOC_V2(100+strlen(table_name));
    return_val_if_fail(sql_str,DB_NOMEM);

    sprintf(sql_str,"DROP %s IF EXISTS %s",tb_type,table_name);
    sqlite_err=sqlite3_exec((sqlite3 *)hdl->sqlite,sql_str,NULL,NULL,&erro_msg);
    TG_FREE(sql_str);
    if (DB_OK!=sqlite_err)
    {
        DB_PRINTF("_db_drop_impl failure-error:%d; msg:%s\n",sqlite_err,erro_msg);
        sqlite3_free(erro_msg);

    }
    return _db_error_code_convert(sqlite_err);
}

INT32 db_table_drop(DatabaseHandle* hdl,const CHAR* table_name)
{
    return _db_drop_impl(hdl,"TABLE",table_name);
}

INT32 db_index_drop(DatabaseHandle* hdl,const CHAR* index_name)
{
    return _db_drop_impl(hdl,"INDEX",index_name);
}

INT32 db_view_drop(DatabaseHandle* hdl,const CHAR* view_name)
{
    return _db_drop_impl(hdl,"VIEW",view_name);
}

INT32 db_table_get_count_v2(DatabaseHandle* hdl,const CHAR* table_name,const CHAR* where_clause,INT32* count)
{
    INT32 ret = DB_OK;
    CHAR* sql_str = NULL;
    INT32 len = 0;
    return_val_if_fail(table_name,DB_ERROR);
    len = 64+strlen(table_name);
    if (where_clause)
        len+=strlen(where_clause);
    sql_str = (char*)TG_CALLOC_V2(len);
    return_val_if_fail(sql_str,DB_NOMEM);
    if (where_clause)
        sprintf(sql_str,"SELECT count(rowid) FROM %s WHERE %s;",table_name,where_clause);
    else
        sprintf(sql_str,"SELECT count(*) FROM %s;",table_name);
    ret = db_simple_execSQL_for_int(hdl,sql_str,count);
    TG_FREE(sql_str);
    return ret;
}

INT32 db_table_get_count(DatabaseHandle* hdl,const CHAR* table_name,INT32* count)
{
    return db_table_get_count_v2(hdl,table_name,NULL,count);
}



INT32 db_last_changes(DatabaseHandle* hdl)
{

    return_val_if_fail((hdl &&hdl->sqlite ),-1);
    return sqlite3_changes(hdl->sqlite);
}

INT64 db_last_insert_rowid(DatabaseHandle* hdl)
{
    return_val_if_fail((hdl &&hdl->sqlite ),-1);

    return sqlite3_last_insert_rowid(hdl->sqlite);
}

INT32 db_last_error(DatabaseHandle* hdl)
{
    return_val_if_fail((hdl &&hdl->sqlite ),-1);

    return _db_error_code_convert(sqlite3_errcode(hdl->sqlite));
}

//for db statement
static DatabaseStatement* _db_stmt_create_impl(sqlite3_stmt * sqlite_stmt)
{
    DatabaseStatement* stmt = NULL;
    return_val_if_fail(( sqlite_stmt),NULL);
    stmt=(DatabaseStatement*)TG_CALLOC_V2(sizeof(DatabaseStatement));
    return_val_if_fail(( stmt),NULL);
    stmt->sqlite_stmt = sqlite_stmt;
    return stmt;
}

void db_stmt_destroy(DatabaseStatement* stmt)
{
    TG_FREE(stmt);
}

static sqlite3_stmt *   _db_stmt_compile_statement( DatabaseHandle* hdl,const CHAR* sql_string)
{
    sqlite3 * sqlite_hdl = (sqlite3 *)hdl->sqlite;
    INT32 sqlite_err;
    sqlite3_stmt * stmt=NULL;

    sqlite_err=sqlite3_prepare_v2(sqlite_hdl,sql_string,strlen(sql_string)+1,&stmt,NULL);
    if ((DB_OK!=sqlite_err))
    {
        DB_PRINTF("db_compile_statement failure-error:%d; msg:%s; sql:%s\n",sqlite_err,sqlite3_errmsg(sqlite_hdl),sql_string);
        return NULL;
    }
    return (DB_OK == sqlite_err)?stmt:NULL;

}

DatabaseStatement* db_stmt_build_statement( DatabaseHandle* hdl,const CHAR* sql_string)
{

    sqlite3_stmt * stmt = NULL;
    DatabaseStatement* db_stmt= NULL;
    return_val_if_fail(( hdl && sql_string ),NULL);

    stmt=_db_stmt_compile_statement(hdl,sql_string);
    return_val_if_fail((stmt),NULL);

    db_stmt=_db_stmt_create_impl(stmt);
    if (!db_stmt)
    {
        sqlite3_finalize(stmt);
    }
    return db_stmt;
}

static printbuf*  _db_stmt_build_insert_statement_impl(const CHAR* table,const CHAR* col_list,INT32 col_num)
{
    //(contact_urn,tag_urn,value) values('%d','%d','%s')
    printbuf* pb =  NULL;

    pb =  printbuf_new_v2(256);
    return_val_if_fail((pb), NULL);
    sprintbuf(pb,"INSERT INTO %s(%s) VALUES(?",table,col_list);
    col_num--;
    while (col_num--)
    {
        printbuf_memappend(pb,",?",2);
    }
    printbuf_memappend(pb,");",2);


    return pb;

}

DatabaseStatement* db_stmt_build_insert_statement( DatabaseHandle* hdl,const CHAR* table_name,const CHAR* col_list,INT32 col_num)
{

    sqlite3_stmt * stmt = NULL;
    printbuf*  pb = NULL;
    DatabaseStatement* db_stmt= NULL;
    return_val_if_fail(( table_name && col_list && col_num>0),NULL);

    pb=_db_stmt_build_insert_statement_impl(table_name,col_list,col_num);
    return_val_if_fail((pb),NULL);

    stmt=_db_stmt_compile_statement(hdl,pb->buf);   //compile statememt
    printbuf_free(pb);
    return_val_if_fail((stmt),NULL);
    db_stmt=_db_stmt_create_impl(stmt);
    if (!db_stmt)
    {
        sqlite3_finalize(stmt);
    }
    return db_stmt;
}

static printbuf*  _db_stmt_build_delete_statement_impl(const CHAR* table,DatabaseClause* clause)
{
    //delete from table where
    printbuf* pb =  NULL;

    return_val_if_fail((table), NULL);

    pb =  printbuf_new_v2(256);
    return_val_if_fail((pb), NULL);
    if (clause)
    {
        const CHAR* clause_str = db_clause_get_statement(clause);
        DB_ASSERT((clause_str));
        sprintbuf(pb,"DELETE FROM %s WHERE %s",table,clause_str);
    }
    else
        sprintbuf(pb,"DELETE FROM %s",table);
    return pb;

}
DatabaseStatement* db_stmt_build_delete_statement( DatabaseHandle* hdl,const CHAR* table_name,DatabaseClause* clause)
{

    sqlite3_stmt * stmt = NULL;
    printbuf*  pb = NULL;
    DatabaseStatement* db_stmt= NULL;
    return_val_if_fail(( hdl && table_name ),NULL);

    pb=_db_stmt_build_delete_statement_impl(table_name,clause);
    return_val_if_fail((pb),NULL);

    stmt=_db_stmt_compile_statement(hdl,pb->buf);   //compile statememt
    printbuf_free(pb);
    return_val_if_fail((stmt),NULL);
    db_stmt=_db_stmt_create_impl(stmt);
    if (!db_stmt)
    {
        sqlite3_finalize(stmt);
    }
    return db_stmt;
}
#if 0
static printbuf*  _db_stmt_build_query_statement_impl(const CHAR* col_list,const CHAR* table,DatabaseClause* clause)
{
    //delete from table where
    printbuf* pb =  NULL;
    return_val_if_fail((table), NULL);

    pb =  printbuf_new_v2(256);
    return_val_if_fail((pb), NULL);
    if (clause)
    {
        sprintbuf(pb,"SELECT %s from %s WHERE %s",col_list,table,db_clause_get_statement(clause));

    }
    else
        sprintbuf(pb,"SELECT %s from %s",col_list,table);
    return pb;

}
#endif


static printbuf*  _db_stmt_build_update_statement_impl(const CHAR* table,ContentValues* values,DatabaseClause* clause)
{
    //(contact_urn,tag_urn,value) values('%d','%d','%s')
    printbuf* pb =  NULL;
    ContentValuesItor itor;
    BOOL  need_separator = FALSE;
    INT32 value_num;
    INT32 ret=0;
    return_val_if_fail((table&&values), NULL);
    ret=content_values_get_count(values,&value_num);
    return_val_if_fail((value_num>0 && ret==ContentValues_SUCC), NULL);
    pb =  printbuf_new_v2(256);
    return_val_if_fail((pb), NULL);
    sprintbuf(pb,"UPDATE %s SET ",table);

    for (itor=content_values_first(values);itor;itor=content_values_next(values,itor))
    {
        const CHAR* key = content_values_get_key(itor);
        if (need_separator)
            sprintbuf(pb, ",%s=?", key);
        else
            sprintbuf(pb, "%s=?", key);
        need_separator=TRUE;
    }

    if (clause)
    {
        const CHAR* clause_str = db_clause_get_statement(clause);
        DB_ASSERT((clause_str));
        sprintbuf(pb," WHERE %s",clause_str);
    }

    return pb;

}
DatabaseStatement* db_stmt_build_update_statement( DatabaseHandle* hdl,const CHAR* table_name,ContentValues* values,DatabaseClause* clause)
{

    sqlite3_stmt * stmt = NULL;
    printbuf*  pb = NULL;
    DatabaseStatement* db_stmt= NULL;
    return_val_if_fail(( hdl && table_name ),NULL);

    pb=_db_stmt_build_update_statement_impl(table_name,values,clause);
    return_val_if_fail((pb),NULL);

    stmt=_db_stmt_compile_statement(hdl,pb->buf);   //compile statememt
    printbuf_free(pb);
    return_val_if_fail((stmt),NULL);
    db_stmt=_db_stmt_create_impl(stmt);
    if (!db_stmt)
    {
        sqlite3_finalize(stmt);
    }
    return db_stmt;
}



static INT32  _db_stmt_bind_content_values_impl(sqlite3_stmt* stmt,ContentValues* values)
{

    ContentValuesItor itor;
    ContentValuesEntry_TYPE type;
    UINT32 len;
    INT32 value_num;
    INT32 ret=0;
    INT32 i;
    ret=content_values_get_count(values,&value_num);
    return_val_if_fail((value_num>0 && ret==ContentValues_SUCC), SQLITE_ERROR);
    for (i=0,itor=content_values_first(values);i<value_num && itor;i++,itor=content_values_next(values,itor))
    {
        const void* value = content_values_get_value(itor,&type,&len);
        if (value==NULL)
            ret=sqlite3_bind_null(stmt,i+1);
        else if (type==ContentValuesEntry_TYPE_INT)
            ret=sqlite3_bind_int(stmt,i+1,*((INT32*)value));
        else if (type==ContentValuesEntry_TYPE_DOUBLE)
            ret=sqlite3_bind_double(stmt,i+1,*((double*)value));
        else if (type==ContentValuesEntry_TYPE_BOOL)
        {
            BOOL b = *((BOOL*)value);
            INT32 value = 0;
            if (b)
                value = 1;
            ret=sqlite3_bind_int(stmt,i+1,value);
        }
        else if (type==ContentValuesEntry_TYPE_STRING)
        {
            if (len>0)
                len--;  //remove /0
            ret=sqlite3_bind_text(stmt,i+1,(const CHAR*)value,len,NULL);
        }
        else if (type==ContentValuesEntry_TYPE_STRING_16)
        {
            if (len>0)
                len-=2;  //remove /0
            ret=sqlite3_bind_text16(stmt,i+1,(const WCHAR*)value,len,NULL);
        }
        else if (type==ContentValuesEntry_TYPE_BLOB)
        {

            ret=sqlite3_bind_blob(stmt,i+1,value,len,NULL);
        }
    }
#if  DB_DEBUG
    if (DB_OK!=ret)
    {
        sqlite3 * sqlite_hdl = sqlite3_db_handle(stmt);
        DB_PRINTF("_db_stmt_bind_values failure-error:%d; msg:%s\n",ret,sqlite3_errmsg(sqlite_hdl));
    }
#endif

    return ret;

}

INT32  db_stmt_bind_content_values(DatabaseStatement* stmt,ContentValues* values)
{
    INT32 sqlite_err;
    return_val_if_fail((stmt && stmt->sqlite_stmt), DB_ERROR);
    return_val_if_fail(sqlite3_reset(stmt->sqlite_stmt)==DB_OK, DB_ERROR);

    sqlite_err= _db_stmt_bind_content_values_impl(stmt->sqlite_stmt,values);
    return _db_error_code_convert(sqlite_err);
}

static INT32  _db_stmt_bind_clause_impl(sqlite3_stmt* stmt,DatabaseClause* clause,INT32 start_pos)
{


    UINT8 type;
    INT32 len;
    INT32 value_num;
    BOOL ret=0;
    INT32 i;
    void* data= NULL;
    value_num=db_clause_get_data_count(clause);
    return_val_if_fail((value_num>0), 0);
    for (i=0;i<value_num ;i++)
    {
        INT32 pos=start_pos+i+1;
        ret = db_clause_get_data(clause,i,&type,&len,&data);
        if (data==NULL)
            ret=sqlite3_bind_null(stmt,pos);
        else if (type==FIELD_TYPE_INTEGER)
            ret=sqlite3_bind_int(stmt,pos,*((INT32*)data));
        else if (type==FIELD_TYPE_FLOAT)
            ret=sqlite3_bind_double(stmt,pos,*((double*)data));
        else if (type==FIELD_TYPE_BOOL)
        {
            BOOL b = *((BOOL*)data);
            INT32 value = 0;
            if (b)
                value = 1;
            ret=sqlite3_bind_int(stmt,pos,value);
        }
        else if (type==FIELD_TYPE_STRING)
        {
            if (len>0)
                len--;   //remove /0
            ret=sqlite3_bind_text(stmt,pos,(const CHAR*)data,len,NULL);
        }
        else if (type==FIELD_TYPE_STRING_16)
        {
            if (len>0)
                len-=2;    //remove /0
            ret=sqlite3_bind_text16(stmt,pos,(const WCHAR*)data,len,NULL);
        }
        else if (type==FIELD_TYPE_BLOB)
        {

            ret=sqlite3_bind_blob(stmt,pos,data,len,NULL);
        }
    }
#if  DB_DEBUG
    if (DB_OK!=ret)
    {
        sqlite3 * sqlite_hdl = sqlite3_db_handle(stmt);
        DB_PRINTF("_db_stmt_bind_clause_impl failure-error:%d; msg:%s\n",ret,sqlite3_errmsg(sqlite_hdl));
    }
#endif


    return ret;

}
INT32  db_stmt_bind_clause(DatabaseStatement* stmt,DatabaseClause* clause)
{
    INT32 sqlite_err;
    return_val_if_fail((stmt && stmt->sqlite_stmt), DB_ERROR);
    return_val_if_fail(sqlite3_reset(stmt->sqlite_stmt)==DB_OK, DB_ERROR);
    sqlite_err= _db_stmt_bind_clause_impl(stmt->sqlite_stmt,clause,0);
    return _db_error_code_convert(sqlite_err);
}

INT32  db_stmt_bind_content_and_clause_values(DatabaseStatement* stmt,ContentValues* values,DatabaseClause* clause)
{

    INT32 ret=db_stmt_bind_content_values(stmt,values);
    INT32 value_num = 0;
    INT32 sqlite_err;
    return_val_if_fail((ret==DB_OK), ret);

    content_values_get_count(values,&value_num);
    sqlite_err= _db_stmt_bind_clause_impl(stmt->sqlite_stmt,clause,value_num);
    return _db_error_code_convert(sqlite_err);

}


INT32  db_stmt_step(DatabaseStatement* stmt)
{
    INT32 sqlite_err;
    return_val_if_fail((stmt && stmt->sqlite_stmt), DB_ERROR);
    sqlite_err=sqlite3_step(stmt->sqlite_stmt);
#if DB_DEBUG
    if (SQLITE_DONE!=sqlite_err)
    {
        sqlite3 * sqlite_hdl = sqlite3_db_handle(stmt->sqlite_stmt);
        DB_PRINTF("db_stmt_step failure-error:%d; msg:%s\n",sqlite_err,sqlite3_errmsg(sqlite_hdl));
    }
#endif
    return _db_error_code_convert(sqlite_err);
}

INT32  db_stmt_finalize(DatabaseStatement* stmt)
{
    INT32 sqlite_err;
    return_val_if_fail((stmt && stmt->sqlite_stmt), DB_ERROR);
    sqlite_err=sqlite3_finalize(stmt->sqlite_stmt);
    if (DB_OK==sqlite_err)
    {
        TG_FREE(stmt);
    }
#if  DB_DEBUG
    else
    {
        sqlite3 * sqlite_hdl = sqlite3_db_handle(stmt->sqlite_stmt);
        DB_PRINTF("db_stmt_finalize failure-error:%d; msg:%s\n",sqlite_err,sqlite3_errmsg(sqlite_hdl));
    }
#endif
    return _db_error_code_convert(sqlite_err);
}




static printbuf*  _db_extract_col_list_from_content_values(ContentValues* values,INT32* count)
{
    //(contact_urn,tag_urn,value) values('%d','%d','%s')
    printbuf* pb =  NULL;
    ContentValuesItor itor;
    BOOL  need_separator = FALSE;
    INT32 value_num;
    INT32 ret;
    return_val_if_fail((values), NULL);
    ret=content_values_get_count(values,&value_num);
    return_val_if_fail((value_num>0 && ret==ContentValues_SUCC), NULL);
    pb =  printbuf_new_v2(256);
    return_val_if_fail((pb), NULL);

    for (itor=content_values_first(values);itor;itor=content_values_next(values,itor))
    {
        const CHAR* key = content_values_get_key(itor);
        if (need_separator)
            sprintbuf(pb, ",%s", key);
        else
            sprintbuf(pb, "%s", key);
        need_separator=TRUE;
    }
    *count = value_num;
    return pb;

}

INT32 db_insert(DatabaseHandle* hdl,const CHAR* table_name, ContentValues* values)
{
    INT32 ret = DB_OK;
    DatabaseStatement* stmt = NULL;
    printbuf*  col_list_pb = NULL;
    INT32 count = 0;
#if DB_GET_PERFORMANCE
    UINT32 t1 = db_get_ticks();
    UINT32 t2 = 0;
#endif
    return_val_if_fail((hdl&&hdl->sqlite && table_name && values),DB_ERROR);
    col_list_pb=_db_extract_col_list_from_content_values((ContentValues*)values,&count);
    return_val_if_fail(col_list_pb&&count>0,DB_ERROR);
    stmt= db_stmt_build_insert_statement( hdl, table_name,col_list_pb->buf,count);

    printbuf_free(col_list_pb);
    return_val_if_fail(stmt,DB_ERROR);
    ret=db_stmt_bind_content_values(stmt,(ContentValues*)values);   //bind  vaules
    if (ret!=DB_OK)
    {

        db_stmt_finalize(stmt);      //clear statememt
        return DB_ERROR;
    }

    ret=db_stmt_step(stmt);  //execute statememt
    ret=db_stmt_finalize(stmt);      //clear statememt
#if DB_GET_PERFORMANCE
    t2 = db_get_ticks();
    DB_PRINTF("db_insert use %d\n",t2-t1);
#endif
    return ret;
}


INT32 db_delete(DatabaseHandle* hdl,const CHAR* table_name, DatabaseWhereClause* clause)
{
    INT32 ret = DB_OK;

    DatabaseStatement* stmt = NULL;

#if DB_GET_PERFORMANCE
    UINT32 t1 = db_get_ticks();
    UINT32 t2 = 0;
#endif
    return_val_if_fail((hdl&&hdl->sqlite && table_name),DB_ERROR);

    stmt= db_stmt_build_delete_statement( hdl, table_name,clause);
    return_val_if_fail(stmt,DB_ERROR);

    ret=db_stmt_bind_clause(stmt,(DatabaseWhereClause*)clause);   //bind  vaules
    if (ret!=DB_OK)
    {

        db_stmt_finalize(stmt);      //clear statememt
        return DB_ERROR;
    }
    ret=db_stmt_step(stmt);  //execute statememt
    ret=db_stmt_finalize(stmt);      //clear statememt
#if DB_GET_PERFORMANCE
    t2 = db_get_ticks();
    DB_PRINTF("db_delete delete %d use %d\n",db_last_changes(hdl),t2-t1);
#endif
    return ret;
}

INT32 db_delete_all(DatabaseHandle* hdl,const CHAR* table_name)
{
    return db_delete(hdl,table_name,NULL);
}


INT32 db_update(DatabaseHandle* hdl,const CHAR* table_name, ContentValues* values, DatabaseWhereClause* clause)
{
    INT32 ret = DB_OK;

    DatabaseStatement* stmt = NULL;


    return_val_if_fail((hdl&&hdl->sqlite && table_name&&values&&clause),DB_ERROR);

    stmt= db_stmt_build_update_statement( hdl, table_name,values,clause);
    return_val_if_fail(stmt,DB_ERROR);

    ret=db_stmt_bind_content_and_clause_values(stmt,values,clause);   //bind  vaules
    if (ret!=DB_OK)
    {

        db_stmt_finalize(stmt);      //clear statememt
        return DB_ERROR;
    }
    ret=db_stmt_step(stmt);  //execute statememt
    ret=db_stmt_finalize(stmt);      //clear statememt

    return ret;
}

static INT32 _db_skip_rows(sqlite3_stmt *statement, int max_rows)
{
    INT32 retry_count = 0;
    INT32 i = 0;
    for (i = 0; i < max_rows; i++)
    {
        int err = sqlite3_step(statement);
        if (err == SQLITE_ROW)
        {
            // do nothing
        }
        else if (err == SQLITE_DONE)
        {
            return i;
        }
        else if (err == SQLITE_LOCKED || err == SQLITE_BUSY)
        {

            if (retry_count > 50)
            {

                break;
            }
            // Sleep to give the thread holding the lock a chance to finish
            UtaOsSleep(1000,1000);
            retry_count++;
            continue;
        }
        else
        {
            return -1;
        }
    }
    return max_rows;
}
static INT32 _db_finish_program_and_get_row_count(sqlite3_stmt *statement)
{
    INT32 num_rows = 0;
    INT32 retry_count = 0;
    while (TRUE)
    {
        INT32 err = sqlite3_step(statement);
        if (err == SQLITE_ROW){
            num_rows++;
        }
        else if (err == SQLITE_LOCKED || err == SQLITE_BUSY)
        {
            // The table is locked, retry
            DB_PRINTF("_db_finish_program_and_get_row_count Database locked, retrying");
            if (retry_count > 50)
            {
                DB_PRINTF("_db_finish_program_and_get_row_count Bailing on database busy rety");
                break;
            }
            // Sleep to give the thread holding the lock a chance to finish
            UtaOsSleep(1000,1000);
            retry_count++;
            continue;
        }
        else
        {
            // no need to throw exception
            break;
        }
    }
    sqlite3_reset(statement);
    DB_PRINTF("_db_finish_program_and_get_row_count row %d", num_rows);
    return num_rows;
}
static INT32 _db_fill_data_window( DatabaseHandle* hdl, sqlite3_stmt* statement,CursorWindow * window,INT32 start_pos,INT32 offset_param,INT32 max_read,INT32 last_pos)
{
    INT32 err;
    INT32 num_rows = last_pos;

    INT32 num_columns;
    INT32 retry_count;

#if DB_GET_PERFORMANCE
    UINT32 t1 = db_get_ticks();
    UINT32 t2,t3,t4;
#endif




    return_val_if_fail((hdl &&hdl->sqlite && window),-1);
    max_read += last_pos;

    // Only do the binding if there is a valid offsetParam. If no binding needs to be done
    // offsetParam will be set to 0, an invliad value.
    if (offset_param > 0)
    {
        // Bind the offset parameter, telling the program which row to start with
        err = sqlite3_bind_int(statement, offset_param, start_pos);
        if (err != DB_OK)
        {

            DB_PRINTF("_db_fill_window failure-error:%d; msg:%s\n",err,sqlite3_errmsg(hdl->sqlite));
            return -1;
        }

    }

    num_columns = sqlite3_column_count(statement);


    retry_count = 0;
    if (start_pos > 0)
    {
        INT32 num = _db_skip_rows(statement, start_pos);
        if (num < 0)
        {
            DB_PRINTF("_db_fill_window failure-error:db skip rows failure\n");
            return -1;
        }
        else if (num < start_pos)
        {
            DB_PRINTF("_db_fill_window failure-error:start_pos %d > actual skip rows %d\n",start_pos, num);
            return -1;
        }
    }

    while (start_pos != 0 || num_rows < max_read)
    {
        INT32 i;



        // DB_PRINTF("_db_fill_data_window start step row %d", start_pos + num_rows);
#if DB_GET_PERFORMANCE
        t2 = db_get_ticks();
#endif
        err = sqlite3_step(statement);


#if DB_GET_PERFORMANCE
        if (num_rows==0)
        {
            t3 = db_get_ticks();

            DB_PRINTF("_db_fill_window step 0 use %d\n",t3-t2);
        }
#endif


        if (err == SQLITE_ROW)
        {
            // DB_PRINTF("_db_fill_data_window Stepped row %d", start_pos + num_rows);
            retry_count = 0;

            // Allocate a new field directory for the row. This pointer is not reused
            // since it mey be possible for it to be relocated on a call to alloc() when
            // the field data is being allocated.
            if (!cursor_window_alloc_row(window))
            {
                DB_PRINTF("_db_fill_data_window Failed allocating row at start_pos %d row %d", start_pos, num_rows);
                return start_pos + num_rows + _db_finish_program_and_get_row_count(statement) + 1;
            }


            // Pack the row into the window

            for (i = 0; i < num_columns; i++)
            {
                INT32 type = sqlite3_column_type(statement, i);
                if (type == SQLITE_TEXT)
                {
                    // TEXT data
#if  DB_TEXT_FORMAT_UTF8
                    CHAR const * text = (UINT8 const *)sqlite3_column_text(statement, i);
                    // SQLite does not include the NULL terminator in size, but does
                    // ensure all strings are NULL terminated, so increase size by
                    // one to make sure we store the terminator.
                    size_t size = sqlite3_column_bytes(statement, i) + 1;
                    if (!cursor_window_put_string(window,i,text))
#else
                    WCHAR const * text = (WCHAR const *)sqlite3_column_text16(statement, i);
                    size_t size = sqlite3_column_bytes16(statement, i);
                    if (!cursor_window_put_string_16(window,i,text))
#endif
                    {
                        cursor_window_delete_last_row(window);
                        DB_PRINTF("_db_fill_data_window cursor_window_put_string Failed %u bytes for text/blob at col:%d", size, i);
                        return start_pos + num_rows + _db_finish_program_and_get_row_count(statement) + 1;
                    }
                    //           DB_PRINTF("_db_fill_data_window cursor_window_put_string  value:%s at col:%d", text, i);
                }
                else if (type == SQLITE_INTEGER)
                {
                    // INTEGER data
                    INT64 value = sqlite3_column_int64(statement, i);
                    if (!cursor_window_put_int(window, i, value))
                    {
                        cursor_window_delete_last_row(window);
                        DB_PRINTF("_db_fill_data_window cursor_window_put_int  fail value:%d at col:%d", value,  i);
                        return start_pos + num_rows + _db_finish_program_and_get_row_count(statement) + 1;
                    }
                    //                 DB_PRINTF("_db_fill_data_window cursor_window_put_int  value:%d at row:%d,col:%d", value,  i);
                }
                else if (type == SQLITE_FLOAT)
                {
                    // FLOAT data
                    double value = sqlite3_column_double(statement, i);
                    if (!cursor_window_put_double(window, i, value))
                    {
                        cursor_window_delete_last_row(window);
                        DB_PRINTF("_db_fill_data_window cursor_window_put_double  fail value:%d at col:%d", value,  i);
                        return start_pos + num_rows + _db_finish_program_and_get_row_count(statement) + 1;
                    }
                    //          DB_PRINTF("_db_fill_data_window cursor_window_put_double  value:%f at col:%d", value, i);
                }
                else if (type == SQLITE_BLOB)
                {
                    // BLOB data
                    UINT8 const * blob = (UINT8 const *)sqlite3_column_blob(statement, i);
                    //size_t size = sqlite3_column_bytes16(statement, i);  i do not know why android use utf16 funtions to store blob data
                    size_t size = sqlite3_column_bytes(statement, i);
                    if (!cursor_window_put_blob(window, i, blob,size))
                    {
                        cursor_window_delete_last_row(window);
                        DB_PRINTF("_db_fill_data_window cursor_window_put_blob  fail at col:%d",  i);
                        return start_pos + num_rows + _db_finish_program_and_get_row_count(statement) + 1;
                    }
                    //          DB_PRINTF("_db_fill_data_window cursor_window_put_blob  at col:%d", i);

                }
                else if (type == SQLITE_NULL)
                {
                    // NULL field
                    cursor_window_put_null(window,i);

                    //DB_PRINTF("_db_fill_data_window cursor_window_put_null  at col:%d",  i);
                }
                else
                {
                    // Unknown data
                    DB_PRINTF("_db_fill_data_window Unknown column type when filling database window");
                    break;
                }
            }

            if (i < num_columns)
            {
                DB_PRINTF("_db_fill_data_window Not all the fields fit in the window Unknown data error happened");
                break;
            }

            // Mark the row as complete in the window
            num_rows++;
        }
        else if (err == SQLITE_DONE)
        {
            // All rows processed, bail
            DB_PRINTF("_db_fill_data_window Processed all rows");
            break;
        }
        else if (err == SQLITE_LOCKED || err == SQLITE_BUSY)
        {
            // The DB_PRINTF is locked, retry
            DB_PRINTF("_db_fill_data_window Database locked, retrying");
            if (retry_count > 50)
            {
                DB_PRINTF("_db_fill_data_window Bailing on database busy rety");
                break;
            }

            // Sleep to give the thread holding the lock a chance to finish
            UtaOsSleep(1000,1000);

            retry_count++;
            continue;
        }
        else
        {

            break;
        }
    }
#if DB_GET_PERFORMANCE
    t4 = db_get_ticks();
    DB_PRINTF("_db_fill_window query %d use %d us\n",num_rows,t4-t1);
#endif
    if (err == SQLITE_ROW)
    {
        return -1;
    }
    else
    {
        sqlite3_reset(statement);
        return start_pos+num_rows;
    }
}

static BOOL _db_fill_name_window(sqlite3_stmt* stmt,Cursor* name_win)
{
    INT32 i = 0;
    INT32 col_num= sqlite3_column_count(stmt);
    for (i=0;i<col_num;i++)
    {
        return_val_if_fail(cursor_set_column_name(name_win,i,sqlite3_column_name(stmt, i)),FALSE);
    }
    return TRUE;


}
static printbuf*  _db_build_query_sql_statement(const CHAR* col_list,const CHAR* table,DatabaseClause* clause)
{
    //delete from table where
    CHAR* col_list_tmp = (CHAR*)col_list;
    printbuf* pb =  NULL;
    return_val_if_fail((table), NULL);

    pb =  printbuf_new_v2(256);
    return_val_if_fail((pb), NULL);
    if (!col_list_tmp)
        col_list_tmp="*";
    if (clause)
    {
        sprintbuf(pb,"SELECT %s from %s WHERE %s",col_list_tmp,table,db_clause_get_statement(clause));

    }
    else
        sprintbuf(pb,"SELECT %s from %s",col_list_tmp,table);

    return pb;

}
static printbuf*  _db_build_query_sql_statement_v2(const CHAR* col_list,const CHAR* table,DatabaseClause* clause,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{
    printbuf* pb = _db_build_query_sql_statement(col_list,table,clause);
    return_val_if_fail(pb,NULL);
    if (having)
        sprintbuf(pb," HAVING %s",having);
    if (order_by)
        sprintbuf(pb," ORDER BY %s",order_by);
    if (limit)
        sprintbuf(pb," LIMIT %s;",limit);
    return pb;

}
static Cursor* _db_query_using_statement( DatabaseHandle* hdl ,sqlite3_stmt* stmt ,DatabaseWhereClause* where_clause,DatabaseHavingClause* having)
{
    INT32 sqlite_err = DB_OK;
    CursorWindow* data_win = NULL;

    Cursor* cursor = NULL;
    INT32 where_value_num = 0;

    if (where_clause)
    {
        sqlite_err=_db_stmt_bind_clause_impl(stmt,(DatabaseWhereClause* )where_clause,0);   //bind  vaules
        return_val_if_fail((sqlite_err==DB_OK),NULL);
        where_value_num= db_clause_get_data_count(where_clause);
    }
    if (having)
    {
        sqlite_err=_db_stmt_bind_clause_impl(stmt,(DatabaseWhereClause* )having,where_value_num);   //bind  vaules
        return_val_if_fail((sqlite_err==DB_OK),NULL);
    }
    cursor=db_cursor_create(sqlite3_column_count(stmt),FALSE, hdl);
    //cursor=cursor_create(sqlite3_column_count(stmt),FALSE);
    return_val_if_fail((cursor),NULL);

    data_win = cursor_get_data_window(cursor);

    //fill cursor window
    //fill name window
    if (!_db_fill_name_window(stmt,cursor))
    {
        cursor_destroy(cursor);
        return NULL;
    }


    //fill data window
    if (_db_fill_data_window(hdl,stmt,data_win,0,0,DB_MAX_QUERY_ROWS_ONE_TIME,0) < 0)
    {
        cursor_destroy(cursor);
        return NULL;
    }

    return cursor;
}

Cursor* db_query(DatabaseHandle* hdl ,const CHAR* table_name,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{
    Cursor* cursor =NULL;
    printbuf*  pb = NULL;
    sqlite3_stmt* stmt = NULL;

    return_val_if_fail((hdl &&hdl->sqlite && table_name && col_list),NULL);
#if  DB_DEBUG
    _db_analyze(hdl->sqlite);
#endif


    pb=_db_build_query_sql_statement_v2(col_list,table_name,(DatabaseWhereClause* )clause,NULL,NULL,order_by,limit);
    return_val_if_fail((pb),NULL);


    stmt=_db_stmt_compile_statement(hdl,pb->buf);   //compile statememt
#ifdef DB_DEBUG
    _db_print_explain_query_plan(hdl->sqlite,pb->buf);
#endif
    printbuf_free(pb);
    return_val_if_fail((stmt),NULL);

    cursor=  _db_query_using_statement( hdl ,stmt ,clause,NULL);

    sqlite3_finalize(stmt);
    return cursor;

}

Cursor* db_query_v2(DatabaseHandle* hdl ,const CHAR* table_name,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by, DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{
    Cursor* cursor =NULL;
    printbuf*  pb = NULL;
    sqlite3_stmt* stmt = NULL;

    return_val_if_fail((hdl &&hdl->sqlite && table_name && col_list),NULL);
#if DB_DEBUG
    _db_analyze(hdl->sqlite);
#endif
    pb=_db_build_query_sql_statement_v2(col_list,table_name,(DatabaseWhereClause* )clause,group_by,having,order_by,limit);
    return_val_if_fail((pb),NULL);
    stmt=_db_stmt_compile_statement(hdl,pb->buf);   //compile statememt
#if DB_DEBUG
    _db_print_explain_query_plan(hdl->sqlite,pb->buf);
#endif
    printbuf_free(pb);
    return_val_if_fail((stmt),NULL);

    cursor=  _db_query_using_statement( hdl ,stmt ,clause,having);

    sqlite3_finalize(stmt);
    return cursor;

}




Cursor* db_raw_query(DatabaseHandle* hdl ,const CHAR* sql_statement)
{
    Cursor* cursor =NULL;
    sqlite3_stmt* stmt = NULL;
    return_val_if_fail((hdl && hdl->sqlite && sql_statement),NULL);
#if DB_DEBUG
    _db_analyze(hdl->sqlite);
#endif

    stmt= _db_stmt_compile_statement(hdl,sql_statement);
    return_val_if_fail((stmt),NULL);
    cursor= _db_query_using_statement( hdl ,stmt ,NULL,NULL);
#if DB_DEBUG
    _db_print_explain_query_plan(hdl->sqlite,sql_statement);
#endif
    sqlite3_finalize(stmt);
    return cursor;
}

INT32 db_begin_transation(DatabaseHandle* hdl)
{

    CHAR statement[16];
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    memset(statement,0,16);
    strcpy(statement,"BEGIN EXCLUSIVE");
    return _db_error_code_convert(db_execSQL(hdl,statement));
}
INT32 db_end_transation(DatabaseHandle* hdl)
{

    CHAR statement[16];
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    memset(statement,0,16);
    strcpy(statement,"END TRANSACTION");
    return _db_error_code_convert(db_execSQL(hdl,statement));
}

INT64 db_memory_used()
{
    return sqlite3_memory_used();
}


INT64 db_memory_highwater()
{
    return sqlite3_memory_highwater(0);
}


INT64 db_vacuum(DatabaseHandle* hdl)
{
    INT32 sqlite_err;
    sqlite_err=db_execSQL(hdl,"VACUUM");
    return _db_error_code_convert(sqlite_err);
}


void db_set_max_heap_size(DatabaseHandle* hdl,INT32 size)
{
    return_if_fail((hdl &&hdl->sqlite));
    sqlite3_soft_heap_limit(size);
}



INT32 db_get_version( DatabaseHandle* hdl,INT32* output_version)
{

    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    return _db_error_code_convert(db_simple_execSQL_for_int(hdl,"PRAGMA user_version;",output_version));

}


INT32 db_set_version(DatabaseHandle* hdl,INT32 version)
{
    INT32 sqlite_err;
    CHAR statement[50];
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    memset(statement,0,50);
    sprintf(statement,"PRAGMA user_version = %d",version);
    sqlite_err=db_execSQL(hdl,statement);
    return _db_error_code_convert(sqlite_err);
}


INT32 db_get_page_size( DatabaseHandle* hdl,INT32* output_size )
{
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    return _db_error_code_convert(db_simple_execSQL_for_int(hdl,"PRAGMA page_size;",output_size));
}

INT32 db_set_page_size( DatabaseHandle* hdl ,INT32 numBytes)
{
    INT32 sqlite_err;
    CHAR statement[50];
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    memset(statement,0,50);
    sprintf(statement,"PRAGMA page_size = %d",numBytes);
    sqlite_err=db_execSQL(hdl,statement);
    return _db_error_code_convert(sqlite_err);
}

INT32 db_get_max_page_count( DatabaseHandle* hdl,INT32* output_cout )
{
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    return _db_error_code_convert(db_simple_execSQL_for_int(hdl,"PRAGMA max_page_count;",output_cout));
}


INT32 db_set_max_page_count( DatabaseHandle* hdl ,INT32 count)
{
    INT32 sqlite_err;
    CHAR statement[50];
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    memset(statement,0,50);
    sprintf(statement,"PRAGMA max_page_count = %d",count);
    sqlite_err=db_execSQL(hdl,statement);
    return _db_error_code_convert(sqlite_err);
}

INT32  db_get_maximum_size( DatabaseHandle* hdl,INT64* max_size)
{
    INT32 page_size;
    INT32 max_page_count;
    INT32 ret;
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    ret = db_get_max_page_count(hdl,&max_page_count);
    return_val_if_fail(ret==DB_OK,ret);
    ret = db_get_page_size(hdl,&page_size);
    return_val_if_fail(ret==DB_OK,ret);
    *max_size = (INT64)max_page_count * (INT64)page_size;
    return DB_OK;
}

INT32 db_set_maximum_size( DatabaseHandle* hdl ,INT64 max_size)
{
    INT32 page_size;
    INT32 max_page_count;
    INT32 ret;
    return_val_if_fail((hdl &&hdl->sqlite),DB_ERROR);
    ret = db_get_page_size(hdl,&page_size);
    return_val_if_fail(ret==DB_OK,ret);
    max_page_count=(INT32)(max_size/(INT64)page_size);
    if ((max_size % (INT64)page_size) != 0)
    {
        max_page_count++;
    }
    return_val_if_fail(max_page_count>0 ,DB_ERROR);
    return db_set_max_page_count(hdl,max_page_count);
}


static INT32 _db_print_explain_query_plan(sqlite3* sqlite,const CHAR* zSql)
{

    char *zExplain;                 /* SQL with EXPLAIN QUERY PLAN prepended */
    sqlite3_stmt *pExplain;         /* Compiled EXPLAIN QUERY PLAN command */
    int rc;                         /* Return code from sqlite3_prepare_v2() */


    if ( zSql==0 ) return SQLITE_ERROR;

    zExplain = sqlite3_mprintf("EXPLAIN QUERY PLAN %s", zSql);
    if ( zExplain==0 ) return SQLITE_NOMEM;

    rc = sqlite3_prepare_v2(sqlite, zExplain, -1, &pExplain, 0);
    sqlite3_free(zExplain);
    if ( rc!=SQLITE_OK ) return rc;

    DB_PRINTF("explain query play %s\n", zSql);
    while ( SQLITE_ROW==sqlite3_step(pExplain) ){

        // int iSelectid = sqlite3_column_int(pExplain, 0);
        int iOrder = sqlite3_column_int(pExplain, 0);
        int iFrom = sqlite3_column_int(pExplain, 1);
        const char *zDetail = (const char *)sqlite3_column_text(pExplain, 2);

        //  DB_PRINTF5("%d %d %d %s\n", iSelectid,iOrder,iFrom, zDetail);
        DB_PRINTF("%d %d %s\n", iOrder,iFrom, zDetail);
    }

    return sqlite3_finalize(pExplain);
}

static INT32 _db_print_explain(sqlite3* sqlite,const CHAR* zSql)
{

    char *zExplain;                 /* SQL with EXPLAIN QUERY PLAN prepended */
    sqlite3_stmt *pExplain;         /* Compiled EXPLAIN QUERY PLAN command */
    int rc;                         /* Return code from sqlite3_prepare_v2() */


    if ( zSql==0 ) return SQLITE_ERROR;

    zExplain = sqlite3_mprintf("EXPLAIN %s", zSql);
    if ( zExplain==0 ) return SQLITE_NOMEM;

    rc = sqlite3_prepare_v2(sqlite, zExplain, -1, &pExplain, 0);
    sqlite3_free(zExplain);
    if ( rc!=SQLITE_OK ) return rc;

    DB_PRINTF("explain %s\n", zSql);
    while ( SQLITE_ROW==sqlite3_step(pExplain) ){

        // int iSelectid = sqlite3_column_int(pExplain, 0);
        int addr = sqlite3_column_int(pExplain, 0);
        const char* opcode =(const char *) sqlite3_column_text(pExplain, 1);
        int p1 = sqlite3_column_int(pExplain, 2);
        int p2 = sqlite3_column_int(pExplain, 3);
        int p3 = sqlite3_column_int(pExplain, 4);

        //  DB_PRINTF5("%d %d %d %s\n", iSelectid,iOrder,iFrom, zDetail);
        DB_PRINTF("%d %s %d %d %d\n", addr,opcode, p1,p2,p3);
    }

    return sqlite3_finalize(pExplain);
}
static void _db_analyze(sqlite3* sqlite)
{

    _db_execSQL_impl(sqlite,"ANALYZE",NULL,NULL);

}

