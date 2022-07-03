#include "proj_main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;


//int _write(int fd, char* ptr, int len) {
//    HAL_UART_Transmit(&huart2, (uint8_t *) ptr, len, HAL_MAX_DELAY);
//    return len;
//}

// Makeshift progress bar
unsigned int g_printN = 0;

// CALLBACK FORWARDING
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  TERM_TXCB(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  TERM_RXCB(huart);
}

// CALLBACKS
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
//	printf("%u : timer callback\r\r\n", g_printN++);

}

// PROJECT CODE
void PROJMAIN_Init()
{
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	HAL_TIM_Base_Start_IT(&htim6);

	TERM_Init(&huart2);

	I2C_COMMANDS("restore all");
}

void PROJMAIN_MainLoop()
{
	const char* cmdbuf = 0;
	if (TERM_PullCmd(&cmdbuf))
	{
		I2C_COMMANDS(cmdbuf);
	}
}


void I2C_COMMANDS(const char* fullCmd)
{
#define IS_COM(incmd) strcmp(incmd, command) == 0
	char command[80];
	char arg1[80] = "";
	int argc = 0;
	argc = sscanf(fullCmd, "%s %s", command, arg1);

	if (argc == 0)
	{
		printf("Please provide a command\r\n");
		return;
	}

	if (IS_COM("toggle"))
	{
		if (argc == 1)
		{
			printf("Please provide an argument (1/2)\r\n");
			return;
		}

		if (argc > 2)
		{
			printf("Too many arguments provided!\r\n");
			return;
		}

		int validLed = 0;
		int whichLed = atoi(arg1);
		if (whichLed == 1)
		{
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

			validLed = 1;
		}
		else if (whichLed == 2)
		{
			HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);

			validLed = 1;
		}

		if (validLed)
		{
			printf("Toggled LED %d\r\n", whichLed);
		}
		else
		{
			printf("LED number %d not recognized!\r\n", whichLed);
		}
	}
	else if (IS_COM("save"))
	{
		uint8_t ledState[1] = { 0 };
		ledState[0] = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin);
		HAL_StatusTypeDef writeStatus1 = HAL_I2C_Mem_Write(&hi2c1, 208, 24, 1, ledState, 1, 0xFF);
		if (writeStatus1 != HAL_OK)
		{
			printf("Failed to save LED state for led 1 with state of '%d'\r\n", ledState[0]);
			return;
		}

		uint8_t led2State[1] = { 0 };
		led2State[0] = HAL_GPIO_ReadPin(LD6_GPIO_Port, LD6_Pin);
		HAL_StatusTypeDef writeStatus2 = HAL_I2C_Mem_Write(&hi2c1, 0xD0, 0x10, 1, led2State, 1, 0xFF);
		if (writeStatus2 != HAL_OK)
		{
			printf("Failed to save LED state for led 2 with state of '%d'\r\n", led2State[0]);
			return;
		}

		printf("Saved LED states '%u, %u'\r\n", ledState[0], led2State[0]);
		return;
	}
	else if (IS_COM("restore"))
	{
		if (argc == 1)
		{
			printf("Please provide an argument (1/2)\r\n");
			return;
		}

		if (argc > 2)
		{
			printf("Too many arguments provided!\r\n");
			return;
		}

		int validLed = 0;
		int all = !strcmp(arg1, "all");
		int whichLed = atoi(arg1);

		uint8_t registerState[1] = { 0 };
		if (whichLed == 1 || all)
		{
			if (HAL_I2C_Mem_Read(&hi2c1, 0xD0, 0x18, 1, registerState, 1, HAL_MAX_DELAY) != HAL_OK)
			{
				printf("Failed to read LED state for led 1\r\n");
				return;
			}

			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, registerState[0]);

			printf("Restored LED 1 to %d\r\n", registerState[0]);

			validLed = 1;
		}
		if (whichLed == 2 || all)
		{
			if (HAL_I2C_Mem_Read(&hi2c1, 0xD0, 0x10, 1, registerState, 1, HAL_MAX_DELAY) != HAL_OK)
			{
				printf("Failed to read LED state for led 2\r\n");
				return;
			}

			HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, registerState[0]);

			printf("Restored LED 2 to %d\r\n", registerState[0]);

			validLed = 1;
		}

		if (!validLed)
		{
			printf("LED number %d not recognized!\r\n", whichLed);
		}
	}
	else
	{
		printf("Invalid command '%s'!\r\n", command);
	}


}
