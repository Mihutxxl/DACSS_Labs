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
        {"Ann", "Book", "So GOOD", "Image"}
    };
    int review_count = 4;

    // Pipes-and-Filters configuration
    int (*pipeline1[])(Review *, int *) = {
        filter_non_buyers,
        filter_profanities,
        transform_resize_pictures,
        transform_analyze_sentiment
    };

    process_reviews(reviews, &review_count, pipeline1, 4);

    // Print results
    for (int i = 0; i < review_count; i++) {
        printf("%s, %s, %s, %s\n", reviews[i].username, reviews[i].productname, reviews[i].reviewtext, reviews[i].attachment);
    }
    return 0;
}