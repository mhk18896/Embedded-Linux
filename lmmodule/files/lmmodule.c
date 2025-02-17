#include <linux/init.h>                       // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>            // Core header for loading LKMs into the kernel
#include <linux/device.h>            // Header to support the kernel Driver Model
#include <linux/kernel.h>          // Contains types, macros, functions for the kernel
#include <linux/fs.h>                 // Header for the Linux file system support
#include<linux/pci.h>             //This header is used to have the capabilities of PCI subsystem in kernel;
#include<linux/slab.h>         //This header is used for memory allocation



#define  DEVICE_NAME "lmmodule"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "LM"        ///< The device class -- this is a character device driver

 #define VID  0x1234    // Vendor ID
 #define DID 0xaaaa  // Device ID
 
MODULE_LICENSE("GPL");   ///< The license type -- this affects available functionality
MODULE_AUTHOR("Muhammad Hamza Khan");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for an emulated PCI device");  ///< The description -- see modinfo
MODULE_VERSION("0.1");    ///< A version number to inform users
 

 
struct mydev{   
int    majorNumber;    ///< Stores the device number -- determined automatically
struct class *LMcharClass ;        ///< The device-driver class struct pointer
struct device *LMcharDevice; ///< The device-driver device struct pointer
struct pci_dev *pdev;               ///<pointer to my PCI device struct
void __iomem *base_addr;   ///<pointer of my device memory region
 };
struct mydev  *my_dev; 

/*ID table for our PCI device*/
struct pci_device_id id_table[]={
   {PCI_DEVICE(VID,DID)},
   { 0 }
};
MODULE_DEVICE_TABLE(pci,id_table);
 


/// @brief  probe function for scaning the availability of our PCI device
/// @param pdev pointer to the struct PCI device managed by the kernel
/// @param id_table pointer to the id table which has Vendor and Device id of our device 
/// @return 
static int probe_pci(struct pci_dev  *pdev, const struct pci_device_id *id_table){
   int err;
   printk(KERN_INFO"PCI:DEVICE with VendorID=%x and DeviceID=%x is detected!!!\n",pdev->vendor,pdev->device);

   /*Here i am using the kzalloc for the memory allocation the memory for my struct the best part 
     about kzalloc is that i don't have to zero out all the members of my struct it will automatically
      do it for me!*/
my_dev=kzalloc(sizeof(struct mydev),GFP_KERNEL);
if(!my_dev){
   printk(KERN_ALERT"PCI: MEMORY ALLOCATION UNSUCCESSFUL FOR KERNEL MOUDLE STRUCT\n");
   return -ENOMEM;
} printk(KERN_INFO"PCI: MEMORY ALLOCATION SUCCESSFUL FOR KERNEL MODULE STRUCT\n");
   my_dev->pdev=pdev;

   //kernel API for saving the address of the struct of our device into the global struct of PCI device which is being maanged by the kernel  
 pci_set_drvdata(pdev,my_dev);

 /*enabling the PCI device so that we can perform the operations on the PCI device
 if the PCI device is disabled in the LKM then even if the device is connected with the system 
 we can not access the PCI device*/
   err=pci_enable_device(pdev);
      if(err){
             printk(KERN_ALERT"PCI:ERROR ENABLING DEVICE\n");
         }
              printk(KERN_ALERT"PCI:DEVICE ENABLED\n");


/*Requesting the mmap memory region for PCI memory mapping from the kernel */
err=pci_request_mem_regions(pdev,"my_pci_driver");
if(err){
printk(KERN_ALERT"PCI:REQUEST FOR MEMORY MAPPING UNSUCCESSFUL\n");
pci_disable_device(pdev);
return err;
}printk(KERN_INFO"PCI:REQUEST FOR MEMORY MAPPING SUCCESSFUL\n");


/*Getting a pointer to a memory map region of my device registers in to kernel virtual address space*/
my_dev->base_addr=pcim_iomap(pdev,0,pci_resource_len(pdev,0));
if(!(my_dev->base_addr)){
   pci_disable_device(pdev);
   pci_release_region(pdev,0);
   printk(KERN_ALERT"PCI:MEMORY MAPPING  UNSUCCESSFUL \n");
   return -ENOMEM;
   }printk(KERN_INFO"PCI:MEMORY MAPPING SUCCESSFUL\n");
      return 0;
}

/* PCI DEVICE remove function is NULL because our PCI device is not hotplug
#)hot plug means that the ability to remove the device while the system is runing
static void remove_pci(struct pci_dev *pdev){
}*/

//struct for my PCI driver callback functions
static struct pci_driver my_pci_driver={
   .name="pci_driver",
   .id_table=id_table,
   .probe=probe_pci,
//   .remove=remove_pci,
   };

 // The prototype functions for the character driver -- must come before the struct definition
   static int     LM_open(struct inode *, struct file *);
   static int     LM_close(struct inode *,struct file *);
   static int     LM_mmap (struct file *, struct vm_area_struct *);

 /** @brief Devices are represented as file structure in the kernel. The file_operations structure from
  *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
  *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */

 ///Struct for my fileoperation callbacks
   static struct file_operations fops={
       .owner = THIS_MODULE,	
      .open = LM_open,
      .mmap=LM_mmap,
      .release = LM_close,
   };


/// @brief Our open function implementation which associated with the kernel fileoperation callback
/// @param inodep pointer to the 
/// @param filep pointer to the filestruct created and managed by the QEMU
/// @return 
static int  LM_open(struct inode* inodep, struct file * filep) 
   {     filep->private_data=my_dev;
         printk(KERN_INFO"PCI: EXECUTING OPEN FUNCITON");
         printk(KERN_INFO"PCI: DEVICE IS OPENED\n");      
return 0;         
}    static int  LM_close (struct inode *inodep, struct file *filep){
   printk(KERN_INFO"PCI: DEVICE CLOSED\n");
   return 0;
}


 //memory mapping functionality used to map the device memory region into the userspace applicaiton virtual address space 
   static int LM_mmap (struct file *filep, struct vm_area_struct *vma)
   {  int status;
      struct mydev *my_dev=(struct mydev *)filep->private_data;
     
      vma->vm_pgoff = pci_resource_start(my_dev->pdev, 0) >> PAGE_SHIFT;
      status = io_remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end
         - vma->vm_start, vma->vm_page_prot);
         if(status) {
            printk("PCI:ERROR IN MEMORY ALLOCATION INTO USERSPCAE\n");
            return -status;
         }
         printk(KERN_INFO"PCI:MEMORY ALLOCATION IS SUCCESSFUL INTO USERSPACE\n");
         return 0;
   }

int ret;

static int __init LM_module_init(void){
printk(KERN_INFO "PCI: Initializing the  LKM\n");

 //This function is being used to start the registration of the pci device which  takes PCI operations struct as an inpute parameter
ret=pci_register_driver(&my_pci_driver);
      if(ret){
         printk(KERN_ALERT"PCI: REGISTRATION UNSUCCESSFUL\n");
         return ret;
      } printk(KERN_INFO"PCI: REGISTRATION SUCCESSFUL!\n");
  

// Try to dynamically allocate a major number for the device -- more difficult but worth it
my_dev->majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
      if (my_dev->majorNumber<0){
         printk(KERN_ALERT "PCI: MAJOR NUMBER REGISTRATION UNSUCCESSFUL\n");
         return my_dev->majorNumber;
      }printk(KERN_INFO "PCI: MAJOR NUMBER REGISTRATION SUCCESSFUL %d\n",my_dev-> majorNumber);
   
// Register the device class
my_dev->LMcharClass = class_create(CLASS_NAME);
      if (IS_ERR(my_dev->LMcharClass)){                                                                                                                  // Check for error and clean up if there is
         unregister_chrdev(my_dev->majorNumber, DEVICE_NAME);
         printk(KERN_ALERT "PCI: DEVICE CLASS REGISTRATION UNSUCCESSFUL\n");
         return PTR_ERR(my_dev->LMcharClass);                                                                                          // Correct way to return an error on a pointer
      }printk(KERN_INFO "PCI: DEVICE CLASS REGISTRATION SUCCESSFUL\n");
   
// Register the device driver
my_dev->LMcharDevice = device_create(my_dev->LMcharClass, NULL, MKDEV(my_dev->majorNumber, 0), NULL, DEVICE_NAME);
      if (IS_ERR(my_dev->LMcharDevice)){                                                             // Clean up if there is an error
         class_destroy(my_dev->LMcharClass);                                                      // Repeated code but the alternative is goto statements
         unregister_chrdev(my_dev->majorNumber, DEVICE_NAME);
         kfree(my_dev);
         printk(KERN_ALERT "PCI: DEVICE CREATION UNSUCCESSFUL\n");
         return PTR_ERR(my_dev->LMcharDevice);
      }printk(KERN_INFO "PCI: DEVICE CREATION SUCCESSFUL\n"); // Made it! device was initialized
      return 0;
   }
 
//module exit function
      static void __exit LM_module_exit(void){
     printk(KERN_INFO"PCI: MODULE EXIT PROCESS INITIATED\n");
     
      pci_unregister_driver(&my_pci_driver);
      printk(KERN_INFO"PCI: DEVICE UNREGISTERED\n");
      device_destroy(my_dev->LMcharClass, MKDEV(my_dev->majorNumber, 0));  // remove the device
      printk(KERN_INFO"PCI: DEVICE DESTROYED\n");
      class_unregister(my_dev->LMcharClass);                                                       // unregister the device class
      printk(KERN_INFO"PCI: DEVICE CLASS UNREGISTERED\n");  
      class_destroy(my_dev->LMcharClass);                                                         // remove the device class
      printk(KERN_INFO"PCI: DEVICE CLASS DESTROYED\n");
      unregister_chrdev(my_dev->majorNumber, DEVICE_NAME);           // unregister the major number
      printk(KERN_INFO"PCI: MAJOR NUMBER UNREGISTERED\n");
      kfree(my_dev);                                                                                                 /*freeing up the allocated memeory for struct*/
      printk(KERN_INFO "PCI: DEVICE STRUCT MEMORY HAS BEEN RELEASED\n");
      printk(KERN_INFO"GOODBYE !");
      }

module_init(LM_module_init);
module_exit(LM_module_exit);