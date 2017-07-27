#!/usr/bin/python
import re
import os
from subprocess import Popen,PIPE
import inspect

if os.environ.has_key('EJTAGEXE'):
 exefile = os.environ['EJTAGEXE'] 
else :
 exefile = 'ejtag_debug_usb'

def do_cmd(*arg):
 return do_cmds(arg)

def do_cmd1(*arg):
 cmd = exefile + ' "' +arg[0] +'"';
 s = cmd % arg[1:]
 f = Popen(s,shell=1,stdout=PIPE)
 f.wait()
 if f.returncode : os.exit(-1)
 return f.stdout.read()

def do_cmd1v(*arg):
 cmd = exefile + ' "' +arg[0] +'"';
 s = cmd % arg[1:]
 f = Popen(s,shell=1,stdout=PIPE)
 ret=''
 l = f.stdout.readline()
 while l:
   print l
   ret += l;
 f.wait()
 if f.returncode : os.exit(-1)
 return ret


def do_cmd2(*arg):
 cmd = exefile + ' "' +arg[0] +'"';
 s = cmd % arg[1:]
 f = os.popen(s)
 return f




def do_cmds(*arg):
 cmd = arg[0];
 s = cmd % arg[1:]
 cmds =s.split('\n')
 for cmd in cmds :
  if not re.findall('\S',cmd):
   continue
  else :
   ret = os.system(exefile + ' "' + cmd +'"')
   if ret:
    os.exit(-1)

def ioall(*arg):
 cmd = inspect.stack()[1][3]
 h = {'b':'1','w':'2','l':'4','q':'8'}
 cmd=re.sub(r'out(l|w|b|q)',lambda x:'m'+h[x.groups()[0]],cmd)
 cmd=re.sub(r'in(l|w|b|q)',lambda x:'d'+h[x.groups()[0]]+'q',cmd)
 if(cmd[0] == 'd'):
  buf= exefile + ' "' + cmd + ' ' + hex(arg[0]) + ' 1"\n';
  print buf
  f=Popen(buf,shell=1,stdout=PIPE)
  s=f.stdout.read()
  f.wait()
  if f.returncode: os.exit(-1)
  return int(re.split('\s+',s)[0],0)
 else :
  buf= exefile + ' "' + cmd + ' ' + hex(arg[0]) + ' ' + hex(arg[1]) + '"' ;
  ret = os.system(buf)
  if ret: os.exit(-1)
  return 0

def outq(*arg):
 return	ioall(*arg);

def inq(*arg):
 return ioall(*arg);


def outl(*arg):
 return	ioall(*arg);

def inl(*arg):
 return ioall(*arg);


def outw(*arg):
 return	ioall(*arg);

def inw(*arg):
 return ioall(*arg);


def outb(*arg):
 return	ioall(*arg);

def inb(*arg):
 return ioall(*arg);

