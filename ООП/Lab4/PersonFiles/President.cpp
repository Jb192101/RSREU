#include "President.h"

std::string President::getPosition() const {
    return "President";
}

std::string President::getDescription() const {
    return getPosition();
}