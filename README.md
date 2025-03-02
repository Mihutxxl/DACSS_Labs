# DACSS_Labs
# Lab 1
Your company develops an Automatic Reviews Moderator bot (well, a simplified mock version only). It can be used as a filtering and labeling preprocessor in online systems handling user reviews of the products of an online shop. Actually, you will maintain a software product line for the product family of review moderators, because you have various clients requiring small variations in the review moderator features.
The incoming messages are simulated by lines of text, containing tokens separated by comma. Their format is: username, productname, reviewtext, attachment. The attachment is a substring that mocks an attached image. We assume that all lines have a valid format. You are able to support following features:

elliminate messages from users who have not bought the reviewed product (username is not found in a list of buyers of the product).
elliminate messages that contain profanities in the review text (mock implementation: reviewtext substring does not contain @#$%)
elliminate messages that contain political propaganda in the review text (mock implementation: reviewtext does not contain +++ or ---)
resize pictures in attachment if they are too large(mock implementation: if the attachment substring representing the attached picture contains uppercase letters, transform them in lowercase).
remove competitor website links from the text of the message (mock implementation: elliminate substring http from review text)
analyze sentiment of the review (positive, negative or neutral) and attach a classification (mock implementation: reviewtext contains more uppercase letters or more lowercase letters determine pozitive or negative sentiment. Append a +, - or = sign at the end of the reviewtext)
Your clients will include various online shops, each requiring a customized version of the reviews moderator bot to suit their specific preferences. For example: One client may accept pictures but require them to be resized if they are too large. Another client just does not care about pictures. One client may accept reviews from any user, while another may require reviews only from certified buyers of the product.

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