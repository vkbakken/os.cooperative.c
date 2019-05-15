#include <stdint.h>
#include <stdbool.h>

#include "workq.h"

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item1);


void fun1(struct workq_item *itm)
{
}


int main(void)
{
  
//  #define WORKQ_DECLARE(__name__)       \
//                static struct workq wq_##__name__

  workq_init(&wq_main);
  workq_item_init(&wqi_item1, fun1);
  workq_post(&wq_main, &wqi_item1);
  while (1) {
    if (workq_iterate(&wq_main) == 0) {
      //Sleep
    } 
  }
  
  return 0;
}
