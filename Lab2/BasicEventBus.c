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
                break;
            }
        }
    }
}

// Sensor simulation
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

void simulateSensorReading(char *sensorType, char *sensorId) {
    float *data = malloc(sizeof(float));
    *data = generateSensorData(sensorType);
    publish(sensorType, data, sensorId);
}

void numericDisplayHandler(Event *event) {
    float *value = (float *)event->data;
    printf("[NumericDisplay] Value from %s: %.2f\n", event->sourceId, *value);
}

void maxValueDisplayHandler(Event *event) {
    static float maxTemp = -999.9, maxHumidity = -999.9, maxWater = -999.9;
    float *value = (float *)event->data;

    if (strcmp(event->type, "Temperature") == 0 && *value > maxTemp) {
        maxTemp = *value;
        printf("[MaxValueDisplay] New max temperature: %.2f from %s\n", maxTemp, event->sourceId);
    } else if (strcmp(event->type, "Humidity") == 0 && *value > maxHumidity) {
        maxHumidity = *value;
        printf("[MaxValueDisplay] New max humidity: %.2f from %s\n", maxHumidity, event->sourceId);
    } else if (strcmp(event->type, "WaterLevel") == 0 && *value > maxWater) {
        maxWater = *value;
        printf("[MaxValueDisplay] New max water level: %.2f from %s\n", maxWater, event->sourceId);
    }
}

void textDisplayHandler(Event *event) {
    float *value = (float *)event->data;
    printf("[TextDisplay] %s reported a %s value of %.2f\n", event->sourceId, event->type, *value);
}

// Main function
int main() {
    initEventBus();
    
    // Register display subscribers
    subscribe("NumericDisplay1", "Temperature", numericDisplayHandler);
    subscribe("NumericDisplay1", "Humidity", numericDisplayHandler);
    subscribe("NumericDisplay1", "WaterLevel", numericDisplayHandler);
    subscribe("MaxValueDisplay1", "Temperature", maxValueDisplayHandler);
    subscribe("MaxValueDisplay1", "WaterLevel", maxValueDisplayHandler);
    subscribe("MaxValueDisplay1", "Humidity", maxValueDisplayHandler);
    subscribe("TextDisplay1", "Temperature", textDisplayHandler);
    subscribe("TextDisplay1", "WaterLevel", textDisplayHandler);
    subscribe("TextDisplay1", "Humidity", textDisplayHandler);
    
    // Simulate sensor readings
    printf("\n--- Simulating Sensor Readings ---\n");
    simulateSensorReading("Temperature", "TemperatureSensorTimisoara");
    simulateSensorReading("Temperature", "TemperatureSensorArad");
    simulateSensorReading("WaterLevel", "WaterLevelSensorTimisoara");
    simulateSensorReading("Humidity", "HumiditySensorArad");
    
    return 0;
}
