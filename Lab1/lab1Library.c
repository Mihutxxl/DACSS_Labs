#include "lab1Library.h"

//Buyers
const char *buyers[][2] = {
    {"John", "Laptop"},
    {"Mary", "Phone"},
    {"Ann", "Book"},
    {NULL, NULL} // End marker
};

int is_buyer(const char *username, const char *productname) {
    for (int i = 0; buyers[i][0] != NULL; i++) {
        if (strcmp(buyers[i][0], username) == 0 && strcmp(buyers[i][1], productname) == 0) {
            return 1;
        }
    }
    return 0;
}

int contains_profanity(const char *text) {
    return strstr(text, "@#$%") != NULL;
}

int contains_political_propaganda(const char *text) {
    return strstr(text, "+++") != NULL || strstr(text, "---") != NULL;
}

void resize_picture(char *attachment) {
    for (int i = 0; attachment[i] != '\0'; i++) {
        attachment[i] = tolower(attachment[i]);
    }
}

void remove_competitor_links(char *text) {
    char *pos;
    while ((pos = strstr(text, "http")) != NULL) {
        memmove(pos, pos + 4, strlen(pos + 4) + 1);
    }
}

void analyze_sentiment(char *text) {
    int upper = 0, lower = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (isupper(text[i])) upper++;
        if (islower(text[i])) lower++;
    }
    strcat(text, (upper > lower) ? "+" : (lower > upper) ? "-" : "=");
}

void process_reviews(Review reviews[], int *count, int (*filters[])(Review *, int *), int num_filters) {
    for (int i = 0; i < num_filters; i++) {
        filters[i](reviews, count);
    }
}

int filter_non_buyers(Review *reviews, int *count) {
    int j = 0;
    for (int i = 0; i < *count; i++) {
        if (is_buyer(reviews[i].username, reviews[i].productname)) {
            reviews[j++] = reviews[i];
        }
    }
    *count = j;
    return 0;
}

int filter_profanities(Review *reviews, int *count) {
    int j = 0;
    for (int i = 0; i < *count; i++) {
        if (!contains_profanity(reviews[i].reviewtext)) {
            reviews[j++] = reviews[i];
        }
    }
    *count = j;
    return 0;
}

int transform_resize_pictures(Review *reviews, int *count) {
    for (int i = 0; i < *count; i++) {
        resize_picture(reviews[i].attachment);
    }
    return 0;
}

int transform_analyze_sentiment(Review *reviews, int *count) {
    for (int i = 0; i < *count; i++) {
        analyze_sentiment(reviews[i].reviewtext);
    }
    return 0;
}
