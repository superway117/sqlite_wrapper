
#ifndef _DB_WRAPPER_H_
#define _DB_WRAPPER_H_

#include "cursor.h"
#include "content_values.h"
#include "db_clause.h"

typedef  struct _DatabaseHandle  DatabaseHandle;

typedef INT32 (*DB_CALLBACK)(void*,INT32,CHAR**, CHAR**);

/*
** CAPI3REF: Result Codes
** KEYWORDS: DB_OK {error code} {error codes}
** KEYWORDS: {result code} {result codes}
**
** Many SQLite functions return an integer result code from the set shown
** here in order to indicates success or failure.
**
** New error codes may be added in future versions of SQLite.
**
** See also: [DB_IOERR_READ | extended result codes]
*/
#define DB_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define DB_ERROR        1   /* SQL error or missing database */
#define DB_INTERNAL     2   /* Internal logic error in SQLite */
#define DB_PERM         3   /* Access permission denied */
#define DB_ABORT        4   /* Callback routine requested an abort */
#define DB_BUSY         5   /* The database file is locked */
#define DB_LOCKED       6   /* A table in the database is locked */
#define DB_NOMEM        7   /* A malloc() failed */
#define DB_READONLY     8   /* Attempt to write a readonly database */
#define DB_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define DB_IOERR       10   /* Some kind of disk I/O error occurred */
#define DB_CORRUPT     11   /* The database disk image is malformed */
#define DB_NOTFOUND    12   /* NOT USED. Table or record not found */
#define DB_FULL        13   /* Insertion failed because database is full */
#define DB_CANTOPEN    14   /* Unable to open the database file */
#define DB_PROTOCOL    15   /* NOT USED. Database lock protocol error */
#define DB_EMPTY       16   /* Database is empty */
#define DB_SCHEMA      17   /* The database schema changed */
#define DB_TOOBIG      18   /* String or BLOB exceeds size limit */
#define DB_CONSTRAINT  19   /* Abort due to constraint violation */
#define DB_MISMATCH    20   /* Data type mismatch */
#define DB_MISUSE      21   /* Library used incorrectly */
#define DB_NOLFS       22   /* Uses OS features not supported on host */
#define DB_AUTH        23   /* Authorization denied */
#define DB_FORMAT      24   /* Auxiliary database format error */
#define DB_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define DB_NOTADB      26   /* File opened that is not a database file */
#define DB_ROW         100  /* sqlite3_step() has another row ready */
#define DB_DONE        101  /* sqlite3_step() has finished executing */
#define DB_TODO        200  /* post msg to queue */
/* end-of-error-codes */


#define DB_OPEN_READONLY         0x00000001  /* Ok for db_open_v2() */
#define DB_OPEN_READWRITE        0x00000002  /* Ok for db_open_v2() */
#define DB_OPEN_CREATE           0x00000004  /* Ok for db_open_v2() */


typedef struct _DatabaseStatement DatabaseStatement;
/**
     * Open or create a SQLite database, 
     *
     * @param [in] path Database filename (UTF-8)
     * @param [out] hdl DatabaseHandle handle 
     * @return  error code
*/
extern INT32 db_open(const CHAR* path,DatabaseHandle** hdl);

/**
     * Open or create a SQLite database, 
     * valid flags:  DB_OPEN_READONLY,DB_OPEN_READWRITE,DB_OPEN_CREATE
     * @param [in] path database path
     * @param [in] flags flags to indicate the open parameters
     * @param [out] hdl DatabaseHandle handle  
     * @return error code
*/
extern INT32 db_open_v2(const CHAR* path, INT32 flags,DatabaseHandle** hdl);

/**
     * Close a SQLite database, 
     *
     * @param [in] hdl DatabaseHandle handle
     * @return error code
*/
extern INT32 db_close(DatabaseHandle* hdl);


/**
     * Execute a SQL statement, 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] sql_statement SQL statement
     * @return error code
*/
extern INT32 db_execSQL(DatabaseHandle* hdl,const CHAR* sql_statement);


/**
     * simple execute a SQL statement to get int value
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] sql_statement SQL statement
     * @param [out] output_int int value
     * @return error code
*/
extern INT32 db_simple_execSQL_for_int(DatabaseHandle* hdl,const CHAR* sql_statement,INT32* output_int);

/**
     * whether or not the table is exist
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @return error code
*/
extern BOOL db_table_is_exist(DatabaseHandle* hdl,const CHAR* table_name);

/**
     * Drop table  
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @return error code
*/
extern INT32 db_table_drop(DatabaseHandle* hdl,const CHAR* table_name);


/**
     * Drop index  
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] index_name index name
     * @return error code
*/
INT32 db_index_drop(DatabaseHandle* hdl,const CHAR* index_name);



/**
     * Drop view  
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] view_name view name
     * @return error code
*/
INT32 db_view_drop(DatabaseHandle* hdl,const CHAR* view_name);

/**
     * Insert a row in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param v values key/value list, key is column name, the key must be contained by the table
     * @return error code
*/
extern INT32 db_insert(DatabaseHandle* hdl,const CHAR* table_name, ContentValues* values);

/**
     * Delete rows in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] filter filter is used to  help to produce where clause
     * @return error code
*/
extern INT32 db_delete(DatabaseHandle* hdl,const CHAR* table_name, DatabaseWhereClause* filter);

/**
     * delete all in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @return error code
*/
extern INT32 db_delete_all(DatabaseHandle* hdl,const CHAR* table_name);

/**
     * Update rows in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] values key/value list, key is column name, the key must be contained by the table
     * @param [in] filter help to produce where clause
     * @return error code
*/
extern INT32 db_update(DatabaseHandle* hdl,const CHAR* table_name, ContentValues* values, DatabaseWhereClause* filter);

/**
     * query rows in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] col_list colume name list
     * @param [in] clause database clause
     * @param [in] order_by order colume
     * @param [in] limit limit string
     * @return a pointer of a Cursor which is used to traverse the query data set.
*/

extern Cursor* db_query( DatabaseHandle* hdl ,const CHAR* table_name,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit);

/**
     * query rows in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] col_list colume name list
     * @param [in] clause database clause
     * @param [in] group_by groub by colume name
     * @param [in] having having clause
     * @param [in] order_by order colume
     * @param [in] limit limit string
     * @return a pointer of a Cursor which is used to traverse the query data set.
*/
extern Cursor* db_query_v2( DatabaseHandle* hdl ,const CHAR* table_name,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by, DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit);

/**
     * query rows in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] sql_statement sql statement
     * @return a pointer of a Cursor which is used to traverse the query data set.
*/
extern Cursor* db_raw_query( DatabaseHandle* hdl ,const CHAR* sql_statement);

/**
     * get count in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [out] count save the count
     * @return error code
*/
extern INT32 db_table_get_count(DatabaseHandle* hdl,const CHAR* table_name,INT32* count);

/**
     * get count in table
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] where_clause where clause string
     * @param [out] count save the count
     * @return error code
*/
extern INT32 db_table_get_count_v2(DatabaseHandle* hdl,const CHAR* table_name,const CHAR* where_clause,INT32* count);

/**
     * Return the number of changes in the most recent call to sqlite3_exec().
     *
     * @param [in] hdl DatabaseHandle handle
     * @return number
*/
extern INT32 db_last_changes(DatabaseHandle* hdl);

/**
     * Return the ROWID of the most recent insert
     *
     * @param [in] hdl DatabaseHandle handle
     * @return ROWID
*/
extern INT64 db_last_insert_rowid(DatabaseHandle* hdl);

/**
     * Return last error code
     *
     * @param [in] hdl DatabaseHandle handle
     * @return last error code
*/
extern INT32 db_last_error(DatabaseHandle* hdl);



/**
     * create a delete DatabaseStatement
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] where_clause where clause
     * @return DatabaseStatement instance
*/
extern DatabaseStatement* db_stmt_build_delete_statement( DatabaseHandle* hdl,const CHAR* table_name,DatabaseWhereClause* where_clause);

/**
     * create a insert DatabaseStatement
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] col_list colume name list
     * @param [in] col_num colume number
     * @return DatabaseStatement instance
*/
extern DatabaseStatement* db_stmt_build_insert_statement( DatabaseHandle* hdl,const CHAR* table_name,const CHAR* col_list,INT32 col_num);

/**
     * create a update DatabaseStatement
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] table_name table name
     * @param [in] values content values
     * @param [in] where_clause where clause
     * @return DatabaseStatement instance
*/
extern DatabaseStatement* db_stmt_build_update_statement( DatabaseHandle* hdl,const CHAR* table_name,ContentValues* values,DatabaseWhereClause* where_clause);

/**
     * bind value to DatabaseStatement instance
     *
     * @param [in] stmt DatabaseStatement handle
     * @param [in] clause DatabaseClause handle
     * @return error code
*/
extern INT32  db_stmt_bind_clause(DatabaseStatement* stmt,DatabaseClause* clause);

/**
     * bind content value to DatabaseStatement instance, for insert and update
     *
     * @param [in] stmt DatabaseStatement handle
     * @param [in] values value
     * @return error code
*/
extern INT32  db_stmt_bind_content_values(DatabaseStatement* stmt,ContentValues* values);

/**
     * bind all values to DatabaseStatement instance
     *
     * @param [in] stmt DatabaseStatement handle
     * @param [in] values value
     * @param [in] clause DatabaseClause handle
     * @return error code
*/
extern INT32  db_stmt_bind_content_and_clause_values(DatabaseStatement* stmt,ContentValues* values,DatabaseClause* clause);

/**
     * step execute a DatabaseStatement instance
     *
     * @param [in] stmt DatabaseStatement handle
     * @return error code
*/
extern INT32  db_stmt_step(DatabaseStatement* stmt);


/**
     * finalize a DatabaseStatement instance
     *
     * @param [in] stmt DatabaseStatement handle
     * @return error code
*/
extern INT32  db_stmt_finalize(DatabaseStatement* stmt);



/**
     * the db default uses auto commit mode, but it call this function, the db will commit after call db_end_transation
     *
     * @param [in] hdl DatabaseHandle handle
     * @return error code
*/
extern INT32 db_begin_transation(DatabaseHandle* hdl);

/**
     * execute commit, and set the db to auto commit mode
     *
     * @param [in] hdl DatabaseHandle handle
     * @return error code
*/
extern INT32 db_end_transation(DatabaseHandle* hdl);

/**
     * set the max heap size for one database
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] size heap size
*/
extern void db_set_max_heap_size(DatabaseHandle* hdl,INT32 size);

/**
     * get user version of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [out] output_version save the version
     * @return error code
*/
extern INT32 db_get_version( DatabaseHandle* hdl,INT32* output_version);


/**
     * set user version of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] version user version
     * @return error code
*/
extern INT32 db_set_version(DatabaseHandle* hdl,INT32 version);

/**
     * get page size of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [out] output_size save the page size
     * @return error code
*/
extern INT32 db_get_page_size( DatabaseHandle* hdl,INT32* output_size );

/**
     * set page of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] numBytes page size
     * @return error code
*/
extern INT32 db_set_page_size( DatabaseHandle* hdl ,INT32 numBytes);

/**
     * get max page cout of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] output_cout save the max page count
     * @return error code
*/
extern INT32 db_get_max_page_count( DatabaseHandle* hdl,INT32* output_cout );

/**
     * set max page cout of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] count max page count
     * @return error code
*/
extern INT32 db_set_max_page_count( DatabaseHandle* hdl ,INT32 count);


/**
     * get max size of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] max_size save the max size
     * @return error code
*/
extern INT32  db_get_maximum_size( DatabaseHandle* hdl,INT64* max_size);


/**
     * set max size of one database 
     *
     * @param [in] hdl DatabaseHandle handle
     * @param [in] max_size the max size
     * @return error code
*/
extern INT32 db_set_maximum_size( DatabaseHandle* hdl ,INT64 max_size);

/**
     * resize the database file,remove no used chunk in database
     *
     * @param [in] hdl DatabaseHandle handle
     * @return error code
*/
extern INT64 db_vacuum(DatabaseHandle* hdl);



/**
     * get current used memory in database
     *
     * @param [in] hdl DatabaseHandle handle
     * @return memory size
*/
extern INT64 db_memory_used();

/**
     * get hight water memory in database
     *
     * @param [in] hdl DatabaseHandle handle
     * @return hight water memory size
*/
extern INT64 db_memory_highwater();

#endif




