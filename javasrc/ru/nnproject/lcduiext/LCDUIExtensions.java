package ru.nnproject.lcduiext;

import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui._LCDUIInvoker1;
import javax.microedition.lcdui.StringItem;
import javax.microedition.midlet.MIDlet;

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
	
	public static final int BUTTON_HORIZONTAL_LEFT = 0;
	public static final int BUTTON_HORIZONTAL_CENTER = 1;
	public static final int BUTTON_HORIZONTAL_RIGHT = 2;
	
	public static final int BUTTON_VERTICAL_CENTER = 0;
	public static final int BUTTON_VERTICAL_TOP = 1;
	public static final int BUTTON_VERTICAL_BOTTOM = 2;
	public static final int BUTTON_VERTICAL_RIGHT = 3;
	public static final int BUTTON_VERTICAL_LEFT = 4;
	
	public static final int BUTTON_ICON_BEFORE_TEXT = 0;
	public static final int BUTTON_ICON_AFTER_TEXT = 1;
	public static final int BUTTON_ICON_UNDER_TEXT = 2;
	public static final int BUTTON_ICON_OVER_TEXT = 3;
	public static final int BUTTON_ICON_OVERLAY = 4;
	
	static final int UPDATE_INTERVAL = 2000;
	
	static final Hashtable components = new Hashtable();
	static final Object updateLock = new Object();
	static boolean loaded;
	
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
							if (!isForeground()) continue;
							synchronized (components) {
								Object k;
								for (Enumeration e = components.keys(); e.hasMoreElements(); ) {
									k = e.nextElement();
									if (k instanceof ImageItem) continue;
									try {
										update(k, components.get(k), false);
									} catch (RuntimeException ex) {
										ex.printStackTrace();
									}
								}
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
	
	//
	
	public static boolean isSupported() {
		return loaded;
	}
	
	public static int getVersion() {
		return 1;
	}
	
	//
	
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
		update(item, p, false);
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
		update(item, p, false);
	}

	public static void setStrikethrough(StringItem item, boolean strikethrough) {
		if (item.getAppearanceMode() != Item.PLAIN) {
			throw new IllegalArgumentException("StringItem appearance mode must be set to plain");
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
			update(item, p, false);
		}
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
		checkError(_setStringItemButtonIcon(inv.itemGetHandle(item),
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
		checkError(_setStringItemButtonTooltipText(getItemHandle(item), getToolkitHandle(), text));
	}
	
	public static void setButtonTooltipText(ImageItem item, String text) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("ImageItem must be button");
		}
		checkError(_setImageItemButtonTooltipText(getItemHandle(item), getToolkitHandle(), text));
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
		checkError(_setStringItemButtonFlags(getItemHandle(item), getToolkitHandle(), flags));
		updateButtonSize(item, p);
		
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static void setButtonAlignment(StringItem item, int horizontal, int vertical, int icon) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}
		if (horizontal < 0 || horizontal > 2) {
			throw new IllegalArgumentException("horizontal");
		}
		if (vertical < 0 || horizontal > 4) {
			throw new IllegalArgumentException("vertical");
		}
		if (horizontal < 0 || horizontal > 4) {
			throw new IllegalArgumentException("icon");
		}
		checkError(_setStringItemButtonAlignment(getItemHandle(item), getToolkitHandle(),
				horizontal, vertical, icon));
	}
	
	public static void setButtonText(StringItem item, String text) {
		if (text == null) text = "";
		checkError(_setStringItemButtonText(getItemHandle(item), getToolkitHandle(), text));
		
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static void setButtonTextColor(StringItem item, int color) {
		if (item.getAppearanceMode() != Item.BUTTON || _LCDUIInvoker1.getCommandCount(item) == 0) {
			throw new IllegalArgumentException("StringItem must be button");
		}

		color = toBGR(color);

		StringItemParams p = null;
		synchronized (components) {
			try {
				p = (StringItemParams) components.get(item);
			} catch (Exception e) {} // ClassCastException
			if (p == null) {
				components.put(item, p = new StringItemParams());
			}
		}
		p.buttonTextColor = color;
		p.buttonColorSet = true;
		
		checkError(_setStringItemButtonTextColor(getItemHandle(item), getToolkitHandle(), color));
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
		update(item, null, true);
		_LCDUIInvoker1.itemRefreshForm(item);
	}
	
	public static Displayable getItemContainer(Item item) {
		return _LCDUIInvoker1.getItemScreen(item);
	}
	
	/**
	 * @deprecated use {@link Displayable#isShown()}
	 */
	public static boolean isForeground() {
		MIDlet midlet = LegacyRtPort.getMidlet();
	    Displayable displayable = Display.getDisplay(midlet).getCurrent();
	    return displayable != null && displayable.isShown();
	}
	
	//

	static void updateButtonSize(StringItem item, StringItemParams p) {
		checkError(_setStringItemButtonMinimumSize(getItemHandle(item), getToolkitHandle(),
				p.icon != null ? p.icon.getImage().getHeight() +
						((p.buttonFlags & KAknButtonNoFrame) == 0 ? 8 : 0) : 0));
		item.setPreferredSize(640, -1);
	}
	
	static void update(Object item, Object p, boolean forced) {
		if (item instanceof StringItem) {
			update((StringItem) item, (StringItemParams) p, forced);
			return;
		}
		if (item instanceof ImageItem) {
			update(components.get(item), null, forced);
			return;
		}
	}
	
	static void update(StringItem item, StringItemParams p, boolean forced) {
		if (item == null) return;
		try {
			if (p == null) p = (StringItemParams) components.get(item);
		} catch (Exception e) {
			return;
		}
		if (forced) {
			updateButtonSize(item, p);
		}
		if (p.buttonColorSet) {
			if (p.buttonColorSet) {
				checkError(
				_setStringItemButtonTextColor(
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
						p.strikethrough
				));
			}
		}
	}
	
	static void checkError(int r) {
		if (r < 0) {
			new Exception("check " + r).printStackTrace();;
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
			boolean strikethrough);

	private static native int _setStringItemButtonTooltipText(int item, int toolkit, String text);
	private static native int _setStringItemButtonIcon(int item, int toolkit, int image, int flags);
	private static native int _setImageItemButtonTooltipText(int item, int toolkit, String text);
	private static native int _setStringItemButtonFlags(int item, int toolkit, int flags);
	private static native int _setStringItemButtonAlignment(int item, int toolkit, int hor, int ver, int icon);
	private static native int _setStringItemButtonText(int item, int toolkit, String text);
	private static native int _setStringItemButtonTextColor(int item, int toolkit, int color);
	private static native int _setStringItemButtonMinimumSize(int item, int toolkit, int height);

}