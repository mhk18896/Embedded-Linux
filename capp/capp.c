#include<stdio.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<stdint.h>

//device path representing our device 
#define DEVICE "/dev/lmmodule"
#define SIZE 128

/// @brief prototypes of our function's
/// @param ptr pointers pointing to the diffrent regions of our device memory reigons
void kernel_sent(uint8_t *ptr);
void image_data_sent(uint8_t *ptr);
void result_func(uint8_t *ptr);


int main(){
    
            
      //Opening our device file for getting a file descriptor
      int fd=open(DEVICE,O_RDWR);
      if(fd<0)
      {   perror("USER:error in file opening\n");
        return -1;
    }
    
    
//getting a DMA(Direct Memory Access) of our device memory region into our userspace virtual address space
    void *ptr=mmap(NULL,SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if (ptr==MAP_FAILED){
        perror("USER:MEMORY MAPPING UNSUCCESFUL\n");
        close(fd);
        return -1;
    }
    printf("USER:MEMORY MAPPING SUCCESFUL\n");
    
    /*type casting the void pointer into the data type that should be pointed*/
    uint8_t *mm=(uint8_t *)ptr;

//Adding the offset to the base address to get the pointers to the diffrent regions of the memory
    uint8_t *kernel= mm;
    uint8_t *data=mm+9;
    uint8_t *result=mm+73;
    uint8_t *csr=mm+109;
    
    //kernel function sending the data to our hardware through DMA(Direct Memory Access)
    kernel_sent(kernel);
    //Image data function sending the data tou our hardware through DMA(DIrect Memory Access)
    image_data_sent(data);
    //updating the CSR register
    *csr=1;

    //Through polling I/O interface we are checking the CSR register
    while(*csr!=2){
        printf("Not yet we will come later\n");
        sleep(10);
    }


    printf("Reading the results\n");
    //If the CSR data is updated accordingly then we will get our result
    result_func(result);
    
    
    /*memory unmapping procedure where we have to unmap the memory*/
    printf("memory unampping\n");
    munmap(mm,SIZE);
    close(fd);
    return 0;
}


void kernel_sent(uint8_t *ptr)
{
    int i,j,index;
    uint8_t kern[3][3]={
        {1,1,1},
        {1,1,1},
        {1,1,1}
    };
    printf("Sending the kernel data\n");
        for(i=0;i<3;i++)
        {
            for(j=0;j<3;j++)
                {
                    index=0;
                    index=i*3+j;
                    ptr[index]=kern[i][j];
                    printf("%d\t",kern[i][j]);
                }
            printf("\n");
        }
            printf("\n");
}


void image_data_sent(uint8_t *ptr)
{
        int i,j;
        int index;
        uint8_t image[8][8]={
                        {1,1,1,1,1,1,1,1},
                        {1,4,4,4,4,4,5,1},
                        {1,8,7,6,6,6,6,1},
                        {1,3,2,1,4,5,3,1},
                        {1,3,7,2,6,2,3,1},
                        {1,2,1,3,3,5,4,1},
                        {11,10,9,8,5,5,6,6},
                        {11,12,13,12,4,2,1,1}                    
                    };

printf("Sendind the image data\n");
    for(i=0;i<8;i++)
    {
            for(j=0;j<8;j++)
                    {
                            index=0;
                            index=i*8+j;
                            ptr[index]=image[i][j];
                            printf("%d\t",image[i][j]);
                    }
                        printf("\n");
    }
                        printf("\n");
                        
}

void result_func(uint8_t *ptr)
{
        int i,j,index;

    for(i=0;i<6;i++)
    {
           for(j=0;j<6;j++)
        {
                index=0;
                index=i*6+j;
                printf("%d\t",ptr[index]);
                    
        }
                printf("\n");
    }
                printf("\n");            
}