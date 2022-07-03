#include "Node.h"

Node *node_alloc(NodeElemCpy elem_copy_func, NodeElemCmp elem_cmp_func,
                 NodeElemFree elem_free_func){
  // if illegal values were given
  if ((!elem_cmp_func)||(!elem_copy_func)
      ||(!elem_free_func)){
      return NULL;
  }
  // allocate a node struct
  Node* res = malloc(sizeof (Node));
  if(!res) return NULL;
  // initialize all fields
  res->data = NULL;
  res->elem_cmp_func = elem_cmp_func;
  res->elem_copy_func = elem_copy_func;
  res->elem_free_func = elem_free_func;
  res->hashCount = 0;

  return res;
}

void node_free(Node **p_node){
  if (!p_node) return;

  Node* node_to_free = *p_node;
  if (!node_to_free) return;

  if (node_to_free->elem_free_func){
    if (node_to_free->data){
      // free data inside the node
      node_to_free->elem_free_func(&(node_to_free->data));
      node_to_free->data = NULL;
    }
    // free the node
    free(node_to_free);
    *p_node = NULL;
  }
}

int check_node(Node *node, Value value){
  // if illegal values were given
  if ((!node)||(!value)) return -1;
  if (node->elem_cmp_func == NULL) return -1;
  // if the data is empty
  if (node->data == NULL) return 0;
  // compare the value and the node data
  int ret = node->elem_cmp_func(value,node->data);
  return ret;
}

int set_node_data(Node *node, Value value){
  if ((!node)||(!value)) return 0;
  if (!(node->elem_free_func)) return 0;
  if(!(node->elem_copy_func)) return 0;

  // if data already exists in the node we free it
  if (node->data){
    node->elem_free_func(&(node->data));
    node->data = NULL;
  }
  // we copy the given value to the node
  node->data = node->elem_copy_func(value);
  if (!node->data) return 0;
  return 1;
}

void clear_node(Node *node){
  if (node == NULL) return;
  if (node->elem_free_func == NULL) return;

  node->elem_free_func(&(node->data));
  node->data = NULL;
}

int get_hash_count(Node *node){
  if (!node) return -1;
  int res = node->hashCount;
  return res;
}