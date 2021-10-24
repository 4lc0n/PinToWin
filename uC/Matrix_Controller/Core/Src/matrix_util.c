/*
 * matrix_util.c
 *
 *  Created on: Oct 24, 2021
 *      Author: h3l
 */

#include "matrix_util.h"
#include "main.h"

#include <stdint.h>

extern volatile uint8_t active_col;
extern volatile int8_t adc_complete;
extern volatile int8_t adc_error;


uint16_t base_level[N_COL][N_ROW];							// form [COL][ROW] used, as data is generated column by column, so indexing is easier
uint16_t treshold_level[N_COL][N_ROW];					// form [COL][ROW] used, ..., compare data with trheshold level to get bool_matrix



/**
 * @brief Function to activate corresponding column
 * @param col: column to be activated, if col == -1: deactivate all
 * @return None
 */
void matr_set_col_active(int8_t col)
{
	switch(col)
	{
	case 0:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_0_Pin, SET);

		break;
	case 1:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_1_Pin, SET);

		break;
	case 2:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_2_Pin, SET);

		break;
	case 3:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_3_Pin, SET);
		break;
	case 4:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_4_Pin, SET);
			break;
	case 5:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_5_Pin, SET);
			break;
	case 6:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_6_Pin, SET);
			break;
	case 7:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_7_Pin, SET);
			break;
	case 8:
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
				HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_8_Pin, SET);
				break;
	case 9:
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
				HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_9_Pin, SET);
				break;
	case 10:
			HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);
				HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, COL_SELECT_10_Pin, SET);
				break;
	case -1:
		HAL_GPIO_WritePin(COL_SELECT_0_GPIO_Port, (COL_SELECT_0_Pin | COL_SELECT_1_Pin | COL_SELECT_2_Pin | COL_SELECT_3_Pin| COL_SELECT_4_Pin| COL_SELECT_5_Pin| COL_SELECT_6_Pin| COL_SELECT_7_Pin| COL_SELECT_8_Pin| COL_SELECT_9_Pin| COL_SELECT_10_Pin), RESET);

		break;
	}

}



/**
 * @brief Function to scan matrix to determine base light level of all elements
 * @param base_data: 2 dimensional vector for base level data of form uint8_t [COL][ROW]
 */
void matr_get_baselevel(ADC_HandleTypeDef hadc){

	adc_complete = 0;							// reset value


	// loop over every column and get data
	for(int i = 0; i < N_COL; i++){

		// activate column
		matr_set_col_active(i);						// select column
		active_col = N_COL - 1;				// prevent ConvCpltCallback from starting a new conversion:
																	// trick into thinking this is the last conversion



		// do one full converion, store into base_data
		if(HAL_ADC_Start_DMA(&hadc, (uint32_t*)(base_level[i]), N_ROW) != HAL_OK){
			while(1);
			// failed to start adc
		}





		while(adc_complete == 0 && adc_error == 0)			// while not finished
			__asm("nop");								// do nothing

		if(adc_error){
			while(1);										// fuck
		}


		adc_complete = 0;							// reset value
	}
	adc_complete = 0;								// reset used variables


	// TODO: calculate threshold level according to a fixed value TRIGGER_LEVEL



}



/**
 * @brief Calculates the output matrix in binary mode (1 column represented by 8 bit
 * uses threshold_level to produce output
 * @param data: input data in format uint16_t array [N_COL][N_ROW]
 * @param output: pointer to output vector of uint8_t with length [N_COL]
 */
void matr_compare(uint16_t data[N_COL][N_ROW], uint8_t* output[N_COL]){

	// loop over all columns first, as output is column sorted
	for(int i = 0; i < N_COL; i++){

		*output[i] = 0;																	// preload

		// loop over all rows
		for(int j = 0; j < N_ROW; j++){

			if(data[i][j] < treshold_level[i][j]){
				*output[i] |= (1 << j);											// mark as dark
			}

		}

		*output[i] = ~output[i];													// invert, as only dark should be highlighted

	}


}



