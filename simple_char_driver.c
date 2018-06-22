/*
 * @file    simple_char_driver.c
 * @author  Austin Rugh
 * @version 1.0
 * @class CSCI-3752: Operating Systems
 * @professor David Knox
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

#define BUFFER_SIZE 1024
#define DEVICE_NAME "simple_char_driver"

//define device buffer and other data structures needed

static char *device_buffer;
static int major_num;
static int open_count;
static int close_count;
static int size_of_kbuff = 0;
static loff_t* offset; //global offset that tracks buffer place

static DEFINE_MUTEX(scd_mutex);

static ssize_t scd_read(struct file *pfile, char char *buffer, size_t length, loff_t* offset);
static ssize_t scd_write(struct file * pfile, const char *buffer, size_t length, loff_t* offset);
static int scd_open(struct inode *pin, struct file* pfile);
static int scd_close(struct inode *pin, struct file* pfile);
static loff_t scd_seek(struct file* pfile, loff_t offset, int where);


//linking structure
static struct file_operations scd_fops = {
  .owner = THIS_MODULE,
  .read = scd_read,
  .write = scd_write,
  .open = scd_open,
  .release = scd_close,
  .seek = scd_seek,
};

/* @brief Initializing the LKM
 * Limits visibility by static keyword.
 * Attempts to dynamically allocate major number
 */
static int scd_init(void){
  printk(KERN_ALERT "Initializsing the Simple Character Driver LKM \n");

  //register the device
  major_num = register_chrdev(0, DEVICE_NAME, &scd_fops);
  if(major_num<0){
    printk(KERN_ALERT "SCD failed to register a major_num\n");
    return -1;
  }

  printk(KERN_ALERT "SCD registered correctly with major_num %d\n", major_num);

  //allocate memory and reset open and close counts
  device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
  open_count = 0;
  close_count = 0;

  mutex_init(&scd_mutex);

  return 0;
}

static void scd_exit(void){
  printk(KERN_ALERT "Exiting the Simple Character Driver");

  mutex_destroy(&scd_mutex);

  //unregister device using unregister_chrdev
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_ALERT "SCD unregistered successfully");

  //free the kernel buffer
  kfree(device_buffer);
  printk(KERN_ALERT "kernel buffer freed");
}

/* @brief Called each time the device is opened
 * Increments open_count
 * @param pin: A pointer to an inode object
 * @param pfile: A pointer to a file object
 */
static int scd_open(struct inode* pin, struct file* pfile){
  if(!mutex_trylock(&scd_mutex)){
    printk(KERN_ALERT "SCD: Device in use\n");
    return -EBUSY;
  }
  open_count++;
  printk(KERN_ALERT "SCD: Device has been opened %d time(s)\n", open_count);
  return 0;
}

/* @brief Release fuction called every time the device is closed by the user program
 * @param pin: A pointer to an inode object
 * @param pfile: A pointer to a file object
 */
static int scd_close(struct inode* pin, struct file* pfile){
  mutex_unlock(&scd_mutex);
  close_count++;
  printk(KERN_ALERT "SCD: Device has been closed %d time(s)\n", close_count);
  return 0;
}

/* @brief read function that reads kernel buffer into user buffer
 */
static ssize_t scd_read (struct file *pfile, char *buffer, size_t length, loff_t *offset){

  unsigned long bytes_not_copied = copy_to_user(buffer, (device_buffer + *offset), length);

  if(bytes_not_copied == 0){
    printk(KERN_ALERT "Success, all bytes copied successfully\n");
    *offset = *offset + length;
    return 0;
  }
  else{
    printk(KERN_ALERT "Failure, some bytes NOT COPIED\n");
  }
  return -1;
}

/*@brief Write function called whenever the device is being written to
 */
static ssize_t scd_write(struct file* pfile, char* buffer,size_t length, loff_t* offset){

  int bytesAvailable = BUFFER_SIZE - size_of_kbuff;
  *offset = size_of_kbuff;

  if(bytesAvailable < length){
    printk(KERN_ALERT "Error: String too long or buffer full. Bytes Available: %d \n", bytesAvailable);
    return -1;
  }

  unsigned long bytes_not_written = copy_from_user((device_buffer + *offset), buffer, length);

  if(bytes_not_written == 0){
    printk(KERN_ALERT "Success, all chars copied to kernel");
    size_of_kbuff += length;
    *offset += length;
    return 0;
  }
  else{
    printk(KERN_ALERT "Failure, some chars not copied");
    size_of_kbuff += (length - bytes_not_written);
    *offset += (length - bytes_not_written);
  }
  return -1;
}

static loff_t scd_seek(struct file* pfile, loff_t* uoffset, int whence){
  loff_t ret_pos;
  switch(whence){
    case 0: //SEEK_SET
      ret_pos = uoffset;
      break;

    case 1: //SEEK_CURR
      ret_pos = *offset + uoffset;
      break;

    case 2: //SEEK_END
      ret_pos = BUFFER_SIZE + uoffset;

    default: //Error
      return -EINVAL;
  }
  if (ret_pos < 0){
    return -EINVAL;
  }
  *offset = ret_pos
  return ret_pos;
}


module_init(scd_init);
module_exit(scd_exit);
