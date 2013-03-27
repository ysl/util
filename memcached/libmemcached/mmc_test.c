#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libmemcached/memcached.h>


memcached_st  *pmmc;

/*
 * mmc_cleanup()
 */
void mmc_cleanup (void)
{
  if (pmmc)
    memcached_free(pmmc);

  pmmc = NULL;
}

/*
 * mmc_init()
 */
int mmc_init (char *server_list)
{
  memcached_server_st   *servers;
  memcached_return      ret;

  pmmc = memcached_create(NULL);
  if (!pmmc) {
    printf("Cannot create mmc object.\n");
    return -1;
  }

  /*
   * Add server.
   */
  servers = memcached_servers_parse(server_list);
  ret = memcached_server_push(pmmc, servers);
  if (ret != MEMCACHED_SUCCESS) {
    printf("Cannot push the mmc server list. [%s]\n",
           memcached_strerror(pmmc, ret));
    return -1;
  }

  /*
   * Set options.
   */
  memcached_behavior_set(pmmc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 5000);
  memcached_behavior_set(pmmc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 5000);
  memcached_behavior_set(pmmc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);

  memcached_server_list_free(servers);

  return 0;
}

/*
 * main()
 */
int main (int argc, char **argv)
{
  char              *server_list;
  char              *key;
  char              *value;
  size_t            value_len;
  uint32_t          flags;
  memcached_return  err;

  /*
   * Get option.
   */
  if (argc != 3) {
    printf("Argument error.\n");
    return -1;
  }
  server_list = argv[1];
  key = argv[2];

  /*
   * Init.
   */
  if (mmc_init(server_list) != 0) {
    printf("mmc_init() failed.\n");
    return -1;
  }

  while (1) {
    /*
     * Get.
     */
    value = memcached_get(pmmc, key, strlen(key), &value_len, &flags, &err);


    if (err == MEMCACHED_SUCCESS) {
      printf("key[%s] value[%s] len[%d]\n", key, value, value_len);

      if (value)
        free(value);

    } else {
      if (err == MEMCACHED_NOTFOUND) {
        printf("MEMCACHED_NOTFOUND\n");
      } else {
        printf("memcached_get() error [%d]\n", err);
      }
    }

    /*
     * Set.
     */
    err = memcached_set(pmmc, key, strlen(key), "",
                        0, 0, 0);
    if (err != MEMCACHED_SUCCESS) {
      printf("memcached_set() error [%d]\n", err);
    }

    sleep(1);
  }

  mmc_cleanup();

  return 0;
}
