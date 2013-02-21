#include <stdio.h>

#include "hash_table.h"


int main (void)
{
  int key, val, retrive;

  ht_init(10);

  /*
   * Insert a new key
   */
  key = 8;
  val = 1;
  ht_insert(key, val);

  /*
   * Lookup
   */
  retrive = 0;
  ht_lookup(key, &retrive);
  printf("lookup for key[%d] val=[%d]\n", key, retrive);

  /*
   * Insert another key, this key will be dispatched to the same bucket.
   */
  key = 18;
  val = 2;
  ht_insert(key, val);

  /*
   * Lookup
   */
  retrive = 0;
  ht_lookup(key, &retrive);
  printf("lookup for key[%d] val=[%d]\n", key, retrive);
  ht_lookup(8, &retrive);
  printf("lookup for key[%d] val=[%d]\n", 8, retrive);

  /*
   * Insert with the same key
   */
  key = 18;
  val = 3;
  ht_insert(key, val);

  /*
   * Lookup
   */
  retrive = 0;
  ht_lookup(key, &retrive);
  printf("lookup for key[%d] val=[%d]\n", key, retrive);

  /*
   * Free object
   */
  ht_cleanup();
}
