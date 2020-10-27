#include <Arduino.h>
#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>

int led_bri = 100;
bool led_power = false;
bool isLedUpdateAvailable = false;
float led_hue = 0;
float led_saturation = 59;

homekit_value_t led_hue_get()
{
	return HOMEKIT_FLOAT(led_hue);
}

void led_hue_set(homekit_value_t value)
{
	if (value.format != homekit_format_float)
	{
		return;
	}
	led_hue = value.float_value;
	isLedUpdateAvailable = true;
}

homekit_value_t led_saturation_get()
{
	return HOMEKIT_FLOAT(led_saturation);
}

void led_saturation_set(homekit_value_t value)
{
	if (value.format != homekit_format_float)
	{
		return;
	}
	led_saturation = value.float_value;
	isLedUpdateAvailable = true;
}

homekit_value_t led_on_get()
{
	return HOMEKIT_BOOL(led_power);
}

void led_on_set(homekit_value_t value)
{
	if (value.format != homekit_format_bool)
	{
		printf("Invalid on-value format: %d\n", value.format);
		return;
	}
	led_power = value.bool_value;
	if (led_power)
	{
		if (led_bri < 1)
		{
			led_bri = 100;
		}
	}
	isLedUpdateAvailable = true;
}

homekit_value_t light_bri_get()
{
	return HOMEKIT_INT(led_bri);
}

void led_bri_set(homekit_value_t value)
{
	if (value.format != homekit_format_int)
	{
		return;
	}
	led_bri = value.int_value;
	isLedUpdateAvailable = true;
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "SK6812");
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, "SN_2222");
homekit_characteristic_t led_on = HOMEKIT_CHARACTERISTIC_(ON, false, .getter=led_on_get, .setter=led_on_set);

void led_toggle()
{
	led_on.value.bool_value = !led_on.value.bool_value;
	led_on.setter(led_on.value);
	homekit_characteristic_notify(&led_on, led_on.value);
}

void accessory_identify(homekit_value_t _value)
{
	printf("accessory identify\n");
	for (int j = 0; j < 3; j++)
	{
		led_power = true;
		delay(100);
		led_power = false;
		isLedUpdateAvailable = true;
		delay(100);
	}
}

homekit_accessory_t *accessories[] = {HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]){
		HOMEKIT_SERVICE(ACCESSORY_INFORMATION,
				.characteristics=(homekit_characteristic_t*[]){
				&name,
				HOMEKIT_CHARACTERISTIC(MANUFACTURER, "DBambus"),
				&serial_number,
				HOMEKIT_CHARACTERISTIC(MODEL, "Wortuhr"),
				HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "2.5.0"),
				HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
				NULL
		}), HOMEKIT_SERVICE(LIGHTBULB,.primary = true,
		.characteristics=(homekit_characteristic_t*[]){
			HOMEKIT_CHARACTERISTIC(NAME, "Wortuhr"),
					&led_on,
					HOMEKIT_CHARACTERISTIC(BRIGHTNESS, 100, .getter=light_bri_get, .setter=led_bri_set),
			HOMEKIT_CHARACTERISTIC(
					HUE, 0,
					.getter = led_hue_get,
			.setter = led_hue_set
			),
			HOMEKIT_CHARACTERISTIC(
					SATURATION, 0,
					.getter = led_saturation_get,
			.setter = led_saturation_set
			),
			NULL
		}), NULL
}), NULL};

homekit_server_config_t config = {.accessories = accessories, .password = "336-85-553",
		//.on_event = on_homekit_event,
		.setupId = "LYPC"};
