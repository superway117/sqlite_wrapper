
#ifndef _DB_CURSOR_H_
#define _DB_CURSOR_H_

#include "cursor.h"
#include "db_wrapper.h"

/**
     * @brief create a cursor
     *
     * @param[in] col_num colume number
     * @param[in] read_only read only or not to create cursor
     * @param[in] hdl DatabaseHandle handle
     * @return Cursor instance
*/
extern Cursor* db_cursor_create(INT32 col_num,BOOL read_only,DatabaseHandle* hdl);
#endif




