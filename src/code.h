#ifndef _CODE_H
#define _CODE_H

#include <string>
#include <sstream>
#include <set>
#include "type.h"

/**
 * @brief Each Expression Will Generate Its Own Code Context
 * 
**/
typedef struct {
    Type type;
    std::string code;
    std::string registerAt; // Maybe call it savedAt
} Code;

#endif