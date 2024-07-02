#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogdan Kolesnikov");
MODULE_DESCRIPTION("Модуль позволяет записывать, читать и удалять секреты");
MODULE_VERSION("1.0");

#define PROCFS_NAME "secret"
#define MAX_SECRET_SIZE 1024

static char *secret_data;
static int secret_size;

// Функция для чтения данных из файла в procfs
static ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *offset) {
    if (*offset >= secret_size) {
        return 0;
    }

    if (count > secret_size - *offset) {
        count = secret_size - *offset;
    }

    if (copy_to_user(buffer, secret_data + *offset, count)) {
        return -EFAULT;
    }

    *offset += count;
    return count;
}

// Функция для записи данных в файл в procfs
static ssize_t proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset) {
    if (count > MAX_SECRET_SIZE) {
        count = MAX_SECRET_SIZE;
    }

    kfree(secret_data);  // Освобождаем предыдущие данные, если они были
    secret_data = kmalloc(count, GFP_KERNEL);
    if (!secret_data) {
        return -ENOMEM;
    }

    if (copy_from_user(secret_data, buffer, count)) {
        kfree(secret_data);
        secret_data = NULL;
        return -EFAULT;
    }

    secret_size = count;

    // Проверка на команду "DELETE"
    if (count >= 6 && !strncmp(secret_data, "DELETE", 6)) {
        kfree(secret_data);
        secret_data = NULL;
        secret_size = 0;
    }

    return count;
}

// Операции для файла в procfs
static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

// Инициализация модуля
static int __init secret_init(void) {
    proc_create(PROCFS_NAME, 0666, NULL, &proc_fops);
    printk(KERN_INFO "Secret module loaded.\n");
    return 0;
}

// Удаление модуля
static void __exit secret_exit(void) {
    remove_proc_entry(PROCFS_NAME, NULL);
    kfree(secret_data);
    secret_data = NULL;
    printk(KERN_INFO "Secret module unloaded.\n");
}

module_init(secret_init);
module_exit(secret_exit);
