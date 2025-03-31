# DACSS_Labs
# Lab 1
Your company develops an Automatic Reviews Moderator bot (well, a simplified mock version only). It can be used as a filtering and labeling preprocessor in online systems handling user reviews of the products of an online shop. Actually, you will maintain a software product line for the product family of review moderators, because you have various clients requiring small variations in the review moderator features.
The incoming messages are simulated by lines of text, containing tokens separated by comma. Their format is: username, productname, reviewtext, attachment. The attachment is a substring that mocks an attached image. We assume that all lines have a valid format. You are able to support following features:

- elliminate messages from users who have not bought the reviewed product (username is not found in a list of buyers of the product).
- elliminate messages that contain profanities in the review text (mock implementation: reviewtext substring does not contain @#$%)
- elliminate messages that contain political propaganda in the review text (mock implementation: reviewtext does not contain +++ or ---)
- resize pictures in attachment if they are too large(mock implementation: if the attachment substring representing the attached picture contains uppercase letters, transform them in lowercase).
- remove competitor website links from the text of the message (mock implementation: elliminate substring http from review text)
  analyze sentiment of the review (positive, negative or neutral) and attach a classification (mock implementation: reviewtext contains more uppercase letters or more lowercase letters determine pozitive or negative sentiment. Append a +, - or = sign at the end of the reviewtext)
Your clients will include various online shops, each requiring a customized version of the reviews moderator bot to suit their specific preferences. For example: One client may accept pictures but require them to be resized if they are too large. Another client just does  not care about pictures. One client may accept reviews from any user, while another may require reviews only from certified buyers of the product.

For example, consider that the required features are: elliminate messages with profanities, accept only reviews from buyers of the product, resize pictures, and determine sentiments. If the registered acquisitions are: John â Laptop; Mary â Phone; Ann - Book, then:

For the input:

John, Laptop, ok, PICTURE
Mary, Phone, @#$%), IMAGE
Peter, Phone, GREAT, ManyPictures
Ann, Book, So GOOD, Image
The output will be:

John, Laptop, ok-, picture
Ann, Book, So GOOD+, image
In both architectural versions, you will implement a collection of specific reusable components handling the various features. It should be easy for your company to reuse them in order to quickly assemble a new custom version of the automatic reviews moderator.

Between certain steps (tasks), there are logical order relationships, while other steps can be done in any order. As a general rule, tasks that eliminate messages should be performed before tasks that transform messages (it makes no sense to first resize pictures and then identify that the message contains profanities, which will cause the message to be eliminated). However, there are no preferred orderings between the different types of eliminator tasks.

# Lab 2
Event-driven application1: monitoring sensors

There are multiple types of sensors (TemperatureSensor, WaterlevelSensor, HumiditySensor, etc.) and multiple sensor instances of each type (TemperatureSensorTimisoara, TemperatureSensorArad, ...). In the implemented application, the sensors will be simulated by simple data sources that generate random data. There are different types of displays (NumericDisplay, MaxValueDisplay, TextDisplay). There can be multiple display instances of each type of display. Each display instance can choose to receive data from certain types of sensors (Temperature, WaterLevel, Humidity, ...) and will receive data from all sensor instances of that type.
Event-driven application2: news agencies and subscribers

There are multiple news agencies that publish news on various domains (sports, politics, culture, etc.). Each agency can publish news from multiple domains. Different people subscribe to receive news from their domains of interest. Each person can subscribe to one or more domains, and can change the domain/domains to which they are subscribed.
EventBus infrastructure: Choice1 - Using GreenRobot

A tutorial on how to get started with GreenRobot and further documentation is available at link
EventBus infrastructure: Choice 2 - Implementat your own very simple BasicEventBus

The BasicEventBus infrastructure must allow the following:
allows interaction between Publishers and Subscribers in the same process
Publisher components publish events of different types, with possible data attached to the event
Subscriber components register for different types of events. The EventBus infrastructure will send them notifications when these events occur
Subscriber components do not need to know in advance the Publisher components that generate the events they subscribe to, and Publisher components do not need to know the Subscriber components that receive notifications
Event types are not fixed in advance, each application can define its own system of event types.
In a very simple implementation of the BasicEventBus, there is a fixed Subscriber interface that must be implemented by any component that wants to be a subscriber to some event types. The class diagram of such a BasicEventBus is given in Lecture3 slides
