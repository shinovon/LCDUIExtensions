/*
 * Description: Automatically generated JNI lookup file. Do not modify manually.
 */

#include "javasymbianoslayer.h"
typedef void (*TFunc)();
#include "ru_nnproject_lcduiext_LCDUIExtensions.h"
const FuncTable funcTable[] = {
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonAlignment", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonAlignment},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonDimmed", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonDimmed},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFlags", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFlags},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFont", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFont},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonIcon", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonIcon},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonMinimumSize", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonMinimumSize},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonText", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonText},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonTextColor", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonTextColor},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonTooltipText", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonTooltipText},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageFont", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageFont},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageTooltipText", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageTooltipText},
   { "Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemParams", (unsigned int) Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemParams}
};

IMPORT_C TFunc jni_lookup(const char* name);
EXPORT_C TFunc jni_lookup(const char* name) {
    return (TFunc)findMethod(name, funcTable, sizeof(funcTable)/sizeof(FuncTable));
}
