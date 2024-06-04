#include "bootloader_patch.h"
#include "shell.h"


int8_t a_function_test(void) __attribute__((section(".patch_functions_1")));
int8_t a_function_test(void)
{
    printf("a_function_test ===\r\n");

    return 0;
}

int8_t a_function_test_1(void) __attribute__((section(".patch_functions_2")));
int8_t a_function_test_1(void)
{
    printf("a_function_test_1 ===\r\n");
    return 0;
}

int8_t a_function_test_2(void) __attribute__((section(".patch_functions_3")));
int8_t a_function_test_2(void)
{
    printf("a_function_test_2 ===\r\n");
    return 0;
}

int8_t a_function_test_3(void) __attribute__((section(".patch_functions_4")));
int8_t a_function_test_3(void)
{
    printf("a_function_test_3 ===\r\n");
    return 0;
}

int8_t run_patch_test(uint32_t addr)
{
    void (*func)(void) = (void (*)(void))addr;
    func();

    return 0;
}

#ifndef BOOTLOADER_PATCH_TEST
#include "shell.h"

int8_t func_test(void)
{
    printf("a_function_test addr: %#.8x\r\n", a_function_test);
    printf("a_function_test_1 addr: %#.8x\r\n", a_function_test_1);
    printf("a_function_test_2 addr: %#.8x\r\n", a_function_test_2);
    printf("a_function_test_3 addr: %#.8x\r\n", a_function_test_3);

    run_patch_test((uint32_t)a_function_test);
    run_patch_test((uint32_t)a_function_test_1);
    run_patch_test((uint32_t)a_function_test_2);
    run_patch_test((uint32_t)a_function_test_3);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(func_test, func_test, test function);

#endif