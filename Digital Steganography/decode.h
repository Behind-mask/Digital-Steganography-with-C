#ifndef DECODE_H
#define DECODE_H
#include "types.h"


   /* Structure to store information required for decoding secret message from the encoded image */
 

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /*Source file info*/
    char *src_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /*Decoded file data*/
    char *secret_fname;
    char op_fname[30];
    FILE *fptr_secret;
    int size_secretf_extn;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;

}DecodeInfo;

/*Decoding function proptotype*/

 /*Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *dncInfo);

 /*Perform the encoding*/
Status do_decoding(DecodeInfo *dncInfo);

 /*Get File pointers for i/p and o/p files8*/
Status open_stego_file(DecodeInfo *dncInfo);

Status decode_magic_string(const char *magic_string,DecodeInfo *dncInfo);

Status get_file_extn_size(DecodeInfo *dncInfo);

Status get_file_extn(DecodeInfo *dncInfo);

Status concat_and_open(DecodeInfo *dncInfo);

Status get_secret_data_size(DecodeInfo *dncInfo);

Status get_secret_data(DecodeInfo *dncInfo);

Status get_image_data(int size,FILE *fptr_stego_image,char *image_data);

Status decode_byte_lsb(const char *image_buffer);

Status decode_byte_lsb_size(const char *image_buffer);

#endif

