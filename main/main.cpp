#include "SmlLexer.hpp"
#include "SmlParser.hpp"
#include <driver/uart.h>
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <cstring>
#include <iostream>

extern "C"
{
	void app_main();
}

static const char *TAG1 = "Main";
static const char *TAG2 = "UART_REC";

/* HW Config */
const uart_port_t UART_PORT = UART_NUM_2;
const int PIN_UART_TX = 17;
const int PIN_UART_RX = 16;
const int PIN_UART_RTS = UART_PIN_NO_CHANGE;
const int PIN_UART_CTS = UART_PIN_NO_CHANGE;

/* Driver Config */
const int UART_BAUDRATE = 9600;
const uart_word_length_t UART_BYTE_SIZE = UART_DATA_8_BITS;
const uart_parity_t UART_PARITY = UART_PARITY_DISABLE;
const uart_stop_bits_t UART_STOP_BITS = UART_STOP_BITS_1;
const uart_hw_flowcontrol_t UART_FLW_CTRL = UART_HW_FLOWCTRL_DISABLE;
const uint8_t UART_RX_FLW_CTRL_THRSHLD = 122;
const uart_sclk_t UART_SRC_CLK = UART_SCLK_DEFAULT;
const uint16_t UART_TIMEOUT_MS = 1000;
const uint32_t UART_RX_BUF_SIZE = 512;
const uint8_t UART_PATTERN_CHR_NUM = 1;

QueueHandle_t uart_queue = NULL;

uart_event_t event;
SmlLogLevel SmlLogger::logLevel{SmlLogLevel::Verbose};

void app_main()
{
	char compute_buffer[UART_RX_BUF_SIZE];

	/* UART */
	uart_config_t uart_config = {
		.baud_rate = UART_BAUDRATE,
		.data_bits = UART_BYTE_SIZE,
		.parity = UART_PARITY,
		.stop_bits = UART_STOP_BITS,
		.flow_ctrl = UART_FLW_CTRL,
		.rx_flow_ctrl_thresh = UART_RX_FLW_CTRL_THRSHLD,
		.source_clk = UART_SRC_CLK,
	};

	ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RX_BUF_SIZE, UART_RX_BUF_SIZE, 1, &uart_queue, 0));
	ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT, PIN_UART_TX, PIN_UART_RX, PIN_UART_RTS, PIN_UART_CTS));
	// uart_enable_pattern_det_baud_intr(UART_PORT, 0x1b1b1b1b01010101, UART_PATTERN_CHR_NUM, 9, 0, 0);

	SmlParser smlParser(reinterpret_cast<unsigned char *>(&compute_buffer), int(UART_RX_BUF_SIZE));

	for (;;)
	{
		bzero(&compute_buffer, UART_RX_BUF_SIZE);
		if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)UART_TIMEOUT_MS / portTICK_PERIOD_MS))
		{
			ESP_LOGV(TAG2, "[Core1]\treceived an uart event\n");
			switch (event.type)
			{
			case UART_DATA:
				uart_read_bytes(UART_PORT, &compute_buffer, UART_RX_BUF_SIZE, UART_TIMEOUT_MS / portTICK_PERIOD_MS);
				uart_flush_input(UART_PORT);
				printf("%02x%02x receoived\n", compute_buffer[0], compute_buffer[1]);
				break;
			case UART_FIFO_OVF:
				ESP_LOGI(TAG2, "hw fifo overflow");
				uart_flush_input(UART_PORT);
				xQueueReset(uart_queue);
				break;
			// Event of UART ring buffer full
			case UART_BUFFER_FULL:
				ESP_LOGI(TAG2, "ring buffer full");
				uart_flush_input(UART_PORT);
				xQueueReset(uart_queue);
				break;
			// Event of UART RX break detected
			case UART_BREAK:
				ESP_LOGI(TAG2, "uart rx break");
				break;
			// Event of UART parity check error
			case UART_PARITY_ERR:
				ESP_LOGI(TAG2, "uart parity error");
				break;
			// Event of UART frame error
			case UART_FRAME_ERR:
				ESP_LOGI(TAG2, "uart frame error");
				break;
			// UART_PATTERN_DET
			case UART_PATTERN_DET:
				// uart_get_buffered_data_len(UART_PORT, &buffered_size);
				// pos = uart_pattern_pop_pos(UART_PORT);
				// ESP_LOGI(TAG2, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
				// if (pos == -1)
				// {
				// 	// There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
				// 	// record the position. We should set a larger queue size.
				// 	// As an example, we directly flush the rx buffer here.
				// 	uart_flush_input(UART_PORT);
				// }
				// else
				// {
				// 	uart_read_bytes(UART_PORT, uart_rx_buffer, pos, 100 / portTICK_PERIOD_MS);
				// 	memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
				// 	uart_read_bytes(UART_PORT, uart_rx_buffer, 8, 100 / portTICK_PERIOD_MS);
				// 	ESP_LOGI(TAG2, "read data: %s", uart_rx_buffer);
				// }
				break;
			default:
				printf("UART event of type %d happened\n", event.type);
				break;
			}
		}
		
		printf("\n\n");
		for(int i=0; i<UART_RX_BUF_SIZE; i++) {
			printf("%02x", compute_buffer[i]);
		}
		printf("\n\n");

		// parse the SMl message
		smlParser.parseSml();
		SmlListEntry manufacturer = smlParser.getElementByObis(OBIS_MANUFACTURER);

		if (manufacturer.objName.empty())
		{
			ESP_LOGW(TAG1, "No manufacturer found");
		}
		else
		{
			if (manufacturer.isString)
			{
				std::cout << "Manufacturer: " << std::hex << manufacturer.sValue << "\n";
			}
			else
			{
				ESP_LOGW(TAG1, "Manufacturer value is not a string");
			}
		}

		SmlListEntry totalEnergy = smlParser.getElementByObis(OBIS_TOTAL_ENERGY);
		std::cout << "totalEnergy: \nvalue:\t" << std::dec << totalEnergy.value() << '\n';
		std::cout << "iValue: " << totalEnergy.iValue << '\n';

		SmlListEntry powerL1 = smlParser.getElementByObis(OBIS_SUM_ACT_INST_PWR);
		std::cout << "\nValue " << std::hex << powerL1.iValue << '\n';
		std::cout << "scaler " << std::hex << powerL1.scaler << '\n';
		std::cout << "Integer:: sum actual instantanious power: " << powerL1.value() << " " << smlParser.getUnitAsString(powerL1.unit) << "\n";

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
