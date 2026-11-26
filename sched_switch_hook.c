#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/sched.h>
#include <linux/tracepoint-defs.h>
#include <linux/tracepoint.h>
#include <linux/string.h>
#include <linux/sched/clock.h>

#define MAX_ITEMS_CAPTURED 10000

// Struct to hold the capture info
struct event_info
{
    char name[TASK_COMM_LEN];
    int prev_pid;
    int next_pid;
    unsigned int state;
    u64 current_time;
};

// Linear buffer that gets exposed to user program
static struct event_info sched_switch_capture[MAX_ITEMS_CAPTURED];

// Counter for linear buffer
static atomic_t event_counter = ATOMIC_INIT(0);


// Find the tracepoint and store it
static struct tracepoint* current_tracepoint = NULL;
static void find_tracepoint(struct tracepoint *tp,
                              void *priv)
{
    char *tracepoint_to_find = "sched_switch";
    if (!strcmp(tp->name, tracepoint_to_find))
    {
        current_tracepoint = tp;
    }
}

// Probe to get info during context switch
static void handler(void* priv,
                    bool preempt,
                    struct task_struct *prev,
                    struct task_struct *next,
                    unsigned int prev_state)
{
    int index = atomic_fetch_inc(&event_counter);
    if (index >= MAX_ITEMS_CAPTURED) return;

    struct event_info* current_event = &sched_switch_capture[index];
    strscpy(current_event->name, prev->comm, TASK_COMM_LEN);
    current_event->prev_pid = prev->pid;
    current_event->next_pid = next->pid;
    current_event->state = prev_state;
    current_event->current_time = sched_clock();
}

static int custom_init(void) 
{
    printk("Module started...\n");

    for_each_kernel_tracepoint(find_tracepoint, NULL);
    if (current_tracepoint == NULL)
    {
        printk("Error : Unable to find the tracepoint\n");
        return -1;
    }

    return tracepoint_probe_register(current_tracepoint, handler, NULL);
}

static void custom_exit(void)
{
    printk("Module ending...\n");
    tracepoint_probe_unregister(current_tracepoint, handler, NULL);
    tracepoint_synchronize_unregister();

    if(atomic_read(&event_counter) >= MAX_ITEMS_CAPTURED)
        printk("Events recorded: %d\n", MAX_ITEMS_CAPTURED);
    else
        printk("Events recorded: %d\n", atomic_read(&event_counter));

    for(int i = 0; i < 3; ++i) {
        printk("Event %d: %s prev_pid: %d next_pid: %d state: %u time: %llu\n",
            i,
            sched_switch_capture[i].name,
            sched_switch_capture[i].prev_pid,
            sched_switch_capture[i].next_pid,
            sched_switch_capture[i].state,
            sched_switch_capture[i].current_time);
    }
}

module_init(custom_init);
module_exit(custom_exit);

MODULE_LICENSE("GPL");
