#include <stdlib.h>

#include "hash_table.h"


static int   _bucket_num = 0;
static HT_T  **_ht = NULL;

/*
 * ht_lookup()
 */
int ht_lookup (int key, int *val)
{
  HT_T *ht;

  ht = _ht[key % _bucket_num];
  if (ht == NULL) {
    return -1;
  }

  /*
   * Iterate all entries
   */
  for (; ht != NULL; ht = ht->next) {
    if (ht->key == key) {
      /*
       * Found
       */
      *val = ht->val;
      return 0;
    }
  }

  /*
   * Not found
   */
  return -1;
}

/*
 * ht_insert()
 */
int ht_insert (int key, int val)
{
  HT_T  **head, *ht;
  HT_T  *new_ht;

  head = &_ht[key % _bucket_num];
  ht = *head;

  while (1) {
    if (ht == NULL) {
      /*
       * Insert new entry
       */
      new_ht = malloc(sizeof(HT_T));
      if (new_ht == NULL)
        return -1;

      /*
       * Set value
       */
      new_ht->key = key;
      new_ht->val = val;
      new_ht->next = *head;
      *head = new_ht;
      return 0;

    } else {
      if (ht->key == key) {
        /*
         * Replace the old one
         */
        ht->val = val;
        return 0;
      }
    }

    /*
     * Go to next one
     */
    ht = ht->next;

  }

  return 0;
}

/*
 * ht_free()
 */
static void ht_free (HT_T *ht)
{
  if (ht != NULL) {
    ht_free(ht->next);
    free(ht);
  }
}

/*
 * ht_destory()
 */
void ht_cleanup (void)
{
  int   i;

  for (i = 0; i <_bucket_num; i++) {
    ht_free(_ht[i]);
  }

  free(_ht);
}

/*
 * ht_init()
 */
int ht_init (int num)
{
  _bucket_num = num;

  _ht = calloc(_bucket_num, sizeof(HT_T *));
  if (_ht == NULL)
    return -1;
  else
    return 0;
}
