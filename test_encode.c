#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include <unistd.h>

OperationType check_operation_type(char *argv[])
{
    
    if(strcmp(argv[1],"-e")==0)
    {
       return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
int main(int argc,char *argv[])
{
    uint img_size;
    EncodeInfo encInfo;
    DecodeInfo dncInfo;
    if(check_operation_type(argv)==e_encode)
    {
        printf("\n***************************\n");
        char *str="     SELECTED ENCODING";
        for(int i=0;str[i]!='\0';i++)
        {
            printf("%c",str[i]);
            fflush(stdout);
            usleep(90000);
        }
        printf("\n***************************\n");
        if(argc>=4)
        {
            if(read_and_validate_encode_args(argv,&encInfo)==e_success)
            {
                if(do_encoding(&encInfo)==e_success)
                {
                    printf("\n***************************\n");
                    char *str="     ENCODING COMPLETED";
                    for(int i=0;str[i]!='\0';i++)
                    {
                        printf("%c",str[i]);
                        fflush(stdout);
                        usleep(90000);
                    }
                    printf("\n***************************\n");
                    return 0;
                }
            }
        }

    }
    else if(check_operation_type(argv)==e_decode)
    {
        printf("\n***************************\n");
        char *str="     SELECTED DECODING";
        for(int i=0;str[i]!='\0';i++)
        {
            printf("%c",str[i]);
            fflush(stdout);
            usleep(90000);
        }
        printf("\n***************************\n");
        if(argc>=3)
        {
            if(read_and_validate_decode_args(argv,&dncInfo)==e_success)
            {
                if(do_decoding(&dncInfo)==e_success)
                {
                    printf("\n**************************\n");
                    char *str="    DECODING COMPLETED";
                    for(int i=0;str[i]!='\0';i++)
                    {
                        printf("%c",str[i]);
                        fflush(stdout);
                        usleep(90000);
                    }
                    printf("\n**************************\n");
                    return 0;
                }
            }
        }
    }
    else
    {
        printf("Error\n");
    }
    return 0;
}
