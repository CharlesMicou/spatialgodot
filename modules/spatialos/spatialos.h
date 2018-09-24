/* spatialos.h */
#ifndef SPATIALOS_H
#define SPATIALOS_H

#include "editor_node.h"

class Spatialos : public Node {
    GDCLASS(Spatialos, Node);
    int workerId;

protected:
    static void _bind_methods();

public:
    void joinGame();

    Spatialos();
};

#endif