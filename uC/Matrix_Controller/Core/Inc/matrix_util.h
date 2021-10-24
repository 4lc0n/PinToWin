/*
 * matrix_util.h
 *
 *  Created on: Oct 24, 2021
 *      Author: h3l
 *
 *  Contains matrix util functions like select row, get base level, etc.
 */

#ifndef INC_MATRIX_UTIL_H_
#define INC_MATRIX_UTIL_H_

#include <stdint.h>
#include "main.h"


/**
 * @brief Function to activate corresponding column
 * @param col: column to be activated, if col == -1: deactivate all
 * @return None
 */
void matr_set_col_active(int8_t col);

/**
 * @brief Function to scan matrix to determine base light level of all elements
 * @param base_data: 2 dimensional vector for base level data of form uint8_t [COL][ROW]
 */
void matr_get_baselevel(ADC_HandleTypeDef hadc);


/**
 * @brief Calculates the output matrix in binary mode (1 column represented by 8 bit
 * uses threshold_level to produce output
 * @param data: input data in format uint16_t array [N_COL][N_ROW]
 * @param output: output vector of uint8_t with length [N_COL]
 */
void matr_compare(uint16_t data[N_COL][N_ROW], uint8_t* output[N_COL]);


#endif /* INC_MATRIX_UTIL_H_ */
