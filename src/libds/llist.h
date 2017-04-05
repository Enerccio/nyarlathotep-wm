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
 * llist.h
 *  Created on: Jan 3, 2016
 *      Author: Peter Vanusanik
 *  Contents: 
 */

#pragma once

#include "_commons.h"
#include "funcops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct list_element list_element_t;

/**************************************************************\\**
 * Single list element
 ******************************************************************/
struct list_element {
    void* data;
    list_element_t* previous;
    list_element_t* next;
};

/**************************************************************\\**
 * List
 ******************************************************************/
typedef struct list {
    /** First element of list */
    list_element_t* first;
    /** Last element of list */
    list_element_t* last;
    /** Number of elements in list */
    uint32_t size;
} list_t;

typedef struct list_iterator {
	list_element_t* current;
	list_element_t** prev;
	list_element_t** pprev;
} list_iterator_t;

list_t* create_list();

void* list_remove_left(list_t* list);
void* list_get_left(list_t* list);
void* list_remove_right(list_t* list);
void* list_get_right(list_t* list);
bool list_push_left(list_t* list, void* data);
bool list_push_right(list_t* list, void* data);

bool list_has_elements(list_t* list);
uint32_t list_size(list_t* list);
void free_list(list_t* list);
void list_remove(void* element, list_t* list);
void* list_find_by_predicate(void* data, search_predicate_t func, list_t* list);
void list_remove_by_predicate(void* data, search_predicate_t func, list_t* list);

void list_create_iterator(list_t* list, list_iterator_t* li);
bool list_has_next(list_iterator_t* li);
void* list_next(list_iterator_t* li);
void list_remove_it(list_iterator_t* li);

#ifdef __cplusplus
}
#endif
