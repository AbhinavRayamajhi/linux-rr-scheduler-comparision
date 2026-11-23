#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/sched.h>
#include <trace/events/sched.h>
#include <linux/tracepoint-defs.h>
#include <linux/tracepoint.h>
#include <linux/string.h>

static atomic_t event_counter = ATOMIC_INIT(0);
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

static void handler(void* data,
                    bool preempt,
                    struct task_struct *prev,
                    struct task_struct *next,
                    unsigned int prev_state)
{
    int counter = atomic_read(&event_counter);
    if (counter < 3)
    {
        printk("Task: %d\n", next->pid);
        atomic_inc(&event_counter);
    }
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
}

module_init(custom_init);
module_exit(custom_exit);

MODULE_LICENSE("GPL");
