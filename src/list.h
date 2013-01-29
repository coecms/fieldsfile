/*
 * \file    list.h
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

#ifndef LIST_H
#define LIST_H
#ifdef __cplusplus
extern "C" {
#endif

struct list;

/** Adds a new value to the list at the correct sorted position
 *
 * @pre: \p *list is either a pointer to list or NULL, indicating an empty list
 * @post: \p *list is modified to hold the new value
 */
void ListAdd(struct list ** list, double value);

/** Returns a count of all items in \p *list
 */
 int ListCount(const struct list * list);

/** Converts the list to an array
 *
 * @pre:  \p *array must be reallocable
 * @post: \p *array holds a sorted list of the unique values held in \p *list
 */
void ListToArray(double ** array, const struct list * list);

/** Finds the sorted index of value within \p *list
 */
 int ListIndex(const struct list * list, double value);

/** Frees data held by \p *list
 */
void ListFree(struct list * list);

#ifdef __cplusplus
}
#endif
#endif
