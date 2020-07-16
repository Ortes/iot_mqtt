#include <stdint.h>
/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "tcpip_adapter.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "dht.h"

static const char* TAG = "MQTT_TEMP";

static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const gpio_num_t dht_gpio = 4;

esp_mqtt_client_handle_t main_client;

_Noreturn void temp_send_task(void* args) {
	int msg_id = 0;
	char char_temp[15];
	float temperature = 0;
	float humidity = 0;
	while (1) {
		if (dht_read_float_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK)
			printf("Humidity: %f%% Temp: %fC\n", humidity, temperature);
		sprintf(char_temp, "%d", (int)temperature);
		msg_id = esp_mqtt_client_publish(main_client, "esp/temp", char_temp, strlen(char_temp), 1, 0);
		ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void led(int state) {
	ESP_LOGI(TAG, "LED STATE CHANGED state = %d", state);
	gpio_set_level(2, state);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event) {
	switch (event->event_id) {
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
			esp_mqtt_client_subscribe(event->client, "esp/led", 0);
			main_client = event->client;
			xTaskCreate(temp_send_task, "temp_send_task", 2048, NULL, tskIDLE_PRIORITY, NULL);
			break;
		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
			break;
		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_DATA:
			ESP_LOGI(TAG, "MQTT_EVENT_DATA");
			if (strncmp(event->topic, "esp/led", 7) == 0)
				led('0' - *event->data);
			break;
		case MQTT_EVENT_ERROR:
			ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
			break;
		default:
			ESP_LOGI(TAG, "Other event id:%d", event->event_id);
			break;
	}
	return ESP_OK;
}

static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
	ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
	mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void) {
	esp_mqtt_client_config_t mqtt_cfg = {
					.uri = CONFIG_BROKER_URL,
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
	esp_mqtt_client_start(client);
}

void app_main() {
	ESP_ERROR_CHECK(nvs_flash_init());
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(example_connect());
	gpio_set_direction(2, GPIO_MODE_OUTPUT);
	mqtt_app_start();
}
