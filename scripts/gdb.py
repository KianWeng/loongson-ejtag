#!/usr/bin/python

# GDB debugging support
#
# Copyright 2012 Red Hat, Inc. and/or its affiliates
#
# Authors:
#  Avi Kivity <avi@redhat.com>
#
# This work is licensed under the terms of the GNU GPL, version 2.  See
# the COPYING file in the top-level directory.
#
# Contributions after 2012-01-13 are licensed under the terms of the
# GNU GPL, version 2 or (at your option) any later version.


import gdb
import types
import re
import os

def test():
	print("test\r\n")

def getversion():
        linux_banner=gdb.parse_and_eval('linux_banner')
        l=list(map(int,re.findall(r'Linux version (\d)\.(\d+)\.(\d+)',str(linux_banner))[0]))
        return (l[0]<<16)|(l[1]<<8)|l[2]


def gdb_eval(s,t=0):
 if not t:
  return int(str(gdb.parse_and_eval('(size_t)(' + s +')')))
 else :
  return re.findall(r'"(.+)"',str(gdb.parse_and_eval('(char *)('+s+')')))[0]

def getcurrent(cpuno=0):
 if linuxver>=0x20600:
   current=gdb_eval('((struct thread_info *)((((long *)&kernelsp)['+str(cpuno)+'])&~(sizeof(init_thread_union)-1)))')
   if current:
    task=gdb_eval('((struct thread_info *)' + str(current) +')->task')
   else:
    task=0
 else :
  current=gdb_eval('((struct task_struct  *)((((long *)&kernelsp)[' + str(cpuno)+ '])&~(sizeof(init_task_union)-1)))')
  task=current
 return (current,task)

def modules24():
 #gdb.execute("monitor setpgsize 0x4000\n");
 mod=gdb_eval('module_list') 
 while mod:
   modnext= gdb_eval('((struct module *)'+str(mod)+')->next')
   if not modnext: break
   modname= re.findall(r'"(.+)"',str(gdb.parse_and_eval('((struct module *)'+str(mod)+')->name')))[0]
   s=os.popen("./tools/load-module24\tmodules/%s.o %#x" %(modname, mod)).read()
   print(s)
   gdb.execute(s)
   mod=modnext

def tasks24(pid=-1):
 current,task = getcurrent()
 curtask=task
 def tp(task,x,type=0):
  if not type:
   return int(str(gdb.parse_and_eval('(size_t)((struct task_struct *)' + str(task)+')'+str(x)))) 
  else :
   return re.findall(r'"(.+)"',str(gdb.parse_and_eval('(char *)((struct task_struct *)' + str(task)+')'+str(x))))[0]
 p0 = p=gdb_eval('init_tasks[0]')
 while p: 
  tinfo = gdb_eval('&((struct task_struct *)'+str(task)+')->thread')
  uregs = gdb_eval('(void *)'+str(task)+'+sizeof(init_task_union)-32-sizeof(struct pt_regs)')

  tpid = tp(p,'->pid')
  print( "%s %d %d %s ra=%#x sp=%#x epc=%#x usp=%#x task=%#x tinfo=%#x uregs=%#x stat=%#x" % ( ("  ","* ")[p==curtask],tp(p,'->pid'), tp(p,'->p_pptr->pid'), tp(p,'->comm',1),tp(p,'->thread.reg31'),tp(p,'->thread.reg29'),gdb_eval('((struct pt_regs *)'+str(uregs)+')->cp0_epc'),gdb_eval('((struct pt_regs *)'+str(uregs)+')->regs[29]'),task, tinfo, uregs, tp(p,'->state')))
  if pid>0:
   if pid == tpid:
    break
  p=tp(p,'->next_task')
  if p== p0: break
 


def tasks(pid=-1):
 def tp(task,x,type=0):
  if not type:
   return int(str(gdb.parse_and_eval('(size_t)((struct task_struct *)' + str(task)+')'+str(x)))) 
  elif type=='&':
   return int(str(gdb.parse_and_eval('(size_t)&((struct task_struct *)' + str(task)+')'+str(x)))) 
  elif type=='s' :
   return re.findall(r'"(.+)"',str(gdb.parse_and_eval('(char *)((struct task_struct *)' + str(task)+')'+str(x))))[0]
  else : raise(TypeError())

 inittask=p=gdb_eval('&init_task->tasks')
 offset=gdb_eval('&((struct task_struct *)0)->tasks')
 cpus = gdb_eval('sizeof(kernelsp)/sizeof(kernelsp[0])')
 stackoff = gdb_eval('sizeof(init_thread_union)-32-sizeof(struct pt_regs)');
 while p: 
  task = p-offset;
  curtask=task
  curgrp = tp(curtask,'->thread_group','&')
  while task:
   if linuxver<=0x20615:
    tinfo = tp(task,'->thread_info')
   else:
    tinfo = tp(task,'->stack')
 
   uregs = tinfo + stackoff
 
   iscur=0
   i=0
   while (i<cpus):
    current1, task1 = getcurrent(i)
    if task1 == 0:
     iscur=0
     break
    if task1 == task:
     iscur=i
     break
    i=i+1
   tpid = tp(task,'->pid')
 
   print("%s %d %d %s ra=%#x sp=%#x epc=%#x usp=%#x task=%#x tinfo=%#x uregs=0x%x stat=0x%x" % (('   ','<'+str(iscur)+'>')[iscur!=0],tpid, tp(task,'->parent->pid'), tp(task,'->comm','s'),tp(task,'->thread.reg31'),tp(task,'->thread.reg29'),gdb_eval('((struct pt_regs *)'+str(uregs)+')->cp0_epc'),gdb_eval('((struct pt_regs *)'+str(uregs)+')->regs[29]'),task, tinfo, uregs, tp(task,'->state')))
   
   if pid==tpid : break
   
   t=tp(task,'->thread_group.next')
   if t==curgrp or t == 0:
    break
   task = t-tp(0,'->thread_group','&')
 
  p=gdb_eval('((struct list_head *)'+str(p)+')->next')
  if p==inittask: break

def modules0(modpath='modules'):
    mymodlist = gdb_eval('modules->next')
    modules = gdb_eval('&modules')
    modoffs = gdb_eval('&((struct module *)0)->list.next')
    mcnt=0
    while mymodlist != modules:
        mymod=mymodlist - modoffs
        s= "add-symbol-file\t" + modpath +"/%s.ko 0 " % (gdb_eval('((struct module *)'+str(mymod)+')->name','s'))
        attrs=gdb_eval('((struct module *)'+str(mymod)+')->sect_attrs')
        nsections = gdb_eval('((struct module_sect_attrs *)'+str(attrs)+')->nsections')
        n=0
        while n<nsections:
         attrsn = gdb_eval('&((struct module_sect_attrs *)'+str(attrs)+')->attrs['+str(n)+']')
         s+= "-s %s %#x " % ( gdb_eval('((struct module_sect_attr *)'+str(attrsn)+')->name','s'),gdb_eval('((struct module_sect_attr *)'+str(attrsn)+')->address'))
         n=n+1
        gdb.execute(s)
        mymodlist = gdb_eval('((struct list_head *)'+str(mymodlist)+')->next')
 
def modules(modpath='modules'):
    mymodlist = gdb_eval('modules->next')
    modules = gdb_eval('&modules')
    modoffs = gdb_eval('&((struct module *)0)->list.next')
    while mymodlist != modules:
        mymod=mymodlist - modoffs
        s=os.popen("./tools/load-module\tmodules/%s.ko %#x %#x %d\n" % (gdb_eval('((struct module *)'+str(mymod)+')->name','s'), gdb_eval('((struct module *)'+str(mymod)+')->module_core'), gdb_eval('((struct module *)'+str(mymod)+')->module_init'), (0,kallsyms)[linuxver<0x2061f])).read()
        gdb.execute(s)
        mymodlist = gdb_eval('((struct list_head *)'+str(mymodlist)+')->next')

'''
def vma(task=0):
 if not task:
   current,task = getcurrent()
 mm = gdb_eval("((struct task_struct *)"+str(task)+")->mm")
if $task->mm
 set $t = $task->mm->mmap
 while $t
  printf "start:\t%p\tend:\t%p\tpgoff:\t%x\t ", $t->vm_start, $t->vm_end, $t->vm_pgoff*16*1024
  if $t->vm_file
    if(linuxver>=0x20613)
     printf "%s\t mapping: (struct address_space *)%p", $t->vm_file->f_path->dentry->d_name.name, $t->vm_file->f_mapping
    else
    if(linuxver>=0x20500)
     printf "%s\t mapping: (struct address_space *)%p", $t->vm_file->f_dentry->d_name.name, $t->vm_file->f_mapping
    else
     printf "%s\t mapping: (struct file_operations *)%p", $t->vm_file->f_dentry->d_name.name, $t->vm_file->f_op
    end
    end
  end
  set $t=$t->vm_next
  echo \n
  end
 end
end
'''

class GetversionCommand(gdb.Command):
    '''Display the memory tree hierarchy'''
    def __init__(self):
        gdb.Command.__init__(self, 'getversion', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
        self.queue = []
    def invoke(self, arg, from_tty):
     print(getversion())


class Modules24Command(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, 'modules24', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
       modules24()

class Tasks24Command(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, 'tasks24', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
       tasks24()

class TasksCommand(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, 'tasks', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
       tasks()

class Modules0Command(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, 'modules0', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
       modules0()

class ModulesCommand(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, 'modules', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
       modules()

class MytestCommand(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, 'mytest', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        gdb.write('%d\n' % (123), gdb.STDOUT)
	
	
#	print gdb.task()
#	help(gdb)
#	print dir(gdb)


linuxver = getversion()

pagesize = 0x4000
if linuxver>=0x20600:
    init_task = gdb.lookup_global_symbol('init_task').value().address
    next_task=gdb.Value(int(init_task['tasks']['next'])-int(gdb.Value(0).cast(init_task.type)['tasks'].address)).cast(init_task.type)
    mm=next_task['mm']
    if mm:
      elf_info=mm['saved_auxv'].address.cast(gdb.lookup_type('int').pointer())
      if elf_info[2] == 0:
        elf_info=mm['saved_auxv'].address.cast(gdb.lookup_type('long long').pointer())
      i=0
      while i<32:
         t= int(elf_info[i])
         if t==6:
           pagesize = int(elf_info[i+1])
           break
         i+=2

if gdb.lookup_global_symbol('module_kallsyms_lookup_name'):
  kallsyms=1
else :
  kallsyms=0
 
try:
 gdb.execute("monitor setpgsize " + str(pagesize))
except gdb.error:
 pass
	

GetversionCommand()
Modules24Command()
Tasks24Command()
ModulesCommand()
TasksCommand()
Modules0Command()
MytestCommand()

