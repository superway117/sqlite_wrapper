#include "cursor_window.h"
#include <tg_utility.h>
#include <common.h>

#define CURSOR_WINDOW_DEFAULT_CAPACITY_EACH_CHUNK  32

struct CursorWindowRowChunk;

typedef struct _CursorWindowRowChunk CursorWindowRowChunk;



struct _CursorWindowRowChunk
{

    INT32  row_num;     //real row num
    CursorWindowRowChunk* next_chunk;

//cached data
    UINT8* data;   //contain: row marks bit + all field slot, do not need to release standalonely

};


struct _CursorWindow
{

    INT32  chunk_num;

    INT32  capacity_each_chunk;
    INT32  col_num;

    CursorWindowRowChunk* last_chunk;

    CursorWindowRowChunk*  start_chunk;
};

#ifndef WIN32
typedef struct
{
    UINT8 type;
    union
    {
        double d;
        INT64 l;
        BOOL b;
        struct
        {
            UINT8* buf;
            INT32  size;
        } buffer;
    } data;
} CursorWindowFieldSlot;//__attribute__((packed)) CursorWindowFieldSlot;
#else
typedef struct
{
    UINT8 type;
    union
    {
        double d;
        INT64 l;
        BOOL b;
        struct
        {
            UINT8* buf;
            INT32  size;
        } buffer;
    } data;
}CursorWindowFieldSlot;
#endif






static CursorWindowFieldSlot* _cursor_window_locate_field_in_chunk(CursorWindow* thiz,CursorWindowRowChunk* chunk,INT32 row_in_chunk,INT32 col)
{

    CursorWindowFieldSlot* field = NULL;
    return_val_if_fail( (chunk->data && chunk->row_num>row_in_chunk),NULL);
    field =(CursorWindowFieldSlot* )(chunk->data+ sizeof(CursorWindowFieldSlot)*row_in_chunk*thiz->col_num)+col;
    return field;
}

static BOOL _cursor_window_locate_field(CursorWindow* thiz,INT32 row,INT32 col,INT32* row_in_chunk,CursorWindowRowChunk** chunk_out,CursorWindowFieldSlot** field_out)
{

    INT32 i = 0;
    CursorWindowRowChunk* chunk;
    CursorWindowFieldSlot* field;
    return_val_if_fail(thiz &&  thiz->col_num>col && row>=0 && col >=0,FALSE);
    for (chunk=thiz->start_chunk;chunk && i<thiz->chunk_num;i++,chunk=chunk->next_chunk)
    {

        if (row<chunk->row_num)
        {
            field =  _cursor_window_locate_field_in_chunk(thiz,chunk,row,col);
            if (field)
            {

                *chunk_out = chunk;
                *field_out = field;
                *row_in_chunk = row;
                return TRUE;
            }
        }
        row-=chunk->row_num;
    }
    return FALSE;
}
static BOOL  _cursor_window_locate_row(CursorWindow* thiz,INT32 row,INT32* row_in_chunk,CursorWindowRowChunk** chunk_out,CursorWindowFieldSlot** filed_out)
{
    return _cursor_window_locate_field(thiz,row,0,row_in_chunk,chunk_out,filed_out);
}


static CursorWindowFieldSlot* _cursor_window_locate_last_row_field_in_chunk(CursorWindow* thiz,INT32 col)
{
    return_val_if_fail(thiz->last_chunk&&  thiz->last_chunk->row_num>=1, NULL);
    return _cursor_window_locate_field_in_chunk(thiz,thiz->last_chunk,thiz->last_chunk->row_num-1,col);
}

static BOOL _cursor_window_buffer_field_set_value(CursorWindowFieldSlot* field,const void* value,INT32 len)
{

    if (value)
    {
        field->data.buffer.size = len;
        field->data.buffer.buf = TG_CALLOC_V2(len);
        return_val_if_fail(field->data.buffer.buf  , FALSE);
        memcpy(field->data.buffer.buf ,value,len);
    }
    else
    {
        field->data.buffer.size = 0;
        field->data.buffer.buf = NULL;
    }
    return TRUE;
}

static void _cursor_window_destroy_field(CursorWindowFieldSlot* field)
{
    if (field->type == FIELD_TYPE_STRING || field->type == FIELD_TYPE_STRING_16||field->type == FIELD_TYPE_BLOB)
    {
        if (field->data.buffer.size!=0 && field->data.buffer.buf)
        {
            TG_FREE(field->data.buffer.buf);
            field->data.buffer.size = 0;
        }
    }

}

static void _cursor_window_destroy_row(CursorWindowFieldSlot* field,INT32 col_num)
{
    INT32 col_idx;

    for (col_idx=0;col_idx<col_num;col_idx++,field++)
    {
        _cursor_window_destroy_field(field);
    }

}
static INT32 _cursor_window_get_chunk_size(INT32 capacity_each_chunk,INT32 col_num)
{
    return  sizeof(CursorWindowFieldSlot)*col_num*capacity_each_chunk + sizeof(CursorWindowRowChunk);

}

static void _cursor_window_destroy_row_chunk(CursorWindow* thiz,CursorWindowRowChunk* chunk,INT32 col_num)
{
    INT32 row=0;
    INT32 col=0;
    CursorWindowFieldSlot* field =NULL;
    for (;row<chunk->row_num;row++)
    {
        for (col=0;col<col_num;col++)
        {
            field=_cursor_window_locate_field_in_chunk(thiz,chunk,row,col);
            exit_if_fail(field);
            _cursor_window_destroy_field(field);
        }
    }
    TG_FREE(chunk);
}

static CursorWindowRowChunk* _cursor_window_create_chunk(INT32 capacity_each_chunk,INT32 col_num)
{
    CursorWindowRowChunk* chunk=TG_CALLOC_V2(_cursor_window_get_chunk_size(capacity_each_chunk,col_num));
    return_val_if_fail(chunk,NULL);
    chunk->data= (UINT8*)chunk+sizeof(CursorWindowRowChunk);
    chunk->next_chunk=NULL;
    chunk->row_num=0;
    return chunk;
}
CursorWindow* cursor_window_create_v2(INT32 capacity_each_chunk,INT32 col_num)
{
    CursorWindow* win = NULL;
    return_val_if_fail((capacity_each_chunk>0&& col_num>0),NULL);
    win = TG_CALLOC_V2(sizeof(CursorWindow));
    return_val_if_fail((win),NULL);


    win->capacity_each_chunk=capacity_each_chunk;
    win->col_num = col_num;
    win->chunk_num= 1;

//init first chunk
    win->start_chunk= _cursor_window_create_chunk(capacity_each_chunk,col_num);
    if (!win->start_chunk)
    {
        TG_FREE(win);
        return NULL;
    }

    win->last_chunk = win->start_chunk;
    return win;

}
CursorWindow* cursor_window_create(INT32 col_num)
{
    return cursor_window_create_v2(CURSOR_WINDOW_DEFAULT_CAPACITY_EACH_CHUNK,col_num);

}
CursorWindow* cursor_window_duplicate(CursorWindow*  src)
{
    CursorWindow* win =NULL;
    BOOL ret = FALSE;
    INT32 row,col;
    INT32 row_num=0;
    UINT8 type;
    INT32 len;
    void* data =NULL;
    return_val_if_fail(src && src->col_num>0,NULL);

    win= cursor_window_create_v2(src->capacity_each_chunk,src->col_num);
    return_val_if_fail(win,NULL);
    for (row=0,row_num=cursor_window_get_row_num(src);row<row_num;row++)
    {
        if (!cursor_window_alloc_row(win))
        {
        	ret=FALSE;
			break;
        }
        for (col=0;col<src->col_num;col++)
        {
            if (cursor_window_get_value(src,row,col,&type,&data,&len))
            {
                

                    switch (type)
                    {
                    case FIELD_TYPE_NULL:
                        ret=cursor_window_put_null(win,col);
                        break;
                    case FIELD_TYPE_INTEGER:
                        ret=cursor_window_put_int(win, col,*((INT64*)data));
                        break;
                    case FIELD_TYPE_BOOL:
                        ret=cursor_window_put_bool(win, col,*((BOOL*)data));
                        break;
                    case FIELD_TYPE_FLOAT:
                        ret=cursor_window_put_double(win, col,*((double*)data));
                        break;
                    case FIELD_TYPE_STRING:
                        ret=cursor_window_put_string(win, col,(const CHAR*)data);
                        break;
                    case FIELD_TYPE_STRING_16:
                        ret=cursor_window_put_string_16(win, col,(const WCHAR*)data);
                        break;
                    case FIELD_TYPE_BLOB:
                        ret=cursor_window_put_blob(win, col,data,len);
                        break;

                    }
                    if (!ret)
                        break;
      
            }
            else
            {
                ret = FALSE;
                break;
            }
        }

    }

    if (!ret)
    {
        cursor_window_destroy(win);
        win = NULL;
    }
    return win;
}
void cursor_window_destroy(CursorWindow* thiz)
{
    CursorWindowFieldSlot* field =NULL;
    CursorWindowRowChunk* chunk;
    CursorWindowRowChunk* next_chunk;
    INT32 i=0;
    return_if_fail(thiz);

    for (chunk=thiz->start_chunk; chunk && i < thiz->chunk_num; i++,chunk=next_chunk)
    {

        next_chunk = chunk->next_chunk;
        _cursor_window_destroy_row_chunk(thiz,chunk,thiz->col_num);
    }
    TG_FREE(thiz);
}



INT32 cursor_window_get_col_num(CursorWindow* thiz)
{

    return_val_if_fail(thiz,0);
    return thiz->col_num;
}

INT32 cursor_window_get_row_num(CursorWindow* thiz)
{

    CursorWindowRowChunk* chunk;

    INT32 i=0;
    INT32 count = 0;
    return_val_if_fail(thiz,0);

    for (chunk=thiz->start_chunk; chunk && i < thiz->chunk_num; i++,chunk=chunk->next_chunk)
    {

        count+=chunk->row_num;

    }
    return count;
}

BOOL cursor_window_alloc_row(CursorWindow* thiz)
{

    return_val_if_fail(thiz && thiz->last_chunk,FALSE);

    if (thiz->last_chunk->row_num>=thiz->capacity_each_chunk)
    {
        thiz->last_chunk->next_chunk = _cursor_window_create_chunk(thiz->capacity_each_chunk,thiz->col_num);
        return_val_if_fail(thiz->last_chunk->next_chunk , FALSE);
        thiz->last_chunk = thiz->last_chunk->next_chunk ;
        thiz->chunk_num++;

    }
    thiz->last_chunk->row_num++;
    return TRUE;
}

BOOL cursor_window_put_int(CursorWindow* thiz,INT32 col,INT64 value)
{
    CursorWindowFieldSlot* field = NULL;
    return_val_if_fail(thiz , FALSE);
    field = _cursor_window_locate_last_row_field_in_chunk(thiz,col);
    return_val_if_fail(field , FALSE);
    field->type=FIELD_TYPE_INTEGER;
    field->data.l = value;
    return TRUE;
}
BOOL cursor_window_put_bool(CursorWindow* thiz,INT32 col,BOOL value)
{
    CursorWindowFieldSlot* field = NULL;
    return_val_if_fail(thiz , FALSE);
    field = _cursor_window_locate_last_row_field_in_chunk(thiz,col);
    return_val_if_fail(field , FALSE);
    field->type=FIELD_TYPE_BOOL;
    field->data.b= value;
    return TRUE;
}

BOOL cursor_window_put_double(CursorWindow* thiz,INT32 col,double value)
{
    CursorWindowFieldSlot* field = NULL;
    return_val_if_fail(thiz , FALSE);
    field = _cursor_window_locate_last_row_field_in_chunk(thiz,col);
    return_val_if_fail(field , FALSE);
    field->type=FIELD_TYPE_FLOAT;
    field->data.d = value;
    return TRUE;
}

BOOL cursor_window_put_null(CursorWindow* thiz,INT32 col)
{
    CursorWindowFieldSlot* field = NULL;
    return_val_if_fail(thiz , FALSE);
    field = _cursor_window_locate_last_row_field_in_chunk(thiz,col);
    return_val_if_fail(field , FALSE);
    field->type=FIELD_TYPE_NULL;
    field->data.buffer.size = 0;
    field->data.buffer.buf = NULL;
    return TRUE;
}
static BOOL _cursor_window_put_buffer_field(CursorWindow* thiz,INT32 col,UINT8 type,const void* value,INT32 len)
{
    CursorWindowFieldSlot* field = NULL;
    return_val_if_fail(thiz , FALSE);
    field = _cursor_window_locate_last_row_field_in_chunk(thiz,col);
    return_val_if_fail(field , FALSE);
    field->type=type;
    return _cursor_window_buffer_field_set_value(field,value,len);
}

BOOL cursor_window_put_string(CursorWindow* thiz,INT32 col,const CHAR* value)
{

    INT32 len = 0;
    if (value)
        len = strlen(value)+1;
    return _cursor_window_put_buffer_field(thiz,col,FIELD_TYPE_STRING,(const void*)value,len);
}

BOOL cursor_window_put_string_16(CursorWindow* thiz,INT32 col,const WCHAR* value)
{

    INT32 len = 0;
    if (value)
        len = 2*wstrlen(value)+2;
    return _cursor_window_put_buffer_field(thiz,col,FIELD_TYPE_STRING_16,(const void*)value,len);
}

BOOL cursor_window_put_blob(CursorWindow* thiz,INT32 col,const void* value,INT32 len)
{
    return _cursor_window_put_buffer_field(thiz,col,FIELD_TYPE_BLOB,(const void*)value,len);
}

BOOL cursor_window_update_int(CursorWindow* thiz,INT32 row,INT32 col,INT64 value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_INTEGER,FALSE);
    field->data.l = value;
    return TRUE;
}

BOOL cursor_window_update_bool(CursorWindow* thiz,INT32 row,INT32 col,BOOL value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_BOOL,FALSE);
    field->data.b= value;
    return TRUE;
}

BOOL cursor_window_update_double(CursorWindow* thiz,INT32 row,INT32 col,double value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_FLOAT,FALSE);
    field->data.d= value;
    return TRUE;
}

BOOL cursor_window_update_string(CursorWindow* thiz,INT32 row,INT32 col,const CHAR* value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;

    INT32 row_in_chunk=0;
    INT32 len = 0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_STRING||field->type==FIELD_TYPE_NULL,FALSE);
    _cursor_window_destroy_field(field);
    field->type=FIELD_TYPE_STRING;
    if (value)
        len = strlen(value)+1;
    return _cursor_window_buffer_field_set_value(field,(const void*)value,len);
}

BOOL cursor_window_update_string_16(CursorWindow* thiz,INT32 row,INT32 col,const WCHAR* value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    INT32 len = 0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_STRING_16||field->type==FIELD_TYPE_NULL,FALSE);
    _cursor_window_destroy_field(field);
    field->type=FIELD_TYPE_STRING_16;
    if (value)
        len = 2*wstrlen(value)+2;
    return _cursor_window_buffer_field_set_value(field,(const void*)value,len);
}

BOOL cursor_window_update_blob(CursorWindow* thiz,INT32 row,INT32 col,const void* value,INT32 len)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_BLOB||field->type==FIELD_TYPE_NULL,FALSE);
    _cursor_window_destroy_field(field);
    field->type=FIELD_TYPE_BLOB;
    return _cursor_window_buffer_field_set_value(field,(const void*)value,len);
}


BOOL cursor_window_get_int(CursorWindow* thiz,INT32 row,INT32 col,INT64* value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail((field->type==FIELD_TYPE_INTEGER),FALSE);
    *value=field->data.l;
    return TRUE;
}

BOOL cursor_window_get_bool(CursorWindow* thiz,INT32 row,INT32 col,BOOL* value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail((field->type==FIELD_TYPE_BOOL),FALSE);
    *value=field->data.b;
    return TRUE;
}

BOOL cursor_window_get_double(CursorWindow* thiz,INT32 row,INT32 col,double* value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail((field->type==FIELD_TYPE_FLOAT),FALSE);
    *value=field->data.d;
    return TRUE;
}

#if 0
BOOL cursor_window_get_string(CursorWindow* thiz,INT32 row,INT32 col,CHAR** value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_STRING||field->type==FIELD_TYPE_NULL,FALSE);
    if (field->data.buffer.size==0 ||  field->data.buffer.buf==NULL)
    {
        *value = NULL;
        return TRUE;
    }

    *value = TG_CALLOC_V2(field->data.buffer.size);
    return_val_if_fail(*value , FALSE);
    strcpy(*value,field->data.buffer.buf);

    return TRUE;

}
#endif

const CHAR* cursor_window_get_string(CursorWindow* thiz,INT32 row,INT32 col)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , NULL);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),NULL);
    return_val_if_fail(field->type==FIELD_TYPE_STRING||field->type==FIELD_TYPE_NULL,NULL);
    return (const CHAR*)field->data.buffer.buf;

}
const CHAR* cursor_window_get_string_coerce(CursorWindow* thiz,INT32 row,INT32 col)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , NULL);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),NULL);
    if (field->type==FIELD_TYPE_STRING||field->type==FIELD_TYPE_NULL)
    {
        return (const CHAR*)field->data.buffer.buf;
    }
    else if (field->type==FIELD_TYPE_STRING_16)
    {
        INT32 len = wstrlen((const WCHAR*)field->data.buffer.buf);
        CHAR* utf8_str = TG_CALLOC_V2(len*6+2);
        return_val_if_fail(utf8_str,NULL);
        tg_UCS2ToUTF8((WCHAR*)field->data.buffer.buf, utf8_str);
        TG_FREE(field->data.buffer.buf);
        field->data.buffer.buf= utf8_str;
        field->data.buffer.size = strlen((const CHAR*)field->data.buffer.buf)+1;
        field->type=FIELD_TYPE_STRING;
        return (const CHAR*)field->data.buffer.buf;
    }
    else
        return NULL;

}

#if 0
BOOL cursor_window_get_string_16(CursorWindow* thiz,INT32 row,INT32 col, WCHAR** value)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_STRING_16||field->type==FIELD_TYPE_NULL,FALSE);
    if (field->data.buffer.size==0 ||  field->data.buffer.buf==NULL)
    {
        *value = NULL;
        return TRUE;
    }

    *value = TG_CALLOC_V2(field->data.buffer.size);
    return_val_if_fail(*value , FALSE);
    wstrcpy(*value,(const WCHAR*)field->data.buffer.buf);

    return TRUE;

}
#endif

const WCHAR* cursor_window_get_string_16(CursorWindow* thiz,INT32 row,INT32 col)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , NULL);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),NULL);
    return_val_if_fail(field->type==FIELD_TYPE_STRING_16||field->type==FIELD_TYPE_NULL,NULL);
    return (const WCHAR*)field->data.buffer.buf;

}

const WCHAR* cursor_window_get_string_16_coerce(CursorWindow* thiz,INT32 row,INT32 col)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , NULL);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),NULL);
    if (field->type==FIELD_TYPE_STRING_16||field->type==FIELD_TYPE_NULL)
        return (const WCHAR*)field->data.buffer.buf;
    else if (field->type==FIELD_TYPE_STRING)
    {
        INT32 len = strlen((const CHAR*)field->data.buffer.buf);
        WCHAR* utf16_str = TG_CALLOC_V2(len*2+2);
        return_val_if_fail(utf16_str , NULL);
        tg_UTF8ToUCS2(( CHAR*)field->data.buffer.buf,utf16_str);
        TG_FREE(field->data.buffer.buf);
        field->data.buffer.buf= (UINT8*)utf16_str;
        field->data.buffer.size = wstrlen((const WCHAR*)field->data.buffer.buf)*2+2;
        field->type=FIELD_TYPE_STRING_16;
        return (const WCHAR*)field->data.buffer.buf;
    }
    else
        return NULL;

}
#if 0
BOOL cursor_window_get_blob(CursorWindow* thiz,INT32 row,INT32 col,void** value,INT32* len)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(field->type==FIELD_TYPE_BLOB||field->type==FIELD_TYPE_NULL,FALSE);
    if (field->data.buffer.size==0 ||  field->data.buffer.buf==NULL)
    {
        *len = 0;
        *value = NULL;
        return TRUE;
    }

    *value = TG_CALLOC_V2(field->data.buffer.size);
    return_val_if_fail(*value , FALSE);
    memcpy(*value,field->data.buffer.buf,field->data.buffer.size);
    *len = field->data.buffer.size;
    return TRUE;

}
#endif
const void* cursor_window_get_blob(CursorWindow* thiz,INT32 row,INT32 col,INT32* len)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , NULL);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),NULL);
    return_val_if_fail(field->type==FIELD_TYPE_BLOB||field->type==FIELD_TYPE_NULL,NULL);
    *len = field->data.buffer.size;
    return (const void*)field->data.buffer.buf;

}

BOOL cursor_window_get_value(CursorWindow* thiz,INT32 row,INT32 col,UINT8* type,void** data,INT32* len)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    *type = field->type;
    if (field->type==FIELD_TYPE_BLOB || field->type==FIELD_TYPE_STRING || field->type==FIELD_TYPE_STRING_16)
    {
        *len = field->data.buffer.size;
        *data = (void*)field->data.buffer.buf;
    }
    else if (field->type==FIELD_TYPE_INTEGER)
    {
        *len = sizeof(INT64);
        *data = (void*)&field->data.l;
    }
    else if (field->type==FIELD_TYPE_BOOL)
    {
        *len = sizeof(BOOL);
        *data = (void*)&field->data.b;
    }
    else if (field->type==FIELD_TYPE_FLOAT)
    {
        *len = sizeof(double);
        *data = (void*)&field->data.d;
    }
    else if (field->type==FIELD_TYPE_NULL)
    {
        *data=NULL;
        *len = 0;
    }
    else
        exit_if_fail(FALSE);
    return TRUE;


}
BOOL cursor_window_get_type(CursorWindow* thiz,INT32 row,INT32 col,UINT8* type)
{
    CursorWindowFieldSlot* field = NULL;
    CursorWindowRowChunk* chunk=NULL;
    INT32 row_in_chunk=0;
    return_val_if_fail(thiz , FALSE);
    return_val_if_fail(_cursor_window_locate_field(thiz,row,col,&row_in_chunk,&chunk,&field),FALSE);
    *type = field->type;
    return TRUE;
}


BOOL cursor_window_delete_row(CursorWindow* thiz,INT32 row)
{
    CursorWindowFieldSlot* field =NULL;
    CursorWindowFieldSlot* next_row = NULL;
    CursorWindowFieldSlot* prior_row = NULL;
    INT32 row_in_chunk=0;
    INT32 row_idx;
    CursorWindowRowChunk* chunk=NULL;
    return_val_if_fail(thiz && row >=0, FALSE);
    return_val_if_fail(_cursor_window_locate_row(thiz,row,&row_in_chunk,&chunk,&field),FALSE);
    return_val_if_fail(chunk && field, FALSE);

    _cursor_window_destroy_row(field,thiz->col_num);
    if (chunk->row_num==1)  //delete the last row in the chunk, directly remove this chunk
    {
        CursorWindowRowChunk* prior_chunk;
        for (prior_chunk=thiz->start_chunk; prior_chunk && prior_chunk->next_chunk!=chunk; prior_chunk = prior_chunk->next_chunk);
        if (prior_chunk)
        {
            prior_chunk->next_chunk = chunk->next_chunk;
            thiz->chunk_num--;
        }
    }
    else
    {
        next_row = field+thiz->col_num;
        prior_row = field;
        for (row_idx=row_in_chunk+1; row_idx < chunk->row_num; row_idx++)
        {

            memcpy(prior_row,next_row,sizeof(CursorWindowFieldSlot)*thiz->col_num);
            prior_row=next_row;
            next_row+=thiz->col_num;
        }
        chunk->row_num--;
    }
    return TRUE;
}
BOOL cursor_window_delete_last_row(CursorWindow* thiz)
{
    INT32 row_num = cursor_window_get_row_num(thiz);
    if (row_num>0)
        return cursor_window_delete_row(thiz,row_num-1);
    return FALSE;

}
