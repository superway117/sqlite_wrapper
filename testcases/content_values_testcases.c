
#include "CUnit.h"
#include "Automated.h"

#include "../content_values.h"
#include "../db_util.h"
static int content_values_test_suit_init(void)
{

    return 0;
}
static int content_values_test_suit_clean(void)
{
  //  tg_heap_traverse(FILE_PRINT);
    return 0;
}



static void content_values_put_test()
{


    INT32 ret = 0;
    INT64 int_value;
    INT32 func_addr=0;
    double double_value;
    BOOL bool_value;
    WCHAR* uni_str = NULL;
    CHAR* str = NULL;
    void* serial_obj = NULL;
	UINT32 blob_len =0;
   ContentValuesItor itor;
    ContentValues* cv = content_values_create();
    if (!cv)
        return;

    CU_ASSERT_EQUAL(content_values_put_int(cv,"height",480),ContentValues_SUCC);
    ret = content_values_get_int(cv,"height",&int_value);
    CU_ASSERT_EQUAL(int_value,480);

    ret = content_values_get_int(cv,"width",&int_value);
    CU_ASSERT_EQUAL(ret,ContentValues_KEY_NOT_EXIST);


    CU_ASSERT_EQUAL(content_values_put_string_16(cv,"company",L"”¢∑…¡Ë"),ContentValues_SUCC);
    ret= content_values_get_string16(cv,"company",&uni_str);
    CU_ASSERT_EQUAL(ret,ContentValues_SUCC);
    if (ret==ContentValues_SUCC)
    {
        CU_ASSERT_EQUAL((wstrcmp(uni_str, L"”¢∑…¡Ë")),0);
        TG_FREE(uni_str);
    }

    CU_ASSERT_EQUAL(content_values_put_string(cv,"new_company","Intel"),ContentValues_SUCC);
    ret= content_values_get_string(cv,"new_company",&str);
    CU_ASSERT_EQUAL(ret,ContentValues_SUCC);
    if (ret==ContentValues_SUCC)
    {
        CU_ASSERT_EQUAL((strcmp(str, "Intel")),0);
        TG_FREE(str);
    }

    str= (CHAR*)content_values_get_string_v2(cv,"new_company");
    if (str)
    {
        CU_ASSERT_EQUAL((strcmp(str, "Intel")),0);
        str=NULL;
    }

    CU_ASSERT_EQUAL(content_values_put_double(cv,"widht",240.4),ContentValues_SUCC);
    ret = content_values_get_double(cv,"widht",&double_value);
    CU_ASSERT_EQUAL(double_value,240.4);

    CU_ASSERT_EQUAL(content_values_put_bool(cv,"has_trans",TRUE),ContentValues_SUCC);
    ret = content_values_get_bool(cv,"has_trans",&bool_value);
    CU_ASSERT_EQUAL(bool_value,TRUE);

    func_addr = (INT32)&content_values_put_test;
    CU_ASSERT_EQUAL( content_values_put_blob(cv,"put_func",(void*)&func_addr,4),ContentValues_SUCC);
    ret = content_values_get_blob(cv,"put_func",&serial_obj,&blob_len);
    CU_ASSERT_EQUAL(ret,ContentValues_SUCC);
    if (serial_obj)
    {
        CU_ASSERT_EQUAL(*((INT32*)serial_obj),(INT32)content_values_put_test);
        TG_FREE(serial_obj);
    }

    serial_obj = content_values_get_blob_v2(cv,"put_func",&blob_len);
    CU_ASSERT_EQUAL(ret,ContentValues_SUCC);
    if (serial_obj)
    {
        CU_ASSERT_EQUAL(*((INT32*)serial_obj),(INT32)content_values_put_test);
        CU_ASSERT_EQUAL(blob_len,4);
    }
    itor = content_values_first(cv);
    while(itor)
    {
    	str=content_values_get_key(itor);
    	itor = content_values_next(cv,itor);
    }
    CU_ASSERT_EQUAL(content_values_delete_key(cv, "widht"),ContentValues_SUCC);
    CU_ASSERT_EQUAL(content_values_delete_key(cv, "widht"),ContentValues_KEY_NOT_EXIST);
    ret = content_values_get_int(cv,"width",&int_value);

    CU_ASSERT_EQUAL(ret,ContentValues_KEY_NOT_EXIST);

    
    content_values_destroy(cv);
}


static CU_TestInfo content_values_put_testcases[] =
{
    {"content_values_put_testcases:", content_values_put_test},
    CU_TEST_INFO_NULL
};



static CU_SuiteInfo content_values_suites[] = {
    {"Testing content_values_put_testcases:", NULL, content_values_test_suit_clean, content_values_put_testcases},
    CU_SUITE_INFO_NULL
};

static void content_values_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(content_values_suites))
    {
        return;
    }
}

void content_values_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        content_values_add_tests();
        CU_set_output_filename("content_values_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}
