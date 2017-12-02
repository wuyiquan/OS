#include <linux/list.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>


int init_module (void)
{
    
    int running_count = 0;
    int interruptible_count = 0;
    int uninterruptible_count = 0;
    int zombie_count = 0;
    int stopped_count = 0;
    int traced_count = 0;
    int dead_count = 0;
    int killable_count = 0;
    int other_count = 0;
    struct task_struct *task;
    printk("Travel2!\n");
    for_each_process(task)
    {
        switch (task->state)
        {
            case TASK_RUNNING:running_count++;break;
            case TASK_INTERRUPTIBLE:interruptible_count++;break;
            case TASK_UNINTERRUPTIBLE:uninterruptible_count++;break;
            case EXIT_ZOMBIE:zombie_count++;break;
            case TASK_STOPPED:stopped_count++;break;
            case TASK_TRACED:traced_count++;break;
            case EXIT_DEAD:dead_count++;break;
            case TASK_KILLABLE:killable_count++;break;
            default:other_count++;break;
        }
        printk("pname = %20s\tpid = %d\tpstate=%ld\tppname=%20s\n",task->comm, task->pid, task->state, task->real_parent->comm);
    }
    printk("TASK_RUNNING:%d\n",running_count);
    printk("TASK_INTERRUPTIBLE:%d\n",interruptible_count);
    printk("TASK_UNINTERRUPTIBLE:%d\n",uninterruptible_count);
    printk("EXIT_ZOMBIE:%d\n",zombie_count);
    printk("TASK_STOPPED:%d\n",stopped_count);
    printk("TASK_TRACED:%d\n",traced_count);
    printk("EXIT_DEAD:%d\n",dead_count);
    printk("TASK_KILLABLE:%d\n",killable_count);
    printk("OHTERS:%d\n", other_count);
    return 0;
}

void cleanup_module (void)
{
    printk ("Goodbye!\n");
}
MODULE_LICENSE("GPL");