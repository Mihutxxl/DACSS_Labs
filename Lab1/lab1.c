#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lab1Library.h"
#include "lab1Library.c"

#define MAX_REVIEWS 100
#define MAX_LENGTH 256

int main() {
    Review reviews[MAX_REVIEWS] = {
        {"John", "Laptop", "ok", "PICTURE"},
        {"Mary", "Phone", "@#$%)", "IMAGE"},
        {"Peter", "Phone", "GREAT", "ManyPictures"},
        {"Ann", "Book", "So GOOD", "Image"},
		{"Dan", "Toy", "Excellenthttp", "Photo"}
    };
    int review_count = 5;

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
        printf("%s, %s, %s, %s\n", reviews[i].username, reviews[i].productname, reviews[i].reviewtext, reviews[i].attachment);
    }
    
    //Blackboard configuration
    Blackboard bb = {
        .reviews = {
            {"John", "Laptop", "ok", "PICTURE"},
            {"Mary", "Phone", "@#$%)", "IMAGE"},
            {"Peter", "Phone", "GREAT", "ManyPictures"},
            {"Ann", "Book", "So GOOD", "Image"},
            {"Dan", "Toy", "Excellenthttp", "Photo"}
        },
        .count = 5,
        .processed = {0} 
    };

    process_blackboard(&bb);

    // Print results
    printf("\nBlackboard Processed Reviews:\n");
    for (int i = 0; i < bb.count; i++) {
        if (!bb.processed[i]) {
            printf("%s, %s, %s, %s\n", bb.reviews[i].username, bb.reviews[i].productname, bb.reviews[i].reviewtext, bb.reviews[i].attachment);
        }
    }

    return 0;
}