#ifndef _KM4_ARGUMENT_HEADER__
#define _KM4_ARGUMENT_HEADER__

struct km4_arguments
{
    int args_argc;
    char** args_argv;
    unsigned int nArgvAlloc;
    unsigned int nArgvUsed;

    char** args_envp;
    unsigned int nEnvpAlloc;
    unsigned int nEnvpUsed;
};
typedef struct km4_arguments km4_arguments;

#endif //_KM4_ARGUMENT_HEADER__
