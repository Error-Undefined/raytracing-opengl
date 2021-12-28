#ifndef HITTABLE_LIST_THREADSAFE_H
#define HITTABLE_LIST_THREADSAFE_H

#include "hittable_type.h"

struct hittable_list_threadsafe
{
  struct hittable_list_ts_node* list_array;
  int size;
  int max_elements;
};

struct hittable_list_ts_node
{
  void* object;
  hittable_type object_type;
};

typedef struct hittable_list_threadsafe hittable_list_ts;
typedef struct hittable_list_ts_node hittable_list_ts_node;


struct hittable_list_threadsafe init_hittable_list_threadsafe(void* obj_data, hittable_type object_type, int min_elements);
void add_hittable_object_ts(struct hittable_list_threadsafe* list, void* obj_data, hittable_type obj_type);
struct hittable_list_ts_node* get_node_ts(struct hittable_list_threadsafe* list, int index);

// TODO: write init function to alloc sequential memory for multiple nodes

#endif
