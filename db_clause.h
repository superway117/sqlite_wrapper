

#ifndef _DATABASE_CLAUSE_H_
#define _DATABASE_CLAUSE_H_
#include <common.h>
#include "cursor_window.h"
typedef enum
{
  DB_WHERE_CLAUSE,
  DB_HAVING_CLAUSE
}DatabaseClauseType;

struct _DatabaseClause;

typedef struct _DatabaseClause DatabaseClause;

/**
     * @brief destroy database clause
     * @param[in] thiz  DatabaseClause instance
*/ 
extern void db_clause_destroy(DatabaseClause*  clause);

/**
     * @brief create a database clause
     * @param[in] statement clause statement
     * @param[in] value_num the number of data which need bind
     * @return DatabaseClause instance
*/ 
extern DatabaseClause* db_clause_create(DatabaseClauseType type,CHAR* statement,INT32 value_num);


/**
     * @brief duplicate a database clause
     * @param[in] src  source database clause
     * @return DatabaseClause instance
*/ 
extern DatabaseClause* db_clause_duplicate(DatabaseClause* src);

/**
     * @brief create a where database clause
     * @param[in] statement clause statement
     * @param[in] value_num the number of data which need bind
     * @return DatabaseClause instance
*/ 
extern DatabaseClause* db_where_clause_create(CHAR* statement,INT32 value_num);

/**
     * @brief create a having database clause
     * @param[in] statement clause statement
     * @param[in] value_num the number of data which need bind
     * @return DatabaseClause instance
*/ 
extern DatabaseClause* db_having_clause_create(CHAR* statement,INT32 value_num);


/**
     * @brief get statement
     * @param[in] thiz DatabaseClause handle
     * @return statement
*/ 
extern const CHAR* db_clause_get_statement(DatabaseClause*  thiz);

/**
     * @brief set statement
     * @param[in] thiz DatabaseClause handle
     * @param[in] statement statement
     * @return error code
*/ 
extern BOOL db_clause_set_statement(DatabaseClause*  thiz,const CHAR* statement);

/**
     * @brief append where statement
     * @param[in] thiz DatabaseClause handle
     * @param[in] where_str where statement
     * @return error code
*/ 
extern BOOL db_clause_append_where(DatabaseClause*  thiz,const CHAR* where_str);

/**
     * @brief get data count which needs  bind
     * @param[in] thiz DatabaseClause handle
     * @return data number
*/ 
extern INT32 db_clause_get_data_count(DatabaseClause*  thiz);

/**
     * @brief get data which needs bind
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] idx index
     * @param[out] len save the data length
     * @param[out] data save the data
     * @return bool code
*/
extern BOOL db_clause_get_data(DatabaseClause*  thiz,INT32 idx,UINT8* type,INT32* len,void** data);

/**
     * @brief put int value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[in] value int value
     * @return bool code
*/
extern BOOL db_clause_put_int(DatabaseClause* thiz,INT32 col,INT64 value);


/**
     * @brief put double value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[in] value double value
     * @return bool code
*/
extern BOOL db_clause_put_double(DatabaseClause* thiz,INT32 col,double value);

/**
     * @brief put string value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[in] value string value
     * @return bool code
*/
extern BOOL db_clause_put_string(DatabaseClause* thiz,INT32 col,const CHAR* value);

/**
     * @brief put wstring value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[in] value wstring value
     * @return bool code
*/
extern BOOL db_clause_put_string_16(DatabaseClause* thiz,INT32 col,const WCHAR* value);

/**
     * @brief put blob value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[in] value blob value
     * @param[in] len blob value length
     * @return bool code
*/
extern BOOL db_clause_put_blob(DatabaseClause* thiz,INT32 col,const void* value,INT32 len);

/**
     * @brief get int value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[out] value int value
     * @return bool code
*/
extern BOOL db_clause_get_int(DatabaseClause* thiz,INT32 col,INT64* value);

/**
     * @brief get double value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[out] value double value
     * @return bool code
*/
extern BOOL db_clause_get_double(DatabaseClause* thiz,INT32 col,double* value);

/**
     * @brief get string value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @return string value
*/
extern const CHAR* db_clause_get_string(DatabaseClause* thiz,INT32 col);

/**
     * @brief get wstring value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @return wstring value
*/
extern const WCHAR* db_clause_get_string_16(DatabaseClause* thiz,INT32 col);

/**
     * @brief get blob value
     *
     * @param[in] thiz DatabaseClause handle
     * @param[in] col index
     * @param[out] value blob value length
     * @return blob value
*/
extern const void* db_clause_get_blob(DatabaseClause* thiz,INT32 row,INT32 col,INT32* len);

/** where clause */
typedef  DatabaseClause DatabaseWhereClause;

/** having clause */
typedef  DatabaseClause DatabaseHavingClause;

#endif
