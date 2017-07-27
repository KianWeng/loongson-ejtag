#!/usr/bin/python
import re
import os
import socket
import select
from signal import signal,SIGINT,SIG_IGN
import select
import sys
import struct

class DataHolder:
    def __init__(self, value=None, attr_name='value'):
        self._attr_name = attr_name
        self.set(value)
    def __call__(self, value):
        return self.set(value)
    def set(self, value):
        setattr(self, self._attr_name, value)
        return value
    def get(self):
        return getattr(self, self._attr_name)

def csum(s):
 sum=0
 for i in xrange(len(s)):
  sum += ord(s[i])
 return sum&0xff


def send_to_qemu(s,b):
 c=csum(b)
 s.sendall("+\$" + b + "#%02x"%(c))
 return s.recv(1)
   

def recv_from_qemu(s,l):
    a='';
    while 1:
       b = s.recv(2048)
       a += b
       m=re.search(r'\$(\w+)#\w{2}',a)
       if m:
          a=m.groups()[0]
          break
    s.send ('+',0)
    return a

def read_addr(s,b):
    send_to_qemu(s,b)
    b=recv_from_qemu(s,100)
    #return re.sub('(\w{2})',lambda x: chr(int(x.group(),16)),b)
    return b.decode('hex')

def read_addr8(s,d):
    send_to_qemu(s,"m%08x,8" %(d))
    b=recv_from_qemu(s,100)
    return struct.unpack('Q',b.decode('hex'))[0]

def read_addr4(s,d):
    send_to_qemu(s,"m%08x,4" %(d))
    b=recv_from_qemu(s,100)
    if b.startswith("E14"): raise Exception("E14")
    return struct.unpack('I',b.decode('hex'))[0]

def getpgd(addr):
    if (addr&0xf0000000) == 0xc0000000:
        curpgd = sym['swapper_pg_dir']
    elif debugpgd:
        curpgd = debugpgd
    else :
        curpgd=read_addr4(sock, sym['pgd_current'])
    return curpgd

def accesshelper(head,addr,tail,pgd):
        mask = ((1<<pageshift)-4);
        mask1 = ((1<<pageshift)-8);
        #printf("read 0x%x\n", $addr);
#right shift(14+1), left shit log2(2item) : 11 = (14+1-4) (pageshift+1-log2(2item)) 
# 11 = (14-3)
# 12-2

        pud=read_addr4(sock,((addr>>((pageshift-2)*2))&mask) + pgd);
        pte=read_addr4(sock,((addr>>(pageshift-2)) & mask1) + pud);
        pte1=read_addr4(sock,((addr>>(pageshift-2)) & mask1) + 4 + pud);
        if debuginfo: print("pud=%x pte=%x pte1=%x\n" % (pud, pte, pte1));

        if addr&(1<<pageshift):
            if pte1&(2<<pteshift):
                phyaddr = ((pte1>>(pteshift+6))<<12) + (addr&((1<<pageshift)-1))
                if phyaddr >= 0x20000000:
                    if not enable64:
                        print("physical address is too large 0x%x" %(phyaddr))
                        raise Exception("E14")
                    addr1 = (0x9800000000000000,0x9000000000000000)[((pte1>>(pteshift+3))&7)!=3] + phyaddr;
                else :
                    addr1 = (0x80000000,0xa0000000)[((pte1>>(pteshift+3))&7)!=3] + phyaddr;
            else :
            	return None;
        else :
            if pte&(2<<pteshift) :
                phyaddr = ((pte>>(pteshift+6))<<12)  + (addr&((1<<pageshift)-1))
                if phyaddr >= 0x20000000:
                     if not enable64:
                        print("physical address is too large 0x%x" %(phyaddr))
                        raise Exception("E14")
                     addr1 = (0x9800000000000000,0x9000000000000000)[((pte>>(pteshift+3))&7)!=3] + phyaddr;
                else :
                     addr1 = (0x80000000,0xa0000000)[((pte>>(pteshift+3))&7)!=3] + phyaddr;
            else :
             	return None;


        newbuf = (head +"%08x" + tail) % (addr1);
        buf='$' + newbuf + ('#%02x' % (csum(newbuf)));
        if debuginfo: print(buf)
        return buf;


def probepte():
    global pteshift
    for i in range(0x30,0x50,4):
        ins = read_addr4(sock,0x80000000+i);
        #srl
        if ((ins & 0xffe0003f) == 2):
            pteshift = ((ins>>6)&0x1f);
        #mtc0
        if ((ins & 0xffe00000) == 0x40800000):
            break;

    if debuginfo: print( 'g: pteshift:'+pteshift+'\n')

def myvar(m0):
    global mm
    mm=m0
    return mm


mm=0
pageshift = 14;
pteshift = 6;
debugpgd = 0;
enablesb = 'ENV_memsize' in os.environ.keys();
debuginfo = 0;
enable64 = 0;
fg = 1
func={}
sym={}

if 'FG' not in os.environ.keys():
    signal(SIGINT,SIG_IGN)
    fg = 0


if  len(sys.argv)>=2:
    f = os.popen('nm ' + sys.argv[1]);
    for x in ('swapper_pg_dir', 'pgd_current', 'module_kallsyms_lookup_name'):
        func[x] = 1

    l=f.readline()
    while l:
        a=re.split(r'\s',l)
        if a[2] in func:
            sym[a[2]] = int(a[0],16)
        l=f.readline()
    f.close()

if 'swapper_pg_dir' in sym:
    print ('swapper_pg_dir %#x\n' % (sym['swapper_pg_dir']))

if not fg and os.fork(): exit(0)


sock=socket.socket()
peeraddr = ('',50010)
if len(sys.argv)>2:
    peeraddr = re.search(r'(.*):(\d+)',sys.argv[2]).groups()
    perraddr = (addr[0],int(addr[1]))

sock.connect(peeraddr)

server = socket.socket()

server.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
server.bind(('',9000))
server.listen(5)

client,ip = server.accept()


sock.setsockopt(socket.SOL_TCP, socket.TCP_NODELAY, 1);
client.setsockopt(socket.SOL_TCP, socket.TCP_NODELAY, 1);
#fix me
def testre(m,v):
    v[0]=m
    return m

while 1:
    while 1:
        sel = select.select([sock,client],[],[])
        
        
        for fh in sel[0]:
                if (fh == sock): 
                    buf = sock.recv(4096, socket.MSG_DONTWAIT);
                    if not len(buf): raise Exception('server quit')
                    client.send(buf);
                else :
                    buf = client.recv(4096, socket.MSG_DONTWAIT);
                    if not len(buf): raise Exception('client quit')
                    m=[0]

                    if testre(re.search(r'\$qRcmd,(\w+)#',buf),m):
                        cmd=m[0].groups()[0].decode('hex')
                        if testre(re.search(r'setpgd\s+(\w+)',cmd),m):
                            print ('cmd ' + cmd + m[0].groups()[0] +'\b');
                            debugpgd=int(m[0].groups()[0],0)
                            client.send('+$OK#9a');
                            continue;
                        elif testre(re.search(r'setpgsize\s+(\w+)',cmd),m):
                            print ('cmd ' + cmd + m[0].groups()[0] +'\b');
                            pagesize=int(m[0].groups()[0],0);
                            for i in range(12,25):
                                if((1<<i) == pagesize): break
                            pageshift = i;
                            probepte();
                            client.send('+$OK#9a');
                            continue
                        elif testre(re.search(r'kernel\s+kallsyms',cmd),m):
                            if 'module_kallsyms_lookup_name' in sym:
                                t = "set \$mykallsyms=1\n".encode('hex');
                            else :
                                t=  "set \$mykallsyms=0\n".encode('hex')
                            client.send('+$%s#%02x' % (t,csum(t)));
                            continue;
                        elif testre(re.search(r'enablesb\s+(\d)',cmd),m):
                            enablesb = int(m[0].groups()[0],0);
                            client.send('+$OK#9a');
                            continue;
                        elif testre(re.search('debug\s+(\d)',cmd),m):
                            debuginfo = int(m[0].groups()[0],0)
                            client.send('+$OK#9a');
                            continue;
                        elif testre(re.search('enable64\s+(\d)',cmd),m):
                            enable64 = int(m[0].groups()[0],0)
                            client.send('+$OK#9a');
                            continue;
                        elif testre(re.search('python\s+',cmd),m):
                           try:
                            exec(cmd[7:]);    
                            client.send('+$OK#9a');
                           except Exception, e:
                            client.send('+$E14#aa');
                           continue;

                    elif 'swapper_pg_dir' in sym:  
                      try:
                        if testre(re.search(r'\$(m|M)([^,]+)(,[^#]+)#',buf),m):
                            cmd=m[0].groups()[0]
                            addr=int(m[0].groups()[1],16)
                            tail=m[0].groups()[2];
                            if  (addr&0xf0000000)==0xc0000000:
                                pgd=sym['swapper_pg_dir'];
                                buf = accesshelper(cmd,addr, tail,pgd);
                            elif addr<0x80000000:
                                buf = accesshelper(cmd, addr, tail, getpgd(addr));
                        elif enablesb and testre(re.search(r'\$z0,(\w+)(,\w+)#', buf),m):
                            addr=int(m[0].groups()[0],16)
                            tail=m[0].groups()[1];
                            if(((addr)&0xf0000000)==0xc0000000):
                                buf = accesshelper("z0,",addr, tail, getpgd(addr))
                        elif enablesb and testre(re.search(r'\$Z0,(\w+)(,\w+)#',buf),m):
                            addr=int(m[0].groups()[0],16)
                            tail=m[0].groups()[1];
                            if(((addr)&0xf0000000)==0xc0000000):
                                buf = accesshelper("Z0,",addr, tail, getpgd(addr)) ;
                      except Exception, e:
                         buf = None   
        
        
                    if not buf:
                        client.send('+$E14#aa');
                        continue
                    sock.send(buf)
                    if debuginfo: print('g: ' + buf + '\n');

#except Exception,e:
#	print e 
    
    
sock.close()
client.close()
server.close()

