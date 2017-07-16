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
 * llist.c
 *  Created on: Jan 3, 2016
 *      Author: Peter Vanusanik
 *  Contents: 
 */

#include "llist.h"

/******************************************************************************//**
 * \brief Creates empty list
 *
 * This list can take any amount of elements and will resize itself when necessary.
 ********************************************************************************/
list_t* create_list() {
    list_t* list = (list_t*) malloc(sizeof(list_t));
    if (list) {
		list->first = 0;
		list->last = 0;
		list->size = 0;
    }
    return list;
}

/******************************************************************************//**
 * \brief Removes left side of the list.
 *
 * Returns NULL if list is empty.
 ********************************************************************************/
void* list_remove_left(list_t* list) {
    list_element_t* list_head = list->first;
    if (list_head == 0)
        return 0;

    if (list->size == 1) {
        list->first = 0;
        list->last = 0;
        list->size = 0;
        void* data = list_head->data;
        free(list_head);
        return data;
    }

    void* data = list_head->data;
    list->first = list_head->next;

    free(list_head);

    list->size--;
    return data;
}

/******************************************************************************//**
 * \brief Removes right side of the list.
 *
 * Returns NULL if list is empty.
 ********************************************************************************/
void* list_remove_right(list_t* list) {
    list_element_t* list_head = list->last;
    if (list_head == 0)
        return 0;

    if (list->size == 1) {
        list->first = 0;
        list->last = 0;
        list->size = 0;
        void* data = list_head->data;
        free(list_head);
        return data;
    }

    void* data = list_head->data;
    list->last = list_head->previous;

    free(list_head);

    list->size--;
    return data;
}

/******************************************************************************//**
 * \brief Returns head of the list without removing it.
 *
 * Returns NULL if list is empty.
 ********************************************************************************/
void* list_get_left(list_t* list) {
    list_element_t* list_head = list->first;
    if (list_head == 0)
        return 0;

    return list_head->data;
}

/******************************************************************************//**
 * \brief Returns last element of the list without removing it.
 *
 * Returns NULL if list is empty.
 ********************************************************************************/
void* list_get_right(list_t* list) {
    list_element_t* list_head = list->last;
    if (list_head == 0)
        return 0;

    return list_head->data;
}

/******************************************************************************//**
 * Returns true if list is not empty.
 ********************************************************************************/
bool list_has_elements(list_t* list) {
    return list_size(list) > 0;
}

/******************************************************************************//**
 * \brief Adds element to the list.
 *
 * Element is put at the head of the list.
 * Returns true if failure happened.
 ********************************************************************************/
bool list_push_left(list_t* list, void* data) {
    list_element_t* element;
    element = (list_element_t*) malloc(sizeof(list_element_t));

    if (element == 0)
        return true;

    element->data = data;
    element->next = 0;
    element->previous = 0;

    if (list->first == 0) {
        list->first = list->last = element;
        list->size = 1;
    } else {
    	element->next = list->first;
    	list->first->previous = element;
    	list->first = element;
        list->size++;
    }

    return false;
}

/******************************************************************************//**
 * \brief Adds element to the list.
 *
 * Element is put at the end of the list.
 * Returns true if failure happened.
 ********************************************************************************/
bool list_push_right(list_t* list, void* data) {
    list_element_t* element;
    element = (list_element_t*) malloc(sizeof(list_element_t));

    if (element == 0)
        return true;

    element->data = data;
    element->next = 0;
    element->previous = 0;

    if (list->first == 0) {
        list->first = list->last = element;
        list->size = 1;
    } else {
        list->last->next = element;
        element->previous = list->last;
        list->last = element;
        list->size++;
    }

    return false;
}

/******************************************************************************//**
 * Returns number of elements in the list.
 ********************************************************************************/
uint32_t list_size(list_t* list) {
    return list->size;
}

/******************************************************************************//**
 * \brief Deallocates list.
 *
 * Removes all memory used by this list. Elements in the list are NOT
 * deallocated.
 ********************************************************************************/
void free_list(list_t* list) {
	list_element_t* el = list->first;
	while (el != 0) {
		list_element_t* tmp = el->next;
		free(el);
		el = tmp;
	}
	free(list);
}

/******************************************************************************//**
 * \brief Removes element from list at any position.
 *
 * Element is compared via ==.
 ********************************************************************************/
void list_remove(void* element, list_t* list) {
    if (list->size == 0)
        return;

    list_element_t* e = list->first;
    while (e != 0 && e->data != element)
        e = e->next;

    if (e) {
    	if (e->previous)
    		e->previous->next = e->next;
    	else {
    		list->first = e->next;
    	}

        if (e->next)
            e->next->previous = e->previous;
        else
        	list->last = e->previous;

        free(e);

        --list->size;
    }
}

bool list_contains(list_t* list, void* element) {
	if (list->size == 0)
		return false;

	list_element_t* e = list->first;
	while (e != 0) {
		if (e->data == element)
			return true;
		e = e->next;
	}

	return false;
}

/******************************************************************************//**
 * \brief Returns element from list found by search predicate.
 *
 * For more info, see array.
 ********************************************************************************/
void* list_find_by_predicate(void* data, search_predicate_t func, list_t* list) {
    if (list->size == 0)
        return 0;

    list_element_t* e = list->first;
    while (e != 0 && func(e->data, data) == false)
        e = e->next;

    if (e)
        return e->data;
    else
        return 0;
}

/******************************************************************************//**
 * \brief Deletes the element from list if predicate returns true
 *
 * Delete all elements that return true for predicate
 ********************************************************************************/
void list_remove_by_predicate(void* data, search_predicate_t func, list_t* list) {
	if (list->size == 0)
		return;

	list_element_t* e = list->first;
	while (e != 0) {
		if (func(e->data, data)) {
			if (e->previous)
				e->previous->next = e->next;
			else {
				list->first = e->next;
			}

			if (e->next)
				e->next->previous = e->previous;
			else
				list->last = e->previous;

			list_element_t* el = e;
			e = e->next;
			free(el);
			--list->size;
		} else
			e = e->next;
	}
}

/******************************************************************************//**
 * Creates iterator for list
 ********************************************************************************/
void list_create_iterator(list_t* list, list_iterator_t* li) {
	li->current = list->first;
	li->prev = &list->first;
	li->pprev = NULL;
	li->ls = &list->size;
}

/******************************************************************************//**
 * Returns true if list iterator has any element, or false if it does not.
 ********************************************************************************/
bool list_has_next(list_iterator_t* li) {
	return li->current != NULL;
}

/******************************************************************************//**
 * \brief Returns next element of the list.
 *
 * Asking list iterator that has no elements left is UB.
 ********************************************************************************/
void* list_next(list_iterator_t* li) {
	list_element_t* le = li->current;
	li->pprev = li->prev;
	li->prev = &le->next;
	li->current = le->next;
	return le->data;
}

/******************************************************************************//**
 * \brief Removes current head from the iterator.
 *
 * Removes previously returned element from list_next.
 ********************************************************************************/
void  list_remove_it(list_iterator_t* li) {
	// TODO: might be broken
	list_element_t* remel = *li->pprev;
	list_element_t* prev = remel->previous;
	list_element_t* next = remel->next;

	if (prev != NULL) {
		prev->next = next;
		li->prev = &prev->next;
	} else {
		li->prev = NULL;
	}

	if (next != NULL) {
		next->previous = prev;
	}

	li->pprev = NULL;
	free(remel);
	--(*li->ls);
}
