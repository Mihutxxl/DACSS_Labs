#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SUBSCRIBERS 100
#define MAX_EVENT_TYPES 20
#define MAX_SENSORS 50
#define MAX_DISPLAYS 50
#define MAX_NEWS_AGENCIES 20
#define MAX_PEOPLE 50
#define MAX_TYPE_LENGTH 50
#define MAX_ID_LENGTH 100
#define MAX_DATA_LENGTH 256
#define MAX_NEWS_LENGTH 512
#define MAX_DOMAIN_LENGTH 50

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

// News structure
typedef struct News {
    char domain[MAX_DOMAIN_LENGTH];
    char content[MAX_NEWS_LENGTH];
    char agency[MAX_ID_LENGTH];
    time_t timestamp;
} News;

// News Agency structure
typedef struct NewsAgency {
    char id[MAX_ID_LENGTH];
    char domains[MAX_EVENT_TYPES][MAX_DOMAIN_LENGTH]; // Domains the agency publishes news on
    int domainCount;
} NewsAgency;

// Person structure
typedef struct Person {
    char id[MAX_ID_LENGTH];
    char interestedDomains[MAX_EVENT_TYPES][MAX_DOMAIN_LENGTH]; // Domains the person is interested in
    int domainCount;
} Person;

// Global variables
EventBus eventBus;
NewsAgency newsAgencies[MAX_NEWS_AGENCIES];
int newsAgencyCount = 0;
Person people[MAX_PEOPLE];
int peopleCount = 0;

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

// Unsubscribe from an event type
void unsubscribe(char *subscriberId, char *eventType) {
    for (int i = 0; i < eventBus.subscriberCount; i++) {
        if (strcmp(eventBus.subscribers[i].id, subscriberId) == 0) {
            for (int j = 0; j < eventBus.subscribers[i].eventTypeCount; j++) {
                if (strcmp(eventBus.subscribers[i].eventTypes[j], eventType) == 0) {
                    // Remove this event type by shifting the remaining ones
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
                break;
            }
        }
    }
}

// Register a news agency
int registerNewsAgency(char *agencyId) {
    if (newsAgencyCount >= MAX_NEWS_AGENCIES) {
        printf("Max news agencies reached!\n");
        return -1;
    }
    
    strcpy(newsAgencies[newsAgencyCount].id, agencyId);
    newsAgencies[newsAgencyCount].domainCount = 0;
    
    printf("News agency %s registered\n", agencyId);
    return newsAgencyCount++;
}

// Add a domain to a news agency
void addDomainToAgency(int agencyIndex, char *domain) {
    if (agencyIndex < 0 || agencyIndex >= newsAgencyCount) {
        printf("Invalid agency index!\n");
        return;
    }
    
    for (int i = 0; i < newsAgencies[agencyIndex].domainCount; i++) {
        if (strcmp(newsAgencies[agencyIndex].domains[i], domain) == 0) {
            printf("Agency %s already publishes on domain: %s\n", 
                   newsAgencies[agencyIndex].id, domain);
            return;
        }
    }
    
    if (newsAgencies[agencyIndex].domainCount >= MAX_EVENT_TYPES) {
        printf("Max domains reached for agency %s\n", newsAgencies[agencyIndex].id);
        return;
    }
    
    strcpy(newsAgencies[agencyIndex].domains[newsAgencies[agencyIndex].domainCount], domain);
    newsAgencies[agencyIndex].domainCount++;
    printf("Domain %s added to agency %s\n", domain, newsAgencies[agencyIndex].id);
}

// Register a person
int registerPerson(char *personId) {
    if (peopleCount >= MAX_PEOPLE) {
        printf("Max people reached!\n");
        return -1;
    }
    
    strcpy(people[peopleCount].id, personId);
    people[peopleCount].domainCount = 0;
    
    printf("Person %s registered\n", personId);
    return peopleCount++;
}

void personNewsHandler(Event *event) {
    News *news = (News *)event->data;
    
    // Find the person from the subscriber ID (format: "Person_personId")
    char personId[MAX_ID_LENGTH];
    sscanf(event->sourceId, "Person_%s", personId);
    
    printf("[News Reception] %s received news in domain %s from %s: %s\n", 
           personId, news->domain, news->agency, news->content);
}

// Person subscribes to a domain
void personSubscribeToDomain(int personIndex, char *domain) {
    if (personIndex < 0 || personIndex >= peopleCount) {
        printf("Invalid person index!\n");
        return;
    }
    
    for (int i = 0; i < people[personIndex].domainCount; i++) {
        if (strcmp(people[personIndex].interestedDomains[i], domain) == 0) {
            printf("Person %s already subscribed to domain: %s\n", 
                   people[personIndex].id, domain);
            return;
        }
    }
    
    if (people[personIndex].domainCount >= MAX_EVENT_TYPES) {
        printf("Max domains reached for person %s\n", people[personIndex].id);
        return;
    }
    
    strcpy(people[personIndex].interestedDomains[people[personIndex].domainCount], domain);
    people[personIndex].domainCount++;
    
    // Also subscribe this person to the EventBus for this domain
    char subscriberId[MAX_ID_LENGTH];
    sprintf(subscriberId, "Person_%s", people[personIndex].id);
    subscribe(subscriberId, domain, personNewsHandler);
    
    printf("Person %s subscribed to domain: %s\n", people[personIndex].id, domain);
}

// Person unsubscribes from a domain
void personUnsubscribeFromDomain(int personIndex, char *domain) {
    if (personIndex < 0 || personIndex >= peopleCount) {
        printf("Invalid person index!\n");
        return;
    }
    
    int domainFound = -1;
    for (int i = 0; i < people[personIndex].domainCount; i++) {
        if (strcmp(people[personIndex].interestedDomains[i], domain) == 0) {
            domainFound = i;
            break;
        }
    }
    
    if (domainFound == -1) {
        printf("Person %s not subscribed to domain: %s\n", people[personIndex].id, domain);
        return;
    }
    
    // Remove this domain by shifting the remaining ones
    for (int i = domainFound; i < people[personIndex].domainCount - 1; i++) {
        strcpy(people[personIndex].interestedDomains[i], people[personIndex].interestedDomains[i + 1]);
    }
    people[personIndex].domainCount--;
    
    // Also unsubscribe this person from the EventBus for this domain
    char subscriberId[MAX_ID_LENGTH];
    sprintf(subscriberId, "Person_%s", people[personIndex].id);
    unsubscribe(subscriberId, domain);
    
    printf("Person %s unsubscribed from domain: %s\n", people[personIndex].id, domain);
}

// Handle news events for people


// Publish news from an agency
void publishNews(int agencyIndex, char *domain, char *content) {
    if (agencyIndex < 0 || agencyIndex >= newsAgencyCount) {
        printf("Invalid agency index!\n");
        return;
    }
    
    // Check if agency publishes in this domain
    int domainFound = 0;
    for (int i = 0; i < newsAgencies[agencyIndex].domainCount; i++) {
        if (strcmp(newsAgencies[agencyIndex].domains[i], domain) == 0) {
            domainFound = 1;
            break;
        }
    }
    
    if (!domainFound) {
        printf("Agency %s does not publish in domain: %s\n", 
               newsAgencies[agencyIndex].id, domain);
        return;
    }
    
    // Create news
    News *news = malloc(sizeof(News));
    strcpy(news->domain, domain);
    strcpy(news->content, content);
    strcpy(news->agency, newsAgencies[agencyIndex].id);
    news->timestamp = time(NULL);
    
    // Publish event
    publish(domain, news, newsAgencies[agencyIndex].id);
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

    printf("[MaxValueDisplay] Received %s: %.2f from %s\n", event->type, *value, event->sourceId);

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
    simulateSensorReading("WaterLevel", "WaterLevelSensorArad");
    simulateSensorReading("Humidity", "HumiditySensorTimisoara");
    simulateSensorReading("Humidity", "HumiditySensorArad");
    
    // Create news agencies
    printf("\n--- Setting up News Agencies ---\n");
    int bbcIndex = registerNewsAgency("BBC");
    addDomainToAgency(bbcIndex, "Politics");
    addDomainToAgency(bbcIndex, "Sports");
    addDomainToAgency(bbcIndex, "Culture");
    
    int cnnIndex = registerNewsAgency("CNN");
    addDomainToAgency(cnnIndex, "Politics");
    addDomainToAgency(cnnIndex, "Business");
    
    int espnIndex = registerNewsAgency("ESPN");
    addDomainToAgency(espnIndex, "Sports");
    
    // Register people
    printf("\n--- Registering People ---\n");
    int aliceIndex = registerPerson("Alice");
    personSubscribeToDomain(aliceIndex, "Politics");
    personSubscribeToDomain(aliceIndex, "Business");
    
    int bobIndex = registerPerson("Bob");
    personSubscribeToDomain(bobIndex, "Sports");
    
    int charlieIndex = registerPerson("Charlie");
    personSubscribeToDomain(charlieIndex, "Politics");
    personSubscribeToDomain(charlieIndex, "Culture");
    personSubscribeToDomain(charlieIndex, "Sports");
    
    // Publish some news
    printf("\n--- Publishing News ---\n");
    publishNews(bbcIndex, "Politics", "New election results announced today");
    publishNews(cnnIndex, "Business", "Stock market reaches all-time high");
    publishNews(espnIndex, "Sports", "Local team wins championship");
    publishNews(bbcIndex, "Culture", "New museum exhibition opens next week");
    
    // Charlie changes subscriptions
    printf("\n--- Updating Subscriptions ---\n");
    personUnsubscribeFromDomain(charlieIndex, "Sports");
    personSubscribeToDomain(charlieIndex, "Business");
    
    // Publish more news
    printf("\n--- Publishing More News ---\n");
    publishNews(bbcIndex, "Sports", "Tennis tournament final results");
    publishNews(cnnIndex, "Business", "New economic forecast released");
    
    return 0;
}