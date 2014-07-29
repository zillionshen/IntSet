IntSet
======
Fast integer hash set/map

It has striking performance in terms of insert/lookup speed and memory consumption.

Here are some of the design principles:
•	No hash functions are used and keys are mapped to the slots by the prime number bucket size 
•	Integer keys are stored in slots directly without any memory overhead (and two specialized values are used to indicate the empty and deleted slots)
•	Buckets are allocated as less as possible to keep the memory overhead in minimum
•	Open addressing strategy is used to resolve the collision

Below is chart of the performance comparing to some other libraries:
Inserting/looking up 10 million integers
Library	          insert (sec)	  lookup(sec)	memory(MB)
zillion's set     3.327	          1.532	      49
google sparse set 20.056	        2.468	      67
google dense set  67.333          15.454      131
STL unordered_set 267.922         43.252      836
STL set           324.055         162.29      783
