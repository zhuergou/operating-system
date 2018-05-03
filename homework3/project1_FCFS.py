'''
Created by:
Pan Deng, dengp
Yuchen Liang, liangy7
Mengwen Zhou, zhoum7

'''

import sys
import copy



# a class to store class 
class Task_D:
    # public variables that can be used 
    all_bnum = 0
    all_num = 0
    all_burst_time = 0
    all_TA_time = 0
    all_wait_time = 0
    cs_num = 0
    preep_num = 0
    def __init__(self, ID , Enter_time , Burst_time , Burst_num , IO_time ):
        self.ID = ID
        self.Enter_time = Enter_time
        self.Burst_time = Burst_time 
        self.Burst_num = Burst_num - 1
        Task_D.all_bnum += Burst_num
        Task_D.all_num += 1
        Task_D.all_burst_time += Burst_num*Burst_time
        self.IO_time = IO_time
        self.Turnaround_time = [ [0, False] for x in range(Burst_num)]
        self.wait_time = 0
    def Reduce_bnum(self): # argument is initial burst_time
        self.Burst_num -= 1
        if self.Burst_num < 0:
            print "Error, B_num run process that has stoped"
            sys.exit(0)
    def Reduce_btime(self):
        self.Burst_time -= 1
        if self.Burst_time < 0:
            print "Error, B_time run process that has stoped"
            sys.exit(0)
    def reset(self, ini_btime, ini_IO_time):
        if self.Burst_time != 0:
            print "ERROR, Not valid"
            sys.exit(0)
        self.Burst_time = ini_btime
        self.IO_time = ini_IO_time
        self.Burst_num -=1

    def Reduce_Etime(self):
        self.Enter_time -= 1
        if self.Enter_time < 0:
            print "Error, E_time run process that has stoped"
            sys.exit(0)
    def Reduce_IO_time(self):
        self.IO_time -= 1
        if self.IO_time < 0:
            print "Error, IO_time run process that has stoped"
            sys.exit(0)
def compare(x,y):
    if x.ID > y.ID:
        return 1
    else:
        return -1
#print the task to debug
def print_task(process):
    print '[ID: %s, Enter_time: %d, Burst_time: %d, Burst_num: %d, IO_time: %d]' % (process.ID,
            process.Enter_time, process.Burst_time, process.Burst_num, process.IO_time)
# print the tasks to debug
def print_tasks(process_vec):
    print "All the task:"
    for process in process_vec:
        print_task(process)

#get process and store in Task_D 
def get_process(line):
    tmp_s = ''
    l = []
    for x in line:
        if x != '|':
            tmp_s += x
        else: 
            l.append(tmp_s)
            tmp_s = ''
            continue
    l.append(tmp_s)

    v = Task_D(l[0], int(l[1]), int(l[2]), int(l[3]), int(l[4]))
    return v
# read all the lines and convert them to a list
def read_process():
    if(len(sys.argv) < 2):
        print "ERROR: Invalid arguments"
        print 'USAGE: ./a.out <input-file> <stats-output-file> [<rr-add>]'
        sys.exit(0)
    try:        
        f = open(sys.argv[1])
    except:
        print 'ERROR: Invalid input file format'
        sys.exit(0)

    all_lines = f.readlines()
    result = [];
    for line in all_lines:
        if line[0] == '#' or len(line) < 9:
            continue
        result.append(get_process(line))
    return result

def find_ini_time(task, p_vec):
    for x in p_vec:
        if task.ID == x.ID:
            tmp = [x.Burst_time, x.IO_time]
            return tmp
    print "Error: <Invalid original list>"
    sys.exit(0)

#processes
prc_vec_ori = read_process()
t_ec = 0
#format(float(a)/float(b),'.2f')

def Cal(): # calculate the related time
    with open(sys.argv[2], 'a') as f:
        f.write("Algorithm FCFS\n")
        f.write("-- average CPU burst time: {:.2f} ms\n".format(float(Task_D.all_burst_time)/float(Task_D.all_bnum)) )
        f.write("-- average wait time: {:.2f} ms\n".format(float(Task_D.all_wait_time)/float(Task_D.all_bnum)))
        f.write("-- average turnaround time: {:.2f} ms\n".format(float(Task_D.all_TA_time)/float(Task_D.all_bnum)))
        # total
        f.write("-- total number of context switches: {}\n".format(Task_D.cs_num + 1) )
        f.write("-- total number of preemptions: {}\n".format(Task_D.preep_num))
    
#arguments
t_cs = 8 # context switch 
t_slice = 800000 # the time slice in RR
# time 
def print_ready(ready_queue):
    if not len(ready_queue):
        print '[Q <empty>]'
    else: 
        print '[Q',
        for x in range(0, len(ready_queue)-1):
            print ready_queue[x].ID,
        print  ready_queue[len(ready_queue)-1].ID + ']'
#=============A bunch of print function ====================
def Enter_print(task, ready_queue):
    print 'time ' +  str(t_ec) + 'ms: Process '+  task.ID + ' arrived and added to ready queue',
    print_ready(ready_queue)

def start_print(task, ready_queue, p_vec_ori):
    tmp1 = find_ini_time(task, p_vec_ori)
    if task.Burst_time == tmp1[0]:
        print "time " + str(t_ec) + 'ms: Process', task.ID, 'started using the CPU',
        print_ready(ready_queue)
    else: 
        print "time " + str(t_ec) + 'ms: Process', task.ID, 'started using the CPU with ' \
        + str(task.Burst_time) + 'ms remaining',
        print_ready(ready_queue)

def burst_print(task, ready_queue):
    tmp_s = 'burst' if task.Burst_num+1 == 1 else 'bursts'
    print "time " + str(t_ec) + 'ms: Process ' + task.ID + ' completed a CPU burst;' , str(task.Burst_num+1) , tmp_s , 'to go',
    print_ready(ready_queue)
    print "time " + str(t_ec) + 'ms: Process ' + task.ID + ' switching out of CPU; will block on I/O until time ' + str(task.IO_time+ t_ec + t_cs/2) + 'ms',
    print_ready(ready_queue)

def return_from_IO_print(task, ready_queue):
    print "time " + str(t_ec) + 'ms: Process ' + task.ID + ' completed I/O; added to ready queue',
    print_ready(ready_queue)


def new_enter(ready_queue, process_vec, IO_q, flag, p_s):#p_s print status 
    result1 = []
    result2 = []
    if len(process_vec):
        for x in range(len(process_vec)-1, -1, -1):
            if process_vec[x].Enter_time == 0:
                if(flag == "END"):
                    result1.insert(0, process_vec[x])
                    process_vec.pop(x)
                else:
                    result1.append(process_vec[x])
                    process_vec.pop(x)
    if len(IO_q):
        for y in range(len(IO_q)-1, -1, -1):
            if IO_q[y].IO_time == 0:
                if(flag == "END"):
                    result2.append(IO_q[y])
                    IO_q.pop(y)
                else:
                    result2.insert(0, IO_q[y])
                    IO_q.pop(y)

    result = result1 + result2

    if len(result):
        result.sort(compare)
        if flag == 'END':
            i = 0
            for k1 in result:
                ready_queue.append(k1)
                if result[i].Turnaround_time[k1.Burst_num][1] == False:
                    result[i].Turnaround_time[k1.Burst_num][1] == True
                    result[i].Turnaround_time[k1.Burst_num][0] = t_ec
                if k1 in result2:
                    if p_s == 'yes':
                        return_from_IO_print(k1, ready_queue)   
                else:
                    if p_s == 'yes':
                        Enter_print(k1, ready_queue)
                i += 1
        else:
            i = 0
            for k1 in result:
                ready_queue.insert(0, k1)
                if result[i].Turnaround_time[k1.Burst_num][1] == False:
                    result[i].Turnaround_time[k1.Burst_num][1] == True
                    result[i].Turnaround_time[k1.Burst_num][0] = t_ec
                if k1 in result1:
                    if p_s == "yes":
                        Enter_print(k1, ready_queue)    
                else:
                    if p_s == "yes":
                        return_from_IO_print(k1, ready_queue)

    return [result,result1,result2]


def time_fly(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag):
    global t_ec
    t_ec += 1
    if len(cur_run):
        cur_run[0].Reduce_btime()
    if len(p_vec):
        for x in range(0, len(p_vec)):
            p_vec[x].Reduce_Etime()
    if len(IO_q):
        for y in range(0, len(IO_q)):
            IO_q[y].Reduce_IO_time()
    Task_D.all_wait_time += len(r_queue)
    
def timeflies(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag, times):
    for x in range(0, times):
        time_fly(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag)
        new_enter(r_queue, p_vec, IO_q, flag, 'yes')
        
def CPU_run(p_vec_ori, p_vec, r_queue, IO_q, flag):
    #pop ready queue and run process
    cur_run = []
    cache = []
    new_enter(r_queue, p_vec, IO_q, flag, 'yes')
    Status = 'switch_in'
    while ( len(p_vec) or len(r_queue) or len(IO_q) or Status == 'run' ):
        if 'switch' in Status:
            if Status == 'switch_in':
                if not len(r_queue):
                    Status == 'wait_IO'
                    continue
                tmp_new1 = r_queue.pop(0)
                timeflies(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag, t_cs/2)
                cur_run.append(tmp_new1)
                start_print(cur_run[0], r_queue, p_vec_ori)
                Status = 'run'
            elif Status == 'switch_out':
                timeflies(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag, t_cs/2)
                r_queue.append(cache.pop())
                Status = 'switch_in'
        elif Status == 'run':
            if_happen = False 
            for x in range(0, t_slice):
                time_fly(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag)
                #if the burst time is zero, move the process to IO or end it
                #and the new process enters in immediately
                if cur_run[0].Burst_time == 0: # end before a time slice 
                    
                    cur_run[0].Turnaround_time[cur_run[0].Burst_num][0] = t_ec - cur_run[0].Turnaround_time[cur_run[0].Burst_num][0] + 4
                    Task_D.all_TA_time += cur_run[0].Turnaround_time[cur_run[0].Burst_num][0]
                    if_happen = True
                    if cur_run[0].Burst_num == 0:
                        print "time " + str(t_ec) + 'ms: Process ' + cur_run[0].ID + ' terminated',
                        print_ready(r_queue)
                        cur_run.pop() # process ends
                        timeflies(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag, t_cs/2)
                    else: #burst number > 0, move to the IOqueue
                        tmp1 = find_ini_time(cur_run[0], p_vec_ori)
                        cur_run[0].reset(tmp1[0], tmp1[1])
                        burst_print(cur_run[0], r_queue)
                        my_result = new_enter(r_queue, p_vec, IO_q, flag, 'yes')
                        tmp_k = cur_run.pop()
                        timeflies(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag, t_cs/2)
                        IO_q.append(tmp_k)
                        new_enter(r_queue, p_vec, IO_q, flag, 'yes')
                    if len(r_queue):
                        Status = 'switch_in'
                        Task_D.cs_num += 1
                    else:
                        Status = 'wait_IO'
                    break
                if x == t_slice - 1:
                    if len(r_queue):    
                        print "time " + str(t_ec) + 'ms: Time slice expired; process ' + cur_run[0].ID + ' preempted with ' + str(cur_run[0].Burst_time) +'ms to go',
                        print_ready(r_queue)
                        cache.append(cur_run.pop())
                        Status = 'switch_out'
                        Task_D.preep_num += 1
                        Task_D.cs_num += 1
                    else:
                        print "time " + str(t_ec) + 'ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]'
                        Status = 'run'
                    new_enter(r_queue, p_vec, IO_q, flag, 'yes')
                    break
                new_enter(r_queue, p_vec, IO_q, flag, 'yes')


        elif Status == 'wait_IO':
            while(not len(r_queue)):
                time_fly(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag)
                new_enter(r_queue, p_vec, IO_q, flag, 'yes')
            Status = 'switch_in'
            Task_D.cs_num += 1

    # switch out the final one
    print "time " + str(t_ec) + 'ms: Simulator ended for FCFS'
    timeflies(cur_run, p_vec_ori, p_vec, r_queue, IO_q, flag, t_cs/2)

def LetsRock():
    #arguments
    print "time " + str(t_ec) + 'ms: Simulator started for FCFS [Q <empty>]'
    process_vec = copy.deepcopy(prc_vec_ori)
    ready_queue = []
    IO_q = []
    flag = 'END'
    CPU_run(prc_vec_ori, process_vec, ready_queue, IO_q, flag )
    Cal()
    
if __name__ == '__main__':
    LetsRock()

    









