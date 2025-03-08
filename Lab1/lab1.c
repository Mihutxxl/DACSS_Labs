#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lab1Library.h"
#include "lab1Library.c"

#define MAX_REVIEWS 100
#define MAX_LENGTH 256

// Function to read reviews from a file
int load_reviews(const char *filename, Review reviews[], int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    *count = 0;
    while (*count < MAX_REVIEWS &&
           fscanf(file, "%[^,], %[^,], %[^,], %[^\n]\n", 
                  reviews[*count].username, 
                  reviews[*count].productname, 
                  reviews[*count].reviewtext, 
                  reviews[*count].attachment) == 4) {
        (*count)++;
    }

    fclose(file);
    return 1;
}

int main() {
    Review reviews[MAX_REVIEWS];
    int review_count = 0;

    // Load reviews from file
    if (!load_reviews("reviews.txt", reviews, &review_count)) {
        return 1;  // Exit if file reading fails
    }

    // Pipes-and-Filters configuration
    int (*pipeline1[])(Review *, int *) = {
        filter_non_buyers,
        filter_propaganda,
        filter_profanities,
        remove_competition_links,
        transform_resize_pictures,
        transform_analyze_sentiment
    };

    process_reviews(reviews, &review_count, pipeline1, 6);

    // Print results
    for (int i = 0; i < review_count; i++) {
        printf("%s, %s, %s, %s\n", 
               reviews[i].username, 
               reviews[i].productname, 
               reviews[i].reviewtext, 
               reviews[i].attachment);
    }

    // Blackboard configuration
    Blackboard bb;
    memcpy(bb.reviews, reviews, review_count * sizeof(Review));
    bb.count = review_count;
    memset(bb.processed, 0, sizeof(bb.processed));

    process_blackboard(&bb);

    // Print results
    printf("\nBlackboard Processed Reviews:\n");
    for (int i = 0; i < bb.count; i++) {
        if (!bb.processed[i]) {
            printf("%s, %s, %s, %s\n", 
                   bb.reviews[i].username, 
                   bb.reviews[i].productname, 
                   bb.reviews[i].reviewtext, 
                   bb.reviews[i].attachment);
        }
    }

    return 0;
}
