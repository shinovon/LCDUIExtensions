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
	TBool aStrikethrough,
	TBool aDrawNow)
{
	CMIDStringItem* item = (CMIDStringItem*) aStringItem;
	if (aLabelColorSet) {
		TRgb color = TRgb(aLabelColor);
		CMIDItemLabel* label = (CMIDItemLabel*) item->ComponentControl(0);
		if (label) {
			// label->iColor = color;
			*(unsigned int *)((unsigned int)label + 88u) = color.Internal();
			
			AknLayoutUtils::OverrideControlColorL(*label, EColorLabelText, color);
			TInt len = label->CountComponentControls();
			for (TInt i = 0; i < len; ++i) {
				CEikLabel* l = (CEikLabel*) label->ComponentControl(i);
				l->OverrideColorL(EColorLabelText, color);
				if (aDrawNow && l->DrawableWindow())
					l->DrawNow();
			}
			if (aDrawNow) {
				if (label->DrawableWindow()) {
					label->DrawNow();
				} else {
					// item->iForm->DrawNow();
					((CCoeControl *)*((unsigned int *)label + 25u))->DrawNow();
				}
			}
		}
	}
	
	if (!aContentColorSet && !aStrikethrough) return;
	CMIDItemLabel* label = (CMIDItemLabel*) item->ComponentControl(1);
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
			if (aDrawNow && l->DrawableWindow())
				l->DrawDeferred();
		}
	}
	if (aDrawNow) {
		if (label->DrawableWindow()) {
			label->DrawNow();
		} else {
			// item->iForm->DrawNow();
			((CCoeControl *)*((unsigned int *)label + 25u))->DrawNow();
		}
	}
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemParams
 (JNIEnv *, jclass,
	jint aStringItem,
	jint aToolkit,
	jint aLabelColor,
	jint aColor,
	jboolean aLabelColorSet,
	jboolean aContentColorSet,
	jboolean aStrikethrough,
	jboolean aDrawNow)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	TBool labelColorSet = aLabelColorSet == JNI_TRUE;
	TBool colorSet = aContentColorSet == JNI_TRUE;
	TBool strikethrough = aStrikethrough == JNI_TRUE;
	TBool drawNow = aDrawNow == JNI_TRUE;
	return toolkit->ExecuteTrap(&SetStringItemParamsL,
		stringItem, aLabelColor, aColor, labelColorSet, colorSet, strikethrough, drawNow);
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
		if (button->DrawableWindow())
			button->DrawNow();
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
		if (button->DrawableWindow())
			button->DrawNow();
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

LOCAL_C void SetButtonAlignmentL(MMIDStringItem* aStringItem,
	TInt aHorizontal, TInt aVertical, TInt aTextAndIcon, TInt aIconHorizontal, TInt aIconVertical) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		if (aHorizontal != -1)
			button->SetTextHorizontalAlignment((CGraphicsContext::TTextAlign) aHorizontal);
		if (aVertical != -1)
			button->SetTextVerticalAlignment((CAknButton::TAlignment) aVertical);
		if (aTextAndIcon != -1)
			button->SetTextAndIconAlignment((CAknButton::TTextAndIconAlignment) aTextAndIcon);
		if (aIconHorizontal != -1)
			button->SetIconHorizontalAlignment((CAknButton::TAlignment) aIconHorizontal);
		if (aIconVertical != -1)
			button->SetIconVerticalAlignment((CAknButton::TAlignment) aIconVertical);
		if (button->DrawableWindow())
			button->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonAlignment
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit,
	 jint aHorizontal, jint aVertical, jint aIcon, jint aIconHorizontal, jint aIconVertical)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	return toolkit->ExecuteTrap(&SetButtonAlignmentL, stringItem,
		aHorizontal, aVertical, aIcon, aIconHorizontal, aIconVertical);
}

LOCAL_C void SetButtonTextL(MMIDStringItem* aStringItem, const TDesC* aText) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->State()->SetTextL(*aText);
		if (button->DrawableWindow())
			button->DrawNow();
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
		if (button->DrawableWindow())
			control->DrawNow();
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
		if (button->DrawableWindow())
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
	CMIDStringItem* item = (CMIDStringItem*) aStringItem;
	CMIDItemLabel* label = (CMIDItemLabel*) item->ComponentControl(0);
	if (label) {
		TInt len = label->CountComponentControls();
		for (TInt i = 0; i < len; ++i) {
			CEikLabel* l = (CEikLabel*) label->ComponentControl(i);
			l->SetUnderlining(aUnderlined);
			if (l->DrawableWindow())
				l->DrawNow();
		}
		if (label->DrawableWindow()) {
			label->DrawNow();
		} else {
			// item->iForm->DrawNow();
			((CCoeControl *)*((unsigned int *)label + 25u))->DrawNow();
		}
	}
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setStringItemUnderlined
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jboolean aUnderline)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	TBool underline = aUnderline == JNI_TRUE;
	return toolkit->ExecuteTrap(&SetUnderlinedL, stringItem, underline);
}

LOCAL_C void SetButtonDimmedL(MMIDStringItem* aStringItem, TBool aDimmed) {
	CCoeControl* control = (CMIDStringItem*) aStringItem;
	control = control->ComponentControl(1);
	if (CAknButton* button = dynamic_cast<CAknButton*>(control)) {
		button->SetDimmed(aDimmed);
		if (button->DrawableWindow())
			control->DrawNow();
		return;
	}
	User::Leave(KErrNotSupported);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonDimmed
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jboolean aDimmed)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	TBool dimmed = aDimmed == JNI_TRUE;
	return toolkit->ExecuteTrap(&SetButtonDimmedL, stringItem, dimmed);
}

LOCAL_C void SetButtonFontL(MMIDStringItem* aStringItem, MMIDFont* aFont) {
	aStringItem->SetFontL(aFont);
}

JNIEXPORT jint JNICALL Java_ru_nnproject_lcduiext_LCDUIExtensions__1setButtonFont
 (JNIEnv *aEnv, jclass, jint aStringItem, jint aToolkit, jint aFont)
{
	MMIDStringItem* stringItem = MIDUnhandObject<MMIDStringItem>(aStringItem);
	CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
	MMIDFont* font = MIDUnhandObject<MMIDFont>(aFont);
	return toolkit->ExecuteTrap(&SetButtonFontL, stringItem, font);
}
}
