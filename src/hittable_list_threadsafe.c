#include <stdlib.h>

#include "hittable_list_threadsafe.h"


struct hittable_list_threadsafe init_hittable_list_threadsafe(void* obj_data, hittable_type object_type, int min_elements)
{
  hittable_list_ts list;
  list.list_array = (struct hittable_list_ts_node*) calloc(min_elements, sizeof(struct hittable_list_ts_node));
  list.list_array[0].object = obj_data;
  list.list_array[0].object_type = object_type;
  list.max_elements = min_elements;
  list.size = 1;

  return list;
}


void add_hittable_object_ts(struct hittable_list_threadsafe* list, void* obj_data, hittable_type obj_type)
{
  if (list->size == list->max_elements)
  {
    struct hittable_list_ts_node* old_array = list->list_array;
    int old_max = list->max_elements;
    list->max_elements += 2;
    list->list_array = (struct hittable_list_ts_node*) calloc(list->max_elements, sizeof(hittable_list_ts_node));
    for(int i = 0; i < old_max; i++)
    {
      list->list_array[i] = old_array[i];
    }
    free(old_array);
  }
  list->list_array[list->size].object = obj_data;
  list->list_array[list->size].object_type = obj_type;
  list->size++;
}



struct hittable_list_ts_node* get_node_ts(struct hittable_list_threadsafe* list, int index)
{
  return &list->list_array[index];
}