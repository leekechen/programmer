#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "firmware.h"


static struct option cspack_options[] = {
    {"tlv",         no_argument,        0,  't'},  
    {"ic_list",     required_argument,  0,  'i'},    
    {"addr",        required_argument,  0,  'a'},
    {"hv",          required_argument,  0,  'd'},
    {"fv",          required_argument,  0,  'f'},
    {"bv",          required_argument,  0,  'b'},
    {"pv",          required_argument,  0,  'p'},
    {"help",        no_argument,        0,  'h'}
} ;

static void usage_show(const char* cmd)
{
    printf("%s", "NAME\n");
    printf("\t %s - pack the binary file to specific format\n", cmd);

    printf("%s", "SYNOPSIS\n");
    printf("%s", "\t %s [OPTION].. [SRC_FILE] [DST_FILE]\n");

    printf("%s", "DESCRIPTION\n");
    printf("%s", "\t pack the source binary file(SRC_FILE) to destination file(DST_FILE)\n");
    printf("%s", "\t destination file support these types:\n");    
    printf("%s", "\t\t tlv format, the destination file is TLV format, which packed the source file\n");
    
    printf("%s", "\t -t, --tlv\n");    
    printf("%s", "\t\t package the source binary file(SRC_FILE) to tlv format.\n");

    printf("%s", "\t -i, --ic_type=[ic_name]\n");
    printf("%s", "\t\t the ic's name which firmware supported to program\n");

    printf("%s", "\t -a, --addr=address\n");
    printf("%s", "\t\t the hex address of firmware which programed\n");
    
    printf("%s", "\t -d, --hv=version\n");
    printf("%s", "\t\t the hardware version of CSWrite3.0 \n");
    
    printf("%s", "\t -f, --fv=version\n");
    printf("%s", "\t\t the firmware version of CSWrite3.0 \n");
    
    printf("%s", "\t -f, --bv=version\n");
    printf("%s", "\t\t the bootloader version of CSWrite3.0 \n");
    
    printf("%s", "\t -f, --pv=version\n");
    printf("%s", "\t\t the PC software version of CSWrite3.0\n");

    printf("%s", "\t -h, --help\n");
    printf("%s", "\t\t show this page!\n");
}

extern int cspack_tlv(struct firmware_info* info);


struct argument {
    uint32_t    type;
    char*       src;
    char*       dst;
    char*       chip_name;
    char*       addr;
    char*       version[MAX_VERSION_NUM];
} ;

static uint32_t version_str2uint32(char* str)
{
    uint32_t version;
    uint8_t  ver[3];
    char*    tmp;
    char*    saveptr;
    int32_t i;

    if (str == NULL) {
        printf("%s(),%d, str==NULL\n", __func__, __LINE__);
        return 0;
    }

    for (i = 0; i < 3; i++) {
        tmp = strtok_r(str, ".", &saveptr);
        if (tmp == NULL) {
            break;
        }
        str = NULL;
        ver[i] = strtol(tmp, NULL, 16);
    }
    version = (ver[0] << 16) + (ver[1] << 8) + ver[2];

    return version;
}

int parse_arg(struct argument* args, struct firmware_info* info)
{
    int32_t i;
    char* token;
    char* delim;
    char* save;
    char* tmp;
    
    delim = ",";
    token = delim;
    if (args->addr == NULL || args->src == NULL || args->dst == NULL || args->chip_name == NULL) { 
        usage_show("cspack");
    exit(0);
    }

    info->addr = strtol(args->addr, NULL, 16);
    info->src  = args->src;
    info->dst  = args->dst;

    for (i = 0, tmp = args->chip_name; token != NULL; ) {
        token = strtok_r(tmp, delim, &save);
        if (token) {
            info->chip_name[i] = strndup(token, 100);
            tmp = NULL;
            i++;
        }
    }
    info->chip_num = i;

    for (i = 0; i < 4; i++) {
        info->version[i] = version_str2uint32(args->version[i]);
    }
    
    cspack_tlv(info);
    
    return 0;
}

int main(int argc, char* argv[]) 
{
    int                     c;
    struct argument         args;
    struct firmware_info    info;

    memset(&info, 0, sizeof(struct firmware_info));
    memset(&args, 0, sizeof(struct argument));
    
    if (argc == 1) {
        usage_show(argv[0]);
        return 0;
    }
    
    while (1) {
        int option_index = 0;
        
        c = getopt_long(argc, argv, "rta:idfbp", cspack_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (cspack_options[option_index].flag != 0) {
                break;
            }
            printf ("option %s", cspack_options[option_index].name);
            if (optarg) {
                printf (" with arg %s", optarg);
            }
            printf ("\n");        
            break;
            
        case '?':
            break;    
        
        case 't':
            args.type = 't';
            break;
            
        case 'a':
            args.addr = strndup(optarg, 100);      
            break;
            
        case 'i':    
            args.chip_name = strndup(optarg, 100);
            break;
        case 'd':
            args.version[0] = strndup(optarg, 100);
            break;
        case 'f':
            args.version[1] = strndup(optarg, 100);
            break;
        case 'b':
            args.version[2] = strndup(optarg, 100);
            break;       
        case 'p':
            args.version[3] = strndup(optarg, 100);
            break;            
        default:
            usage_show(argv[0]);
            exit(0);
        }
    }

    if (optind < argc) {
        args.src = strndup(argv[optind++], 100);
    }

    if (optind < argc) {
        args.dst = strndup(argv[optind++], 100);
    }

    if (args.type == 't') {
        parse_arg(&args, &info);
    }

    if (args.src != NULL) {
        free(args.src);
    }

    if (args.dst != NULL) {
        free(args.dst);
    }

    if (args.addr != NULL) {
        free(args.addr);
    }

    if (args.chip_name != NULL) {
        free(args.chip_name);
    }

    return 0;
}

