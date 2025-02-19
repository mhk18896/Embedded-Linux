#include "qemu/osdep.h"
#include "qemu/units.h"
#include "hw/pci/pci.h"
#include "hw/hw.h"
#include "hw/pci/msi.h"
#include "qom/object.h"
#include "qemu/main-loop.h"
#include "qemu/module.h"
#include "qapi/visitor.h"
#include "exec/memory.h"




/*This will be used as a device name while using in the QEMU device script*/
#define TYPE_PCI_DEV_DEVICE "pci-mat-dev"

//DEBUG_INFO macro is for debugging purposes!
#define DEBUG_INFO 0



/// @brief using typedef so that i don't have to use the "struct" again and again
typedef struct Pci_Mat_Dev Pci_Mat_Dev;

DECLARE_INSTANCE_CHECKER(Pci_Mat_Dev,PCI_MAT_DEV,TYPE_PCI_DEV_DEVICE);

/// @brief struct of my custom device model in QEMU.Contains all the infromation regarding my device functionality
struct Pci_Mat_Dev{
PCIDevice pdev;                                  ///instance of the QEMU PCI device struct
MemoryRegion iomem_bar0;       ///instance of QEMU memory region struct
uint8_t  bar0[128];                         ///my device BAR memory resion of 128bytes in size
uint8_t *kernel;                             ///pointer to the kernel memory regions in my BAR memory reigon array
uint8_t *data;                               ///pointer to the input matrix data memory region in my BAR memory region
uint8_t *result;                          ///pointer to the  result matrix data in my BAR memory region 
uint8_t *csr;                              ///pointer to my CSR register in the BAR memory region


QEMUBH *bh;                        /*pointer to the bottom halve deffered execution mechanisim of the QEMU which is pointing towards our convoloution function!
                                
#)The idea behind the QEMUBH is that some process requires time and we can not block all other operations such as timers,interupts etc for those
    functions to be full filled first.So using the QEMUBH mechanism we schedule those operations(in our case convoloution fucntion) and don't block
    the execution flow!
#)QEMBH is an opaque struct so we have only API's that will help us to point towards our functions and register that into the main-loop mechanism
*/
};


/// @brief we are implementing the convoloution algorithim for image processing.
/// @param pmd this is  the pointer to my device struct.
static void pci_conv(Pci_Mat_Dev *pmd){
uint8_t buffer=0;
uint8_t kernel_width=3;
uint8_t input_width=8;
uint8_t i,j;
uint8_t HEIGHT=input_width-kernel_width+1;
uint8_t WIDTH=input_width-kernel_width+1;
for(i=0;i<HEIGHT;i++)
    { 
        for(j=0;j<WIDTH;j++)
        { 
 //emptying the buffer for  the calculation for the new values
  buffer=0;
           for(int ki=0;ki<kernel_width;ki++) 
                  {
                    for(int kj=0;kj<kernel_width;kj++)
                        {
                            uint8_t image_index=(i+ki)*input_width+(j+kj);
                            uint8_t kernel_index=  ki*kernel_width+kj;
                            buffer+=pmd->data[image_index]   *  pmd->kernel[kernel_index];                                                                          
                                                                                                                        }
                                                                                                                    }
                        pmd->result[i*WIDTH+j]=buffer/9;
                                                                                                                }
    }
   /*Updating the CSR register after completing the calculations to Result Calculated and stored so that our usersapce application can start reading the data*/
pmd->csr[0]=2;
}


/// @brief This is the read function that we are implementing for reading from the registers of our device 
/// @param opaque pointer to our read function
/// @param addr address to address particular byte in our BAR
/// @param size size that we have to read form the BAR memory region
/// @return
static uint64_t pcidev_bar0_mmio_read(void *opaque, hwaddr addr, unsigned size){

/*we are pointer type casting so that we can access our device struct members*/
Pci_Mat_Dev  *pmd=PCI_MAT_DEV(opaque);

if(DEBUG_INFO){
    printf("Reading value: %d from address: %ld\n",pmd->bar0[addr],addr);   
    return pmd->bar0[addr];
}
else if(pmd->csr[0]==1){
    printf("conv function trigered\n");
        qemu_bh_schedule(pmd->bh);
}
else if(pmd->csr[0]==2){
    return pmd->bar0[addr];
}
return 0;
}



/// @brief This the write function that we are implementing to write to the registers of our device
/// @param oapque pointer to our read function
/// @param addr offset that my QEMU device will be recieving from the userspace application
/// @param data the data that QEMU device will be recieving form the user space application
/// @param size size of byte that we would be writing 
static void pcidev_bar0_mmio_write(void *oapque,hwaddr addr,uint64_t data,unsigned size){
    Pci_Mat_Dev *pmd=PCI_MAT_DEV(oapque);
    
    //The macro DEBUG_INFO is used when we have to debug that our write function is working or not! 
    if(DEBUG_INFO){
        printf("writing the value: %d at addr: %ld \n",(uint8_t)data,addr);
    }   
    
        pmd->bar0[addr]=(uint8_t)data;
    return;
}


/// @brief this is the memory regions struct of QEMU that we have to populate with  our logic so that our 
///device registers could be memory mapped by the QEMU 
static const MemoryRegionOps pcidev_bar0_mmio_ops = {
    .read = pcidev_bar0_mmio_read,
	.write = pcidev_bar0_mmio_write,
	.endianness = DEVICE_NATIVE_ENDIAN,
	.valid = {
		.min_access_size = 1,
		.max_access_size = 1,
	},
	.impl = {
		.min_access_size = 1,
		.max_access_size = 1,
	},

};

/// @brief this is the realize funciton that will be instantiated by the QEMU fo creating the instance of our device
/// @param pdev  pointer to the PCIDevice struct managed by the QEMU internal sybsystem
/// @param errp    error pointer if the funciton tries to 
static void pcidev_realize(PCIDevice *pdev, Error **errp)
{
	Pci_Mat_Dev *pmd=PCI_MAT_DEV(pdev);

	memset(pmd->bar0,0,sizeof(pmd->bar0));//setting the device memory regions to the initial value of zero so that there should not be any garbage values inside our device registers

pmd->bh= qemu_bh_new((QEMUBHFunc *)pci_conv,pmd);//we registering the custom convolution fucntion with the QEMUBH deffered execution system

	memory_region_init_io(&pmd->iomem_bar0,OBJECT(pmd),&pcidev_bar0_mmio_ops,pmd,"pcimatdev-mmio",sizeof(pmd->bar0));//initializing the memory region of my device registers
	pci_register_bar(pdev,0,PCI_BASE_ADDRESS_SPACE_MEMORY,&pmd->iomem_bar0);//now mapping those memory region into the QEMU PCI devices designated address space

}
/// @brief This  is uninitialize function whose main task is when we are going 
/// @param pdev pointer to the PCI device struct
static void pcidev_uninit(PCIDevice *pdev){
    
    Pci_Mat_Dev *pmd=PCI_MAT_DEV(pdev);
       
    if(pmd->bh){
        qemu_bh_delete(pmd->bh);
        pmd->bh=NULL;
    }
return ;
}

/// @brief The main task of this function is to setup the configuration of the PCI device instance
/// @param obj Generic pointer to the object struct managed by the QEMU! 
static void pcidev_instance_init(Object *obj){
Pci_Mat_Dev *pmd=PCI_MAT_DEV(obj);//we are type casting here the generic object pointer to our device struct
     pmd->kernel=pmd->bar0;
     pmd->data=pmd->bar0+9;
     pmd->result=pmd->bar0+73;
     pmd->csr=pmd->bar0+109;

}

/// @brief the main task of this function is to initialize the new class which we have develop for our device
/// @param  
/// @param data 
static void pcidev_class_init(ObjectClass *class,void *data){
DeviceClass *dc=DEVICE_CLASS(class);                                //here we are typecasting the ObjectClass pointer to the device class pointer because the DeviceClass is inherited by the PCIDevice class  
PCIDeviceClass *k=PCI_DEVICE_CLASS(class);                  //here we are typecasting the ObjectClass pointer to the PCIDeviceClass struct
k->realize=pcidev_realize;
k->exit=pcidev_uninit;
k->vendor_id=PCI_VENDOR_ID_QEMU;
k->device_id=0xaaaa;
k->revision=0x01;
k->class_id=PCI_CLASS_OTHERS;

set_bit(DEVICE_CATEGORY_MISC,dc->categories);
}

static void pcidev_register_types(void){

static InterfaceInfo interfaces[]={
	{INTERFACE_CONVENTIONAL_PCI_DEVICE},
	{},
};

static const TypeInfo pci_dev_info={
.name=TYPE_PCI_DEV_DEVICE,
.parent=TYPE_PCI_DEVICE,
.instance_size=sizeof(Pci_Mat_Dev),
.instance_init=pcidev_instance_init,
.class_init=pcidev_class_init,
.interfaces=interfaces,
};

type_register_static(&pci_dev_info);
}
type_init(pcidev_register_types);
/*






*/