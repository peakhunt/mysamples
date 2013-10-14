#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "rbtree.h"

typedef struct
{
   struct rb_node    rb_node;
   int               key;
   int               data;
} MyRBDemo;

static int call_depth, max_call_depth = 0;
static int item_count = 0;

typedef void (*myrb_iterae_func)(MyRBDemo* n);

MyRBDemo*
myrbdemo_search(struct rb_root*  root, int   key)
{
   struct rb_node*   n = root->rb_node;
   MyRBDemo*         demo_node;

   while(n)
   {
      demo_node = rb_entry(n, MyRBDemo, rb_node);

      if(key < demo_node->key)
      {
         n = n->rb_left;
      }
      else if(key > demo_node->key)
      {
         n = n->rb_right;
      }
      else
      {
         return demo_node;
      }
   }

   return NULL;
}

int
myrbdemo_insert(struct rb_root* root, MyRBDemo* n)
{
   struct rb_node**  p = &root->rb_node;
   struct rb_node*   parent = NULL;
   MyRBDemo*         page;

   while(*p)
   {
      parent = *p;
      page = rb_entry(parent, MyRBDemo, rb_node);

      if(n->key < page->key)
      {
         p = &(*p)->rb_left;
      }
      else if(n->key > page->key)
      {
         p = &(*p)->rb_right;
      }
      else
      {
         return -1;
      }
   }

   rb_link_node(&n->rb_node, parent, p);
   rb_insert_color(&n->rb_node, root);
   return 0;
}

void
myrbdemo_delete(struct rb_root* root, MyRBDemo* n)
{
   rb_erase(&n->rb_node, root);
}

static inline void
__myrbdemo_do_preoder_traversal(struct rb_node* node, myrb_iterae_func cb)
{
   if(node == NULL)
   {
      call_depth--;
      return;
   }

   call_depth++;
   if(call_depth > max_call_depth)
   {
      max_call_depth = call_depth;
   }

   __myrbdemo_do_preoder_traversal(node->rb_left, cb);

   cb(rb_entry(node, MyRBDemo, rb_node));

   __myrbdemo_do_preoder_traversal(node->rb_right, cb);
}

void
myrbdemo_do_preorder_traversal(struct rb_root* root, myrb_iterae_func cb)
{
   __myrbdemo_do_preoder_traversal(root->rb_node, cb);
}

static void
myrbdemo_preorder_callback(MyRBDemo* d)
{
   static int prev_key = -1;

   item_count++;
   if(prev_key != -1 && prev_key + 1 != d->key)
   {
      printf("Error in sequencing %d\n", d->key);
      exit(0);
   }
   prev_key = d->key;
   //printf("callback --> %d, %d\n", d->key, d->data);
}

int
main()
{
   struct rb_root    root = RB_ROOT;
   int i;
   MyRBDemo*      d;

   printf("beginning to insert\n");
   for(i = 0; i < 3000000; i++)
   {
      d = (MyRBDemo*)malloc(sizeof(MyRBDemo));
      rb_init_node(&d->rb_node);

      d->key   = i;
      d->data  = i;

      if(myrbdemo_insert(&root, d) != 0)
      {
         printf("insert failed for %d\n", i);
         return -1;
      }
   }
   printf("insert done\n");

#if 0
   printf("deleting all test\n");
   for(i = 0; i < 3000000; i++)
   {
      d = myrbdemo_search(&root, i);
      if(d == NULL)
      {
         printf("search failed for %d\n", i);
         return -1;
      }
      myrbdemo_delete(&root, d);
      free(d);
   }
   printf("deleting all test done\n");
   return 0;
#endif

   printf("search start\n");
   d = myrbdemo_search(&root, 1555);
   if(d == NULL)
   {
      printf("search failed. test failed!\n");
      return -1;
   }
   printf("search success for 1555: %d, %d\n", d->key, d->data);

   printf("failed search start\n");
   d = myrbdemo_search(&root, 3000001);
   if(d != NULL)
   {
      printf("something wrong. search for 3000001 is supposed to fail.  %d, %d\n", d->key, d->data);
      return -1;
   }
   printf("success: search failed for 3000001\n");

   printf("search and delete start for 29845\n");
   d = myrbdemo_search(&root, 29845);
   if(d == NULL)
   {
      printf("search failed for 29845\n");
      return -1;
   }
   myrbdemo_delete(&root, d);
   free(d);
   printf("search and delete done for 29845\n");

   printf("searching deleted item : 29845\n");
   d = myrbdemo_search(&root, 29845);
   if(d != NULL)
   {
      printf("failed search failed for deleted item 29845\n");
      return -1;
   }
   printf("searching deleted item : 29845 done\n");

   printf("adding duplicate 13333\n");
   d = (MyRBDemo*)malloc(sizeof(MyRBDemo));
   rb_init_node(&d->rb_node);
   d->key = 13333;
   d->data = 13333;

   if(myrbdemo_insert(&root, d) != -1)
   {
      printf("adding duplicate 13333 failed\n");
   }
   free(d);
   printf("adding duplicate done\n");

   printf("adding back deleted 29845\n");

   d = (MyRBDemo*)malloc(sizeof(MyRBDemo));
   rb_init_node(&d->rb_node);
   d->key = 29845;
   d->data = 29845;
   if(myrbdemo_insert(&root, d) != 0)
   {
      printf("adding back deleted 29845 failed\n");
      return -1;
   }
   printf("adding back deleted 29845 done\n");

   printf("preorder traversal begin\n");
   myrbdemo_do_preorder_traversal(&root, myrbdemo_preorder_callback);
   printf("preorder traversal end\n");
   printf("max call depth: %d, item_count %d\n", max_call_depth, item_count);

   printf("deleting all test\n");
   for(i = 0; i < 3000000; i++)
   {
      d = myrbdemo_search(&root, i);
      if(d == NULL)
      {
         printf("search failed for %d\n", i);
         return -1;
      }
      myrbdemo_delete(&root, d);
      free(d);
   }
   printf("deleting all test done\n");

   printf("final simple search\n");
   d = myrbdemo_search(&root, 15);
   if(d != NULL)
   {
      printf("search for 15 in an empty ebtree has not failed\n");
      return -1;
   }
   printf("final simple search done\n");
   return 0;
}
