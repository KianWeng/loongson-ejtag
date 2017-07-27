import re
import sys

f=open('ddr3.txt' if len(sys.argv)==1 else sys.argv[1])
l =f.readline()
p={}

while l:
 m = re.findall(r'0x900000000ff00(\S+)\s+(\S+)',l)
 if m:
   p[m[0][0]]=m[0][1]
 l =f.readline()
f.close()

p["018"] = p["018"][:-1]+'0'

f=open('Targets/Bonito3c780e/Bonito/loongson3C_ddr_param.S');
l =f.readline()
while l:
 if re.match('MC0_DDR3_RDIMM_CTRL_0x',l):
  l=re.sub('MC0_DDR3_RDIMM_CTRL_0x([^:]+):\s+.dword\s+(\S+)', lambda x: 'MC0_DDR3_RDIMM_CTRL_0x'+ x.groups()[0] + ': .dword ' + p[x.groups()[0]], l)
 sys.stdout.write(l)
 l =f.readline()
  

