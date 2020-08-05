// TODO: add header
#ifndef STOC_MANGLER_H
#define STOC_MANGLER_H

#include "stoc/SemanticAnalysis/Type.h"

#include <string>

namespace mangler {

std::string mangle(std::string functionName, std::shared_ptr<FunctionType> functionType);

}

#endif // STOC_MANGLER_H
