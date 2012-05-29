
#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "common.h"
#include "cursor_window.h"
typedef struct _Cursor Cursor;


/**
      virtual function list
*/

/**
      destroy
*/
typedef BOOL  (*CursorOnDestroy)(Cursor* thiz);


/**
      delete
*/
typedef BOOL  (*CursorDeleteRow)(Cursor* thiz,INT32 row);


/**
      update
*/
typedef BOOL  (*CursorUpdateINT)(Cursor* cursor,INT32 row,INT32 col,INT64 value);
typedef BOOL  (*CursorUpdateString)(Cursor* cursor,INT32 row,INT32 col,const CHAR* value);
typedef BOOL  (*CursorUpdateString16)(Cursor* cursor,INT32 row,INT32 col,const WCHAR* value);
typedef BOOL  (*CursorUpdateBool)(Cursor* cursor,INT32 row,INT32 col,BOOL value);
typedef BOOL  (*CursorUpdateBlob)(Cursor* cursor,INT32 row,INT32 col,UINT8* value,INT32 len);
typedef BOOL  (*CursorUpdateDouble)(Cursor* cursor,INT32 row,INT32 col,double value);



typedef const CHAR* (*CursorGetString)(const Cursor*  cursor,INT32 column_index);
typedef const WCHAR* (*CursorGetString16)(const Cursor*  cursor,INT32 column_index);


struct _Cursor 
{
	CursorWindow* data_win;
	CursorWindow* name_win;
	INT32 cur_row;
	BOOL read_only;

/**
      destroy
*/
	CursorOnDestroy on_destroy;


/**
      delete
*/
	CursorDeleteRow  delete_row;

/**
      update
*/
	CursorUpdateINT  update_int;
	CursorUpdateString  update_string;
	CursorUpdateString16  update_string16;
	CursorUpdateBool	     update_bool;
	CursorUpdateBlob	     update_blobl;
	CursorUpdateDouble  update_double;
	
 
/**
      for db special string proces, need to make the get string functions as virtual funtions
*/
	CursorGetString   get_string;
	CursorGetString16	get_string_16;
	
	void* priv_subclass;
};


/**
     * @brief create a cursor
     * @param[in] col_num colume number
     * @param[in] read_only read only or not
     * @return Cursor instance
*/
extern Cursor* cursor_create(INT32 col_num,BOOL read_only);


/**
     * @brief get data window
     * @param[in] thiz Cursor handle
     * @return CursorWindow instance
*/
extern CursorWindow*  cursor_get_data_window(Cursor* thiz);

/**
     * @brief get name window
     * @param[in] thiz Cursor handle
     * @return CursorWindow instance
*/
extern CursorWindow*  cursor_get_name_window(Cursor* thiz) ;

/**
     * @brief destroy cursor, the name window and data window will be destoried
     * @param[in] thiz Cursor handle
*/
extern void cursor_destroy(Cursor* cursor);

/**
     * @brief get count in data window
     * @param[in] thiz Cursor handle
     * @return count
*/
extern INT32 cursor_get_count(Cursor* cursor);

/**
     * @brief get count in name window
     * @param[in] thiz Cursor handle
     * @return count
*/
extern INT32  cursor_get_column_num(Cursor* cursor);

/**
     * @brief get colume index
     * @param[in] thiz Cursor handle
     * @param[in] col_name colume name
     * @return index
*/
extern INT32 cursor_get_column_index(Cursor* cursor,const CHAR*  col_name);

/**
     * @brief get colume name
     * @param[in] thiz Cursor handle
     * @param[in] col colume index
     * @return colume name
*/
extern const CHAR* cursor_get_column_name( Cursor* cursor,INT32 col);

/**
     * @brief set colume name
     * @param[in] thiz Cursor handle
     * @param[in] col colume index
     * @param[in] col_name colume name
     * @return error code
*/
extern BOOL cursor_set_column_name( Cursor* cursor,INT32 col,const CHAR*  col_name);

/**
     * @brief get current position
     * @param[in] thiz Cursor handle
     * @return position
*/
extern INT32 cursor_get_position( Cursor* cursor);

/**
     * @brief move to first 
     * @param[in] thiz Cursor handle
     * @return error code
*/
extern BOOL cursor_move_to_first(Cursor* cursor);

/**
     * @brief move to last 
     * @param[in] thiz Cursor handle
     * @return error code
*/
extern BOOL cursor_move_to_last(Cursor* cursor);

/**
     * @brief move to next 
     * @param[in] thiz Cursor handle
     * @return error code
*/
extern BOOL cursor_move_to_next(Cursor* cursor);

/**
     * @brief move to previous 
     * @param[in] thiz Cursor handle
     * @return error code
*/
extern BOOL cursor_move_to_previous(Cursor* cursor);

/**
     * @brief move to given position 
     * @param[in] thiz Cursor handle
     * @param[in] pos position
     * @return error code
*/
extern BOOL cursor_move_to_position(Cursor* cursor,INT32 pos);

/**
     * @brief whether or not current is last position
     * @param[in] thiz Cursor handle
     * @return bool code
*/
extern BOOL cursor_is_last(const Cursor* cursor);

/**
     * @brief whether or not current is first position
     * @param[in] thiz Cursor handle
     * @return bool code
*/
extern BOOL cursor_is_first(const Cursor* cursor);

/**
     * @brief get int64 value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @param[out] value save int value
     * @return error code
*/
extern BOOL cursor_get_int(const Cursor*  cursor,INT32 column_index,INT64* value);

/**
     * @brief get bool value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @param[out] value save bool value
     * @return error code
*/
extern BOOL cursor_get_bool(const Cursor*  cursor,INT32 column_index,BOOL* value);

/**
     * @brief get double value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @param[out] value save double value
     * @return error code
*/
extern BOOL cursor_get_double(const Cursor*  cursor,INT32 column_index,double* value);

/**
     * @brief get string value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return string value
*/
extern const CHAR* cursor_get_string(const Cursor*  cursor,INT32 column_index);

/**
     * @brief get wstring value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return wstring value
*/
extern const WCHAR* cursor_get_string_16(const Cursor*  cursor,INT32 column_index);

/**
     * @brief get blob value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @param[out] data colume save blob data, do not need to release the buffer
     * @param[out] output_len blob data lenght
     * @return wstring value
*/
extern BOOL  cursor_get_blob(const Cursor*  cursor,INT32 column_index,void** data,INT32* output_len);

/**
     * @brief whether or not the given colume is null
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_null(const Cursor* cursor,INT32 column_index);

/**
     * @brief whether or not the given colume is int
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_int(const Cursor* cursor,INT32 column_index);

/**
     * @brief whether or not the given colume is bool
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_bool(const Cursor* cursor,INT32 column_index);

/**
     * @brief whether or not the given colume is double
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_double (const Cursor* cursor,INT32 column_index);

/**
     * @brief whether or not the given colume is string
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_string (const Cursor* cursor,INT32 column_index);

/**
     * @brief whether or not the given colume is wstring
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_string_16 (const Cursor* cursor,INT32 column_index);

/**
     * @brief whether or not the given colume is blob
     *
     * @param[in] thiz ContentValues handle
     * @param[in] column_index colume index
     * @return bool code
*/
extern BOOL cursor_is_blob (const Cursor* cursor,INT32 column_index);

/**
     * @brief update int value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @param[in] col colume index
     * @param[in] value int value
     * @return bool code
*/
extern BOOL cursor_update_int(Cursor* cursor,INT32 row,INT32 col,INT64 value);

/**
     * @brief update bool value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @param[in] col colume index
     * @param[in] value bool value
     * @return bool code
*/
extern BOOL cursor_update_bool(Cursor* cursor,INT32 row,INT32 col,BOOL value);

/**
     * @brief update double value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @param[in] col colume index
     * @param[in] value double value
     * @return bool code
*/
extern BOOL cursor_update_double(Cursor* cursor,INT32 row,INT32 col,double value);

/**
     * @brief update string value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @param[in] col colume index
     * @param[in] value string value
     * @return bool code
*/
extern BOOL cursor_update_string(Cursor* cursor,INT32 row,INT32 col,const CHAR* value);

/**
     * @brief update wstring value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @param[in] col colume index
     * @param[in] value wstring value
     * @return bool code
*/
extern BOOL cursor_update_string_16(Cursor* cursor,INT32 row,INT32 col,const WCHAR* value);

/**
     * @brief update blob value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @param[in] col colume index
     * @param[in] value blob value
     * @return bool code
*/
extern BOOL cursor_update_blob(Cursor* cursor,INT32 row,INT32 col,UINT8* value,INT32 len);

/**
     * @brief delete row
     *
     * @param[in] thiz ContentValues handle
     * @param[in] row row index
     * @return bool code
*/
extern BOOL cursor_delete_row(Cursor* cursor,INT32 row);

#endif




