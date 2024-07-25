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
		TRgb color = TRgb(aLabelColor);
		CMIDItemLabel* label = (CMIDItemLabel*) control->ComponentControl(0);
		if (label != NULL) {
			// label->iColor = color;
			*(unsigned int *)((unsigned int)label + 88u) = color.Internal();
			
			AknLayoutUtils::OverrideControlColorL(*label, EColorLabelText, color);
			TInt len = label->CountComponentControls();
			for (TInt i = 0; i < len; ++i) {
				CEikLabel* l = (CEikLabel*) label->ComponentControl(i);
				l->OverrideColorL(EColorLabelText, color);
				if (l->DrawableWindow()) l->DrawNow();
			}
			if (label->DrawableWindow()) label->DrawNow();
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
		
		AknLayoutUtils::OverrideControlColorL(*label, EColorLabelText, color);
	}
	TInt len = label->CountComponentControls();
	for (TInt i = 0; i < len; ++i) {
		CCoeControl* t = label->ComponentControl(i);
		if (t == NULL) continue;
		if (CEikLabel* l = dynamic_cast<CEikLabel*>(t)) {
			if (aContentColorSet)
				l->OverrideColorL(EColorLabelText, color);
			if (aStrikethrough)
				l->SetStrikethrough(ETrue);
			if (l->DrawableWindow()) l->DrawDeferred();
		}
		if (label->DrawableWindow()) label->DrawNow();
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

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonTooltipText
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

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setImageTooltipText
(JNIEnv *aEnv, jclass, jint aImageItem, jint aToolkit, jstring aText)
{
	MMIDImageItem* imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	RJString text(*aEnv,aText);
	return toolkit->ExecuteTrap(&SetImageItemTooltipL, imageItem, (const TDesC*)&text);
}

LOCAL_C void SetButtonImageL(MMIDStringItem* aStringItem, MMIDImageItem* aImageItem, TInt aFlags) {
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
		if (button->DrawableWindow()) button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonIcon
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aImageItem, jint aFlags)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	MMIDImageItem* imageItem = MIDUnhandObject<MMIDImageItem>(aImageItem);
	return toolkit->ExecuteTrap(&SetButtonImageL, stringItem, imageItem, aFlags);
}

LOCAL_C void SetButtonFlagsL(MMIDStringItem* aStringItem, TInt aFlags) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->SetButtonFlags(aFlags);
		if (button->DrawableWindow()) button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFlags
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aFlags)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetButtonFlagsL, stringItem, aFlags);
}

LOCAL_C void SetButtonAlignmentL(MMIDStringItem* aStringItem, TInt aHorizontal, TInt aVertical, TInt aIcon) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->SetTextHorizontalAlignment((CGraphicsContext::TTextAlign) aHorizontal);
		button->SetTextVerticalAlignment((CAknButton::TAlignment) aVertical);
		button->SetTextAndIconAlignment((CAknButton::TTextAndIconAlignment) aIcon);
		if (button->DrawableWindow()) button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonAlignment
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aHorizontal, jint aVertical, jint aIcon)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetButtonAlignmentL, stringItem, aHorizontal, aVertical, aIcon);
}

LOCAL_C void SetButtonTextL(MMIDStringItem* aStringItem, const TDesC* aText) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->State()->SetTextL(*aText);
		if (button->DrawableWindow()) button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonText
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jstring aText)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	RJString text(*aEnv,aText);
	return toolkit->ExecuteTrap(&SetButtonTextL, stringItem, (const TDesC*)&text);
}

LOCAL_C void SetButtonColorL(MMIDStringItem* aStringItem, TLogicalColor aType, TInt aColor) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	TRgb rgb = TRgb(aColor);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		control->OverrideColorL(aType, rgb);
		if (control->DrawableWindow()) control->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonTextColor
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aColor)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetButtonColorL, stringItem, EColorButtonText, aColor);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFaceColor
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aColor)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetButtonColorL, stringItem, EColorButtonFaceClear, aColor);
}

LOCAL_C void SetButtonMinimumSizeL(MMIDStringItem* aStringItem, TInt aHeight) {
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

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonMinimumSize
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aHeight)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetButtonMinimumSizeL, stringItem, aHeight);
}

LOCAL_C void SetUnderlinedL(MMIDStringItem* aStringItem, TBool aUnderlined) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CEikLabel* label = dynamic_cast<CEikLabel*>(control)) {
		label->SetUnderlining(aUnderlined);
		if (label->DrawableWindow()) label->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemUnderlined
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jboolean aUnderline)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetUnderlinedL, stringItem, (TBool) (aUnderline == JNI_TRUE));
}
