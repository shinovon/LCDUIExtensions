package ru.nnproject.lcduiext;

import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui._LCDUIInvoker1;
import javax.microedition.lcdui.StringItem;
import javax.microedition.midlet.MIDlet;

import com.nokia.mid.ui.FreeSizeFontInvoker;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.support.Jvm;

public class LCDUIExtensions {
	
	// For Text&Icon button, by default text is on the right side.
	public static final int KAknButtonTextLeft = 0x00000001;
	// MinimumSize() will return the width to fit the text.
	public static final int KAknButtonSizeFitText = 0x00000002;
	// No frame is drawn for the button, useful for icon buttons.
	public static final int KAknButtonNoFrame = 0x00000004;
	
	public static final int TTextAlign_Left = 0;
	public static final int TTextAlign_Center = 1;
	public static final int TTextAlign_Right = 2;
	
	public static final int TAlignment_Center = 0;
	public static final int TAlignment_Top = 1;
	public static final int TAlignment_Bottom = 2;
	public static final int TAlignment_Right = 3;
	public static final int TAlignment_Left = 4;
	
	public static final int TTextAndIconAlignment_IconBeforeText = 0;
	public static final int TTextAndIconAlignment_IconAfterText = 1;
	public static final int TTextAndIconAlignment_IconUnderText = 2;
	public static final int TTextAndIconAlignment_IconOverText = 3;
	public static final int TTextAndIconAlignment_IconOverlay = 4;
	
	static final int UPDATE_INTERVAL = 2000;
	
	static final Hashtable components = new Hashtable();
	static final Object updateLock = new Object();
	static boolean loaded;
	
	// public
	
	public static boolean isSupported() {
		return loaded;
	}
	
	public static int getVersion() {
		return 1;
	}
	
	public static void setColor(StringItem item, int color) {
		if (item.getAppearanceMode() == Item.BUTTON && _LCDUIInvoker1.getCommandCount(item) > 0) {
			setButtonTextColor(item, color);
			return;
		}
		if (item.getAppearanceMode() != Item.PLAIN) {
			throw new IllegalArgumentException("StringItem appearance mode must be set to plain");
		}
		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
			p.contentColor = toBGR(color);
			p.contentColorSet = true;
		}
		update(item, p, false, true);
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static void setLabelColor(StringItem item, int color) {
		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
			p.labelColor = toBGR(color);
			p.labelColorSet = true;
		}
		update(item, p, false, true);
	}

	public static void setStrikethrough(StringItem item, boolean strikethrough) {
		if (item.getAppearanceMode() == Item.BUTTON) {
			throw new IllegalArgumentException("StringItem must not be button");
		}
		synchronized (components) {
			StringItemParams p = null;
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
			p.strikethrough = strikethrough;
			update(item, p, false, true);
		}
	}
	
	public static void setUnderline(StringItem item, boolean underline) {
		if (item.getAppearanceMode() == Item.BUTTON) {
			throw new IllegalArgumentException("StringItem must not be button");
		}
		ToolkitInvoker inv = ToolkitInvoker.getToolkitInvoker();
		checkError(_setStringItemUnderlined(inv.itemGetHandle(item), inv.toolkitGetHandle(inv.getToolkit()), underline));
	}
	
	// use with LAYOUT_EXPAND
	public static void setButtonIcon(StringItem item, Image image) {
		setButtonIcon(item, image == null ? null : new ImageItem(null, image, 0, null, Item.BUTTON));
	}

	public static void setButtonIcon(StringItem item, ImageItem image) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("ImageItem must be button");
		}
		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
		}
		// store icon so it won't get garbage collected
		p.icon = image;
		if (p.buttonFlags == 0) {
			p.buttonFlags = KAknButtonSizeFitText;
		}
		
		ToolkitInvoker inv = ToolkitInvoker.getToolkitInvoker();
		checkError(_setButtonIcon(inv.itemGetHandle(item),
				inv.toolkitGetHandle(inv.getToolkit()),
				image != null ? inv.itemGetHandle(image) : 0,
						p.buttonFlags));
		updateButtonSize(item, p);
		
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	// for calling setImage()
	public static ImageItem getButtonIconItem(StringItem item) {
		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
		}
		ImageItem img = p.icon;
		if (img == null) {
			img = new ImageItem(null, null, 0, null, Item.BUTTON);
		}
		components.put(img, item);
		return p.icon = img;
	}
	
	public static void setButtonTooltipText(StringItem item, String text) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		checkError(_setButtonTooltipText(getItemHandle(item), getToolkitHandle(), text));
	}
	
	public static void setButtonTooltipText(ImageItem item, String text) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("ImageItem must be button");
		}
		checkError(_setImageTooltipText(getItemHandle(item), getToolkitHandle(), text));
	}
	
	public static void setButtonFlags(StringItem item, int flags) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		if ((flags & ~(KAknButtonTextLeft | KAknButtonNoFrame | KAknButtonSizeFitText)) != 0) {
			throw new IllegalArgumentException("flags");
		}
		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
		}
		p.buttonFlags = flags;
		checkError(_setButtonFlags(getItemHandle(item), getToolkitHandle(), flags));
		updateButtonSize(item, p);
		
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	/**
	 * @param item Buttom item
	 * @param horizontal TTextAlign or -1
	 * @param vertical TAlignment or -1
	 * @param textAndIcon TTextAndIconAlignment or -1
	 * @param iconHorizontal TAlignment or -1
	 * @param iconVertical TAlignment or -1
	 */
	public static void setButtonAlignment(StringItem item,
			int horizontal, int vertical, int textAndIcon, int iconHorizontal, int iconVertical) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		if (horizontal < -1 || horizontal > 2) {
			throw new IllegalArgumentException("horizontal");
		}
		if (vertical < -1 || horizontal > 4) {
			throw new IllegalArgumentException("vertical");
		}
		if (textAndIcon < -1 || textAndIcon > 4) {
			throw new IllegalArgumentException("icon");
		}
		if (iconHorizontal < -1 || iconHorizontal > 4) {
			throw new IllegalArgumentException("iconHorizontal");
		}
		if (iconVertical < -1 || iconVertical > 4) {
			throw new IllegalArgumentException("iconVertical");
		}
		checkError(_setButtonAlignment(getItemHandle(item), getToolkitHandle(),
				horizontal, vertical, textAndIcon, iconHorizontal, iconVertical));
	}
	
	public static void setButtonText(StringItem item, String text) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		if (text == null) text = "";
		checkError(_setButtonText(getItemHandle(item), getToolkitHandle(), text));
		
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static void setButtonTextColor(StringItem item, int color) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
		}
		p.buttonTextColor = color = toBGR(color);
		p.buttonColorSet = true;
		
		checkError(_setButtonTextColor(getItemHandle(item), getToolkitHandle(), color));
	}
	
	public static void setButtonDimmed(StringItem item, boolean dimmed) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		
		checkError(_setButtonDimmed(getItemHandle(item), getToolkitHandle(), dimmed));
	}
	
	/**
	 * 
	 * @param item
	 * @param fontStyle {@link Font#STYLE_PLAIN}, or combination of 
	 * {@link Font#STYLE_BOLD},
	 * {@link Font#STYLE_ITALIC},
	 * {@link Font#STYLE_UNDERLINED}
	 * @param fontHeight Free size height
	 */
	public static void setButtonFont(StringItem item, int fontStyle, int fontHeight) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		Font font = FreeSizeFontInvoker.getFont(0, fontStyle, fontHeight);
		
		checkError(_setButtonFont(getItemHandle(item), getToolkitHandle(), _LCDUIInvoker1.getFontHandle(font)));
	}
	
	public static void setImageFont(ImageItem item, Font font) {
		if (item == null || font == null) throw new NullPointerException();
		
		checkError(_setImageFont(getItemHandle(item), getToolkitHandle(), _LCDUIInvoker1.getFontHandle(font)));
		
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static void unregisterExtension(Object component) {
		synchronized (components) {
			components.remove(component);
			if (component instanceof Form) {
				Form form = (Form) component;
				int size = form.size();
				while (size > 0) components.remove(_LCDUIInvoker1.formGet(form, size--));
			}
		}
	}
	
	public static void update() {
		synchronized (updateLock) {
			updateLock.notify();
		}
	}
	
	public static void update(Item item) {
		update(item, null, true, false);
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static Displayable getItemContainer(Item item) {
		return _LCDUIInvoker1.getItemScreen(item);
	}
	
	/**
	 * @deprecated use {@link Displayable#isShown()}
	 */
	public static boolean isForeground() {
	    Displayable displayable = getCurrent();
	    return displayable != null && displayable.isShown();
	}
	
	static Displayable getCurrent() {
		MIDlet midlet = LegacyRtPort.getMidlet();
		return Display.getDisplay(midlet).getCurrent();
	}
	
	// private

	static void updateButtonSize(StringItem item, StringItemParams p) {
		checkError(_setButtonMinimumSize(getItemHandle(item), getToolkitHandle(),
				p.icon != null ? p.icon.getImage().getHeight() +
						((p.buttonFlags & KAknButtonNoFrame) == 0 ? 12 : 0) : 0));
		item.setPreferredSize(640, -1); // will be limited to actual screen width in native
	}
	
	static void update(Object item, Object p, boolean resize, boolean drawNow) {
		if (item instanceof StringItem) {
			update((StringItem) item, (StringItemParams) p, resize, drawNow);
			return;
		}
		if (item instanceof ImageItem) {
			update(components.get(item), null, resize, drawNow);
			return;
		}
	}
	
	static void update(StringItem item, StringItemParams p, boolean resize, boolean drawNow) {
		if (item == null) return;
		try {
			if (p == null) p = (StringItemParams) components.get(item);
		} catch (Exception e) {
			return;
		}
		if (p == null) return;
		if (resize) {
			updateButtonSize(item, p);
		}
		if (p.buttonColorSet) {
			if (p.buttonColorSet) {
				checkError(
				_setButtonTextColor(
						getItemHandle(item), getToolkitHandle(),
						p.buttonTextColor
				));
			}
			return;
		}
		if (p.labelColorSet || p.contentColorSet) {
			synchronized (ToolkitInvoker.getToolkitInvoker().getToolkit()) {
				checkError(
				_setStringItemParams(
						getItemHandle(item), getToolkitHandle(),
						p.labelColor,
						p.contentColor,
						p.labelColorSet,
						p.contentColorSet,
						p.strikethrough, drawNow
				));
			}
		}
	}
	
	static void checkError(int r) {
		if (r < 0) {
			throw new RuntimeException("LCDUIExtensions native error " + r);
		}
	}

	private static int getToolkitHandle() {
		ToolkitInvoker inv = ToolkitInvoker.getToolkitInvoker();
		return inv.toolkitGetHandle(inv.getToolkit());
	}
	
	private static int getItemHandle(Object item) {
		return ToolkitInvoker.getToolkitInvoker().itemGetHandle(item);
	}
	
	private static int toBGR(int color) {
		int swap = color & 0xFF;
		swap = swap << 16 | (color >>> 16 & 0xFF);
		return color & 0xFF00FF00 | swap;
	}
	
	private static native int _setStringItemParams(int item, int toolkit,
			int labelColor, int contentColor,
			boolean labelColorSet, boolean contentColorSet,
			boolean strikethrough,
			boolean drawNow);

	private static native int _setButtonTooltipText(int item, int toolkit, String text);
	private static native int _setButtonIcon(int item, int toolkit, int image, int flags);
	private static native int _setImageTooltipText(int item, int toolkit, String text);
	private static native int _setButtonFlags(int item, int toolkit, int flags);
	private static native int _setButtonAlignment(int item, int toolkit, int hor, int ver, int icon, int iconHor, int iconVer);
	private static native int _setButtonText(int item, int toolkit, String text);
	private static native int _setButtonTextColor(int item, int toolkit, int color);
	private static native int _setButtonMinimumSize(int item, int toolkit, int height);
	private static native int _setStringItemUnderlined(int item, int toolkit, boolean underlined);
	private static native int _setButtonDimmed(int item, int toolkit, boolean dimmed);
	private static native int _setButtonFont(int item, int toolkit, int font);
	private static native int _setImageFont(int item, int toolkit, int font);
	
	static {
		try {
			Jvm.loadSystemLibrary("lcduiext");
			loaded = true;
			new Thread("LCDUIExtensions") {
				public void run() {
					try {
						while (true) {
							synchronized (updateLock) {
								updateLock.wait(UPDATE_INTERVAL);
							}
							Displayable d = getCurrent();
							if (!(d instanceof Form) || !d.isShown()) continue;
							Item redrawItem = null;
							synchronized (components) {
								if (components.isEmpty()) continue;
								Object k;
								for (Enumeration e = components.keys(); e.hasMoreElements(); ) {
									k = e.nextElement();
									if (k instanceof ImageItem ||
											(_LCDUIInvoker1.getItemScreen((Item) k) != d))
										continue;
									try {
										update(k, components.get(k), false, false);
										if (redrawItem == null) redrawItem = (Item) k;
									} catch (RuntimeException ex) {
										ex.printStackTrace();
									}
								}
							}
							if (redrawItem != null) {
//								_LCDUIInvoker1.itemRefreshForm(redrawItem);
								update(redrawItem, components.get(redrawItem), false, true);
							}
						}
					} catch (Throwable e) {
						e.printStackTrace();
					}
				}
			}.start();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}