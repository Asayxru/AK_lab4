/*
 * Hello, world module for lab 4
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Ваше ім'я");
MODULE_DESCRIPTION("Hello, world for lab 4");
MODULE_LICENSE("Dual BSD/GPL");

/* Параметр модуля: скільки разів друкувати Hello, world! */
static unsigned int hello_count = 1;

module_param(hello_count, uint, 0644);
MODULE_PARM_DESC(hello_count, "Number of times to print Hello, world!");

/* Структура для елемента списку з часом події */
struct hello_event {
	struct list_head list;
	ktime_t time;
};

/* Голова списку подій */
static LIST_HEAD(events_list);

static int __init hello_init(void)
{
	unsigned int i;
	struct hello_event *event;

	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10))
		pr_warn("hello: suspicious hello_count = %u\n", hello_count);

	if (hello_count > 10) {
		pr_err("hello: invalid hello_count = %u\n", hello_count);
		return -EINVAL;
	}

	for (i = 0; i < hello_count; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event) {
			pr_err("hello: kmalloc failed on iteration %u\n", i);
			return -ENOMEM;
		}

		event->time = ktime_get();
		list_add_tail(&event->list, &events_list);

		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event, *tmp;

	list_for_each_entry_safe(event, tmp, &events_list, list) {
		pr_info("hello: event time = %lld ns\n",
			(long long)ktime_to_ns(event->time));
		list_del(&event->list);
		kfree(event);
	}
}

module_init(hello_init);
module_exit(hello_exit);
