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
                static struct workq k_##__name__



void workq_item_init(struct k_workq_item *w, void (*fun)(struct k_workq_item * work));

void workq_init(struct k_workq *q);
bool workq_is_empty(struct k_workq *q);
void workq_post(struct k_workq *q, struct k_workq_item *w);
void workq_post_delayed(struct k_workq *q, struct k_workq_item *w, uint32_t dly);
void workq_cancel(struct k_workq *q, struct k_workq_item *w);


uint32_t workq_iterate(struct k_workq *q);
#endif /*WQUEUE_H_INCLUDED*/
