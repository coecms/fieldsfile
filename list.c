/*
 * \file    list.c
 * \author  Scott Wales (scott.wales@unimelb.edu.au)
 * \brief   A list holding sorted, unique values
 * 
 * Copyright 2013 ARC Centre of Excellence for Climate System Science
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

#include "list.h"
#include <stdlib.h>

struct list {
    double value;
    struct list * next;
};

void ListAdd(struct list ** list, double value) {
    if (*list == NULL || value < (*list)->value) {
        struct list * new = malloc(sizeof(**list));
        new->value = value;
        new->next = *list;
        *list = new;
    } else if (value != (*list)->value) {
        ListAdd(&((*list)->next),value);
    }
    return;
}
int ListCount(const struct list * list){
    int count = 0;
    while (list != NULL){
        count++;
        list = list->next;
    }
    return count;
}
void ListToArray(double ** array, const struct list * list){
    *array = realloc(*array,ListCount(list)*sizeof(**array));
    int i=0;
    while (list != NULL){
        (*array)[i++] = list->value;
        list = list->next;
    }
}
int ListIndex(const struct list * list, double value){
    int i=0;
    while (list != NULL){
        if (list->value == value) return i;
        ++i;
        list = list->next;
    }
    return -1;
}
void ListFree(struct list * list){
    if (list){
        ListFree(list->next);
    }
    free(list);
}
