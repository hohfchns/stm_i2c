/*
 * proj_main.h
 *
 *  Created on: Jun 30, 2022
 *      Author: student
 */

#ifndef INC_PROJ_MAIN_H_
#define INC_PROJ_MAIN_H_

#include "main.h"
#include "stm_term.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

void PROJMAIN_Init();
void PROJMAIN_MainLoop();
void I2C_COMMANDS(const char* cmd);

#endif /* INC_PROJ_MAIN_H_ */
