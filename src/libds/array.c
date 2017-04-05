/*
 * The MIT License (MIT)
 * Copyright (c) 2015 Peter Vanusanik <admin@en-circle.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the 
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * array.c
 *  Created on: Dec 24, 2015
 *      Author: Peter Vanusanik
 *  Contents: 
 */

#include "array.h"

/******************************************************************************//**
 * \brief Creates array with ARRAY_STARTING_SIZE preallocation count.
 *
 * Starting allocation size will be set to ARRAY_STARTING_SIZE * sizeof(void*).
 ********************************************************************************/
array_t* create_array() {
    return create_array_spec(ARRAY_STARTING_SIZE);
}

/******************************************************************************//**
 * \brief Creates array with starting_elementa_size preallocation count.
 *
 * Starting allocation size will be set to starting_elementa_size * sizeof(void*).
 ********************************************************************************/
array_t* create_array_spec(uint32_t starting_elementa_size) {
    array_t* array = (array_t*) malloc(sizeof(array_t));
    if (array == NULL)
        return NULL;
    array->size = 0;
    array->data_size = starting_elementa_size * sizeof(void*);
    array->data = (void**) malloc(array->data_size);
    if (array->data == NULL) {
        free(array);
        return NULL;
    }
    array->starting_size = starting_elementa_size;
    return array;
}

/******************************************************************************//**
 * \brief Adds data on the end of the array.
 *
 * This is O(1) operation most of the time, unless array has to resize, then it
 * is O(n) operation. data is always placed as last element.
 ********************************************************************************/
uint32_t array_push_data(array_t* array, void* data) {
    if (array->size == array->data_size/sizeof(void*)) {
        array->data = (void**) realloc(array->data, array->data_size + (array->starting_size * sizeof(void*)));
        array->data_size += array->starting_size * sizeof(void*);
    }

    array->data[array->size++] = data;
    return array->size-1;
}

/******************************************************************************//**
 * \brief Returns index to the data in the array.
 *
 * Returns -1 if data is not found in the array. Compares data via ==.
 ********************************************************************************/
int32_t array_find_data(array_t* array, void* data) {
    uint32_t iterator = 0;
    for (; iterator<array->size; iterator++)
        if (array_get_at(array, iterator) == data)
            return iterator;
    return -1;
}

/******************************************************************************//**
 * \brief Inserts data at position.
 *
 * This operation is O(n) at worst case due to shifting. If n == size of array-1,
 * then this operation is same as array_push_data.
 ********************************************************************************/
void array_insert_at(array_t* array, uint32_t position, void* data) {
    if (position == array->size-1) {
        array_push_data(array, data);
        return;
    }

    if (position > array->size-1)
        return;

    if (array->size == 0) {
        if (position == 0) {
            array_push_data(array, data);
            return;
        } else
            return;
    }

    uint32_t it = position;
    void* prev = data;
    while (it <= array->size-1) {
        void* tmp = prev;
        prev = array->data[it];
        array->data[it] = tmp;
        ++it;
    }

    array_push_data(array, prev); // to ensure that we only have one code that will enlarge array
}

/******************************************************************************//**
 * \brief Returns data at position.
 *
 * Returns NULL if position is larger than size of an array or equal.
 ********************************************************************************/
void* array_get_at(array_t* array, uint32_t position) {
    if (position > array->size-1)
        return 0;

    return array->data[position];
}

/******************************************************************************//**
 * \brief Replaces content at position with data.
 *
 * Does nothing if position is larger than size of an array or equal.
 ********************************************************************************/
void array_set_at(array_t* array, uint32_t position, void* data) {
    if (position < array->size-1)
        array->data[position] = data;
}

/******************************************************************************//**
 * \brief Removes item at position.
 *
 * O(n) operation, due to shifting. If position == size of array, it is O(1)
 * operation instead.
 ********************************************************************************/
void array_remove_at(array_t* array, uint32_t position) {
    if (position == array->size-1) {
        --array->size;
        return;
    }

    int32_t p = ((int32_t)position)-1;
    while ((++p)<(int32_t)array->size-1)
        array->data[p] = array->data[p+1];
    --array->size;
}

/******************************************************************************//**
 * Returns number of elements in the array
 ********************************************************************************/
uint32_t array_get_size(array_t* array) {
    return array->size;
}

/******************************************************************************//**
 * \brief Deallocates all memory used by this array.
 *
 * Elements itself are NOT deallocated!
 ********************************************************************************/
void destroy_array(array_t* array) {
    free(array->data);
    free(array);
}

/******************************************************************************//**
 * \brief Returns first element that matches predicate.
 *
 * Calls predicate function with each element of an array and with data provided,
 * first true returned from the predicate is returned or NULL if nothing matched
 * the predicate.
 ********************************************************************************/
void* array_find_by_pred(array_t* array, search_predicate_t predicate, void* data) {
    uint32_t i = 0;

    for (; i<array->size; i++)
        if (predicate(array->data[i], data) == true)
            return array->data[i];

    return NULL;
}

/******************************************************************************//**
 * \brief Returns random element from array.
 *
 * Returns NULL if array is empty.
 ********************************************************************************/
void* array_get_random(array_t* array, rg_t* rg) {
    if (array->size == 0)
        return NULL;
    ruint_t idx = rg_next_uint_l(rg, array->size);
    return array->data[idx];
}

/******************************************************************************//**
 * Removes all elements from array.
 ********************************************************************************/
void array_clean(array_t* array) {
	array->size = 0;
}
