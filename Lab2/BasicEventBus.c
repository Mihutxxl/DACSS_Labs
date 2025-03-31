#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SUBSCRIBERS 100
#define MAX_EVENT_TYPES 20
#define MAX_SENSORS 50
#define MAX_DISPLAYS 50
#define MAX_TYPE_LENGTH 50
#define MAX_ID_LENGTH 100
#define MAX_DATA_LENGTH 256

// Event structure
typedef struct Event {
    char type[MAX_TYPE_LENGTH];
    void *data;
    char sourceId[MAX_ID_LENGTH]; // ID of the publisher that generated the event
} Event;

// Subscriber interface
typedef void (*EventHandler)(Event *);

typedef struct Subscriber {
    char id[MAX_ID_LENGTH];
    char eventTypes[MAX_EVENT_TYPES][MAX_TYPE_LENGTH]; // Array of event types the subscriber is interested in
    int eventTypeCount;
    EventHandler handler;
} Subscriber;

// EventBus structure
typedef struct EventBus {
    Subscriber subscribers[MAX_SUBSCRIBERS];
    int subscriberCount;
} EventBus;

// Global EventBus instance
EventBus eventBus;

// Initialize the EventBus
void initEventBus() {
    eventBus.subscriberCount = 0;
    srand(time(NULL)); // Initialize random number generator for sensor simulation
}

// Register a subscriber for an event type
void subscribe(char *subscriberId, char *eventType, EventHandler handler) {
    int i;
    
    // Check if the subscriber already exists
    for (i = 0; i < eventBus.subscriberCount; i++) {
        if (strcmp(eventBus.subscribers[i].id, subscriberId) == 0) {
            // Subscriber exists, add the event type if not already subscribed
            int j;
            for (j = 0; j < eventBus.subscribers[i].eventTypeCount; j++) {
                if (strcmp(eventBus.subscribers[i].eventTypes[j], eventType) == 0) {
                    printf("Subscriber %s already subscribed to %s\n", subscriberId, eventType);
                    return;
                }
            }
            
            // Add the new event type
            if (eventBus.subscribers[i].eventTypeCount < MAX_EVENT_TYPES) {
                strcpy(eventBus.subscribers[i].eventTypes[eventBus.subscribers[i].eventTypeCount], eventType);
                eventBus.subscribers[i].eventTypeCount++;
                printf("Subscriber %s subscribed to additional event type: %s\n", subscriberId, eventType);
            } else {
                printf("Max event types reached for subscriber %s\n", subscriberId);
            }
            return;
        }
    }
    
    // New subscriber
    if (eventBus.subscriberCount >= MAX_SUBSCRIBERS) {
        printf("Max subscribers reached!\n");
        return;
    }
    
    strcpy(eventBus.subscribers[eventBus.subscriberCount].id, subscriberId);
    strcpy(eventBus.subscribers[eventBus.subscriberCount].eventTypes[0], eventType);
    eventBus.subscribers[eventBus.subscriberCount].eventTypeCount = 1;
    eventBus.subscribers[eventBus.subscriberCount].handler = handler;
    eventBus.subscriberCount++;
    printf("New subscriber %s registered for event type: %s\n", subscriberId, eventType);
}

// Unsubscribe from a specific event type
void unsubscribe(char *subscriberId, char *eventType) {
    for (int i = 0; i < eventBus.subscriberCount; i++) {
        if (strcmp(eventBus.subscribers[i].id, subscriberId) == 0) {
            // Found the subscriber, now find and remove the event type
            for (int j = 0; j < eventBus.subscribers[i].eventTypeCount; j++) {
                if (strcmp(eventBus.subscribers[i].eventTypes[j], eventType) == 0) {
                    // Remove this event type by shifting the remaining types
                    for (int k = j; k < eventBus.subscribers[i].eventTypeCount - 1; k++) {
                        strcpy(eventBus.subscribers[i].eventTypes[k], eventBus.subscribers[i].eventTypes[k + 1]);
                    }
                    eventBus.subscribers[i].eventTypeCount--;
                    printf("Subscriber %s unsubscribed from event type: %s\n", subscriberId, eventType);
                    return;
                }
            }
            printf("Subscriber %s was not subscribed to event type: %s\n", subscriberId, eventType);
            return;
        }
    }
    printf("Subscriber %s not found\n", subscriberId);
}

// Publish an event
void publish(char *eventType, void *data, char *sourceId) {
    Event event;
    strcpy(event.type, eventType);
    event.data = data;
    strcpy(event.sourceId, sourceId);
    
    printf("Publishing event type: %s from source: %s\n", eventType, sourceId);
    
    for (int i = 0; i < eventBus.subscriberCount; i++) {
        for (int j = 0; j < eventBus.subscribers[i].eventTypeCount; j++) {
            if (strcmp(eventBus.subscribers[i].eventTypes[j], eventType) == 0) {
                eventBus.subscribers[i].handler(&event);
                break; // Found a match, no need to check other event types for this subscriber
            }
        }
    }
}

//
// Sensor simulation
//

// Generate random sensor data
float generateSensorData(char *sensorType) {
    float value;
    
    if (strcmp(sensorType, "Temperature") == 0) {
        value = 15.0 + ((float)rand() / RAND_MAX) * 25.0; // 15-40°C
    } else if (strcmp(sensorType, "WaterLevel") == 0) {
        value = ((float)rand() / RAND_MAX) * 10.0; // 0-10m
    } else if (strcmp(sensorType, "Humidity") == 0) {
        value = 30.0 + ((float)rand() / RAND_MAX) * 70.0; // 30-100%
    } else {
        value = ((float)rand() / RAND_MAX) * 100.0; // Generic 0-100
    }
    
    return value;
}

// Simulate sensor reading
void simulateSensorReading(char *sensorType, char *sensorId) {
    float *data = malloc(sizeof(float));
    *data = generateSensorData(sensorType);
    publish(sensorType, data, sensorId);
}

//
// Display handlers
//

// Numeric display handler
void numericDisplayHandler(Event *event) {
    float value = *(float*)event->data;
    printf("[NumericDisplay] Value from %s: %.2f\n", event->sourceId, value);
    free(event->data); // Free the allocated data
}

// Max value display handler
void maxValueDisplayHandler(Event *event) {
    static float maxValues[MAX_EVENT_TYPES];
    static char maxValueTypes[MAX_EVENT_TYPES][MAX_TYPE_LENGTH];
    static int maxValueCount = 0;
    
    float value = *(float*)event->data;
    int i;
    int found = 0;
    
    // Check if we have seen this event type before
    for (i = 0; i < maxValueCount; i++) {
        if (strcmp(maxValueTypes[i], event->type) == 0) {
            found = 1;
            if (value > maxValues[i]) {
                maxValues[i] = value;
                printf("[MaxValueDisplay] New max value for %s: %.2f from %s\n", event->type, value, event->sourceId);
            } else {
                printf("[MaxValueDisplay] Value for %s: %.2f from %s (max: %.2f)\n", event->type, value, event->sourceId, maxValues[i]);
            }
            break;
        }
    }
    
    // If this is a new event type, add it
    if (!found && maxValueCount < MAX_EVENT_TYPES) {
        strcpy(maxValueTypes[maxValueCount], event->type);
        maxValues[maxValueCount] = value;
        printf("[MaxValueDisplay] First value for %s: %.2f from %s\n", event->type, value, event->sourceId);
        maxValueCount++;
    }
}

// Text display handler
void textDisplayHandler(Event *event) {
    float value = *(float*)event->data;
    printf("[TextDisplay] Reading from %s - %s: ", event->sourceId, event->type);
    
    if (strcmp(event->type, "Temperature") == 0) {
        printf("%.2f°C ", value);
        if (value < 18) printf("(Cold)");
        else if (value > 30) printf("(Hot)");
        else printf("(Normal)");
    } else if (strcmp(event->type, "Humidity") == 0) {
        printf("%.2f%% ", value);
        if (value < 40) printf("(Dry)");
        else if (value > 70) printf("(Humid)");
        else printf("(Normal)");
    } else if (strcmp(event->type, "WaterLevel") == 0) {
        printf("%.2fm ", value);
        if (value < 2) printf("(Low)");
        else if (value > 8) printf("(High)");
        else printf("(Normal)");
    } else {
        printf("%.2f", value);
    }
    
    printf("\n");
}

//
// News agency simulation
//

// News subscriber handler
void newsSubscriberHandler(Event *event) {
    char *news = (char*)event->data;
    printf("[NewsSubscriber] Received news on %s from %s: %s\n", event->type, event->sourceId, news);
}

// Publish news
void publishNews(char *agency, char *domain, char *news) {
    char *newsData = malloc(MAX_DATA_LENGTH);
    strcpy(newsData, news);
    publish(domain, newsData, agency);
}

// Main function
int main() {
    initEventBus();
    
    // Register display subscribers
    subscribe("NumericDisplay1", "Temperature", numericDisplayHandler);
    subscribe("NumericDisplay1", "Humidity", numericDisplayHandler);
    subscribe("MaxValueDisplay1", "Temperature", maxValueDisplayHandler);
    subscribe("MaxValueDisplay1", "WaterLevel", maxValueDisplayHandler);
    subscribe("TextDisplay1", "Temperature", textDisplayHandler);
    subscribe("TextDisplay1", "WaterLevel", textDisplayHandler);
    subscribe("TextDisplay1", "Humidity", textDisplayHandler);
    
    // Register news subscribers
    subscribe("Person1", "Sports", newsSubscriberHandler);
    subscribe("Person1", "Politics", newsSubscriberHandler);
    subscribe("Person2", "Culture", newsSubscriberHandler);
    subscribe("Person2", "Sports", newsSubscriberHandler);
    
    // Simulate sensor readings
    printf("\n--- Simulating Sensor Readings ---\n");
    simulateSensorReading("Temperature", "TemperatureSensorTimisoara");
    simulateSensorReading("Temperature", "TemperatureSensorArad");
    simulateSensorReading("WaterLevel", "WaterLevelSensorTimisoara");
    simulateSensorReading("Humidity", "HumiditySensorArad");
    
    // Simulate news publishing
    printf("\n--- Simulating News Publishing ---\n");
    publishNews("NewsAgency1", "Sports", "Local team wins championship");
    publishNews("NewsAgency2", "Politics", "New government policies announced");
    publishNews("NewsAgency1", "Culture", "New museum opening next month");
    
    // Demonstrate unsubscribe
    printf("\n--- Demonstrating Unsubscribe ---\n");
    unsubscribe("Person1", "Politics");
    publishNews("NewsAgency3", "Politics", "International summit begins");
    
    // Demonstrate subscribing to a new event type
    printf("\n--- Demonstrating New Subscription ---\n");
    subscribe("Person1", "Culture", newsSubscriberHandler);
    publishNews("NewsAgency1", "Culture", "Famous artist exhibition opens today");
    
    // Clean up any allocated memory
    // In a real application, we would free all allocated resources here
    
    return 0;
}