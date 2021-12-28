#include <stdlib.h>

#include "hittable_list.h"
#include "hittable_type.h"

hittable_list* init_hittable_list(void* object_data, hittable_type obj_type)
{
  hittable_list* list = calloc(1, sizeof(hittable_list));
  
  if (list == NULL)
  {
    return NULL;
  }
  
  struct hittable_list_node* list_node = calloc(1, sizeof(struct hittable_list_node));
  if(list_node == NULL)
  {
    free(list);
    return NULL;
  }

  list_node->next = NULL;
  list_node->object = object_data;
  list_node->object_type = obj_type;

  list->list_head = list_node;
  list->list_end = list_node;
  list->list_curr = list_node;

  return list;
}


int add_hittable_object(struct hittable_list* list, void* obj_data, hittable_type obj_type)
{
  struct hittable_list_node* next_node = malloc(sizeof(struct hittable_list_node));
  if (next_node == NULL)
  {
    return -1;
  }

  next_node->next = NULL;
  next_node->object = obj_data;
  next_node->object_type = obj_type;

  list->list_end->next = next_node;
  list->list_end = next_node;

  return 0;
}

struct hittable_list_node* get_next_node(struct hittable_list* list)
{
  if (list->list_curr == NULL)
  {
    return NULL;
  }
  struct hittable_list_node* node = list->list_curr;
  // Not end of list
  if (node != NULL)
  {
    list->list_curr = node->next;
  }
  return node;
}

void reset_current_list_node(struct hittable_list* list)
{
  if (list == NULL)
  {
    return;
  }
  list->list_curr = list->list_head;
}
