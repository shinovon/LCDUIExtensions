#include "CMIDToolkit.h"
#include "MIDUtils.h"
#include "ru_nnproject_lcduiext_LCDUIExtensions.h"
#include "CMIDStringItem.h"
#include "CMIDItemLabel.h"
#include "CMIDImageItem.h"
#include <eikcapc.h>
#include <gulicon.h>

LOCAL_C void SetStringItemParamsL
(MMIDStringItem* aStringItem,
	TInt aLabelColor,
	TInt aColor,
	TBool aLabelColorSet,
	TBool aContentColorSet,
	TBool aStrikethrough)
{
	CMIDStringItem* control = (CMIDStringItem*) aStringItem;
	if (aLabelColorSet) {
		TRgb labelColor = TRgb(aLabelColor);
		CMIDItemLabel* labelControl = (CMIDItemLabel*) control->ComponentControl(0);
		if (labelControl != NULL) {
			*(unsigned int *)((unsigned int)labelControl + 88u) = color.Internal();
//			AknLayoutUtils::OverrideControlColorL(*labelControl, EColorLabelText, labelColor);
			TInt len = labelControl->CountComponentControls();
			for (TInt i = 0; i < len; ++i) {
				CEikLabel* l = (CEikLabel*) labelControl->ComponentControl(i);
				l->OverrideColorL(EColorLabelText, labelColor);
				l->DrawNow();
			}
		}
	}
	if (!aContentColorSet && !aStrikethrough) return;
	CMIDItemLabel* label = (CMIDItemLabel*) control->ComponentControl(1);
	if (label == NULL) {
		User::Leave(KErrGeneral);
		return;
	}
	TRgb color = TRgb(aColor);
	if (aContentColorSet) {
		// label->iColor = color;
		*(unsigned int *)((unsigned int)label + 88u) = color.Internal();
//		AknLayoutUtils::OverrideControlColorL(*label, EColorLabelText, color);
	}
	TInt len = label->CountComponentControls();
	for (TInt i = 0; i < len; ++i) {
		CCoeControl* t = label->ComponentControl(i);
		if (t == NULL) continue;
		if (CEikLabel* l = dynamic_cast<CEikLabel*>(t)) {
			if (aContentColorSet)
				t->OverrideColorL(EColorLabelText, color);
			if (aStrikethrough)
				l->SetStrikethrough(ETrue);
			t->DrawNow();
		}
	}
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemParams
(JNIEnv *, jclass, jint aStringItem, jint aToolkit, jint aLabelColor, jint aColor, jboolean aLabelColorSet, jboolean aContentColorSet, jboolean aStrikethrough)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	TBool labelColorSet = aLabelColorSet == JNI_TRUE;
	TBool colorSet = aContentColorSet == JNI_TRUE;
	TBool strikethrough = aStrikethrough == JNI_TRUE;
	return toolkit->ExecuteTrap(&SetStringItemParamsL, stringItem, aLabelColor, aColor, labelColorSet, colorSet, strikethrough);
}

LOCAL_C void SetStringItemTooltipL(MMIDStringItem* aStringItem, const TDesC* aText) {

	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->State()->SetHelpTextL(*aText);
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonTooltipText
(JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jstring aText)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	RJString text(*aEnv,aText);
	return toolkit->ExecuteTrap(&SetStringItemTooltipL, stringItem, (const TDesC*)&text);
}

LOCAL_C void SetImageItemTooltipL(MMIDImageItem* aImageItem, const TDesC* aText) {

	CCoeControl* control = (CMIDImageItem*) aImageItem;
	if (control->CountComponentControls() < 2) {
		User::Leave(KErrNotSupported);
		return;
	}
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->State()->SetHelpTextL(*aText);
	}
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageItemButtonTooltipText
(JNIEnv *aEnv, jclass, jint aImageItem, jint aToolkit, jstring aText)
{
	MMIDImageItem* imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	RJString text(*aEnv,aText);
	return toolkit->ExecuteTrap(&SetImageItemTooltipL, imageItem, (const TDesC*)&text);
}

//LOCAL_C void SetImageItemTextL(MMIDImageItem* aImageItem, const TDesC* aText) {
//	CCoeControl* control = (CMIDImageItem*) aImageItem;
//	control = control->ComponentControl(1);
//	CAknButton* button = (CAknButton*) control;
//	button->SetButtonFlags(KAknButtonSizeFitText);
//	button->State()->SetTextL(*aText);
//	button->SetSize(button->MinimumSize());
//	button->DrawNow();
//}
//
//JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageItemButtonText
//(JNIEnv *aEnv, jclass, jint aImageItem, jint aToolkit, jstring aText)
//{
//	MMIDImageItem* imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
//	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
//	RJString text(*aEnv,aText);
//	return toolkit->ExecuteTrap(&SetImageItemTextL, imageItem, (const TDesC*)&text);
//}

LOCAL_C void SetStringItemImageL(MMIDStringItem* aStringItem, MMIDImageItem* aImageItem, TInt aFlags) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		CGulIcon* icon = NULL;
		TSize old = control->Size();
		TSize min = control->MinimumSize();
		if (aImageItem) {
			CCoeControl* imageControl = (CMIDImageItem*) aImageItem;
			imageControl = imageControl->ComponentControl(1);
			if (CAknButton* imgButton = dynamic_cast<CAknButton*>(imageControl)) {
				icon = const_cast<CGulIcon*>(imgButton->State()->Icon());
			}
		}
		button->SetButtonFlags(aFlags);
		button->State()->SetIcon(icon);
		button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonIcon
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aImageItem, jint aFlags)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	MMIDImageItem* imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
	return toolkit->ExecuteTrap(&SetStringItemImageL, stringItem, imageItem, aFlags);
}

LOCAL_C void SetStringItemButtonFlagsL(MMIDStringItem* aStringItem, TInt aFlags) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->SetButtonFlags(aFlags);
		button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonFlags
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aFlags)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetStringItemButtonFlagsL, stringItem, aFlags);
}

LOCAL_C void SetStringItemButtonAlignmentL(MMIDStringItem* aStringItem, TInt aHorizontal, TInt aVertical, TInt aIcon) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->SetTextHorizontalAlignment((CGraphicsContext::TTextAlign) aHorizontal);
		button->SetTextVerticalAlignment((CAknButton::TAlignment) aVertical);
		button->SetTextAndIconAlignment((CAknButton::TTextAndIconAlignment) aIcon);
		button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonAlignment
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aHorizontal, jint aVertical, jint aIcon)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetStringItemButtonAlignmentL, stringItem, aHorizontal, aVertical, aIcon);
}

LOCAL_C void SetStringItemButtonTextL(MMIDStringItem* aStringItem, const TDesC* aText) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->State()->SetTextL(*aText);
		button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonText
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jstring aText)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	RJString text(*aEnv,aText);
	return toolkit->ExecuteTrap(&SetStringItemButtonTextL, stringItem, (const TDesC*)&text);
}

LOCAL_C void SetStringItemButtonColorL(MMIDStringItem* aStringItem, TLogicalColor aType, TInt aColor) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	TRgb rgb = TRgb(aColor);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		control->OverrideColorL(aType, rgb);
		control->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonTextColor
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aColor)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetStringItemButtonColorL, stringItem, EColorButtonText, aColor);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonFaceColor
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aColor)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetStringItemButtonColorL, stringItem, EColorButtonFaceClear, aColor);
}

LOCAL_C void SetStringItemButtonMinimumSizeL(MMIDStringItem* aStringItem, TInt aHeight) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		TSize old = control->Size();
		TSize min = control->MinimumSize();
		control->SetSize(TSize(old.iWidth, aHeight > 0 ? aHeight : min.iHeight));
		control->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemButtonMinimumSize
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aHeight)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetStringItemButtonMinimumSizeL, stringItem, aHeight);
}
