/* spatialos.cpp */

#include "spatialos.h"

void Spatialos::add(int value) {
    count += value;
}

void Spatialos::reset() {
    count = 0;
}

int Spatialos::get_total() const {
    return count;
}

void Spatialos::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add", "value"), &Spatialos::add);
    ClassDB::bind_method(D_METHOD("reset"), &Spatialos::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Spatialos::get_total);
}

Spatialos::Spatialos() {
    count = 0;
}

