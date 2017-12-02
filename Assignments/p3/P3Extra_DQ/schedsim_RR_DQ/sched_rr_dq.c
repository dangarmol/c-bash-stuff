#include "sched.h"


int rr_quantum=3;

void sched_init_rr_dq(void) {}

void sched_destroy_rr_dq(void) {}
  

static void task_new_rr_dq(task_t* t){ }

static task_t* pick_next_task_rr_dq(runqueue_t* rq,int cpu) { }

static void enqueue_task_rr_dq(task_t* t,int cpu, int runnable) { }


static void task_tick_rr_dq(runqueue_t* rq,int cpu){ }

static task_t* steal_task_rr_dq(runqueue_t* rq,int cpu){ }
    

sched_class_t rr_dq_sched={
    .sched_init=sched_init_rr_dq,
    .sched_destroy=sched_destroy_rr_dq,
    .task_new=task_new_rr_dq,
    .pick_next_task=pick_next_task_rr_dq,
    .enqueue_task=enqueue_task_rr_dq,
    .task_tick=task_tick_rr_dq,
    .steal_task=steal_task_rr_dq
};
