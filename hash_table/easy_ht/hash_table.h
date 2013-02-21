#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__


typedef struct _ht_st HT_T;
struct _ht_st {
  int  key;
  int  val;
  HT_T *next;
};

extern int  ht_lookup(int key, int *val);
extern int  ht_insert(int key, int val);
extern void ht_cleanup(void);
extern int  ht_init(int num);


#endif /* __HASH_TABLE_H__ */
