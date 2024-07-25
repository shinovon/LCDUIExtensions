package javax.microedition.lcdui;

public class LCDUIInvoker1 {
	
	public static int getCommandCount(Item item) {
		return item.iCommands.count();
	}
	
	public static int getImageHandle(Image image) {
		if (image == null) return 0;
		return image.getHandle(false);
	}

	public static Item formGet(Form form, int i) {
		return form.Get(i);
	}

	public static void formUpdate(Form form, Item item) {
		form.Update(item);
	}
	
	public static Form itemGetForm(Item item) {
		return item.getForm();
	}
	
	public static void itemRefreshForm(Item item) {
		item.RefreshForm();
	}
	
	public static void itemUpdateForm(Item item) {
		item.UpdateForm();
	}

}
