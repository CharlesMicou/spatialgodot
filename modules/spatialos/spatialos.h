/* spatialos.h */
#ifndef SPATIALOS_H
#define SPATIALOS_H

#include "editor_node.h"

class Spatialos : public Node {
    GDCLASS(Spatialos, Node);

    int count;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;

    Spatialos();
};

#endif