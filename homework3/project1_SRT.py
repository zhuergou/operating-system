'''
Created by:
Pan Deng, dengp
Yuchen Liang, liangy7
Mengwen Zhou, zhoum7

'''


from __future__ import print_function
import copy
import sys

t_cs = 8;

class pcb:
	pid = None
	arrival_time = 0
	burst_time = 0
	execute_time = 0
	num_burst = 0
	num_burst_copy = 0
	io_time = 0
	wait_time = 0
	last_burst_finished = 0
	leaving_time = 0
	number_of_switch = 0
	number_of_preempt = 0
	arrive_flag = False
	finish_flag = False
	in_queue = False
	usingCpu = False
	preempt_flag = False


def importFunc(filename):
	temp = pcb()
	holder = []
	f = open(filename, 'r')

	for line in f:
		if line.startswith("#"):
			continue;
		raw = line.split('|')
		if len(raw)<5 :
			continue
		temp.pid = raw[0]
		temp.arrival_time = int(raw[1])
		temp.burst_time = int(raw[2])
		temp.num_burst = int(raw[3])
		temp.num_burst_copy = int(raw[3])
		temp.io_time = int(raw[4].strip())

	
		holder.append(copy.deepcopy(temp))

	f.close()
	return holder

def comp(a, b):
	x = cmp(a.arrival_time, b.arrival_time)
	if x == 0:
		x = cmp(a.burst_time, b.burst_time)
	return x


def comp2(a, b):
	x = cmp(a.burst_time, b.burst_time)
	if x == 0:
		x = cmp(a.pid, b.pid)
	return x

def comp3(a, b):
	x = cmp((a.burst_time - a.execute_time), (b.burst_time - b.execute_time))
	if x == 0:
		x = cmp((a.last_burst_finished + a.io_time), (b.last_burst_finished + b.io_time))
	return x

def getPid(ready_queue):
	result = ['Q']
	if len(ready_queue) == 0:
		result.append('<empty>')
	for x in ready_queue:
		result.append(x.pid)
	print ('[', end = '')
	for i in range(len(result) - 1):
		print ("{}".format(result[i]), end = ' ')

	print ("{}]".format(result[-1]))
		

def updateQueue(ready_queue, holder, t):
	for x in holder:
		if x.finish_flag == True:
			continue
		if x.in_queue == True:
			continue
		if x.arrive_flag == False:
			if x.arrival_time <= t:
				x.num_burst -= 1
				if len(ready_queue) == 0:
					ready_queue.append(x)
					print("time {}ms: Process {} arrived and added to ready queue ".format(x.arrival_time, x.pid), end = '')
					getPid(ready_queue)
				
				else:
					old_queue = ready_queue[:]
					ready_queue.append(x)
					ready_queue.sort(cmp = comp3)
					if x.pid != ready_queue[0].pid:
						if old_queue[0].usingCpu == False:
							print("time {}ms: Process {} arrived and added to ready queue ".format(x.arrival_time, x.pid), end = '')
							getPid(ready_queue)
						else:
							print("time {}ms: Process {} arrived and added to ready queue ".format(x.arrival_time, x.pid), end = '')
							getPid(ready_queue[1:])
					else:
						if old_queue[0].usingCpu == False:
							'''
							print("time {}ms: Process {} arrived and will preempt {} ".format(x.arrival_time, x.pid, old_queue[0].pid), end = '')
							getPid(old_queue)
							'''
							print("time {}ms: Process {} arrived and added to ready queue ".format(x.arrival_time, x.pid), end = '')
							getPid(ready_queue)

						else:
							print("time {}ms: Process {} arrived and will preempt {} ".format(x.arrival_time, x.pid, old_queue[0].pid), end = '')
							getPid(old_queue[1:])
				
				x.arrive_flag = True
				x.in_queue = True
		
		else:
			if x.last_burst_finished + x.io_time <= t:
				x.num_burst -= 1
				if len(ready_queue) == 0:
					ready_queue.append(x)
					print("time {}ms: Process {} completed I/O; added to ready queue ".format(x.last_burst_finished + x.io_time, x.pid), end = '')
					getPid(ready_queue)
				else:
					old_queue = ready_queue[:]
					ready_queue.append(x)
					ready_queue.sort(cmp = comp3)
					if x.pid != ready_queue[0].pid:
						if old_queue[0].usingCpu == False:
							print("time {}ms: Process {} completed I/O; added to ready queue ".format(x.last_burst_finished + x.io_time, x.pid), end = '')
							getPid(ready_queue)
						else:
							print("time {}ms: Process {} completed I/O; added to ready queue ".format(x.last_burst_finished + x.io_time, x.pid), end = '')
							getPid(ready_queue[1:])

					else:
						if old_queue[0].usingCpu == False:
							'''
							print("time {}ms: Process {} completed I/O and will preempt {} ".format(x.last_burst_finished + x.io_time, x.pid, old_queue[0].pid), end = '')
							getPid(old_queue)
							'''
							print("time {}ms: Process {} completed I/O; added to ready queue ".format(x.last_burst_finished + x.io_time, x.pid), end = '')
							getPid(ready_queue)

						else:
							print("time {}ms: Process {} completed I/O and will preempt {} ".format(x.last_burst_finished + x.io_time, x.pid, old_queue[0].pid), end = '')
							getPid(old_queue[1:])


				x.in_queue = True


def srtf(holder, flag):
	t = 0
	number_of_process = len(holder)
	ready_queue = []
	current_running = None

	print("time {}ms: Simulator started for {} ".format(t,flag),  end = '')
	getPid(ready_queue)
	while True:
		updateQueue(ready_queue, holder, t)
		if (len(ready_queue) != 0):
			break
		t += 1
	
	while True:
		current_running = ready_queue[0]
		for i in range(t_cs/2):
			for j in range(len(ready_queue)):
				ready_queue[j].wait_time += 1
			current_running.wait_time -= 1
			t += 1
			updateQueue(ready_queue, holder, t)

		if current_running.pid == ready_queue[0].pid:
			current_running.number_of_switch += 1
			print("time {}ms: Process {} started using the CPU ".format(t, ready_queue[0].pid), end = '')
			getPid(ready_queue[1:])
			ready_queue[0].usingCpu = True
			break
		else:
			for i in range(t_cs/2):
				for j in range(len(ready_queue)):
					ready_queue[j].wait_time += 1
				current_running.wait_time -= 1
				t += 1
				updateQueue(ready_queue, holder, t)

		

	while number_of_process != 0:
		current_running = ready_queue[0]
		t += 1
		ready_queue[0].execute_time += 1
		for i in range(1, len(ready_queue)):
			ready_queue[i].wait_time += 1

		if ready_queue[0].execute_time == ready_queue[0].burst_time:
			'''
			print("time <{}>ms: <Process finishes using the CPU> {}".format(t, getPid(ready_queue)))
			'''

			ready_queue[0].last_burst_finished = t + t_cs/2
			ready_queue[0].execute_time = 0
			ready_queue[0].in_queue = False
			current_running = None

			if ready_queue[0].num_burst == 0:
				ready_queue[0].leaving_time = t + t_cs/2
				ready_queue[0].finish_flag = True
				number_of_process -= 1
				l = ready_queue.pop(0)
				print("time {}ms: Process {} terminated ".format(t, l.pid), end = '')
				getPid(ready_queue)

				for i in range(t_cs/2):
					updateQueue(ready_queue, holder, t)
					t += 1
					for j in range(len(ready_queue)):
						ready_queue[j].wait_time += 1
				l.usingCpu = False

			else:
				l = ready_queue.pop(0)
				if l.num_burst == 1:
					print("time {}ms: Process {} completed a CPU burst; {} burst to go ".format(t, l.pid, l.num_burst), end = '')
				else:
					print("time {}ms: Process {} completed a CPU burst; {} bursts to go ".format(t, l.pid, l.num_burst), end = '')
				getPid(ready_queue)
				print("time {}ms: Process {} switching out of CPU; will block on I/O until time {}ms ".format(t, l.pid, l.last_burst_finished + l.io_time), end = '')
				getPid(ready_queue)

				for i in range(t_cs/2):
					updateQueue(ready_queue, holder, t)
					t += 1
					for j in range(len(ready_queue)):
						ready_queue[j].wait_time += 1
				
				l.usingCpu = False

		if number_of_process == 0:
			print("time {}ms: Simulator ended for {}".format(t,flag))
			break

		while True:
			updateQueue(ready_queue, holder, t)
			if (len(ready_queue) != 0):
				break
			t += 1
		
		if current_running == None:
			while True:
				current_running = ready_queue[0]
				for i in range(t_cs/2):
					for j in range(len(ready_queue)):
						ready_queue[j].wait_time += 1
					current_running.wait_time -= 1
					t += 1
					updateQueue(ready_queue, holder, t)
				if current_running.pid == ready_queue[0].pid:
					current_running.number_of_switch += 1
				 	if ready_queue[0].preempt_flag == True:
				 		print("time {}ms: Process {} started using the CPU with {}ms remaining ".format(t, ready_queue[0].pid, ready_queue[0].burst_time - ready_queue[0].execute_time), end = '')
				 		getPid(ready_queue[1:])
				 	else:
				 		print("time {}ms: Process {} started using the CPU ".format(t, ready_queue[0].pid), end = '')
						getPid(ready_queue[1:])

				 	ready_queue[0].usingCpu = True
					ready_queue[0].preempt_flag = False
				 	break
				else:
					for i in range(t_cs/2):
						for j in range(len(ready_queue)):
							ready_queue[j].wait_time += 1
						current_running.wait_time -= 1
						t += 1
						updateQueue(ready_queue, holder, t)

		elif current_running.pid != ready_queue[0].pid:
			current_running.preempt_flag = True
			for i in range(t_cs/2):
				for j in range(len(ready_queue)):
					ready_queue[j].wait_time += 1
				current_running.wait_time -= 1
				t += 1
				updateQueue(ready_queue, holder, t)
			current_running.number_of_preempt += 1
			current_running.usingCpu = False
			while True:
				current_running = ready_queue[0]
				for i in range(t_cs/2):
					for j in range(len(ready_queue)):
						ready_queue[j].wait_time += 1
					current_running.wait_time -= 1
					t += 1
					updateQueue(ready_queue, holder, t)
				if current_running.pid == ready_queue[0].pid:
					current_running.number_of_switch += 1
				 	if ready_queue[0].preempt_flag == True:
				 		print("time {}ms: Process {} started using the CPU with {}ms remaining ".format(t, ready_queue[0].pid, ready_queue[0].burst_time - ready_queue[0].execute_time), end = '')
				 		getPid(ready_queue[1:])
				 	else:
				 		print("time {}ms: Process {} started using the CPU ".format(t, ready_queue[0].pid), end = '')
						getPid(ready_queue[1:])
				 	ready_queue[0].usingCpu = True
					ready_queue[0].preempt_flag = False
				 	break
				else:
					for i in range(t_cs/2):
						for j in range(len(ready_queue)):
							ready_queue[j].wait_time += 1
						current_running.wait_time -= 1
						t += 1
						updateQueue(ready_queue, holder, t)

			

def getAverageBurst(holder):
	time = 0
	num = 0
	for i in holder:
		time += i.burst_time* i.num_burst_copy
		num += i.num_burst_copy
	return time*1.0/(num)

def totalPreempt(holder):
	num = 0
	for i in holder:
		num += i.number_of_preempt
	return num

def getAverageWait(holder):
	time = 0
	num = 0
	for i in holder:
		time += i.wait_time
		num += i.num_burst_copy

	# since when the preemption occur, the preempting process is not in the ready queue.
	time -= t_cs/2 * totalPreempt(holder) 
	return time*1.0/(num)

def getAverageTurnaround(holder):
	time = 0
	num = 0
	for i in holder:
		time += i.leaving_time - i.arrival_time - (i.num_burst_copy - 1)* (i.io_time)
		num += i.num_burst_copy
	return time*1.0/(num)

def totalSwitch(holder):
	num = 0
	for i in holder:
		num += i.number_of_switch
	return num
def LetsRock(flag):
	holder = importFunc(sys.argv[1])
	srtf(holder,flag)
	with open(sys.argv[2], 'a') as f:
		f.write("Algorithm {}\n".format(flag) )
		f.write("-- average CPU burst time: {:.2f} ms\n".format(getAverageBurst(holder)))
		f.write("-- average wait time: {:.2f} ms\n".format(getAverageWait(holder)))
		f.write("-- average turnaround time: {:.2f} ms\n".format(getAverageTurnaround(holder)))
		f.write("-- total number of context switches: {}\n".format(totalSwitch(holder)))
		f.write("-- total number of preemptions: {}\n".format(totalPreempt(holder)))


if __name__ == "__main__":
	LetsRock()
