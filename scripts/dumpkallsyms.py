#!/usr/bin/python
import sys
import struct
import getopt

opts,args = getopt.getopt(sys.argv[1:],'lh')
kopts = dict(opts)
ksize = 0x2000000
if kopts.has_key('-h'):
 print(sys.argv[0]+" [-l] vmlinux: with -l for 64bit kernel,else 32bit kernel")
 exit(0)
if kopts.has_key('-l'):
 sfmt='Q'
 ssize=8
 align=256
 kstart=0xffffffff80000000
else :
 sfmt='I'
 ssize=4
 align=16
 kstart=0x80000000



fname = len(args)>0 and args[0] or 'vmlinux'
f=open(fname,'rb')
pos = 0
l = 0
maxl = 0
maxpos = 0
while pos<ksize:
  d = f.read(ssize)
  if len(d)<ssize : break
  a, = struct.unpack(sfmt,d)
  if a>=kstart and a<kstart+ksize:
    l+=ssize
  else :
    if l>maxl:
      maxl = l
      maxpos = pos-l
    l = 0
  pos += ssize

kallsym_addr =  maxpos
kallsym_num_addr = (maxpos+maxl+(align-1))&~(align-1)
f.seek(kallsym_num_addr)
num,=struct.unpack(sfmt,f.read(ssize))
if num != maxl/ssize:
  print('kallsyms can not find')
  exit(-1)

kallsyms_name_addr = kallsym_num_addr+align

pos=kallsyms_name_addr
i = 0
while i<num:
  f.seek(pos)
  n,=struct.unpack('B',f.read(1))
  pos += n+1
  i += 1
kallsyms_name_size = pos-kallsyms_name_addr

kallsyms_markers_addr = (pos+(align-1))&~(align-1)
marker_size = ((num + 255) >> 8)*ssize
kallsyms_token_table_addr = (kallsyms_markers_addr + marker_size + (align-1))&~(align-1)

pos = kallsyms_token_table_addr
f.seek(pos)
i=0
while i<256:
  while f.read(1) != '\x00':
    pos += 1
  pos += 1
  i += 1

kallsyms_token_table_size = pos - kallsyms_token_table_addr
kallsyms_token_index_addr = (pos+(align-1))&~(align-1)


f.seek(kallsym_addr);
sym_addr = struct.unpack(str(num)+sfmt,f.read(num*ssize))
f.seek(kallsyms_name_addr)
sym_name = struct.unpack(str(kallsyms_name_size)+'B', f.read(kallsyms_name_size))
f.seek(kallsyms_token_index_addr)
sym_tokeidx = struct.unpack('256H',f.read(256*2))
f.seek(kallsyms_token_table_addr)
sym_toketbl = f.read(kallsyms_token_table_size)


idx=0
pos=0
while idx<num:
 n=sym_name[pos]
 pos += 1 
 name=''
 for i in range(n):
  k = sym_tokeidx[sym_name[pos]]
  e = sym_toketbl.find('\x00',k)
  s=sym_toketbl[k:e]
  name += s
  pos += 1
 print("0x%x %s %s" %(sym_addr[idx],name[1:],name[0]))
 idx += 1

  






   

