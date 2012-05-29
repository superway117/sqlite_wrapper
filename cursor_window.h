
#ifndef _DB_CURSOR_WINDOW_H_
#define _DB_CURSOR_WINDOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
typedef struct _CursorWindow CursorWindow;

#define FIELD_TYPE_NULL 0     /**init value when create cursor window */

#define FIELD_TYPE_INTEGER 1
#define FIELD_TYPE_BOOL 2
#define FIELD_TYPE_FLOAT 3
#define FIELD_TYPE_STRING 4
#define FIELD_TYPE_STRING_16 5
#define FIELD_TYPE_BLOB 6

//#define FIELD_TYPE_NULL 7 

/**
     * @brief create a cursor window
     * @param[in] col_num colume number
     * @return CursorWindow instance
*/ 
extern CursorWindow* cursor_window_create(INT32 col_num);

/**
     * @brief duplicate a cursor window
     * @param[in] src source  CursorWindow
     * @return CursorWindow instance
*/ 
extern CursorWindow* cursor_window_duplicate(CursorWindow*  src);

/**
     * @brief destroy  cursor window
     * @param[in] thiz  CursorWindow instance
*/ 
extern void cursor_window_destroy(CursorWindow* thiz);

/**
     * @brief get row numbner
     * @param[in] thiz  CursorWindow instance
     * @return row nunber
*/ 
extern INT32 cursor_window_get_row_num(CursorWindow* thiz);

/**
     * @brief get colume numbner
     * @param[in] thiz  CursorWindow instance
     * @return colume nunber
*/ 
extern INT32 cursor_window_get_col_num(CursorWindow* thiz);

/**
     * @brief alloc new row
     * @return error code
*/ 
extern BOOL cursor_window_alloc_row(CursorWindow* thiz);


/**
     * @brief get int64 value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[int] value  int value
     * @return error code
*/
extern BOOL cursor_window_put_int(CursorWindow* thiz,INT32 row,INT64 value);

/**
     * @brief get bool value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[int] value  bool value
     * @return error code
*/
extern BOOL cursor_window_put_bool(CursorWindow* thiz,INT32 col,BOOL value);

/**
     * @brief get double value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[int] value  double value
     * @return error code
*/
extern BOOL cursor_window_put_double(CursorWindow* thiz,INT32 row,double value);

/**
     * @brief get null value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @return error code
*/
extern BOOL cursor_window_put_null(CursorWindow* thiz,INT32 col);

/**
     * @brief get string value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[int] value  string value
     * @return error code
*/
extern BOOL cursor_window_put_string(CursorWindow* thiz,INT32 row,const CHAR* value);

/**
     * @brief get wstring value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[int] value  wstring value
     * @return error code
*/
extern BOOL cursor_window_put_string_16(CursorWindow* thiz,INT32 row,const WCHAR* value);

/**
     * @brief get blob value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[int] value  blob value
     * @return error code
*/
extern BOOL cursor_window_put_blob(CursorWindow* thiz,INT32 row,const void* value,INT32 len);

/**
     * @brief update int value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[int] value  int value
     * @return error code
*/
extern BOOL cursor_window_update_int(CursorWindow* thiz,INT32 row,INT32 col,INT64 value);

/**
     * @brief update bool value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[int] value  bool value
     * @return error code
*/
extern BOOL cursor_window_update_bool(CursorWindow* thiz,INT32 row,INT32 col,BOOL value);

/**
     * @brief update double value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[int] value  double value
     * @return error code
*/
extern BOOL cursor_window_update_double(CursorWindow* thiz,INT32 row,INT32 col,double value);

/**
     * @brief update string value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[int] value  string value
     * @return error code
*/
extern BOOL cursor_window_update_string(CursorWindow* thiz,INT32 row,INT32 col,const CHAR* value);

/**
     * @brief update wstring value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[int] value  wstring value
     * @return error code
*/
extern BOOL cursor_window_update_string_16(CursorWindow* thiz,INT32 row,INT32 col,const WCHAR* value);

/**
     * @brief update blob value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[int] value  blob value
     * @param[int] len  blob value length
     * @return error code
*/

extern BOOL cursor_window_update_blob(CursorWindow* thiz,INT32 row,INT32 col,const void* value,INT32 len);

/**
     * @brief get int value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] value  save int value 
     * @return error code
*/
extern BOOL cursor_window_get_int(CursorWindow* thiz,INT32 row,INT32 col,INT64* value);

/**
     * @brief get bool value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] value  save bool value 
     * @return error code
*/
extern BOOL cursor_window_get_bool(CursorWindow* thiz,INT32 row,INT32 col,BOOL* value);

/**
     * @brief get double value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] value  save double value 
     * @return error code
*/
extern BOOL cursor_window_get_double(CursorWindow* thiz,INT32 row,INT32 col,double* value);


/**
     * @brief get string value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] value  save string value 
     * @return error code
*/
extern const CHAR* cursor_window_get_string(CursorWindow* thiz,INT32 row,INT32 col);

/**
     * @brief get string value, if the source data is wstring, this funtion will convert the wstring into string
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @return string value
*/
extern const CHAR* cursor_window_get_string_coerce(CursorWindow* thiz,INT32 row,INT32 col);


/**
     * @brief get wstring value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @return wstring value
*/
extern const WCHAR* cursor_window_get_string_16(CursorWindow* thiz,INT32 row,INT32 col);

/**
     * @brief get wstring value, if the source data is string, this funtion will convert the string into wstring
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @return wstring value
*/
extern const WCHAR* cursor_window_get_string_16_coerce(CursorWindow* thiz,INT32 row,INT32 col);

/**
     * @brief get blob value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] len save the blob data length
     * @return blob value 
*/
extern const void* cursor_window_get_blob(CursorWindow* thiz,INT32 row,INT32 col,INT32* len);

/**
     * @brief get value
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] type save the value type
     * @param[out] data save the value address
     * @param[out] len save the blob data length
     * @return error code
*/
extern BOOL cursor_window_get_value(CursorWindow* thiz,INT32 row,INT32 col,UINT8* type,void** data,INT32* len);

/**
     * @brief get type
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @param[in] col colume index
     * @param[out] type save the value type
     * @return error code
*/
extern BOOL cursor_window_get_type(CursorWindow* thiz,INT32 row,INT32 col,UINT8* type);

/**
     * @brief delete row
     *
     * @param[in] thiz  CursorWindow instance
     * @param[in] row row index
     * @return error code
*/
extern BOOL cursor_window_delete_row(CursorWindow* thiz,INT32 row);

/**
     * @brief delete last row
     *
     * @param[in] thiz  CursorWindow instance
     * @return error code
*/
extern BOOL cursor_window_delete_last_row(CursorWindow* thiz);

#ifdef __cplusplus
}
#endif
#endif




