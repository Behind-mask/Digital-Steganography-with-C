#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <unistd.h>

/* Function Definitions */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char arr[2];
    FILE *fptr=fopen(argv[2],"r");
    fread(arr,2,1,fptr);
    if(strcmp(arr,"BM")==0)
    {
        encInfo -> src_image_fname=argv[2];
    }
    else
    {
        return e_failure;
    }
    encInfo -> secret_fname=argv[3];
    if(strcmp(strstr(argv[3],"."),".txt")==0)
    {
        strcpy(encInfo -> extn_secret_file,".txt");
    }
    else if(strcmp(strstr(argv[3],"."),".c")==0)
    {
        strcpy(encInfo -> extn_secret_file,".c");
    }
    else if(strcmp(strstr(argv[3],"."),".exe")==0)
    {
        strcpy(encInfo -> extn_secret_file,".exe");
    }
    else
    {
        printf("File type not found!");
        return e_failure;
    }
    encInfo->extn_size=strlen(encInfo->extn_secret_file);
    if(argv[4]!=NULL)
    {
        if(strcmp(strstr(argv[4],"."),".bmp")==0)
        {
            encInfo -> stego_image_fname=argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        encInfo -> stego_image_fname = "stego_image.bmp";
    }

    return e_success;
}


    /* Get image size
    * Input: Image file ptr
    * Output: width * height * bytes per pixel (3 in our case)
    * Description: In BMP Image, width is stored in offset 18,
    * and height after that. size is 4 bytes
    */
    uint get_image_size_for_bmp(FILE *fptr_image)
    {
        uint width, height;
        // Seek to 18th byte
        fseek(fptr_image, 18, SEEK_SET);
        printf("DIMENSIONS OF THE IMAGE: ");
        // Read the width (an int)
        fread(&width, sizeof(int), 1, fptr_image);
        printf("Width = %u, ", width);

        // Read the height (an int)
        fread(&height, sizeof(int), 1, fptr_image);
        printf("Height = %u\n", height);

        // Return image capacity
        return width * height * 3;
    }   


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("STATUS:All files are opened successfully!\n");
        if(check_capacity(encInfo)==e_success)
        {
            usleep(90000);
            printf("STATUS:Check Capacity Success!\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
            {
                usleep(90000);
                printf("STATUS:Header copied successfully to the output file!\n");
                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
                {
                    usleep(90000);
                    printf("STATUS:Encoded MAGIC STRING successfully!\n");
                    if(encode_size(encInfo->extn_size,encInfo)==e_success)
                    {
                        usleep(90000);
                        printf("STATUS:Encoded the size of extention successfully!\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                        {
                            usleep(90000);
                            printf("STATUS:Encoded secret file extention successfully!\n");
                            if(encode_size(encInfo->size_secret_file,encInfo)==e_success)
                            {
                                usleep(90000);
                                printf("STATUS:Encoded secret file size successfully!\n");
                                if(encode_secret_file_data(encInfo)==e_success)
                                {
                                    usleep(90000);
                                    printf("STATUS:Encoded secret file data successfully!\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
                                    {
                                        usleep(90000);
                                        printf("STATUS:Remaining data copied to the output file successfully!\n");
                                        return e_success;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
else
        {
            printf("STATUS:Error:Unable to open files\n");
        }
    }
}


Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    printf("SIZE:Secret file: %ld\n",encInfo->size_secret_file);
    int size=(54+(strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+(encInfo->size_secret_file))*8);
    int image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("SIZE:Image file: %d\n",image_capacity);
    if(image_capacity>=size)
    {
        return e_success;
    }
    else
        return e_failure;
}


uint get_file_size(FILE *fptr)
{
    int file_size;
    fseek(fptr,0,SEEK_END);
    file_size = ftell(fptr);
    fseek(fptr,0,SEEK_SET);
    return file_size;
}


Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    rewind(fptr_src_image);
    rewind(fptr_stego_image);
    char buffer[54];
    fread(buffer,sizeof(char),54,fptr_src_image);
    fwrite(buffer,sizeof(char),54,fptr_stego_image);
    return e_success;
}


Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo -> fptr_src_image,encInfo -> fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
        return e_failure;
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
    if(encode_data_to_image(encInfo -> extn_secret_file,strlen(encInfo->extn_secret_file),encInfo -> fptr_src_image,encInfo -> fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        fprintf(stderr,"ERROR: Encoding secret file extension failed!\n");
        return e_failure;
    }
}


Status encode_size(long file_size,EncodeInfo *encInfo)

{
    char image_buffer[32];
    fread(image_buffer,1,32,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=((image_buffer[i]&(~1)) | ((file_size&(1<<i))>>i));
    }
    fwrite(image_buffer,1,32,encInfo->fptr_stego_image);
    return e_success;
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_data[encInfo->size_secret_file+1];
    rewind(encInfo->fptr_secret);
    fread(secret_data,1,encInfo -> size_secret_file,encInfo -> fptr_secret);
    if(encode_data_to_image(secret_data,encInfo -> size_secret_file,encInfo -> fptr_src_image,encInfo -> fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        fprintf(stderr,"ERROR: Encoding secret file data failed!\n");
        return e_failure;
    }
}


Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char image[8];
    for(int i=0;i<size;i++)
    {
        fread(image,1,8,fptr_src_image);
        encode_byte_to_lsb(data[i],image);
        fwrite(image,1,8,fptr_stego_image);
    }
    return e_success;
}
   


Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=((image_buffer[i]&(~1)) | ((data&(1<<i))>>i));
    }
        return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
     char c;
     while(fread(&c, 1, 1, fptr_src) >0)
     {
         fwrite(&c,1,1,fptr_dest);
     }
     return e_success;
}








