
#include "CUnit.h"
#include "Automated.h"

#include "../cursor.h"
#include <common.h>

static int cursor_test_suit_init(void)
{

    return 0;
}
static int cursor_test_suit_clean(void)
{
  //  tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void cursor_test()
{
#define cursor_put_test_row 1000
#define cursor_put_test_col 10
    INT32 row_idx,col_idx;
    BOOL ret;
    INT64 int_value;
    Cursor* cursor = cursor_create(cursor_put_test_col,TRUE);
    CursorWindow* data_win =  cursor_get_data_window(cursor);
    CursorWindow* name_win =  cursor_get_name_window(cursor);
    for (row_idx=0;row_idx<cursor_put_test_row;row_idx++)
    {
        ret = cursor_window_alloc_row(data_win);
        CU_ASSERT_EQUAL(ret,TRUE);
        if (!ret)
        {
            return;
        }
        for (col_idx=0;col_idx<cursor_get_column_num(cursor);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_window_put_int(data_win,col_idx,row_idx*col_idx),TRUE);
        }
    }
    CU_ASSERT_EQUAL(cursor_move_to_first(cursor),TRUE);
    for (row_idx=0;row_idx<cursor_put_test_row;row_idx++,cursor_move_to_next(cursor))
    {
        for (col_idx=0;col_idx<cursor_get_column_num(cursor);col_idx++)
        {
            CU_ASSERT_EQUAL(cursor_get_int(cursor,col_idx,&int_value),TRUE);
            CU_ASSERT_EQUAL(int_value,row_idx*col_idx);
        }
    }

    cursor_destroy(cursor);
}


static CU_TestInfo cursor_testcases[] =
{
    {"cursor_testcases:", cursor_test},
    CU_TEST_INFO_NULL
};



static CU_SuiteInfo cursor_suites[] = {
    {"Testing cursor_testcases:", NULL, cursor_test_suit_clean, cursor_testcases},
    CU_SUITE_INFO_NULL
};

static void cursor_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(cursor_suites))
    {
        return;
    }
}

void cursor_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        cursor_add_tests();
        CU_set_output_filename("cursor_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}
