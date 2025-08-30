/* Linux HID driver for LG Magic Remote (MR20 etc.) */
#include <linux/module.h>
#include <linux/hid.h>
#include <linux/input.h>
#include <linux/slab.h>

struct lgmagic_drvdata {
	struct input_dev *input;
    u16 last_keycode;
    u16 last_btncode;
};

static const struct {
	u16 code;
	u16 keycode;
} lg_btn_map[] = {
	{ 0x8000, KEY_POWER },
	{ 0x8099, KEY_SLEEP },
	{ 0x8010, KEY_0 }, { 0x8011, KEY_1 }, { 0x8012, KEY_2 },
	{ 0x8013, KEY_3 }, { 0x8014, KEY_4 }, { 0x8015, KEY_5 },
	{ 0x8016, KEY_6 }, { 0x8017, KEY_7 }, { 0x8018, KEY_8 }, { 0x8019, KEY_9 },
	{ 0x8044, BTN_MIDDLE },
	{ 0x8053, KEY_LIST },
	{ 0x8045, KEY_MENU }, // ... button
	{ 0x8002, KEY_VOLUMEUP },
	{ 0x8003, KEY_VOLUMEDOWN },
	{ 0x8009, KEY_MUTE },
	{ 0x808B, KEY_VOICECOMMAND },
	{ 0x807C, KEY_HOME },
	{ 0x8043, KEY_SETUP },
	{ 0x8028, KEY_BACK },
	{ 0x80AB, KEY_PROGRAM },
	{ 0x805D, KEY_MEDIA }, // IVI
	{ 0x800B, KEY_TV },
	{ 0x8098, KEY_CONTEXT_MENU }, // STB MENU
	//{ 0x8000, KEY_CHANNELUP }, // Somehow it collides with power button
	{ 0x8001, KEY_CHANNELDOWN },
	{ 0x8072, KEY_RED },
	{ 0x8071, KEY_GREEN },
	{ 0x8063, KEY_YELLOW },
	{ 0x8061, KEY_BLUE },
	{ 0x8081, KEY_VIDEO }, // MOVIES
	{ 0x80B0, KEY_PLAY },
	{ 0x80BA, KEY_PAUSE },
	{ 0x8040, KEY_UP },
	{ 0x8041, KEY_DOWN },
	{ 0x8006, KEY_RIGHT },
	{ 0x8007, KEY_LEFT },
};

static int lgmagic_raw_event(struct hid_device *hdev, struct hid_report *report,
				u8 *data, int size)
{
	struct lgmagic_drvdata *drvdata = hid_get_drvdata(hdev);
    u8 reporting = 0;
    
	if (!drvdata || !drvdata->input)
    {
        dev_warn(&hdev->dev, "No drvdata or no input dev");
        return 0;
    }
        
    if (size != 20 || data[0] != 0xFD)
    {
        dev_warn(&hdev->dev, "Unknown descriptor with size %d and type %x", size, data[0]);
		return 0; // Not ours
    }

	// Button is last two bytes before wheel
	u16 btn_code = (data[17] << 8) | data[18];
	s8 wheel = (s8)data[19];

    if (btn_code != drvdata->last_btncode)
    {
        input_report_key(drvdata->input, drvdata->last_keycode, 0);
        reporting = 1;
        drvdata->last_keycode = 0;
        drvdata->last_btncode = btn_code;
        if (btn_code != 0)
        {
            for (int i = 0; i < ARRAY_SIZE(lg_btn_map); i++) {
                if (lg_btn_map[i].code == btn_code) {
                    input_report_key(drvdata->input, lg_btn_map[i].keycode, 1);
                    drvdata->last_keycode = lg_btn_map[i].keycode;
                    break;
                }
            }
        }
    }

	if (wheel != 0)
    {
		input_report_rel(drvdata->input, REL_WHEEL, wheel);
        reporting = 1;
    }

    if (reporting)
    {
        input_sync(drvdata->input);
    }

	return 0;
}

static int lgmagic_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret, i;
	struct lgmagic_drvdata *drvdata;
    struct input_dev *input;


	drvdata = devm_kzalloc(&hdev->dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	hid_set_drvdata(hdev, drvdata);

	ret = hid_parse(hdev);
	if (ret)
		return ret;

	ret = hid_hw_start(hdev, HID_CONNECT_HIDRAW);
	if (ret)
		return ret;

	input = devm_input_allocate_device(&hdev->dev);
	if (!input)
		return -ENOMEM;

	drvdata->input = input;
	input->name = "LG Magic Remote";
	input->id.bustype = hdev->bus;
	input->id.vendor = hdev->vendor;
	input->id.product = hdev->product;

	set_bit(EV_KEY, input->evbit);
	set_bit(EV_REL, input->evbit);
	set_bit(REL_WHEEL, input->relbit);

	for (i = 0; i < ARRAY_SIZE(lg_btn_map); i++)
		set_bit(lg_btn_map[i].keycode, input->keybit);

	ret = input_register_device(input);
	if (ret)
		return ret;

	return 0;
}

static void lgmagic_remove(struct hid_device *hdev)
{
	hid_hw_stop(hdev);
}

static const struct hid_device_id lgmagic_devices[] = {
	{ HID_BLUETOOTH_DEVICE(0x000f, 0x3412) }, // LG Magic Remote
	{ }
};
MODULE_DEVICE_TABLE(hid, lgmagic_devices);

static struct hid_driver lgmagic_driver = {
	.name = "lgmagic",
	.id_table = lgmagic_devices,
	.raw_event = lgmagic_raw_event,
	.probe = lgmagic_probe,
	.remove = lgmagic_remove,
};

module_hid_driver(lgmagic_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("LG Magic Remote HID Driver");
