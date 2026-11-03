/* HTTP low level API client example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "cmd.h"
#include "esp_http_client.h"

extern QueueHandle_t xQueueRequest;

#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char *TAG = "HTTP-CLIENT";

/*
 *	http_native_request() demonstrates use of low level APIs to connect to a server,
 *	make a http request and read response. Event handler is not used in this case.
 *	Note: This approach should only be used in case use of low level APIs is required.
 *	The easiest way is to use esp_http_perform()
 */
static esp_err_t http_native_request(char * ImageFileName, char * NtfyTopic)
{
	// Get the size of an image file
	struct stat statBuf;
	if (stat(ImageFileName, &statBuf) == 0) {
		ESP_LOGI(TAG, "st_size=%d", (int)statBuf.st_size);
	} else {
		ESP_LOGE(TAG, "%s not found", ImageFileName);
		return ESP_FAIL;
	}

	// Allocate space for the image file
	size_t FileSize = statBuf.st_size;
	char* image_buffer = NULL;
	image_buffer = malloc(FileSize);
	if (image_buffer == NULL) {
		ESP_LOGE(TAG, "malloc fail. image_buffer %d", FileSize);
		return ESP_FAIL;
	}

	// Read an image file
	FILE * fp;
	if((fp=fopen(ImageFileName,"rb"))==NULL){
		ESP_LOGE(TAG, "fopen fail. [%s]", ImageFileName);
		free(image_buffer);
		return ESP_FAIL;
	}else{
		for (int i=0;i<FileSize;i++) {
			fread(&image_buffer[i],sizeof(char),1,fp);
		}
		fclose(fp);
	}

	// Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
	// it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
	char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};	// Buffer to store response of http request
	int content_length = 0;
#if 0
	esp_http_client_config_t config = {
		.url = "http://192.168.0.40:5001",
	};
#endif
	char url[128];
	sprintf(url, "http://ntfy.sh/%s", NtfyTopic);
	ESP_LOGI(TAG, "HTTP native request. url=[%s]", url);
	esp_http_client_config_t config = {
		.url = url,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	// PUT Request
	esp_err_t ret = ESP_FAIL;
	esp_http_client_set_method(client, HTTP_METHOD_PUT);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_err_t err = esp_http_client_open(client, FileSize);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
	} else {
		int wlen = esp_http_client_write(client, image_buffer, FileSize);
		if (wlen < 0) {
			ESP_LOGE(TAG, "Failed to esp_http_client_write");
		}
		content_length = esp_http_client_fetch_headers(client);
		if (content_length < 0) {
			ESP_LOGE(TAG, "HTTP client fetch headers failed");
		} else {
			int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
			if (data_read >= 0) {
				ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %"PRId64,
				esp_http_client_get_status_code(client),
				esp_http_client_get_content_length(client));
				ESP_LOGI(TAG, "%s", output_buffer);
				//ESP_LOG_BUFFER_HEX(TAG, output_buffer, strlen(output_buffer));
				ret = ESP_OK;
			} else {
				ESP_LOGE(TAG, "Failed to read response");
			}
		}
	}
	esp_http_client_cleanup(client);
	free(image_buffer);
	return ret;
}

void http_client(void *pvParameters)
{
	ESP_LOGI(TAG, "Start NTFY_PUB_TOPIC:%s", CONFIG_NTFY_PUB_TOPIC);
	REQUEST_t requestBuf;
	while(1) {
		ESP_LOGD(TAG,"Waitting....");
		xQueueReceive(xQueueRequest, &requestBuf, portMAX_DELAY);
		ESP_LOGI(TAG,"requestBuf.command=%d", requestBuf.command);
		if (requestBuf.command == CMD_HALT) break;
		ESP_LOGI(TAG,"requestBuf.localFileName=[%s]", requestBuf.localFileName);

		//esp_err_t ret = http_native_request("/images/esp32.jpeg", CONFIG_NTFY_PUB_TOPIC);
		esp_err_t ret = http_native_request(requestBuf.localFileName, CONFIG_NTFY_PUB_TOPIC);
		ESP_LOGI(TAG, "http_native_request=%d", ret);
		uint32_t ulValue = 0x00;
		if (ret != ESP_OK) {
			ESP_LOGW(TAG, "Server does not return a successful response");
			ulValue = 0x90;
		}	
		if (requestBuf.taskHandle != NULL)
			xTaskNotify(requestBuf.taskHandle, ulValue, eSetValueWithOverwrite);
	}

	vTaskDelete(NULL);
}
