#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/slab.h>  //kmalloc
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/ioctl.h>
#include <linux/of_device.h>  /* of_match_device */
#include <linux/of_address.h> /* of_iomap */
#include "vga_ctl.h"

/*
#define VRAM_BASE 0x000A0000
#define VRAM_SIZE 0x00020000
#define VRAM_MASK (VRAM_SIZE - 1)
*/

#define VGA_CONTROLLER_BASE 0x00001008
#define VGA_CONTROLLER_SPAN 4
#define VGA_CONTROLLER_END 0x0000100b


#define HW_REGS_BASE ( 0xfc000000 )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

#define ALT_LWFPGASLVS_OFST        0xff200000


static void __iomem *vga;
static dev_t first; //global variable for first device number
static struct cdev c_dev; //global variable for char device
static struct class *cl; //global variable for device class
u32 center;
unsigned int length = 0, width = 0, radius = 0, move_up = 0, move_down = 0, move_left = 0, move_right = 0;
vga_arg_t v;

static void write_to_vga (u32 val)
{
   //printk(KERN_INFO "vga driver: writting %x to %p\n", val, (vga + ((u32)(HW_REGS_BASE + ALT_LWFPGASLVS_OFST + VGA_CONTROLLER_BASE) & (u32)(HW_REGS_MASK))));
   //iowrite32((val), (vga + ((u32)(HW_REGS_BASE + ALT_LWFPGASLVS_OFST + VGA_CONTROLLER_BASE) & (u32)(HW_REGS_MASK))));
   iowrite32(val, vga);
   printk(KERN_INFO "vga driver: writting %x to %p\n", val, vga);
   
}


static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
//static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
{
   
 //  unsigned int x_start, x_end, y_start, y_end;

   printk (KERN_INFO "vga driver: ioctl()\n");

   switch (cmd)
   {
	case SHOW_FIGURE:
        if (copy_from_user(&v, (vga_arg_t *)arg, sizeof(vga_arg_t)))
          {
            printk(KERN_INFO "in cmd copy_from_user got wrong!");
            return -EACCES;
          }
printk(KERN_INFO "center x address is %x\n", v.center_x);
printk(KERN_INFO "center y address is %x\n", v.center_y);
             center = (v.center_x<<16) | (v.center_y);
printk(KERN_INFO "center address is %x\n", center);
             write_to_vga (center);
             break;

   


	default:
                return -EINVAL;
   }




return 0;

}



static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: vga driver open()\n");
  return 0;
}

static int my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: vga driver close()\n");
  return 0;
}

/*
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{  
  printk(KERN_INFO "Driver: vga read()\n");
  if (copy_to_user(buf, &v.center, len) != 0)
      return -EFAULT;

  return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  u32 type_center; 
   
  printk(KERN_INFO "VGA Driver: vga write()\n");
  if (copy_from_user(&type_center, buf ,len) != 0)
      return -EFAULT;
  else

      write_to_vga(type_center);
      return 1;
}
*/

//if (copy_from_user(&v, (vga_arg_t *)arg, sizeof(vga_arg_t)))
static struct file_operations vga_fops =
{
  .owner   = THIS_MODULE,
  .open    = my_open,
  .release = my_close,
  //.ioctl   = my_ioctl
  .unlocked_ioctl = my_ioctl
};

 
static struct of_device_id vga_of_match[] = {
	{ .compatible = "ALTR,vgactl","altr,vgactl", },
	{},
};
MODULE_DEVICE_TABLE(of, vga_of_match);

static int vga_probe(struct platform_device *op)
{
        int err;
        struct resource res;
	const struct of_device_id *match;

	match = of_match_device(vga_of_match, &op->dev);
	if (!match)
		return -EINVAL;

	err = of_address_to_resource(op->dev.of_node, 0, &res);
	if (err) {
		err = -EINVAL;
                goto err_mem;
	}
	if (!request_mem_region(res.start, resource_size(&res), "vga")) {
		err = -EBUSY;
                goto err_mem;
	}

	vga = of_iomap(op->dev.of_node, 0);
	if (!vga) {
                err = -ENOMEM;
                goto err_ioremap;
	}
	
  	printk(KERN_INFO "VGA Driver: io remapped to %x with size %x\n", res.start, resource_size(&res));
	return 0;


err_ioremap:
        release_mem_region(res.start, resource_size(&res));
err_mem:
        printk(KERN_ERR ": Failed to register vga: %d\n", err);
        return err;
}

static int vga_remove(struct platform_device *pdev)
{
        struct resource *iomem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
        iounmap(vga);
        release_mem_region(iomem->start, resource_size(iomem));

        return 0;
}

static struct platform_driver vga_driver = {
	.driver = {
		.name	= "vgactl",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(vga_of_match),
	},
	.probe		= vga_probe,
	.remove		= vga_remove,
};

static int __init vga_ctl_init(void)
{
  printk(KERN_INFO "vga Driver: registered\n");
/*
 if ((vga = ioremap(HW_REGS_BASE, HW_REGS_SPAN)) == NULL)
  {
    printk (KERN_ERR "Mapping vga failed\n");
    return -1;
  }
*/
  if (platform_driver_register(&vga_driver)< 0)
  {
    return -1;
  }

  if (alloc_chrdev_region (&first, 0,1,"mvga") < 0)
  {
    return -1;
  }
  
  if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
  {
    unregister_chrdev_region (first, 1);
    return -1;
  }
if (device_create(cl, NULL, first, NULL, "mvga") == NULL)
  //if (device_create(cl, NULL, first, "mvga") == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first,1);
    return -1;
  }

  cdev_init (&c_dev, &vga_fops);
  if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl,first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }
  return 0;
}

static void __exit vga_ctl_exit(void)
{
  cdev_del(&c_dev);
  device_destroy(cl,first);
  class_destroy(cl);
  unregister_chrdev_region(first,1);
  platform_driver_unregister(&vga_driver);
  printk(KERN_INFO "vga Driver: unregistered\n");

}

module_init(vga_ctl_init);
module_exit(vga_ctl_exit);
MODULE_LICENSE("GPL");   
  

