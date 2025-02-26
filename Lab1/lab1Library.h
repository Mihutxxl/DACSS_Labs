#ifndef LAB1LIBRARY_H
#define LAB1LIBRARY_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 256

typedef struct {
    char username[MAX_LENGTH];
    char productname[MAX_LENGTH];
    char reviewtext[MAX_LENGTH];
    char attachment[MAX_LENGTH];
} Review;

int is_buyer(const char *username, const char *productname);
int contains_profanity(const char *text);
int contains_political_propaganda(const char *text);
void resize_picture(char *attachment);
void remove_competitor_links(char *text);
void analyze_sentiment(char *text);
void process_reviews(Review reviews[], int *count, int (*filters[])(Review *, int *), int num_filters);
int filter_non_buyers(Review *reviews, int *count);
int filter_profanities(Review *reviews, int *count);
int transform_resize_pictures(Review *reviews, int *count);
int transform_analyze_sentiment(Review *reviews, int *count);

#endif // LAB1LIBRARY_H