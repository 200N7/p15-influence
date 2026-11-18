#ifndef INFLUENCE_H
#define INFLUENCE_H

#include "graph.h"

int most_likely_path(
    const struct Graph *g,
    const char *source,
    const char *target
);

void rank_influencers(const struct Graph *g);

void detect_communities(
    const struct Graph *g,
    int k
);

void plan_dissemination(
    const struct Graph *g,
    int budget,
    const int costs[]
);
void analyze_audience_capacity(
    const struct Graph *g,
    const char *source,
    const char *target
);
void plan_campaign_assignments(
    const struct Graph *g,
    int campaign_count
);
void analyze_post_pattern(
    const char *text,
    const char *pattern
);
void analyze_text_compression(
    const char *text
);

#endif