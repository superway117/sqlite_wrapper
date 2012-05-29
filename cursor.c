#include "cursor.h"
#include <tg_utility.h>

Cursor* cursor_create(INT32 col_num,BOOL read_only)
{
    Cursor* cursor = TG_CALLOC_V2(sizeof(Cursor));
    return_val_if_fail(cursor && col_num >0, NULL);
    cursor->read_only = read_only;
    cursor->name_win = cursor_window_create(col_num);
    cursor->data_win = cursor_window_create(col_num);
    if (cursor->name_win==NULL || cursor->data_win==NULL)
    {
        cursor_window_destroy(cursor->name_win);
        cursor_window_destroy(cursor->data_win);
        TG_FREE(cursor);
        return NULL;
    }
    cursor_window_alloc_row(cursor->name_win);

    return cursor;
}


CursorWindow*  cursor_get_data_window(Cursor* thiz)
{
    return_val_if_fail(thiz, NULL);

    return thiz->data_win;
}
CursorWindow*  cursor_get_name_window(Cursor* thiz)
{
    return_val_if_fail(thiz, NULL);

    return thiz->name_win;
}

void cursor_destroy(Cursor* cursor)
{
    return_if_fail(cursor);
    if (cursor->on_destroy)
        cursor->on_destroy(cursor);
    if (cursor->data_win)
    {
        cursor_window_destroy(cursor->data_win);
        cursor->data_win= NULL;
    }
    if (cursor->name_win)
    {
        cursor_window_destroy(cursor->name_win);
        cursor->name_win= NULL;
    }
    TG_FREE(cursor->priv_subclass);
    TG_FREE(cursor);
}

INT32 cursor_get_count(Cursor* cursor)
{
    return_val_if_fail(cursor&&cursor->data_win,0);
    return cursor_window_get_row_num(cursor->data_win);
}

INT32  cursor_get_column_num(Cursor* cursor)
{
    return_val_if_fail(cursor&&cursor->name_win,0);
    return cursor_window_get_col_num(cursor->data_win);
}


INT32 cursor_get_column_index( Cursor* cursor,const CHAR*  col_name)
{
    INT32 col_num;
    INT32 i=0;
    return_val_if_fail(cursor&&cursor->data_win && col_name,0);
    col_num = cursor_window_get_col_num(cursor->data_win);
    for (i=0;i<col_num;i++)
    {
        const CHAR* name = cursor_window_get_string(cursor->data_win,0,i);
        if (name && strcmp(col_name,name)==0)
            return i;


    }
    return -1;
}


const CHAR* cursor_get_column_name( Cursor* cursor,INT32 col)
{
    return_val_if_fail(cursor&&cursor->name_win,NULL);
    return cursor_window_get_string(cursor->name_win,0,col);
}

BOOL cursor_set_column_name(Cursor* cursor,INT32 col,const CHAR*  col_name)
{
    return_val_if_fail(cursor&&cursor->name_win&&col_name,FALSE);
    return cursor_window_put_string(cursor->name_win,col,col_name);
}
INT32 cursor_get_position(Cursor* cursor)
{
    return_val_if_fail(cursor&&cursor->data_win,0);
    return cursor->cur_row;
}



BOOL cursor_move_to_first(Cursor* cursor)
{
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    cursor->cur_row=0;
    return TRUE;
}

BOOL cursor_move_to_last(Cursor* cursor)
{
    INT32 row_num;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    row_num =cursor_window_get_row_num(cursor->data_win);
    if (row_num>=1)
        cursor->cur_row=row_num-1;
    return TRUE;
}

BOOL cursor_move_to_next(Cursor* cursor)
{
    INT32 row_num;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    row_num =cursor_window_get_row_num(cursor->data_win);
    if (row_num>=1 && cursor->cur_row < row_num-1)
    {
        cursor->cur_row++;
        return TRUE;
    }
    return FALSE;

}

BOOL cursor_move_to_previous(Cursor* cursor)
{

    return_val_if_fail(cursor&&cursor->data_win,FALSE);

    if ( cursor->cur_row >0)
    {
        cursor->cur_row--;
        return TRUE;
    }
    return FALSE;

}

BOOL cursor_move_to_position(Cursor* cursor,INT32 pos)
{
    INT32 row_num;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    row_num =cursor_window_get_row_num(cursor->data_win);
    return_val_if_fail(row_num>=1,FALSE);
    return_val_if_fail(pos <=  row_num-1,FALSE);
    cursor->cur_row=pos;
    return TRUE;
}

BOOL cursor_is_last(const Cursor* cursor)
{
    INT32 row_num;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    row_num =cursor_window_get_row_num(cursor->data_win);
    return_val_if_fail(row_num>=1,FALSE);
    return (cursor->cur_row==row_num-1)?TRUE:FALSE;

}

BOOL cursor_is_first(const Cursor* cursor)
{
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return (cursor->cur_row==0)?TRUE:FALSE;
}



BOOL cursor_get_int(const Cursor*  cursor,INT32 column_index,INT64* value)
{
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return cursor_window_get_int(cursor->data_win,cursor->cur_row,column_index,value);
}

BOOL cursor_get_bool(const Cursor*  cursor,INT32 column_index,BOOL* value)
{
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return cursor_window_get_bool(cursor->data_win,cursor->cur_row,column_index,value);
}


BOOL cursor_get_double(const Cursor*  cursor,INT32 column_index,double* value)
{
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return cursor_window_get_double(cursor->data_win,cursor->cur_row,column_index,value);
}


const CHAR* cursor_get_string(const Cursor*  cursor,INT32 column_index)
{
    return_val_if_fail(cursor&&cursor->data_win,NULL);
    if (cursor->get_string)
    {
        return cursor->get_string(cursor,column_index);
    }
    else
    {
        return cursor_window_get_string(cursor->data_win,cursor->cur_row,column_index);
    }

}
const WCHAR* cursor_get_string_16(const Cursor*  cursor,INT32 column_index)
{
    return_val_if_fail(cursor&&cursor->data_win,NULL);
    if (cursor->get_string_16)
    {
        return cursor->get_string_16(cursor,column_index);
    }
    else
    {
        return cursor_window_get_string_16(cursor->data_win,cursor->cur_row,column_index);
    }
}

BOOL  cursor_get_blob(const Cursor*  cursor,INT32 column_index,void** data,INT32* output_len)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret= cursor_window_get_value(cursor->data_win,cursor->cur_row,column_index,&type,(void**)data,output_len);
    return (ret && (type==FIELD_TYPE_BLOB|| type==FIELD_TYPE_NULL))?TRUE:FALSE;
}


BOOL cursor_is_null(const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    void* data=NULL;
    INT32 len=0;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret= cursor_window_get_value(cursor->data_win,cursor->cur_row,column_index,&type,(void**)&data,&len);
    return_val_if_fail(ret,FALSE);
    return (type==FIELD_TYPE_NULL || data==NULL)?TRUE:FALSE;

}

BOOL cursor_is_int(const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret=cursor_window_get_type(cursor->data_win,cursor->cur_row,column_index,&type);
    return (ret && (type==FIELD_TYPE_INTEGER))?TRUE:FALSE;
}
BOOL cursor_is_bool(const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret=cursor_window_get_type(cursor->data_win,cursor->cur_row,column_index,&type);
    return (ret && (type==FIELD_TYPE_BOOL))?TRUE:FALSE;
}
BOOL cursor_is_double (const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret=cursor_window_get_type(cursor->data_win,cursor->cur_row,column_index,&type);
    return (ret && (type==FIELD_TYPE_FLOAT))?TRUE:FALSE;
}

BOOL cursor_is_string (const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret=cursor_window_get_type(cursor->data_win,cursor->cur_row,column_index,&type);
    return (ret && (type==FIELD_TYPE_STRING||type==FIELD_TYPE_NULL))?TRUE:FALSE;
}

BOOL cursor_is_string_16 (const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret=cursor_window_get_type(cursor->data_win,cursor->cur_row,column_index,&type);
    return (ret && (type==FIELD_TYPE_STRING_16||type==FIELD_TYPE_NULL))?TRUE:FALSE;
}



BOOL cursor_is_blob (const Cursor* cursor,INT32 column_index)
{
    BOOL ret;
    UINT8 type;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    ret=cursor_window_get_type(cursor->data_win,cursor->cur_row,column_index,&type);
    return (ret && (type==FIELD_TYPE_BLOB||type==FIELD_TYPE_NULL))?TRUE:FALSE;
}


BOOL cursor_update_int(Cursor* cursor,INT32 row,INT32 col,INT64 value)
{
    BOOL ret = TRUE;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);

    if (cursor->update_int)
        return cursor->update_int(cursor,row,col,value);
    else
        return cursor_window_update_int(cursor->data_win,row,col,value);
}
BOOL cursor_update_bool(Cursor* cursor,INT32 row,INT32 col,BOOL value)

{
    BOOL ret = TRUE;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);
    if (cursor->update_bool)
        return cursor->update_bool(cursor,row,col,value);
    else
        return cursor_window_update_bool(cursor->data_win,row,col,value);


}
BOOL cursor_update_double(Cursor* cursor,INT32 row,INT32 col,double value)
{
    BOOL ret = TRUE;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);
    if (cursor->update_double)
        return cursor->update_double(cursor,row,col,value);
    else
        return cursor_window_update_double(cursor->data_win,row,col,value);


}

BOOL cursor_update_string(Cursor* cursor,INT32 row,INT32 col,const CHAR* value)
{
    BOOL ret = TRUE;
    INT32 len=0;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);
    if (cursor->update_string)
        return cursor->update_string(cursor,row,col,value);

    else
        return cursor_window_update_string(cursor->data_win,row,col,value);


}

BOOL cursor_update_string_16(Cursor* cursor,INT32 row,INT32 col,const WCHAR* value)
{
    BOOL ret = TRUE;
    INT32 len=0;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);
    if (cursor->update_string16)
        return cursor->update_string16(cursor,row,col,value);
    else
        return cursor_window_update_string_16(cursor->data_win,row,col,value);


}

BOOL cursor_update_blob(Cursor* cursor,INT32 row,INT32 col,UINT8* value,INT32 len)
{
    BOOL ret = TRUE;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);
    if (cursor->update_blobl)
        return  cursor->update_blobl(cursor,row,col,value,len);
    else
        return cursor_window_update_blob(cursor->data_win,row,col,value,len);

}


BOOL cursor_delete_row(Cursor* cursor,INT32 row)
{
    BOOL ret = TRUE;
    return_val_if_fail(cursor&&cursor->data_win,FALSE);
    return_val_if_fail(!cursor->read_only,FALSE);
    if (cursor->delete_row)
        return cursor->delete_row(cursor,row);
    else
        return cursor_window_delete_row(cursor->data_win,row);


}

