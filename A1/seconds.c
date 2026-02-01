/*
 * seconds.c
 *
 * Kernel module that creates /proc/seconds and reports
 * the number of seconds elapsed since the module was loaded.
 *
 * Linux Distribution Information:
 * --------------------------------
 * Distributor ID: Ubuntu
 * Description:    Ubuntu 25.10
 * Release:        25.10
 * Codename:       questing
 *
 * Kernel Version:
 * ----------------
 * 6.17.0-8-generic
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>

#define PROC_NAME "seconds"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tridib Banik and Ali Shareeff");
MODULE_DESCRIPTION("Reports elapsed seconds since module load");
MODULE_VERSION("1.0");

static unsigned long start_jiffies;

/* Function called when /proc/seconds is read */
static int seconds_show(struct seq_file *m, void *v)
{
    unsigned long elapsed_jiffies = jiffies - start_jiffies;
    unsigned long elapsed_seconds = elapsed_jiffies / HZ;

    seq_printf(m, "%lu\n", elapsed_seconds);
    return 0;
}

/* Open function */
static int seconds_open(struct inode *inode, struct file *file)
{
    return single_open(file, seconds_show, NULL);
}

/* File operations structure */
static const struct proc_ops seconds_ops = {
    .proc_open    = seconds_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* Module initialization */
static int __init seconds_init(void)
{
    start_jiffies = jiffies;

    proc_create(PROC_NAME, 0, NULL, &seconds_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    return 0;
}

/* Module cleanup */
static void __exit seconds_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

module_init(seconds_init);
module_exit(seconds_exit);
