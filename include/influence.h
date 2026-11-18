#ifndef INFLUENCE_H
#define INFLUENCE_H

#include "graph.h"

int most_likely_path(
    const struct Graph *g,
    const char *source,
    const char *target
);

#endif