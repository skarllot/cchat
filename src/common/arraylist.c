/*
 * Copyright (C) 2009 Fabrício Godoy <skarllot@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Fabrício Godoy <skarllot@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "arraylist.h"

struct _ArrayList_private
{
    void **data;
    int num_elements;
    int num_allocated;
};

static void ArrayList_alloc(ArrayList *this);
static void ArrayList_check(ArrayList *this);

ArrayList *ArrayList_init(ArrayList *this)
{
    if (this == NULL) {
        this = (ArrayList *)malloc(sizeof(ArrayList));
    }

    this->priv = (ArrayList_private *)malloc(sizeof(ArrayList_private));
    this->priv->data = NULL;
    this->priv->num_elements = 0;
    this->priv->num_allocated = 0;
    ArrayList_alloc(this);

    return this;
}

void ArrayList_free(ArrayList* this, BOOLEAN dynamic)
{
    free(this->priv->data);
    free(this->priv);

    if (dynamic == TRUE) {
        free(this);
    }
}

void ArrayList_add(ArrayList *this, void *item)
{
    ArrayList_check(this);
    
    this->priv->data[this->priv->num_elements] = item;
    this->priv->num_elements++;
}

void ArrayList_clean(ArrayList* this)
{
    int i;
    for (i = 0; i < this->priv->num_elements; ++i) {
        this->priv->data[i] = NULL;
    }

    this->priv->num_elements = 0;
}

void *ArrayList_get(ArrayList *this, int index)
{
    if (index < 0 || index >= this->priv->num_elements) {
        fprintf(stderr, "Out of range array index\n");
        exit(EXIT_FAILURE);
    }

    return this->priv->data[index];
}

int ArrayList_getcount(ArrayList* this)
{
    return this->priv->num_elements;
}

void ArrayList_remove(ArrayList* this, int index)
{
    if (index < 0 || index >= this->priv->num_elements) {
        fprintf(stderr, "Out of range array index\n");
        exit(EXIT_FAILURE);
    }

    this->priv->num_elements--;
    int i;
    for (i = index; i < this->priv->num_elements; ++i) {
        this->priv->data[i] = this->priv->data[i + 1];
    }
    this->priv->data[this->priv->num_elements] = NULL;
}

void ArrayList_alloc(ArrayList* this)
{
    if (this->priv->num_allocated == 0) {
        this->priv->num_allocated = 3;
    }
    else {
        this->priv->num_allocated *= 2;
    }

    void *_tmp = realloc(this->priv->data,
        this->priv->num_allocated * sizeof(void *));
    if (!_tmp) {
        perror("Couldn't alloc memory");
        exit(EXIT_FAILURE);
    }

    this->priv->data = (void **)_tmp;
}

void ArrayList_check(ArrayList* this)
{
    if (this->priv->num_elements == this->priv->num_allocated) {
        ArrayList_alloc(this);
    }
}
