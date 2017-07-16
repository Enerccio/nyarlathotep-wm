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
 * hmap.c
 *  Created on: Jan 3, 2016
 *      Author: Peter Vanusanik
 *  Contents:
 */

#include "hmap.h"

/******************************************************************************//**
 * \brief Creates new hash table.
 *
 * This hash table will use provided hash_fn and cmpr_fn to hash/compare keys.
 * Default size of inner table is HASH_STARTING_SIZE.
 * Returns NULL on allication error.
 ********************************************************************************/
hash_table_t* create_table(hash_function_t hash_fn, eq_function_t cmpr_fn) {
    hash_table_t* table = (hash_table_t*) malloc(sizeof(hash_table_t));
    if (table == NULL)
        return NULL;
    table->hash_fn = hash_fn;
    table->cmpr_fn = cmpr_fn;
    table->max_size = HASH_STARTING_SIZE;
    table->hash_table = (hash_pair_t**) malloc(sizeof(hash_pair_t*) * table->max_size);
    if (table->hash_table == NULL) {
        free(table);
        return NULL;
    }
    table->len = 0;
    memset(table->hash_table, 0, sizeof(hash_pair_t*) * table->max_size);
    return table;
}

/******************************************************************************//**
 * \brief Deallocates table.
 *
 * Removes all data from the table and table itself from memory.
 * Will NOT deallocate table keys nor entries!
 ********************************************************************************/
void destroy_table(hash_table_t* table) {
    uint32_t i = 0;
    for (; i<table->len; i++) {
        hash_pair_t* cell = table->hash_table[i];
        if (cell != FREE_CELL && cell != DELETED_CELL)
            free(cell);
    }

    free(table->hash_table);
    free(table);
}

/******************************************************************************//**
 * Returns cell from the hash table.
 ********************************************************************************/
static hash_pair_t**  find_cell(hash_table_t* table, void* key, uint32_t hashed, bool dont_skip_deleted) {
    uint32_t primary_hash = hashed % table->max_size;
    uint32_t secondary_hash = 1 + (hashed % (table->max_size - 1));
    uint32_t hash = primary_hash;
    uint32_t i = 1;

    hash_pair_t* pair = table->hash_table[hash];
    while (pair != FREE_CELL || (dont_skip_deleted && pair == DELETED_CELL)) {
        if (pair != DELETED_CELL)
                if (table->cmpr_fn(key, pair->key) == true)
                        return &table->hash_table[hash];

        hash = (primary_hash + (secondary_hash * i++)) % table->max_size;
        pair = table->hash_table[hash];
    }

    return &table->hash_table[hash];
}

/******************************************************************************//**
 * Resizes the hash table to accomodate new elements.
 * Returns true on failure.
 ********************************************************************************/
static bool resize_table(hash_table_t* table) {
    hash_table_t* new_table = (hash_table_t*) malloc(sizeof(hash_table_t));
    if (new_table == NULL)
        return true;
    new_table->hash_fn = table->hash_fn;
    new_table->cmpr_fn = table->cmpr_fn;
    new_table->len = 0;
    new_table->max_size = (table->max_size * 2) + 1;
    new_table->hash_table = (hash_pair_t**) malloc(sizeof(hash_pair_t*) * new_table->max_size);
    if (new_table->hash_table == NULL) {
        free (new_table);
        return true;
    }
    memset(new_table->hash_table, 0, sizeof(hash_pair_t*) * new_table->max_size);

    uint32_t i = 0;
    for (; i<table->max_size; i++) {
        hash_pair_t* cell = table->hash_table[i];
        if (cell != FREE_CELL && cell != DELETED_CELL) {
            if (table_set(new_table, cell->key, cell->data)) {
            	destroy_table(new_table);
            	return true;
            }
            free(cell);
        }
    }

    free(table->hash_table);
    table->hash_table = new_table->hash_table;
    table->len = new_table->len;
    table->max_size = new_table->max_size;
    free(new_table);
    return false;
}

/******************************************************************************//**
 * Returns whether key is in the table or not.
 ********************************************************************************/
bool table_contains(hash_table_t* table, void* key) {
    hash_pair_t** cell = find_cell(table, key, table->hash_fn(key), false);
    if (*cell != FREE_CELL && *cell != DELETED_CELL)
        return true;
    else
        return false;
}

/******************************************************************************//**
 * \brief Returns value of key in the table.
 *
 * Returns NULL if key is not in the table.
 ********************************************************************************/
void* table_get(hash_table_t* table, void* key) {
    hash_pair_t** cell = find_cell(table, key, table->hash_fn(key), false);
    if (*cell != FREE_CELL && *cell != DELETED_CELL)
        return (*cell)->data;
    else
        return NULL;
}

/******************************************************************************//**
 * \brief Sets the key in the table to the data.
 *
 * Will remove old key-data pair if it exists (does not deallocate old data value!).
 * Returns true if space for new element (or increased table size)
 * cannot be allocated.
 ********************************************************************************/
bool table_set(hash_table_t* table, void* key, void* data) {
    hash_pair_t** cell = find_cell(table, key, table->hash_fn(key), true);
    if (*cell == FREE_CELL || *cell == DELETED_CELL) {
        hash_pair_t* pair = (hash_pair_t*) malloc(sizeof(hash_pair_t));
        if (pair == NULL)
            return true;
        pair->key = key;
        pair->data = data;
        *cell = pair;

        ++table->len;

        if (table->len > (table->max_size / 4) * 3)
            resize_table(table);
    } else if (table->cmpr_fn(key, (*cell)->key) == true) {
        (*cell)->data = data;
    } else {
        // this should not happen
    }
    return false;
}

/******************************************************************************//**
 * \brief Removes key from the table.
 *
 * Returns true if operation was done successfully, false if there was no such
 * key.
 ********************************************************************************/
bool table_remove(hash_table_t* table, void* key) {
    hash_pair_t** cell = find_cell(table, key, table->hash_fn(key), false);
    if (*cell != FREE_CELL && *cell != DELETED_CELL) {
        free(*cell);
        *cell = DELETED_CELL;
        --table->len;
        return true;
    }
    return false;
}

/******************************************************************************//**
 * Returns number of elements in the table.
 ********************************************************************************/
uint32_t table_size(hash_table_t* table) {
    return table->len;
}

/******************************************************************************//**
 * Copies table to another table.
 ********************************************************************************/
hash_table_t* copy_table(hash_table_t* table) {
    hash_table_t* copy = (hash_table_t*) malloc(sizeof(hash_table_t));
    copy->cmpr_fn = table->cmpr_fn;
    copy->hash_fn = table->hash_fn;
    copy->len = table->len;
    copy->max_size = table->max_size;
    copy->hash_table = (hash_pair_t**) malloc(sizeof(hash_pair_t*) * copy->max_size);

    uint32_t i = 0;
    for (; i<table->max_size; i++) {
        hash_pair_t* cell = table->hash_table[i];
        if (cell != FREE_CELL && cell != DELETED_CELL) {
            hash_pair_t* new = (hash_pair_t*) malloc(sizeof(hash_pair_t));
            new->data = cell->data;
            new->key = cell->key;
            copy->hash_table[i] = new;
        } else
            copy->hash_table[i] = cell;
    }

    return copy;
}

/******************************************************************************//**
 * Returns value from iterator and moves iterator to next element.
 ********************************************************************************/
void* hash_it_next(hash_table_t* table, hash_it_t* iterator) {
    void* data = 0;
    while (data == 0 && *iterator < table->max_size) {
        hash_pair_t* pair = table->hash_table[*iterator];
        if (pair != FREE_CELL && pair != DELETED_CELL)
            data = pair->data;

        *iterator += 1;
    }
    return data;
}

/******************************************************************************//**
 * Creates table for uint32_t keys.
 ********************************************************************************/
hash_table_t* create_uint32_table() {
    return create_table(uint32_hash_function, uint32_eq_function);
}

/******************************************************************************//**
 * Creates table for uint64_t keys.
 ********************************************************************************/
hash_table_t* create_uint64_table() {
    return create_table(uint64_hash_function, uint64_eq_function);
}

/******************************************************************************//**
 * Creates table for char* string keys.
 ********************************************************************************/
hash_table_t* create_string_table() {
    return create_table(string_hash_function, string_eq_function);
}
