/*
 * Amplifier.cpp
 *
 *  Created on: Jan. 6, 2023
 *      Author: duane
 */

#include "Amplifier.h"
#include "main.h"

Amplifier::Amplifier() {
	// TODO Auto-generated constructor stub

}

Amplifier::~Amplifier() {
	// TODO Auto-generated destructor stub
}

void Amplifier::run() {
	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_SET );

	  HAL_Delay( 200 );

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LED_MUTE_GPIO_Port, LED_MUTE_Pin, GPIO_PIN_RESET);

	  HAL_Delay( 200 );

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_SET );

	  HAL_Delay( 200 );

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LED_DOLBY_GPIO_Port, LED_DOLBY_Pin, GPIO_PIN_RESET);

	  HAL_Delay( 200 );

}

