#ifndef _ASM_H
#define _ASM_H

#include <string>

/**
 * @brief Directives Context
 * 
**/
typedef struct {
    std::string data;
    std::string text;
    std::string global;
} Asm;

/* Asm Output File (Global) */
extern Asm assemblyFile;

#endif