/*
 * Description: Automatically generated JNI lookup file. Do not modify manually.
 */

#include "javasymbianoslayer.h"
typedef void (*TFunc)();
#include "ru_nnproject_lcduiext_LCDUIExtensions.h"
const FuncTable funcTable[] = {
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageItemButtonTooltipText", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageItemButtonTooltipText},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonAlignment", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonAlignment},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonFlags", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonFlags},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonIcon", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonIcon},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonMinimumSize", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonMinimumSize},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonText", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonText},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonTextColor", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonTextColor},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonTooltipText", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonTooltipText},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemParams", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemParams}
};

IMPORT_C TFunc jni_lookup(const char* name);
EXPORT_C TFunc jni_lookup(const char* name) {
    return (TFunc)findMethod(name, funcTable, sizeof(funcTable)/sizeof(FuncTable));
}
