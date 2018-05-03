
'''
Created by:
Pan Deng, dengp
Yuchen Liang, liangy7
Mengwen Zhou, zhoum7

'''


import project1_RR
import project1_SRT
import project1_FCFS
import sys

class Stack:
    "A container with a last-in-first-out (LIFO) queuing policy."
    def __init__(self):
        self.list = []

    def push(self,item):
        "Push 'item' onto the stack"
        self.list.append(item)

    def pop(self):
        "Pop the most recently pushed item from the stack"
        return self.list.pop()

    def isEmpty(self):
        "Returns true if the stack is empty"
        return len(self.list) == 0

class Queue:
    "A container with a first-in-first-out (FIFO) queuing policy."
    def __init__(self):
        self.list = []

    def push(self,item):
        "Enqueue the 'item' into the queue"
        self.list.insert(0,item)

    def pop(self):
        """
          Dequeue the earliest enqueued item still in the queue. This
          operation removes the item from the queue.
        """
        return self.list.pop()

    def isEmpty(self):
        "Returns true if the queue is empty"
        return len(self.list) == 0

def LetsRock():
	if project1_RR.sukey1 in sys.argv[1]:
		flag = 'FCFS'
		project1_SRT.LetsRock(flag)
		print
		flag = 'SRT'
		project1_SRT.LetsRock(flag)
		print
		flag = 'RR'
		project1_SRT.LetsRock(flag)
	else:
		project1_FCFS.LetsRock()
		print
		flag = 'SRT'
		project1_SRT.LetsRock(flag)
		flag = 'RR'
		project1_RR.LetsRock(flag)

class PriorityQueue:
   
    def  __init__(self):
        self.heap = []
        self.count = 0

    def push(self, item, priority):
        # FIXME: restored old behaviour to check against old results better
        # FIXED: restored to stable behaviour
        entry = (priority, self.count, item)
        # entry = (priority, item)
        heapq.heappush(self.heap, entry)
        self.count += 1

    def pop(self):
        (_, _, item) = heapq.heappop(self.heap)
        #  (_, item) = heapq.heappop(self.heap)
        return item

    def isEmpty(self): 
        return len(self.heap) == 0


	
		
