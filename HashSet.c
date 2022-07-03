#include "Node.h"
#include "HashSet.h"



unsigned int quadratic_probing(HashSet *hash_set, Value value, int resize_indicator)
{
  unsigned int index_to_hash_to = 0;
  unsigned int j = 0; // To find the index to hash to (using hash function, quadratic probing)
  while (j < hash_set->capacity)
    {
      if (resize_indicator == 1)
        {
          unsigned int hash_to =
              (hash_set->hash_func(value) + ((j + (j * j)) / 2))& (hash_set->capacity - 1);
          if (j == 0)
            {
              hash_set->NodesList[hash_to]->hashCount++; // We tried hashing to this index the first time, we'll increment the hashcount
            }
          if (hash_set->NodesList[hash_to]->data == NULL)
            {
              hash_set->size++; // We added a new value, the size is incremented
              return index_to_hash_to; // We found the index
            }
          j++;

        }
      else
        {
          unsigned int hash_to =
              (hash_set->hash_func(value) + ((j + (j * j)) / 2))
              & (hash_set->capacity - 1);
          if (j == 0)
            {
              hash_set->NodesList[hash_to]->hashCount--; // We tried hashing to this index the first time, we'll decrement the hashcount
            }
          if (check_node (hash_set->NodesList[hash_to], value))
            {
//              hash_set->size--; // We added a new value, the size is incremented
              return index_to_hash_to; // We found the index
            }
          j++;

        }
    }
  return 0;
}

/**
 * The function receives a HashSet and resizes its nodeslist according to its
 * capacity and load factor
 * @param hash_set The HashSet we want to apply the changes on
 * @param if resize = 1, we'll double the size of the capacity, else we'll divide it by 2.
 * @return 1 if we succeeded in resizing the nodesList, 0 otherwise
 */
int resize_nodes_list(HashSet *hash_set, int resize)
{
  // We don't need to check the input (we receive it from a function we already checked it in).
  unsigned int new_capacity;
  // Checking if we need to double the size of the capacity or divide it by two.
  if (resize ==1)
    {
      new_capacity = hash_set->capacity * HASH_SET_GROWTH_FACTOR;
    }
  else{
      new_capacity = hash_set->capacity / HASH_SET_GROWTH_FACTOR;
    }

  unsigned int current_capacity = hash_set->capacity;
  hash_set->capacity = new_capacity;
  // We'll dynamically allocate two lists, one is current capacity size, one is
  // new capacity size.
  Node **current_capacity_list = malloc(current_capacity * sizeof(Node*));
  Node **new_capacity_list = malloc(new_capacity * sizeof(Node*));
  // Looping through the HashSet's nodeslist in order to allocate the nodes
  // in the list that will hold the values until we free the NodeList
  for (unsigned int i = 0; i < current_capacity; ++i)
    {
      // Creating new nodes that will take the data of the node in the same index in hashset's nodelist
      Node *curr_node = node_alloc(hash_set->value_cpy, hash_set->value_cmp,
                                   hash_set->value_free);
      if (hash_set->NodesList[i]->data != NULL){
          curr_node->data = hash_set->value_cpy(hash_set->NodesList[i]->data);
      }
      current_capacity_list[i] = curr_node; // The node will point now to the node with the data with the same index
      node_free(&hash_set->NodesList[i]); // freeing the data from hashset's nodelist
    }
  free(hash_set->NodesList); // After freeing the internal data we'll free the whole list
  hash_set->NodesList = NULL;
//  hash_set->NodesList = malloc()
  // Dynamically allocating the nodes in the new list we'll hash the nodes to
  for (unsigned int i = 0; i < new_capacity; ++i)
    {
      Node *internal_node = node_alloc(hash_set->value_cpy,
                                       hash_set->value_cmp,
                                       hash_set->value_free);
      new_capacity_list[i] = internal_node;
    }
  // Now we'll hash the data in the list to the new list (with new capacity)
  unsigned int index_to_hash_to = 0;
  hash_set->NodesList = new_capacity_list;
  for (unsigned int i = 0; i < current_capacity; ++i)
    {
      Value value_to_hash = current_capacity_list[i]->data;
      unsigned int j = 0;
      if (value_to_hash)
        {
          while (j < hash_set->capacity)
            {
              index_to_hash_to =
                  (((j + (j * j)) / 2) + hash_set->hash_func (value_to_hash))
                  & (hash_set->capacity - 1);
              if (j == 0) hash_set->NodesList[index_to_hash_to]->hashCount++;
              if (hash_set->NodesList[index_to_hash_to]->data == NULL)
                {
                  hash_set->NodesList[index_to_hash_to]->data = hash_set->value_cpy (value_to_hash);
                  break;
                }
              j++;
            }
        }
    }
  // We'll free the internal data of the current list because we finished creating the
  // new capacity list, then we'll free the list
  for (unsigned int i = 0; i < current_capacity; ++i)
    {
      node_free(&current_capacity_list[i]); // freeing the data from the nodelist
    }
  free(current_capacity_list);
  return 1; // We finished resizing successfully
}


int hash_set_insert(HashSet *hash_set, Value value){
  // The function inserts *new*, *copied*, *dynamically allocated* Value.
  // We'll check if the input is valid
  if (!hash_set || !value)
    {
      return 0;
    }
  if (!hash_set->NodesList){
      return 0;
    }
  // TODO: Check here if  hashset_contains_value no need to add
//  if (hash_set_contains_value(hash_set, value)){
//      return 0; // Set doesn't contain double values
//    }
  //hash_set->size = hash_set->size + 1;
  unsigned int index_to_hash_to = 0;
  unsigned int j = 0;
  while (j<hash_set->capacity){
      index_to_hash_to = (((j+(j*j))/2)+hash_set->hash_func(value))&(hash_set->capacity-1);
      if (hash_set->NodesList[index_to_hash_to]->data == NULL){
          hash_set->NodesList[index_to_hash_to]->data = hash_set->value_cpy(value);
          hash_set->size++;
          break;
        }
      j++;
    }
  double load_factor = hash_set_get_load_factor(hash_set);
  if (load_factor == -1)
    {
      return 0;
    }
  if (load_factor >= HASH_SET_MAX_LOAD_FACTOR){
      // We need to resize the nodeslist in order to insert the new value
      resize_nodes_list(hash_set, 1);
    }
  return 1;
  // We'll create a node with the value, check if the node exists in the set and if not add it
  //  Node *node = Node() TODO: get back here to finish the function
}





// function to resize our NodesList if our load factor is beyond our limits
void resize(HashSet* hash_set, size_t new_cap)
{
  // the new list we will hash our data into
  Node **new_list = malloc (sizeof (Node *) * new_cap);
  // where we will store the old values we already have
  Node **old_list = malloc (sizeof (Node *) * hash_set->capacity);
  // the capacity of our old list
  size_t old_cap = hash_set->capacity;
  // new_cap is the new desired capacity
  hash_set->capacity = new_cap;

  for (size_t j = 0; j < new_cap; j++){
    //allocate nodes to the new list
    new_list[j] = node_alloc (hash_set->value_cpy, hash_set->value_cmp,
                                hash_set->value_free);
  }

  for (size_t i = 0; i < old_cap; i++){
    //allocate nodes to the old list
    old_list[i] = node_alloc (hash_set->value_cpy, hash_set->value_cmp,
                                hash_set->value_free);
    // if there is data to copy to our old list
    if (hash_set->NodesList[i]->data){
      //copy the data to our old list
      old_list[i]->data = hash_set->value_cpy (hash_set->NodesList[i]->data);
    }
    //copy the hashcount
    old_list[i]->hashCount = hash_set->NodesList[i]->hashCount;
    // free the node we copied from the hashtable
    node_free (&(hash_set->NodesList[i]));
  }
  // free the hashtable as we have a copy of it
  free (hash_set->NodesList);
  hash_set->NodesList = new_list;
  hash_set->size = 0;

  for (size_t i = 0; i < old_cap; i++){
    //rehash all the elements from the old list to our hash table
    hash_set_insert (hash_set, old_list[i]->data);
  }
  // free our copied old_list
  for (size_t i = 0; i < old_cap; i++){
    node_free (&old_list[i]);
    old_list[i] = NULL;
  }
  free(old_list);
  old_list = NULL;
}

HashSet *hash_set_alloc(
    HashFunc hash_func, HashSetValueCpy value_cpy,
    HashSetValueCmp value_cmp, HashSetValueFree value_free){

  if ((!hash_func)||(!value_cpy)||(!value_cmp)||(!value_free)) return NULL;

  HashSet* set = malloc(sizeof(HashSet));
  if (!set) return NULL;

  // initialize our fields
  set->value_free = value_free;
  set->value_cmp = value_cmp;
  set->value_cpy = value_cpy;
  set->hash_func = hash_func;
  set->NodesList = malloc(HASH_SET_INITIAL_CAP* sizeof (Node*));
  set->capacity = HASH_SET_INITIAL_CAP;
  set->size = 0;

  // allocate memory to each node in our NodesList
  for (int i=0; i<HASH_SET_INITIAL_CAP; i++){
    set->NodesList[i] = node_alloc(value_cpy, value_cmp, value_free);
  }
  return set;
}

void hash_set_free(HashSet **p_hash_set){
  if (!p_hash_set) return;

  HashSet* set_to_free = *p_hash_set;
  if (!set_to_free) return;

  // the hashtable
  Node** nodes = set_to_free->NodesList;

  if (nodes != NULL){
    for (size_t i=0; i<set_to_free->capacity; i++){
      // each node we want to free
      Node* curr = nodes[i];
      // if it's a valid node free it
      if (curr) node_free (&curr);
    }
    free(set_to_free->NodesList);
  }
  set_to_free->size = 0;
  free(set_to_free);
  *p_hash_set = NULL;
}

int hash_set_contains_value(HashSet *hash_set, Value value){
  if ((!hash_set)||(!value)) return 0;
  int res = 0;
  size_t i = 0;
  size_t index = 0;

  if((!hash_set->hash_func)||(!hash_set->value_cpy)
     ||(!hash_set->value_cmp)||(!hash_set->value_free)) return 0;

  while (i<hash_set->capacity){
    // do quadratic probing until we find our value
    index =(((i+(i*i))/2)+hash_set->hash_func(value))&(hash_set->capacity-1);
    Node* node = hash_set->NodesList[index];
    // check each node we land on against the given value
    res = check_node (node, value);
    if (res == 1) return 1;
    i++;
  }
  return 0;
}

int hash_set_erase(HashSet *hash_set, Value value){
  if (!hash_set || !value) return 0;
  if (!hash_set_contains_value (hash_set, value)) return 0;

  size_t i = 0;
  size_t index = 0;
  size_t hash_count_index = 0;
  // boolean to store if we erased the value successfully or not
  int erased = 0;

  if((!hash_set->hash_func)||(!hash_set->value_cpy)
     ||(!hash_set->value_cmp)||(!hash_set->value_free)) return 0;

  while (i<hash_set->capacity){
    // do quadratic probing
    index =(((i+(i*i))/2)+hash_set->hash_func(value))&(hash_set->capacity-1);

    if (i==0) hash_count_index = index;

    Node* node = hash_set->NodesList[index];
    erased = check_node (node, value);
    // if we find the node we want to delete
    if (erased==1) {
      clear_node (node);
      hash_set->size--;
      break;
    }
    i++;
  }
  // decrease the hashCount of the first hash if we erased the value
  if (erased==1){
    Node* node = hash_set->NodesList[hash_count_index];
    node->hashCount--;
  }

  // same logic as insert but we decrease the hashtable
  double threshhold = hash_set_get_load_factor (hash_set);
  if (threshhold<=HASH_SET_MIN_LOAD_FACTOR){
    size_t new_cap = (hash_set->capacity)/HASH_SET_GROWTH_FACTOR;
    if (new_cap>=1){
      resize (hash_set, new_cap);
    }
  }
  return (erased==1);
}

double hash_set_get_load_factor(HashSet *hash_set){
  if (!hash_set) return -1;
  if(hash_set->NodesList == NULL) return -1;

  double num = (double)hash_set->size;
  double cap = (double)hash_set->capacity;
  double res = num/cap;
  return res;
}

void hash_set_clear(HashSet *hash_set){
  if (!hash_set) return;
  if (!hash_set->NodesList) return;

  for (size_t i=0; i< hash_set->capacity; i++){
    // if the data is not null
    if (hash_set->NodesList[i]->data){
      clear_node (hash_set->NodesList[i]);
      hash_set->NodesList[i]->hashCount = 0;
    }
  }
  hash_set->size = 0;
}

Value hash_set_at(HashSet *hash_set, int value){
  if (!hash_set) return NULL;
  if (!hash_set->NodesList) return NULL;

  Value res = NULL;

  if (value >= (int)hash_set->capacity || value < 0) return res;
  if (hash_set->NodesList[value] == NULL) return res;

  res = hash_set->NodesList[value]->data;
  return res;
}
