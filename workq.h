#ifndef WQUEUE_H_INCLUDED
#define WQUEUE_H_INCLUDED


#include <stdint.h>
#include <stdbool.h>


struct workq_item {
        struct workq_item *next;
        void (*fun)(struct workq_item *work);
        uint32_t time;
};


struct workq {
        struct workq_item *start;
        struct workq_item *end;
};


#define WORKQ_DECLARE(__name__)       \
                static struct workq wq_##__name__



void workq_item_init(struct workq_item *w, void (*fun)(struct workq_item * work));

void workq_init(struct workq *q);
bool workq_is_empty(struct workq *q);
void workq_post(struct workq *q, struct workq_item *w);
void workq_post_delayed(struct workq *q, struct workq_item *w, uint32_t dly);
void workq_cancel(struct workq *q, struct workq_item *w);


uint32_t workq_iterate(struct workq *q);
#endif /*WQUEUE_H_INCLUDED*/
