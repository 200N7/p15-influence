#ifndef INFLUENCE_H
#define INFLUENCE_H

#include "graph.h"

int most_likely_path(
    const struct Graph *g,
    const char *source,
    const char *target
);

void rank_influencers(const struct Graph *g);

#endif