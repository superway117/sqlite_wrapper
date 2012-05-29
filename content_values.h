
#ifndef _DB_CONTENT_VALUES_H_
#define _DB_CONTENT_VALUES_H_
#include <common.h>

typedef enum
{
	ContentValues_SUCC=0,
	ContentValues_ERROR=-1,
	ContentValues_KEY_NOT_EXIST=-2,
	ContentValues_NO_MEMORY=-3,
}ContentValuesErrcode;

typedef enum
{
	ContentValuesEntry_TYPE_INT,
	ContentValuesEntry_TYPE_DOUBLE,
	ContentValuesEntry_TYPE_BOOL,
	ContentValuesEntry_TYPE_STRING,
	ContentValuesEntry_TYPE_STRING_16,
	ContentValuesEntry_TYPE_BLOB,
}ContentValuesEntry_TYPE;

typedef struct _ContentValues  ContentValues;


typedef void* ContentValuesItor;

#define ContentValuesInvalidItor NULL


typedef INT32 (*ContentValues_Traverse_Callback ) (ContentValuesEntry_TYPE type,const CHAR* key, void* value );

/**
     * @brief create a content value
     * @return ContentValues instance
*/
extern ContentValues* content_values_create();

/**
     * @brief destroy content value
     *
     * @param[in] thiz ContentValues handle
*/
extern void content_values_destroy(ContentValues* thiz);

/**
     * @brief duplicate content values
     *
     * @param[in] thiz ContentValues handle
*/
extern ContentValues* content_values_duplicate(ContentValues* values);

/**
     * @brief get values number
     *
     * @param[in] thiz ContentValues handle
     * @param[out] count save count 
     * @return error code
*/
extern INT32 content_values_get_count(ContentValues* thiz,INT32* count);

/**
     * @brief get if contain some key
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @return error code
*/
extern BOOL content_values_contains_key(ContentValues* thiz,const CHAR* key);

/**
     * @brief get blob value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] ret blob value, the buffer is a new buffer, need to be released by invoker
     * @param[out] len blob value length
     * @return error code
*/
extern INT32 content_values_get_blob(ContentValues* thiz,const CHAR* key,void** ret,UINT32* len);

/**
     * @brief get blob value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] len blob value length
     * @return blob value, the buffer do not need to be released
*/
extern void* content_values_get_blob_v2(ContentValues* thiz,const CHAR* key,UINT32* len);

/**
     * @brief get wstring value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] ret wstring value, the buffer is a new buffer, need to be released by caller
     * @return error code
*/
extern INT32 content_values_get_string16(ContentValues* thiz,const CHAR* key,WCHAR** ret);


/**
     * @brief get wstring value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @return wstring value
*/
extern const WCHAR* content_values_get_string16_v2(ContentValues* thiz,const CHAR* key);

/**
     * @brief get string value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] ret string value, the buffer is a new buffer, need to be released by caller
     * @return error code
*/
extern INT32 content_values_get_string(ContentValues* thiz,const CHAR* key,CHAR** ret);

/**
     * @brief get string value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @return string value
*/
extern const CHAR* content_values_get_string_v2(ContentValues* thiz,const CHAR* key);

/**
     * @brief get int64 value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] ret save int value
     * @return error code
*/
extern INT32 content_values_get_int(ContentValues* thiz,const CHAR* key,INT64* ret);

/**
     * @brief get double value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] ret save double value
     * @return error code
*/
extern INT32 content_values_get_double(ContentValues* thiz,const CHAR* key,double* ret);

/**
     * @brief get bool value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[out] ret save bool value
     * @return error code
*/
extern INT32 content_values_get_bool(ContentValues* thiz,const CHAR* key,BOOL* ret);

/**
     * @brief put blob value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[in] value blob value
     * @param[in] len  blob value length
     * @return error code
*/
extern INT32 content_values_put_blob(ContentValues* thiz,const CHAR* key,const void* value,UINT32 len);

/**
     * @brief put wstring value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[in] value wstring value
     * @return error code
*/
extern INT32 content_values_put_string_16(ContentValues* thiz,const CHAR* key,const WCHAR* value);

/**
     * @brief put string value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[in] value string value
     * @return error code
*/
extern INT32 content_values_put_string(ContentValues* thiz,const CHAR* key,const CHAR* value);


/**
     * @brief put int64 value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[in] value int64 value
     * @return error code
*/
extern INT32 content_values_put_int(ContentValues* thiz,const CHAR* key,INT64 value);

/**
     * @brief put double value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[in] value double value
     * @return error code
*/
extern INT32 content_values_put_double(ContentValues* thiz,const CHAR* key,double value);

/**
     * @brief put bool value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @param[in] value bool value
     * @return error code
*/
extern INT32 content_values_put_bool(ContentValues* thiz,const CHAR* key,BOOL value);

/**
     * @brief delete value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] key key string
     * @return error code
*/
extern INT32 content_values_delete_key(ContentValues* thiz,const CHAR* key);

/**
     * @brief get the first value
     *
     * @param[in] thiz ContentValues handle
     * @return ContentValuesItor is user for traversal the content value list
*/
extern ContentValuesItor content_values_first(ContentValues* thiz);

/**
     * @brief get the next value
     *
     * @param[in] thiz ContentValues handle
     * @param[in] itor ContentValuesItor handle
     * @return ContentValuesItor is user for traversal the content value list
*/
extern ContentValuesItor content_values_next(ContentValues* thiz,ContentValuesItor* itor);


/**
     * @brief get key 
     *
     * @param[in] itor ContentValuesItor handle
     * @return key string
*/
extern const CHAR* content_values_get_key(ContentValuesItor* itor);

/**
     * @brief get value 
     *
     * @param[in] itor ContentValuesItor handle
     * @param[out] type save type
     * @param[in] len save value length
     * @return value
*/
extern const void* content_values_get_value(ContentValuesItor* itor,ContentValuesEntry_TYPE* type,UINT32* len);
#endif




