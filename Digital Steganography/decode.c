#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <unistd.h>

Status read_and_validate_decode_args(char *argv[],DecodeInfo *dncInfo)
{
    if(strcmp(strstr(argv[2],"."),".bmp")==0)
    {
        dncInfo ->src_image_fname=argv[2];
    }
    else
        return e_failure;
    if(argv[3]!=NULL)
    {
        dncInfo -> secret_fname=argv[3];
    }
    else
    {
        dncInfo ->secret_fname = "secret_message";
    }
    return e_success;
}

Status do_decoding(DecodeInfo *dncInfo)
{
    if(open_stego_file(dncInfo)==e_success)
    {
        printf("STATUS:File Opened successfully\n");
        fseek(dncInfo->fptr_stego_image,54,SEEK_SET);
        if(decode_magic_string(MAGIC_STRING,dncInfo)==e_success)
        {
            usleep(90000);
            printf("STATUS:Magic string decoded successfully\n");
            if(get_file_extn_size(dncInfo)==e_success)
            {
                usleep(90000);
                printf("STATUS:Output file extension size decoded successfully\n");
                if(get_file_extn(dncInfo)==e_success)
                {
                    usleep(90000);
                    printf("STATUS:Output file extension decoded successfully\n");
                    if(concat_and_open(dncInfo)==e_success)
                    {
                        usleep(90000);
                        printf("STATUS:Output file opened in write mode\n");
                        if(get_secret_data_size(dncInfo)==e_success)
                        {
                            usleep(90000);
                            printf("STATUS:Decoded the secret message size successfully\n");
                            if(get_secret_data(dncInfo)==e_success)
                            {
                                usleep(90000);
                                printf("STATUS:Decoded the secret message into the output file successfully\n");
                                usleep(90000);
                                printf("YOUR OUTPUT FILE NAME IS %s",dncInfo->op_fname);
                                return e_success;
                           }
                        }
                    }
                }
            }
        }
        else
        {
            return e_failure;
        }    
    }
}

Status open_stego_file(DecodeInfo *dncInfo)
{
    dncInfo->fptr_stego_image = fopen(dncInfo->src_image_fname,"r");
    if(dncInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",dncInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string,DecodeInfo *dncInfo)
{
    int size=strlen(MAGIC_STRING);
    if(get_image_data(size,dncInfo ->fptr_stego_image,dncInfo->image_data)==e_success)
    {   
        dncInfo->image_data[size]='\0';
        if(strcmp(MAGIC_STRING,dncInfo->image_data)==0)
        {
            printf("STATUS:FILE IS ENCODED\n");
            return e_success;
        }
       else
        {
            printf("STATUS:FILE IS NOT ENCODED\n");
            return e_failure;
        }
    }
}

Status get_file_extn_size(DecodeInfo *dncInfo)
{
    char image_buffer[32];
    fread(image_buffer,1,32,dncInfo->fptr_stego_image);
    dncInfo->size_secretf_extn = decode_byte_lsb_size(image_buffer);
    return e_success;
}

Status get_file_extn(DecodeInfo *dncInfo)
{
    if(get_image_data(dncInfo->size_secretf_extn,dncInfo->fptr_stego_image,dncInfo ->image_data)==e_success)
    {
        dncInfo->image_data[dncInfo->size_secretf_extn]='\0';
        strcpy(dncInfo->extn_secret_file,dncInfo->image_data);
        return e_success;
    }
    else
        return e_failure;
}

Status concat_and_open(DecodeInfo *dncInfo)
{
    strcpy(dncInfo->op_fname,dncInfo->secret_fname);
    strcat(dncInfo->op_fname,dncInfo->extn_secret_file);
    dncInfo-> fptr_secret = fopen(dncInfo->op_fname,"w");
    if(dncInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",dncInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}


Status get_secret_data_size(DecodeInfo *dncInfo)
{
    char image_buffer[32];
    fread(image_buffer,1,32,dncInfo->fptr_stego_image);
    dncInfo->size_secret_file = decode_byte_lsb_size(image_buffer);
    return e_success;
}


Status get_secret_data(DecodeInfo *dncInfo)
{
    char buffer[dncInfo -> size_secret_file+1];
    get_image_data(dncInfo->size_secret_file,dncInfo -> fptr_stego_image,buffer);
    fwrite(buffer,1,dncInfo->size_secret_file,dncInfo -> fptr_secret);
    return e_success;
}

Status get_image_data(int size,FILE *fptr_stego_image,char *decode_data)
{
    int i;
    char image_buffer[8];
    for(i=0;i<size;i++)
    {
        fread(image_buffer,1,8,fptr_stego_image);
        decode_data[i]=decode_byte_lsb(image_buffer);
    }
    decode_data[i]='\0';
    return e_success;
}

Status decode_byte_lsb(const char *image_buffer)
{
    char ch=0;
    for(int i=0;i<8;i++)
    {
        ch=((image_buffer[i] & 1)<<i)|ch;
    }
    return ch;
}

Status decode_byte_lsb_size(const char *image_buffer)
{
    int ch=0;
    for(int i=0;i<32;i++)
    {
        ch=((image_buffer[i] & 1)<<i)|ch;
    }
    return ch;
}

