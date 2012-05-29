
#include "CUnit.h"
#include "Automated.h"

#include "../cursor_window.h"
#include <common.h>

static int cursor_window_test_suit_init(void)
{

    return 0;
}
static int cursor_window_test_suit_clean(void)
{
  //  tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void cursor_window_test()
{


#define cursor_window_put_test_row 1000
#define cursor_window_put_test_col 10
    BOOL ret = FALSE;
    INT32 row_idx,col_idx;
    INT64 int_value;
    double double_value;
    BOOL bool_value;
    UINT8 buf[100];
    CHAR tmp_buf[100];
    INT32 row_num1,row_num2;
    CursorWindow* cw = cursor_window_create(cursor_window_put_test_col);
    if (!cw)
        return;

//int
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        ret = cursor_window_alloc_row(cw);
        CU_ASSERT_EQUAL(ret,TRUE);
        if (!ret)
        {
            cursor_window_destroy(cw);
            return;
        }
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_put_int(cw,col_idx,row_idx*col_idx),TRUE);
        }
    }
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_get_int(cw,row_idx,col_idx,&int_value),TRUE);
            CU_ASSERT_EQUAL(int_value,row_idx*col_idx);
        }
    }
    CU_ASSERT_EQUAL(cursor_window_get_row_num(cw),cursor_window_put_test_row);
//double
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        ret = cursor_window_alloc_row(cw);
        CU_ASSERT_EQUAL(ret,TRUE);
        if (!ret)
        {
            cursor_window_destroy(cw);
            return;
        }
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_put_double(cw,col_idx,row_idx*col_idx+0.1),TRUE);
        }
    }
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_get_double(cw,cursor_window_put_test_row+row_idx,col_idx,&double_value),TRUE);
            CU_ASSERT_EQUAL(double_value,row_idx*col_idx+0.1);
        }
    }
    CU_ASSERT_EQUAL(cursor_window_get_row_num(cw),cursor_window_put_test_row*2);
//string
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        CHAR* put_string_value = (CHAR*)buf;
        CHAR* get_string_value = NULL;
        const WCHAR* get_string_16_value=NULL;
        ret = cursor_window_alloc_row(cw);
        CU_ASSERT_EQUAL(ret,TRUE);
        if (!ret)
        {
            cursor_window_destroy(cw);
            return;
        }
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            sprintf(put_string_value,"row:%d,col:%d",row_idx,col_idx);
            if (strcmp("row:268,col:45",put_string_value)==0)
                ret=TRUE;
            ret = cursor_window_put_string(cw,col_idx,put_string_value);
            CU_ASSERT_EQUAL_PRINTF(ret,TRUE,put_string_value);

            get_string_value=(CHAR*)cursor_window_get_string(cw,cursor_window_get_row_num(cw)-1,col_idx);
            CU_ASSERT_PTR_NOT_NULL(get_string_value);
            if (get_string_value)
            {

                CU_ASSERT_STRING_EQUAL_PRINT(put_string_value,get_string_value,put_string_value);
            }
            get_string_16_value=cursor_window_get_string_16_coerce(cw,cursor_window_get_row_num(cw)-1,col_idx);


            CU_ASSERT_PTR_NOT_NULL(get_string_16_value);

        }
    }
    CU_ASSERT_EQUAL(cursor_window_get_row_num(cw),cursor_window_put_test_row*3);
//string 16
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        WCHAR* put_string_value = (WCHAR*)buf;
        const WCHAR* get_string_value = NULL;
        const CHAR*   get_string_8_value = NULL;
        ret = cursor_window_alloc_row(cw);
        CU_ASSERT_EQUAL(ret,TRUE);
        if (!ret)
        {
            cursor_window_destroy(cw);
            return;
        }
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            sprintf(tmp_buf,"row:%d,col:%d",row_idx,col_idx);
            Utf8ToUnicode(put_string_value, tmp_buf);

            CU_ASSERT_EQUAL(cursor_window_put_string_16(cw,col_idx,put_string_value),TRUE);
            get_string_value=(WCHAR*)cursor_window_get_string_16(cw,cursor_window_get_row_num(cw)-1,col_idx);
            CU_ASSERT_PTR_NOT_NULL(get_string_value);
            if (get_string_value)
            {

                CU_ASSERT_EQUAL(wstrcmp(put_string_value,get_string_value),0);
            }
            get_string_8_value=cursor_window_get_string_coerce(cw,cursor_window_get_row_num(cw)-1,col_idx);
            CU_ASSERT_PTR_NOT_NULL(get_string_8_value);
        }
    }

    CU_ASSERT_EQUAL(cursor_window_get_row_num(cw),cursor_window_put_test_row*4);
//bool
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        ret = cursor_window_alloc_row(cw);
        CU_ASSERT_EQUAL(ret,TRUE);
        if (!ret)
        {
            cursor_window_destroy(cw);
            return;
        }
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_put_bool(cw,col_idx,TRUE),TRUE);
        }
    }
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {

        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_get_bool(cw,cursor_window_put_test_row*4+row_idx,col_idx,&bool_value),TRUE);
            CU_ASSERT_EQUAL(bool_value,TRUE);
        }
    }
    CU_ASSERT_EQUAL(cursor_window_get_row_num(cw),cursor_window_put_test_row*5);
//get  data out of table
    CU_ASSERT_EQUAL(cursor_window_get_int(cw,cursor_window_put_test_row*100,0,&int_value),FALSE);
    CU_ASSERT_EQUAL(cursor_window_get_int(cw,0,cursor_window_put_test_col*100,&int_value),FALSE);
    CU_ASSERT_EQUAL(cursor_window_get_int(cw,0,-1,&int_value),FALSE);
    CU_ASSERT_EQUAL(cursor_window_get_int(cw,-1,-1,&int_value),FALSE);

//get  data using wrong type
    CU_ASSERT_EQUAL(cursor_window_get_int(cw,cursor_window_put_test_row*4+1,0,&int_value),FALSE);   //bool type
    CU_ASSERT_EQUAL(cursor_window_get_double(cw,1,0,&double_value),FALSE);   //int type


//update int
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_update_int(cw,row_idx,col_idx,row_idx*col_idx+1),TRUE);
        }
    }
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_get_int(cw,row_idx,col_idx,&int_value),TRUE);
            CU_ASSERT_EQUAL(int_value,row_idx*col_idx+1);
        }
    }

//update string
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        CHAR* put_string_value = (CHAR*)buf;
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            sprintf(put_string_value,"row:%d,col:%d 1",row_idx,col_idx);
            CU_ASSERT_EQUAL(cursor_window_update_string(cw,row_idx+cursor_window_put_test_row*2,col_idx,put_string_value),TRUE);
        }
    }
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        CHAR* put_string_value = (CHAR*)buf;
        CHAR* get_string_value = NULL;
        for (col_idx=0;col_idx<cursor_window_get_col_num(cw);col_idx++)
        {
            sprintf(put_string_value,"row:%d,col:%d 1",row_idx,col_idx);

            get_string_value=(CHAR*)cursor_window_get_string(cw,row_idx+cursor_window_put_test_row*2,col_idx);
            if (get_string_value)
            {

                CU_ASSERT_STRING_EQUAL_PRINT(put_string_value,get_string_value,put_string_value);
            }
        }
    }

    //delete
    row_num1=cursor_window_get_row_num(cw);
    for (row_idx=0;row_idx<cursor_window_put_test_row;row_idx++)
    {
        cursor_window_delete_row(cw,row_idx);
    }

    row_num2=cursor_window_get_row_num(cw);
    CU_ASSERT_EQUAL(row_num1,row_num2+cursor_window_put_test_row);


    cursor_window_destroy(cw);
}


static CU_TestInfo cursor_window_testcases[] =
{
    {"cursor_window_testcases:", cursor_window_test},
    CU_TEST_INFO_NULL
};



static CU_SuiteInfo cursor_window_suites[] = {
    {"Testing cursor_window_testcases:", NULL, cursor_window_test_suit_clean, cursor_window_testcases},
    CU_SUITE_INFO_NULL
};

static void cursor_window_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(cursor_window_suites))
    {
        return;
    }
}

void cursor_window_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        cursor_window_add_tests();
        CU_set_output_filename("cursor_window_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}
