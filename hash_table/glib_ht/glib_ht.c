#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>


/*
 * free_key()
 */
void free_key (gpointer data)
{
  free(data);
}

/*
 * free_value()
 */
void free_value (gpointer value)
{
  free(value);
}

/*
 * main()
 */
int main (void)
{
  GHashTable   *ht;
  char         key[20];
  char         val[20];
  char         *tmp;

  /*
   * Create hashtable
   */
  ht = g_hash_table_new_full(g_str_hash, g_str_equal, free_key, free_value);
  if (!ht) {
      printf("Create hash table failed\n");
      return -1;
  }

  strcpy(key, "foo");
  strcpy(val, "bar");

  /*
   * Intert into hash table.
   */
  g_hash_table_insert(ht, strdup(key), strdup(val));

  /*
   * Show total element number.
   */
  printf("Total element [%d]\n", g_hash_table_size(ht));

  /*
   * Lookup from hash table.
   */
  tmp = g_hash_table_lookup(ht, key);
  if (tmp) {
    printf("Found value[%s] for key[foo]\n", tmp);
  }

  /*
   * Remove one key pair from hash table.
   */
  g_hash_table_remove(ht, "foo");


  /*
   * Destroy hash table.
   */
  g_hash_table_destroy(ht);

  return 0;
}
