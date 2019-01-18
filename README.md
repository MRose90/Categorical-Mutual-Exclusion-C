# Categorical-Mutual-Exclusion-C

A program written that has searchers, writers and deleters. Searches cannot be executed if more than 10 searches are being done at the same time. Mutex locking occurs to make sure that the number of searches is not modified by multiple threads at the same time. They search for a random done in a linked linked. Writers write a random number to a node and make it the head of the list. Deletes cannot occur unless no searches are going on and will lock out all searches while a delete is occuring.
