#include <stdio.h>

int main(void) {
    const int users = 500;

    printf("# P15 deterministic stress scenario\n");
    printf("# 500 users with sparse directed influence relationships\n");

    for (int i = 0; i < users; i++) {
        int targets[4] = {
            (i + 1) % users,
            (i + 7) % users,
            (i + 31) % users,
            (i + 97) % users
        };

        for (int j = 0; j < 4; j++) {
            int target = targets[j];

            double probability =
                0.20 +
                (double)(
                    (i * 17 + j * 23) % 71
                ) / 100.0;

            printf(
                "User%03d User%03d %.2f\n",
                i,
                target,
                probability
            );
        }
    }

    return 0;
}