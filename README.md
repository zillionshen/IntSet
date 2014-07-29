IntSet
======
Fast integer hash set/map

It has striking performance in terms of insert/lookup speed and memory consumption.

Here are some of the design principles:
? No hash functions are used and keys are mapped to the slots by the prime number bucket size 
? Integer keys are stored in slots directly without any memory overhead (and two specialized values are used to indicate the empty and deleted slots)
? Buckets are allocated as less as possible to keep the memory overhead in minimum
? Open addressing strategy is used to resolve the collision

Below is chart of the performance comparing to some other libraries:

Inserting/looking up 10 million integers

zillion's set: insert: 3.327 seconds, lookup: 1.532 seconds, memory used: 49 MB

google sparse set: insert: 20.056 seconds, lookup: 2.468 seconds, memory used: 67 MB

google dense set: insert: 67.333 seconds, lookup: 15.454 seconds, memory used: 131 MB

STL unordered_set: insert: 267.922 seconds, lookup: 43.252 seconds, memory used: 836 MB

STL set: insert: 324.055 seconds, lookup: 162.29 seconds, memory used: 783 MB
