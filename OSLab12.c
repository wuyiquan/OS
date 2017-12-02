#include <linux/list.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>


int init_module (void)
{
    
    int running_count = 0;//记录running状态的进程个数
    int interruptible_count = 0;//记录interruptible状态的进程个数
    int uninterruptible_count = 0;//记录uninterruptible状态的进程个数
    int zombie_count = 0;//记录zombie状态的进程个数
    int stopped_count = 0;//记录stopped状态的进程个数
    int traced_count = 0;//记录traced状态的进程个数
    int dead_count = 0;//记录dead状态的进程个数
    int killable_count = 0;//记录killable状态的进程个数
    int other_count = 0;//记录其他状态的进程个数
    struct task_struct *task;//用来指向遍历到的进程
    printk("Travel2!\n");
    for_each_process(task)
    {
        switch (task->state)
        {
            //根据不同状态，给不同的计数器加一
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
        //输出所需的信息，包括进程名，进程ID，进程状态以及父进程名
        printk("pname = %20s\tpid = %d\tpstate=%ld\tppname=%20s\n",task->comm, task->pid, task->state, task->real_parent->comm);
        
    }
    printk("TASK_RUNNING:%d\n",running_count);//输出running状态的进程个数
    printk("TASK_INTERRUPTIBLE:%d\n",interruptible_count);//输出interruptible状态的进程个数
    printk("TASK_UNINTERRUPTIBLE:%d\n",uninterruptible_count);//输出uninterruptible状态的进程个数
    printk("EXIT_ZOMBIE:%d\n",zombie_count);//输出zombie状态的进程个数
    printk("TASK_STOPPED:%d\n",stopped_count);//输出stopped状态的进程个数
    printk("TASK_TRACED:%d\n",traced_count);//输出traced状态的进程个数
    printk("EXIT_DEAD:%d\n",dead_count);//输出dead状态的进程个数
    printk("TASK_KILLABLE:%d\n",killable_count);//输出killable状态的进程个数
    printk("OHTERS:%d\n", other_count);//输出其他状态的进程个数
    return 0;
}

void cleanup_module (void)
{
    printk ("Goodbye!\n");
}
MODULE_LICENSE("GPL");