/**
 * Publisher-Subscriber System Implementation
 * 
 * This code implements an event-driven architecture using the publisher-subscriber pattern.
 * It has two main applications:
 * 1. Sensor data distribution to various display types
 * 2. News distribution from agencies to interested people
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 
 /* Maximum capacity constants */
 #define MAX_SUBSCRIBERS 100   // Maximum number of subscribers in the system
 #define MAX_EVENT_TYPES 20    // Maximum number of event types a single subscriber can register for
 #define MAX_SENSORS 50        // Maximum number of sensors in the system
 #define MAX_DISPLAYS 50       // Maximum number of displays in the system
 #define MAX_NEWS_AGENCIES 20  // Maximum number of news agencies in the system
 #define MAX_PEOPLE 50         // Maximum number of people in the system
 
 /* String length constants */
 #define MAX_TYPE_LENGTH 50    // Maximum length of event type string
 #define MAX_ID_LENGTH 100     // Maximum length of ID strings
 #define MAX_DATA_LENGTH 256   // Maximum length of generic data strings
 #define MAX_NEWS_LENGTH 512   // Maximum length of news content
 #define MAX_DOMAIN_LENGTH 50  // Maximum length of news domain name
 
 /**
  * Event structure - Core data structure for the pub-sub system
  * Contains the type of event, the actual data, and the source ID
  */
 typedef struct Event {
     char type[MAX_TYPE_LENGTH];     // Type of the event (e.g., "Temperature", "Sports")
     void *data;                     // Pointer to the actual data (can be any type)
     char sourceId[MAX_ID_LENGTH];   // ID of the publisher that generated the event
 } Event;
 
 /**
  * Event handler function pointer type definition
  * Functions of this type will process events when they are received
  */
 typedef void (*EventHandler)(Event *);
 
 /**
  * Subscriber structure - Represents an entity that can receive events
  * Contains subscriber ID, list of event types they're interested in, and handler function
  */
 typedef struct Subscriber {
     char id[MAX_ID_LENGTH];                           // Unique identifier for the subscriber
     char eventTypes[MAX_EVENT_TYPES][MAX_TYPE_LENGTH]; // Array of event types this subscriber listens for
     int eventTypeCount;                               // Number of event types currently registered
     EventHandler handler;                             // Function to call when matching event is received
 } Subscriber;
 
 /**
  * EventBus structure - Central hub for managing subscribers and event distribution
  */
 typedef struct EventBus {
     Subscriber subscribers[MAX_SUBSCRIBERS];  // Array of all subscribers in the system
     int subscriberCount;                      // Number of registered subscribers
 } EventBus;
 
 /**
  * News structure - Contains news content and metadata
  */
 typedef struct News {
     char domain[MAX_DOMAIN_LENGTH];    // Domain/category of the news (e.g., "Politics", "Sports")
     char content[MAX_NEWS_LENGTH];     // Actual news content
     char agency[MAX_ID_LENGTH];        // ID of the news agency that published it
     time_t timestamp;                  // When the news was published
 } News;
 
 /**
  * NewsAgency structure - Represents a publisher of news
  */
 typedef struct NewsAgency {
     char id[MAX_ID_LENGTH];                          // Unique identifier for the agency
     char domains[MAX_EVENT_TYPES][MAX_DOMAIN_LENGTH]; // Domains this agency can publish in
     int domainCount;                                 // Number of domains the agency covers
 } NewsAgency;
 
 /**
  * Person structure - Represents a consumer of news
  */
 typedef struct Person {
     char id[MAX_ID_LENGTH];                                // Unique identifier for the person
     char interestedDomains[MAX_EVENT_TYPES][MAX_DOMAIN_LENGTH]; // News domains the person is interested in
     int domainCount;                                       // Number of domains they follow
 } Person;
 
 /* Global state variables */
 EventBus eventBus;                        // Central event bus for the entire system
 NewsAgency newsAgencies[MAX_NEWS_AGENCIES]; // Array of all news agencies
 int newsAgencyCount = 0;                  // Number of registered news agencies
 Person people[MAX_PEOPLE];                // Array of all people
 int peopleCount = 0;                      // Number of registered people
 
 /**
  * Initialize the EventBus and random number generator for sensor simulation
  */
 void initEventBus() {
     eventBus.subscriberCount = 0;
     srand(time(NULL)); // Initialize random number generator for sensor simulation
 }
 
 /**
  * Register a subscriber for an event type
  * If the subscriber already exists, adds the new event type to their interests
  * 
  * @param subscriberId - Unique ID for the subscriber
  * @param eventType - Event type to subscribe to
  * @param handler - Function to handle the event when received
  */
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
 
 /**
  * Unsubscribe from an event type
  * Removes the specified event type from a subscriber's interests
  * 
  * @param subscriberId - ID of the subscriber
  * @param eventType - Event type to unsubscribe from
  */
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
 
 /**
  * Publish an event to all interested subscribers
  * Notifies all subscribers that are registered for the given event type
  * 
  * @param eventType - Type of event being published
  * @param data - Pointer to the event data
  * @param sourceId - ID of the publisher
  */
 void publish(char *eventType, void *data, char *sourceId) {
     Event event;
     strcpy(event.type, eventType);
     event.data = data;
     strcpy(event.sourceId, sourceId);
     
     printf("Publishing event type: %s from source: %s\n", eventType, sourceId);
     
     // Notify all subscribers who are interested in this event type
     for (int i = 0; i < eventBus.subscriberCount; i++) {
         for (int j = 0; j < eventBus.subscribers[i].eventTypeCount; j++) {
             if (strcmp(eventBus.subscribers[i].eventTypes[j], eventType) == 0) {
                 eventBus.subscribers[i].handler(&event);
                 break;
             }
         }
     }
 }
 
 /**
  * Register a new news agency in the system
  * 
  * @param agencyId - Unique ID for the agency
  * @return - Index of the newly registered agency or -1 if failed
  */
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
 
 /**
  * Add a news domain to a news agency's list of covered domains
  * 
  * @param agencyIndex - Index of the agency in the newsAgencies array
  * @param domain - Domain name to add
  */
 void addDomainToAgency(int agencyIndex, char *domain) {
     if (agencyIndex < 0 || agencyIndex >= newsAgencyCount) {
         printf("Invalid agency index!\n");
         return;
     }
     
     // Check if domain already exists for this agency
     for (int i = 0; i < newsAgencies[agencyIndex].domainCount; i++) {
         if (strcmp(newsAgencies[agencyIndex].domains[i], domain) == 0) {
             printf("Agency %s already publishes on domain: %s\n", 
                    newsAgencies[agencyIndex].id, domain);
             return;
         }
     }
     
     // Add new domain if limit not reached
     if (newsAgencies[agencyIndex].domainCount >= MAX_EVENT_TYPES) {
         printf("Max domains reached for agency %s\n", newsAgencies[agencyIndex].id);
         return;
     }
     
     strcpy(newsAgencies[agencyIndex].domains[newsAgencies[agencyIndex].domainCount], domain);
     newsAgencies[agencyIndex].domainCount++;
     printf("Domain %s added to agency %s\n", domain, newsAgencies[agencyIndex].id);
 }
 
 /**
  * Register a new person in the system
  * 
  * @param personId - Unique ID for the person
  * @return - Index of the newly registered person or -1 if failed
  */
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
 
 /**
  * Event handler for news events received by people
  * Processes news events and displays appropriate messages
  * 
  * @param event - The news event that was received
  */
 void personNewsHandler(Event *event) {
     News *news = (News *)event->data;
     
     // Extract the person ID from the subscriber ID (format: "Person_personId")
     char personId[MAX_ID_LENGTH];
     sscanf(event->sourceId, "Person_%s", personId);
     
     printf("[News Reception] %s received news in domain %s from %s: %s\n", 
            personId, news->domain, news->agency, news->content);
 }
 
 /**
  * Subscribe a person to a specific news domain
  * 
  * @param personIndex - Index of the person in the people array
  * @param domain - News domain to subscribe to
  */
 void personSubscribeToDomain(int personIndex, char *domain) {
     if (personIndex < 0 || personIndex >= peopleCount) {
         printf("Invalid person index!\n");
         return;
     }
     
     // Check if already subscribed to this domain
     for (int i = 0; i < people[personIndex].domainCount; i++) {
         if (strcmp(people[personIndex].interestedDomains[i], domain) == 0) {
             printf("Person %s already subscribed to domain: %s\n", 
                    people[personIndex].id, domain);
             return;
         }
     }
     
     // Add new domain if limit not reached
     if (people[personIndex].domainCount >= MAX_EVENT_TYPES) {
         printf("Max domains reached for person %s\n", people[personIndex].id);
         return;
     }
     
     strcpy(people[personIndex].interestedDomains[people[personIndex].domainCount], domain);
     people[personIndex].domainCount++;
     
     // Also subscribe this person to the EventBus for this domain
     // Subscribers are named with format "Person_personId"
     char subscriberId[MAX_ID_LENGTH];
     sprintf(subscriberId, "Person_%s", people[personIndex].id);
     subscribe(subscriberId, domain, personNewsHandler);
     
     printf("Person %s subscribed to domain: %s\n", people[personIndex].id, domain);
 }
 
 /**
  * Unsubscribe a person from a specific news domain
  * 
  * @param personIndex - Index of the person in the people array
  * @param domain - News domain to unsubscribe from
  */
 void personUnsubscribeFromDomain(int personIndex, char *domain) {
     if (personIndex < 0 || personIndex >= peopleCount) {
         printf("Invalid person index!\n");
         return;
     }
     
     // Find the domain in person's interested domains
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
 
 // The function comment below was in the original code but the function was missing
 // Handle news events for people
 
 
 /**
  * Publish news from a news agency to interested subscribers
  * 
  * @param agencyIndex - Index of the publishing agency
  * @param domain - Domain/category of the news
  * @param content - The actual news content
  */
 void publishNews(int agencyIndex, char *domain, char *content) {
     if (agencyIndex < 0 || agencyIndex >= newsAgencyCount) {
         printf("Invalid agency index!\n");
         return;
     }
     
     // Check if agency is authorized to publish in this domain
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
     
     // Create news object
     News *news = malloc(sizeof(News));
     strcpy(news->domain, domain);
     strcpy(news->content, content);
     strcpy(news->agency, newsAgencies[agencyIndex].id);
     news->timestamp = time(NULL);
     
     // Publish event with domain as the event type
     publish(domain, news, newsAgencies[agencyIndex].id);
 }
 
 /**
  * Generate realistic random sensor data based on sensor type
  * 
  * @param sensorType - Type of the sensor (e.g., "Temperature", "WaterLevel")
  * @return - Random value appropriate for the sensor type
  */
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
 
 /**
  * Simulate a sensor reading and publish it to the event bus
  * 
  * @param sensorType - Type of the sensor
  * @param sensorId - Unique ID for the sensor
  */
 void simulateSensorReading(char *sensorType, char *sensorId) {
     float *data = malloc(sizeof(float));
     *data = generateSensorData(sensorType);
     publish(sensorType, data, sensorId);
 }
 
 /**
  * Event handler for displaying numeric sensor values
  * Simply displays the raw value from the sensor
  * 
  * @param event - The sensor event that was received
  */
 void numericDisplayHandler(Event *event) {
     float *value = (float *)event->data;
     printf("[NumericDisplay] Value from %s: %.2f\n", event->sourceId, *value);
 }
 
 /**
  * Event handler for tracking and displaying maximum sensor values
  * Keeps track of maximum values seen for each sensor type
  * 
  * @param event - The sensor event that was received
  */
 void maxValueDisplayHandler(Event *event) {
     // Static variables retain their values between function calls
     static float maxTemp = -999.9, maxHumidity = -999.9, maxWater = -999.9;
     float *value = (float *)event->data;
 
     printf("[MaxValueDisplay] Received %s: %.2f from %s\n", event->type, *value, event->sourceId);
 
     // Update maximum values if new value is higher
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
 
 /**
  * Event handler for displaying formatted text about sensor readings
  * Presents sensor data in a more descriptive format
  * 
  * @param event - The sensor event that was received
  */
 void textDisplayHandler(Event *event) {
     float *value = (float *)event->data;
     printf("[TextDisplay] %s reported a %s value of %.2f\n", event->sourceId, event->type, *value);
 }
 
 /**
  * Main function - Entry point of the program
  * Sets up the event bus, subscribers, simulates sensors, and demonstrates the news system
  */
 int main() {
     initEventBus();
     
     // Register display subscribers for various sensor types
     subscribe("NumericDisplay1", "Temperature", numericDisplayHandler);
     subscribe("NumericDisplay1", "Humidity", numericDisplayHandler);
     subscribe("NumericDisplay1", "WaterLevel", numericDisplayHandler);
     subscribe("MaxValueDisplay1", "Temperature", maxValueDisplayHandler);
     subscribe("MaxValueDisplay1", "WaterLevel", maxValueDisplayHandler);
     subscribe("MaxValueDisplay1", "Humidity", maxValueDisplayHandler);
     subscribe("TextDisplay1", "Temperature", textDisplayHandler);
     subscribe("TextDisplay1", "WaterLevel", textDisplayHandler);
     subscribe("TextDisplay1", "Humidity", textDisplayHandler);
     
     // Simulate sensor readings from different locations
     printf("\n--- Simulating Sensor Readings ---\n");
     simulateSensorReading("Temperature", "TemperatureSensorTimisoara");
     simulateSensorReading("Temperature", "TemperatureSensorArad");
     simulateSensorReading("WaterLevel", "WaterLevelSensorTimisoara");
     simulateSensorReading("WaterLevel", "WaterLevelSensorArad");
     simulateSensorReading("Humidity", "HumiditySensorTimisoara");
     simulateSensorReading("Humidity", "HumiditySensorArad");
     
     // Create news agencies and specify their domains
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
     
     // Register people and their news interests
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
     
     // Publish some news and observe delivery to interested people
     printf("\n--- Publishing News ---\n");
     publishNews(bbcIndex, "Politics", "New election results announced today");
     publishNews(cnnIndex, "Business", "Stock market reaches all-time high");
     publishNews(espnIndex, "Sports", "Local team wins championship");
     publishNews(bbcIndex, "Culture", "New museum exhibition opens next week");
     
     // Demonstrate subscription changes
     printf("\n--- Updating Subscriptions ---\n");
     personUnsubscribeFromDomain(charlieIndex, "Sports");
     personSubscribeToDomain(charlieIndex, "Business");
     
     // Publish more news to demonstrate updated subscriptions
     printf("\n--- Publishing More News ---\n");
     publishNews(bbcIndex, "Sports", "Tennis tournament final results");
     publishNews(cnnIndex, "Business", "New economic forecast released");
     
     return 0;
 }