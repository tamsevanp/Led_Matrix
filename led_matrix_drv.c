#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVICE_NAME "led_matrix"
#define CLASS_NAME "led_matrix_class"
#define SPI_BUS 0
#define SPI_BUS_CS1 0
#define SPI_BUS_SPEED 1000000 // 1 MHz

// MAX7219 Registers
#define MAX7219_REG_NOOP        0x00
#define MAX7219_REG_DIGIT0      0x01
#define MAX7219_REG_DIGIT1      0x02
#define MAX7219_REG_DIGIT2      0x03
#define MAX7219_REG_DIGIT3      0x04
#define MAX7219_REG_DIGIT4      0x05
#define MAX7219_REG_DIGIT5      0x06
#define MAX7219_REG_DIGIT6      0x07
#define MAX7219_REG_DIGIT7      0x08
#define MAX7219_REG_DECODE      0x09
#define MAX7219_REG_INTENSITY   0x0A
#define MAX7219_REG_SCANLIMIT   0x0B
#define MAX7219_REG_SHUTDOWN    0x0C
#define MAX7219_REG_DISPLAYTEST 0x0F

// SPI Device
static struct spi_device *led_matrix_spi_device;

// Device file major number
static int major_number;
static struct class* led_matrix_class = NULL;
static struct device* led_matrix_device = NULL;

// Function to send data to the MAX7219 via SPI
static int max7219_send(unsigned char address, unsigned char data)
{
    unsigned char tx_buf[2];
    struct spi_transfer transfer = {
        .tx_buf = tx_buf,
        .len = 2,
        .speed_hz = SPI_BUS_SPEED,
        .bits_per_word = 8,
    };

    tx_buf[0] = address;
    tx_buf[1] = data;

    return spi_sync_transfer(led_matrix_spi_device, &transfer, 1);
}

// Initialize MAX7219 LED driver
static void max7219_init(void)
{
    max7219_send(MAX7219_REG_SHUTDOWN, 0x01);  // Turn on
    max7219_send(MAX7219_REG_DECODE, 0x00);    // No decode for matrix
    max7219_send(MAX7219_REG_SCANLIMIT, 0x07); // Scan all digits (8 digits)
    max7219_send(MAX7219_REG_INTENSITY, 0x0F); // Maximum intensity
    max7219_send(MAX7219_REG_DISPLAYTEST, 0x00); // No display test
}

// Write function to update the LED matrix
static ssize_t led_matrix_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    char data[8];

    if (len != 8)
        return -EINVAL;

    if (copy_from_user(data, buffer, 8))
        return -EFAULT;

    for (int i = 0; i < 8; i++) {
        max7219_send(MAX7219_REG_DIGIT0 + i, data[i]);
    }

    return len;
}

// Open function
static int led_matrix_open(struct inode *inode, struct file *file)
{
    pr_info("LED Matrix device opened\n");
    return 0;
}

// Release function
static int led_matrix_release(struct inode *inode, struct file *file)
{
    pr_info("LED Matrix device closed\n");
    return 0;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = led_matrix_write,
    .open = led_matrix_open,
    .release = led_matrix_release,
};

// SPI probe function
static int led_matrix_probe(struct spi_device *spi_device)
{

    led_matrix_spi_device = spi_device;

    // Register the character device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Failed to register a major number\n");
        return major_number;
    }

    // Create the device class
    led_matrix_class = class_create(CLASS_NAME);
    if (IS_ERR(led_matrix_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("Failed to create the device class\n");
        return PTR_ERR(led_matrix_class);
    }

    // Create the device
    led_matrix_device = device_create(led_matrix_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_matrix_device)) {
        class_destroy(led_matrix_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("Failed to create the device\n");
        return PTR_ERR(led_matrix_device);
    }

    // Initialize the MAX7219
    max7219_init();
    pr_info("LED Matrix SPI device initialized\n");

    return 0;
}

// SPI remove function
static void led_matrix_remove(struct spi_device *spi_device)
{
    device_destroy(led_matrix_class, MKDEV(major_number, 0));
    class_destroy(led_matrix_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    pr_info("LED Matrix SPI device removed\n");
    return ; 
}

// SPI driver structure
static struct spi_driver led_matrix_spi_driver = {
    .driver = {
        .name = "led_matrix_driver",
        .owner = THIS_MODULE,
    },
    .probe = led_matrix_probe,
    .remove = led_matrix_remove,
};

// Module initialization function
static int __init led_matrix_init_module(void)
{
    pr_info("LED Matrix Module Init\n");
    return spi_register_driver(&led_matrix_spi_driver);
}

// Module exit function
static void __exit led_matrix_exit_module(void)
{
    spi_unregister_driver(&led_matrix_spi_driver);
    pr_info("LED Matrix Module Exit\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tamil Selvan");
MODULE_DESCRIPTION("Custom LED Matrix Driver with SPI on Raspberry Pi");
MODULE_VERSION("1.0");

module_init(led_matrix_init_module);
module_exit(led_matrix_exit_module);

