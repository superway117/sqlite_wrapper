
#include "content_values.h"
#include <json_object.h>
#include <linkhash.h>
#include <printbuf.h>
#include <tg_utility.h>
#include "db_util.h"
#include <stdio.h>

#define CONTENT_VALUSES_DEF_HASH_ENTRIES 16

extern char *tg_strdup( const char *src );



struct _ContentValues
{
    struct lh_table* hash;
};

typedef struct
{
    void *data;
    UINT32 len;

}ContentValuesBlobEntry;

struct _ContentValuesEntry
{
    ContentValuesEntry_TYPE type;
    union data
    {
        BOOL c_bool;
        DOUBLE c_double;
        INT64 c_int;
        CHAR *c_string;
        ContentValuesBlobEntry c_blob;
        WCHAR *c_wstring;
    } o;

};



typedef struct  _ContentValuesEntry ContentValuesEntry;

static void _content_values_lh_entry_free(struct lh_entry *ent)
{

    TG_FREE(ent->k);
    TG_FREE((void*)ent->v);
}

ContentValues* content_values_create()
{
    ContentValues* thiz = NULL;

    struct lh_table* hash = lh_kchar_table_new(CONTENT_VALUSES_DEF_HASH_ENTRIES, NULL, &_content_values_lh_entry_free);
    return_val_if_fail(hash, NULL);

    thiz = TG_CALLOC_V2(sizeof(ContentValues));
    thiz->hash = hash;

    return thiz;
}


void content_values_destroy(ContentValues* thiz)
{
    return_if_fail(thiz);
    lh_table_free(thiz->hash);
    TG_FREE(thiz);
}

ContentValues* content_values_duplicate(ContentValues* values)
{

    ContentValues* dup_values = NULL;
    ContentValuesItor itor;
    ContentValuesEntry_TYPE type;
    UINT32 len;
    INT32 value_num;
    INT32 ret=0;
    INT32 i;
    ret=content_values_get_count(values,&value_num);
    return_val_if_fail((value_num>0 && ret==ContentValues_SUCC), NULL);
    dup_values = content_values_create();
    return_val_if_fail(dup_values, NULL);
    for (i=0,itor=content_values_first(values);i<value_num && itor;i++,itor=content_values_next(values,itor))
    {
        const void* value = content_values_get_value(itor,&type,&len);
	 const CHAR* key = content_values_get_key(itor);
        if (type==ContentValuesEntry_TYPE_INT)
            ret=content_values_put_int(dup_values,key,*((INT64*)value));
        else if (type==ContentValuesEntry_TYPE_DOUBLE)
            ret=content_values_put_double(dup_values,key,*((double*)value));
        else if (type==ContentValuesEntry_TYPE_BOOL)
        {
            ret=ret=content_values_put_bool(dup_values,key,*((BOOL*)value));
        }
        else if (type==ContentValuesEntry_TYPE_STRING)
        {
            ret=content_values_put_string(dup_values,key,(const CHAR*)value);
        }
        else if (type==ContentValuesEntry_TYPE_STRING_16)
        {
            ret=content_values_put_string_16(dup_values,key,(const WCHAR*)value);
        }
        else if (type==ContentValuesEntry_TYPE_BLOB)
        {

            ret=content_values_put_blob(dup_values,key,(const void*)value,len);
        }
    }


    return dup_values;

}

INT32 content_values_get_count(ContentValues* thiz,INT32* count)
{
   
    return_val_if_fail((thiz && thiz->hash),ContentValues_ERROR);

    *count = thiz->hash->count;
    return ContentValues_SUCC;

}

BOOL content_values_contains_key(ContentValues* thiz,const CHAR* key)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((thiz && thiz->hash),FALSE);
    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);
    return (entry?TRUE:FALSE);

}

INT32 content_values_get_blob(ContentValues* thiz,const CHAR* key,void** ret,UINT32* len)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_BLOB),ContentValues_KEY_NOT_EXIST);

    if ( entry->o.c_blob.data && entry->o.c_blob.len > 0)
    {
        *ret=TG_MALLOC(entry->o.c_blob.len );
        *len = entry->o.c_blob.len;
        memcpy(*ret,entry->o.c_blob.data,entry->o.c_blob.len);
    }
    else
    {
        *ret = NULL;
        *len =0;
    }
    return ContentValues_SUCC;

}

void* content_values_get_blob_v2(ContentValues* thiz,const CHAR* key,UINT32* len)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),NULL);
    *len = 0;
    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_BLOB),NULL);
    *len = entry->o.c_blob.len;
    return entry->o.c_blob.data;

}

INT32 content_values_get_string16(ContentValues* thiz,const CHAR* key,WCHAR** ret)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_STRING_16),ContentValues_KEY_NOT_EXIST);
    *ret=wstrdup(entry->o.c_wstring);

    return ContentValues_SUCC;

}

const WCHAR* content_values_get_string16_v2(ContentValues* thiz,const CHAR* key)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),NULL);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_STRING_16),NULL);

    return entry->o.c_wstring;

}

INT32 content_values_get_string(ContentValues* thiz,const CHAR* key,CHAR** ret)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_STRING),ContentValues_KEY_NOT_EXIST);
    if ( entry->o.c_string)
        *ret=tg_strdup(entry->o.c_string);
    else
        *ret = NULL;
    return ContentValues_SUCC;

}

const CHAR* content_values_get_string_v2(ContentValues* thiz,const CHAR* key)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),NULL);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_STRING),NULL);
    return  entry->o.c_string;

}

INT32 content_values_get_int(ContentValues* thiz,const CHAR* key,INT64* ret)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_INT),ContentValues_KEY_NOT_EXIST);
    *ret = entry->o.c_int;
    return ContentValues_SUCC;

}

INT32 content_values_get_double(ContentValues* thiz,const CHAR* key,double* ret)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_DOUBLE),ContentValues_KEY_NOT_EXIST);
    *ret = entry->o.c_double;
    return ContentValues_SUCC;

}


INT32 content_values_get_bool(ContentValues* thiz,const CHAR* key,BOOL* ret)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL && entry->type ==ContentValuesEntry_TYPE_BOOL),ContentValues_KEY_NOT_EXIST);
    *ret = entry->o.c_bool;
    return ContentValues_SUCC;

}




INT32 content_values_delete_key(ContentValues* thiz,const CHAR* key)
{
    ContentValuesEntry* entry = NULL;
    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);
    entry = (ContentValuesEntry*)lh_table_lookup(thiz->hash,key);

    return_val_if_fail((entry!=NULL),ContentValues_KEY_NOT_EXIST);
    lh_table_delete(thiz->hash,key);
    return ContentValues_SUCC;

}



INT32 content_values_put_blob(ContentValues* thiz,const CHAR* key,const void* value,UINT32 len)
{
    ContentValuesEntry* entry = NULL;

    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);


    entry = (ContentValuesEntry*)TG_CALLOC_V2(sizeof(ContentValuesEntry) + len);
    return_val_if_fail((entry),ContentValues_NO_MEMORY);

    entry->type=ContentValuesEntry_TYPE_BLOB;
    if (value)
    {
        entry->o.c_blob.data=(UINT8*)entry+ sizeof(ContentValuesEntry);
        entry->o.c_blob.len = len;
        memcpy(entry->o.c_blob.data,value,len);
    }

    lh_table_delete(thiz->hash,key);
    lh_table_insert(thiz->hash, tg_strdup(key), entry);

    return ContentValues_SUCC;

}

INT32 content_values_put_string_16(ContentValues* thiz,const CHAR* key,const WCHAR* value)
{
    ContentValuesEntry* entry = NULL;
    INT32 len = 0;

    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);
    if (value)
        len = 2*wstrlen(value)+2;

    entry = (ContentValuesEntry*)TG_CALLOC_V2(sizeof(ContentValuesEntry)+len);
    return_val_if_fail((entry),ContentValues_NO_MEMORY);

    entry->type=ContentValuesEntry_TYPE_STRING_16;
    if (value)
    {
        entry->o.c_wstring=(WCHAR*)((CHAR*)entry+ sizeof(ContentValuesEntry));
        wstrcpy(entry->o.c_wstring,value);
    }

    lh_table_delete(thiz->hash,key);
    lh_table_insert(thiz->hash, tg_strdup(key), entry);

    return ContentValues_SUCC;

}

INT32 content_values_put_string(ContentValues* thiz,const CHAR* key,const CHAR* value)
{
    ContentValuesEntry* entry = NULL;
    INT32 len = 0;

    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);
    if (value)
        len = strlen(value)+1;

    entry = (ContentValuesEntry*)TG_CALLOC_V2(sizeof(ContentValuesEntry)+len);
    return_val_if_fail((entry),ContentValues_NO_MEMORY);

    entry->type=ContentValuesEntry_TYPE_STRING;
    if (value)
    {
        entry->o.c_string=(CHAR*)entry+ sizeof(ContentValuesEntry);
        strcpy(entry->o.c_string,value);
    }

    lh_table_delete(thiz->hash,key);
    lh_table_insert(thiz->hash, tg_strdup(key), entry);

    return ContentValues_SUCC;

}
INT32 content_values_put_int(ContentValues* thiz,const CHAR* key,INT64 value)
{
    ContentValuesEntry* entry = NULL;

    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)TG_CALLOC_V2(sizeof(ContentValuesEntry));
    return_val_if_fail((entry),ContentValues_NO_MEMORY);
    entry->type=ContentValuesEntry_TYPE_INT;
    entry->o.c_int=value;
    lh_table_delete(thiz->hash,key);
    lh_table_insert(thiz->hash, tg_strdup(key), entry);

    return ContentValues_SUCC;

}

INT32 content_values_put_double(ContentValues* thiz,const CHAR* key,double value)
{
    ContentValuesEntry* entry = NULL;

    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)TG_CALLOC_V2(sizeof(ContentValuesEntry));
    return_val_if_fail((entry),ContentValues_NO_MEMORY);
    entry->type=ContentValuesEntry_TYPE_DOUBLE;
    entry->o.c_double=value;
    lh_table_delete(thiz->hash,key);
    lh_table_insert(thiz->hash, tg_strdup(key), entry);

    return ContentValues_SUCC;

}

INT32 content_values_put_bool(ContentValues* thiz,const CHAR* key,BOOL value)
{
    ContentValuesEntry* entry = NULL;

    return_val_if_fail((key&&thiz&&thiz->hash),ContentValues_ERROR);

    entry = (ContentValuesEntry*)TG_CALLOC_V2(sizeof(ContentValuesEntry));
    return_val_if_fail((entry),ContentValues_NO_MEMORY);
    entry->type=ContentValuesEntry_TYPE_BOOL;
    entry->o.c_bool=value;
    lh_table_delete(thiz->hash,key);
    lh_table_insert(thiz->hash, tg_strdup(key), entry);

    return ContentValues_SUCC;

}


ContentValuesItor content_values_first(ContentValues* thiz)
{


    return_val_if_fail((thiz&&thiz->hash),ContentValuesInvalidItor);
    return  (ContentValuesItor)thiz->hash->head;
}

ContentValuesItor content_values_next(ContentValues* thiz,ContentValuesItor* itor)
{
    struct lh_entry * entry =( struct lh_entry *)itor;

    return_val_if_fail((thiz&&thiz->hash),ContentValuesInvalidItor);
    return  (ContentValuesItor)entry->next;
}
const CHAR* content_values_get_key(ContentValuesItor* itor)
{
    struct lh_entry * entry =( struct lh_entry *)itor;
    return_val_if_fail((itor),NULL);
    return (const CHAR*)entry->k;
}

const void* content_values_get_value(ContentValuesItor* itor,ContentValuesEntry_TYPE* type,UINT32* len)
{
    void* data = NULL;
    ContentValuesEntry* content_entry = NULL;
    struct lh_entry * entry =( struct lh_entry *)itor;
    *len = 0;
    return_val_if_fail((itor),NULL);
    content_entry = (ContentValuesEntry*)entry->v;
    *type = content_entry->type;
    if (content_entry->type == ContentValuesEntry_TYPE_INT)
    {
        data =(void*)&content_entry->o.c_int;
        *len =sizeof(INT32);
    }
    else if (content_entry->type == ContentValuesEntry_TYPE_DOUBLE)
    {
        data =(void*)&content_entry->o.c_double;
        *len =sizeof(double);
    }
    else if (content_entry->type == ContentValuesEntry_TYPE_BOOL)
    {
        data =(void*)&content_entry->o.c_bool;
        *len =sizeof(BOOL);
    }
    else if (content_entry->type == ContentValuesEntry_TYPE_STRING)
    {
        data =(void*)content_entry->o.c_string;
        if (data)
            *len =strlen(content_entry->o.c_string)+1;
    }
    else if (content_entry->type == ContentValuesEntry_TYPE_STRING_16)
    {
        data =(void*)content_entry->o.c_wstring;
        if (data)
            *len =2*wstrlen(content_entry->o.c_wstring)+2;
    }
    else if (content_entry->type == ContentValuesEntry_TYPE_BLOB)
    {
        data =(void*)content_entry->o.c_blob.data;
        if (data)
            *len =content_entry->o.c_blob.len;
    }
    return data;

}

