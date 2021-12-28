#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable_type.h"

struct hittable_list
{
  struct hittable_list_node* list_head;
  struct hittable_list_node* list_end;
  struct hittable_list_node* list_curr;
};

struct hittable_list_node
{
  struct hittable_list_node* next;
  void* object;
  hittable_type object_type;
};

typedef struct hittable_list hittable_list;
typedef struct hittable_list_node hittable_list_node;


struct hittable_list* init_hittable_list(void* obj_data, hittable_type object_type);
int add_hittable_object(struct hittable_list* list, void* obj_data, hittable_type obj_type);
struct hittable_list_node* get_next_node(struct hittable_list* list);
void reset_current_list_node(struct hittable_list* list);

void dealloc_list(struct hittable_list* list);

// TODO: write init function to alloc sequential memory for multiple nodes

#endif
