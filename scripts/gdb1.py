import re, sys, os, gdb, math
gdb.execute('set python print-stack full')

def ulong(x):
   return int(x) & longmask

def container_of(p,stype,member):
   t=gdb.lookup_type(stype).pointer()
   z=gdb.Value(0).cast(t) 
   for i in member.split('.'):
     z=z[i]
   return gdb.Value(int(p)-int(z.address)).cast(t)

pgdorder = 0

class MycmdCommand(gdb.Command):
    def accessvm(self,addr):
            addr = int(addr)
            if (addr&0xf000000000000000) == 0xc000000000000000:
                pgd = ulong(self['swapaddr'])
            elif (addr&0xffffffffc0000000) == 0xffffffffc0000000:
                pgd = ulong(self['swapaddr'])
            else :
                pgd = ulong(self['pgd'])
            pagesize = self['pagesize']
            pageshift = self['pageshift']
            pteshift = self['pteshift']
            enable64 = self['enable64']
            pgdmask = (((1+pgdorder)<<pageshift)-longsize);
            mask = ((1<<pageshift)-longsize);
            mask1 = ((1<<pageshift)-longsize*2);
    #right shift(14+1), left shit log2(2item) : 11 = (14+1-4) (pageshift+1-log2(2item)) 
    # 11 = (14-3)
    # 12-2
    
            if self['pagelevel'] == 3:
              pud=self._read_word(((addr>>((pageshift-longshift)*3))&pgdmask) + pgd);
            else :
              pud = pgd
            pmd=self._read_word(((addr>>((pageshift-longshift)*2))&mask) + pud);
            pte=self._read_word(((addr>>(pageshift-longshift)) & mask1) + pmd);
            pte1=self._read_word(((addr>>(pageshift-longshift)) & mask1) + longsize + pmd);
            self['d_pgd'] = pgd
            self['d_pud'] = pud
            self['d_pmd'] = pmd
            self['d_pmd_addr'] = ((addr>>((pageshift-longshift)*2))&mask) + pud
            self['d_pte'] = pte
            self['d_pte_addr'] = ((addr>>(pageshift-longshift)) & mask1) + pmd
            self['d_pte1'] = pte1
            self['d_pte1_addr'] = ((addr>>(pageshift-longshift)) & mask1) + longsize + pmd
    
            if addr&(1<<pageshift):
                if pte1&(2<<pteshift):
                    phyaddr = ((pte1>>(pteshift+6))<<12) + (addr&((1<<pageshift)-1))
                    if phyaddr >= 0x20000000:
                        if not enable64:
                            print("physical address is too large 0x%x" %(phyaddr))
                            return {'addr':0,'valid':False}
                        addr1 = (0x9800000000000000,0x9000000000000000)[((pte1>>(pteshift+3))&7)!=3] + phyaddr;
                    else :
                        addr1 = (ulong(0xffffffff80000000),ulong(0xffffffffa0000000))[((pte1>>(pteshift+3))&7)!=3] + phyaddr;
                else :
                	return {'addr':0, 'valid':False};
            else :
                if pte&(2<<pteshift) :
                    phyaddr = ((pte>>(pteshift+6))<<12)  + (addr&((1<<pageshift)-1))
                    if phyaddr >= 0x20000000:
                         if not enable64:
                            print("physical address is too large 0x%x" %(phyaddr))
                            return {'addr':0, 'valid':False}
                         addr1 = (0x9800000000000000,0x9000000000000000)[((pte>>(pteshift+3))&7)!=3] + phyaddr;
                    else :
                         addr1 = (ulong(0xffffffff80000000),ulong(0xffffffffa0000000))[((pte>>(pteshift+3))&7)!=3] + phyaddr;
                else :
                 	return {'addr':0, 'valid':False};
    
    
            return {'addr':addr1, 'valid':True, 'phyaddr':phyaddr, 'pte0':pte, 'pte1':pte1,'len':pagesize-(addr&(pagesize-1))};
    def fullpath(self, d):
     pname=[]
     while 1: 
      pname.insert(0,d['d_name']['name'].string())
      if int(d['d_parent']) == int(d): break
      d=d['d_parent']
     return("/".join(pname))
    def page2pfn(page):
      pgt = gdb.lookup_type('struct page').pointer()
      mem_map = gdb.parse_and_eval('mem_map')
      if mem_map:
         pfn=gdb.Value(p).cast(pgt)-mem_map
      else :
       mem_section = gdb.parse_and_eval('mem_section')
       n= mem_section.type.sizeof//mem_section[0].type.sizeof
       o = int(gdb.Value(p).cast(pgt)['flags'])//(0x100000000//n)
       pfn=gdb.Value(p).cast(pgt)-gdb.Value(mem_section[o][0]['section_mem_map']&~3).cast(pgt)
      return pfn

    def downloadfile(self,d):
      name = d['d_name']['name'].string()
      print(name)
      size=d['d_inode']['i_size']
      t=d['d_inode']['i_mapping']['page_tree']
      rnode = int(str(t['rnode']),0)
      
      indirect = (rnode&1)
      if gdb.lookup_global_symbol('init_fs').type['root'].type==gdb.lookup_type('struct dentry').pointer():
        indirect=not indirect
      if indirect:
       stack=[]
       h = int(t['height'])
       nd = rnode&~1
       stack.append([nd,h])
       nodes=[]
       
       while len(stack):
         n,h=stack.pop(0)
         i=0
         node = gdb.Value(n).cast(t['rnode'].type)
         while 1:
           n1= node['slots'][i]
           if n1:
            if h>1:
             stack.insert(i,[n1,h-1])
            else:
             nodes.append(n1)
           else :
            break
           i+=1
      else :
       nodes=[rnode]
      

      pgt = gdb.lookup_type('struct page').pointer()
      mem_map = gdb.lookup_global_symbol('mem_map')
      if not mem_map or not int(mem_map.value()):
         usememmap = 0
      else :
         usememmap =1
         mem_map = mem_map.value()
      page_offset = 0x9800000000000000 if gdb.lookup_type('long').sizeof == 8 else 0x80000000
      if not usememmap:
       mem_section = gdb.parse_and_eval('mem_section')
       n= mem_section.type.sizeof//mem_section[0].type.sizeof
       sections_pgdiv = (1<<gdb.lookup_type('long').sizeof*8)//n

      pagesize = self['pagesize']
      addrs=[]
      inf = gdb.selected_inferior()

      f=open('/tmp/' + name,'wb')
      for p in nodes:
        if usememmap:
         pfn=gdb.Value(p).cast(pgt)-mem_map
        else :
         o = int(gdb.Value(p).cast(pgt)['flags'])//sections_pgdiv
         pfn=gdb.Value(p).cast(pgt)-gdb.Value(mem_section[o][0]['section_mem_map']&~3).cast(pgt)
        a=page_offset+pfn*pagesize
        addrs.append(a)
        m = inf.read_memory(a, pagesize if size>pagesize else size)
        f.write(m.tobytes())
        size -= pagesize
         
      f.close()
    def __init__(self):
        gdb.Command.__init__(self, 'mycmd', gdb.COMMAND_DATA,
                             gdb.COMPLETE_COMMAND, True)
        global longmask, longsize, longshift
        longsize = gdb.lookup_type('unsigned long').sizeof
        longmask = ((1<<longsize*8)-1)
        longshift = int(math.log(longsize, 2))
        self.data={}
        self.data['char_ptr_t'] = gdb.lookup_type("unsigned char").pointer()
        self.data['char_ptr_ptr_t'] = self.data['char_ptr_t'].pointer()
        self.data['pagelevel'] = 2 if longsize==4 else 3
        self.data['enable64'] = 1 if longsize==8 else 0
  
    def _read_word(self, address):
        return ulong(gdb.Value(address).cast(self['char_ptr_ptr_t']).dereference())
    def ls(self, arg, from_tty):
       dirs=list(filter(len,arg.split('/')))
       fs=gdb.parse_and_eval('init_fs')
       if fs['root'].type==gdb.lookup_type('struct dentry').pointer():
         rd = fs['root']
       else :
         rd = fs['root']['dentry'] 
       t = rd['d_subdirs']['next']
       while int(t)!=int(rd['d_subdirs'].address):
        a = int(t) - int(gdb.Value(0).cast(rd.type)['d_u']['d_child'].address)
        d=gdb.Value(a).cast(rd.type)
        name = d['d_name']['name'].string()
        if not dirs:
           print(name)
        elif name == dirs[0]:
          
           if ((d['d_inode']['i_mode']>>12)&0xf) != 4:
            print(d['d_name']['name'])
           else :
             dirs.pop(0)
             rd = d
             t = rd['d_subdirs']['next']
             continue
        t=t['next']
    def get(self, arg, from_tty):
       dirs=list(filter(len,arg.split('/')))
       fs=gdb.parse_and_eval('init_fs')
       if fs['root'].type==gdb.lookup_type('struct dentry').pointer():
         rd = fs['root']
       else :
         rd = fs['root']['dentry'] 
       t = rd['d_subdirs']['next']
       while int(t)!=int(rd['d_subdirs'].address):
        a = int(t) - int(gdb.Value(0).cast(rd.type)['d_u']['d_child'].address)
        d=gdb.Value(a).cast(rd.type)
        name = d['d_name']['name'].string()
        if not dirs:
           print('missing file name')
           return
        elif name == dirs[0]:
          
           if ((d['d_inode']['i_mode']>>12)&0xf) != 4:
            self.downloadfile(d)
            return
           else :
             dirs.pop(0)
             rd = d
             t = rd['d_subdirs']['next']
             continue
        t=t['next']
    def linuxver(self):
        if 'ver' not in self.data:
           linux_banner=gdb.lookup_global_symbol('linux_banner').value()
           l=list(map(int,re.findall(r'Linux version (\d)\.(\d+)\.(\d+)',str(linux_banner))[0]))
           self.data['linuxver'] = (l[0]<<16)|(l[1]<<8)|l[2]
        return self.data['linuxver']
    def getpagesize(self):
        if 'pagesize' not in self.data:
          pagesize = 0x4000
          if self.linuxver()>=0x20600:
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
          self.data['pagesize'] = pagesize
          try:
           gdb.execute("monitor setpgsize " + str(mycmd['pagesize']))
          except gdb.error:
           pass
        return self.data['pagesize']

    def getcurrent(self,cpuno=0):
     kernelsp = gdb.lookup_global_symbol('kernelsp').value()
     if self['linuxver']>=0x20600:
       current = gdb.Value(kernelsp[cpuno]&~(gdb.lookup_type('union thread_union').sizeof-1)).cast(gdb.lookup_type('struct thread_info').pointer())
       if current:
         task = current['task']
       else :
         task = None
     else :
       current = gdb.Value(kernelsp[cpuno]&~(gdb.lookup_type('union thread_union').sizeof-1)).cast(gdb.lookup_type('struct task_struct').pointer())
       task = current
     return (current,task)

    def task(self):
           init_task = gdb.lookup_global_symbol('init_task').value().address
           task= self['curtask']
           curtask=task
           while task:
             if not init_task.type.has_key('stack'):
               tinfo = task['thread_info']
               uregs = gdb.Value(int(task['thread_info'])+gdb.lookup_type('union thread_union').sizeof-32-gdb.lookup_type('struct pt_regs').sizeof).cast(gdb.lookup_type('struct pt_regs').pointer())
             else :
               tinfo = task['stack'].cast(gdb.lookup_type('struct thread_info').pointer())
               uregs = gdb.Value(int(task['stack'])+gdb.lookup_type('union thread_union').sizeof-32-gdb.lookup_type('struct pt_regs').sizeof).cast(gdb.lookup_type('struct pt_regs').pointer())
    
    
             sys.stdout.write("%d %d %s ra=%#x sp=%#x epc=%#x usp=%#x task=%#x tinfo=%#x uregs=%#x stat=%#x\n" %(task['pid'], task['parent']['pid'], task['comm'].string(),ulong(task['thread']['reg31']),ulong(task['thread']['reg29']),ulong(uregs['cp0_epc']),ulong((uregs['regs'][29])),ulong(task), ulong(tinfo), ulong(uregs), ulong(task['state'])))
          
             t = task['thread_group']['next']
             if t==curtask['thread_group'].address or not t:
                break
             task = container_of(t, 'struct task_struct', 'thread_group')

    def tasks(self,show=1):
      init_task = gdb.lookup_global_symbol('init_task').value().address
      kernelsp = gdb.lookup_global_symbol('kernelsp').value()

      curtasks = []
      atasks = {}
      for i in  range(kernelsp.type.sizeof//kernelsp[0].type.sizeof):
             current  = self.getcurrent(i)[1]
             if not current: break
             curtasks.append(current)

      p = init_task['tasks'].address

      while p:
       task= gdb.Value(int(p)-int(gdb.Value(0).cast(init_task.type)['tasks'].address)).cast(init_task.type)
       curtask=task
       while task:
         if not init_task.type.has_key('stack'):
           tinfo = task['thread_info']
           uregs = gdb.Value(int(task['thread_info'])+gdb.lookup_type('union thread_union').sizeof-32-gdb.lookup_type('struct pt_regs').sizeof).cast(gdb.lookup_type('struct pt_regs').pointer())
         else :
           tinfo = task['stack'].cast(gdb.lookup_type('struct thread_info').pointer())
           uregs = gdb.Value(int(task['stack'])+gdb.lookup_type('union thread_union').sizeof-32-gdb.lookup_type('struct pt_regs').sizeof).cast(gdb.lookup_type('struct pt_regs').pointer())
         atasks[int(task['pid'])]=[task,tinfo,uregs]


         if show:
           if task in curtasks:
                 sys.stdout.write("*<%d> "%curtasks.index(task))
           sys.stdout.write("%d %d %s ra=%#x sp=%#x epc=%#x usp=%#x task=%#x tinfo=%#x uregs=%#x stat=%#x\n" %(task['pid'], task['parent']['pid'], task['comm'].string(),ulong(task['thread']['reg31']),ulong(task['thread']['reg29']),ulong(uregs['cp0_epc']),ulong((uregs['regs'][29])),ulong(task), ulong(tinfo), ulong(uregs), ulong(task['state'])))
      
         t = task['thread_group']['next']
         if t==curtask['thread_group'].address or not t:
            break
         task = gdb.Value(int(t) - int(gdb.Value(0).cast(task.type)['thread_group'].address)).cast(task.type)
       p=p['next']
       if p==init_task['tasks'].address:
         break
      self['tasks']=atasks
      self['curtasks'] = curtask
    def probepte(self):
        pteshift = 6
        for i in range(0x30,0x80,4):
            ins = self._read_word(ulong(0xffffffff80000000+(longsize==8)*0x80)+i);
            #srl
            if ((ins & 0xffe0003f) == 2):
                pteshift = ((ins>>6)&0x1f);
            #dsrl
            if ((ins & 0xffe0003f) == 0x3a):
                pteshift = ((ins>>6)&0x1f);
            #mtc0
            if ((ins & 0xffe00000) == 0x40800000):
                break;
        self.data['pteshift'] = pteshift
    def __getitem__(self,key):
        if key in self.data: return self.data[key]
        elif key.startswith('_'):
           self.data[key] = None
        elif key == 'linuxver': return self.linuxver()
        elif key == 'tasks':
           self.tasks(False)
        elif key == 'curtasks':
           self.tasks(False)
        elif key == 'curtask':
           task = self.data['curtask'] = self.getcurrent()[1]
           pgd = task['mm']['pgd'] if task['mm'] else self['swapaddr']
           self.data['pgd'] = pgd
        elif key == 'pgd':
           task=self['curtask']   
        elif key == 'pagesize': self.getpagesize()
        elif key == 'pageshift':
           self.data['pageshift'] = int(math.log(self['pagesize'],2))
        elif key == 'pteshift': self.probepte()
        elif key == 'swapaddr': self.data['swapaddr'] = gdb.lookup_global_symbol('swapper_pg_dir').value().address 
        else : raise Exception('error no key')
        return self.data[key]

    def __setitem__(self,key,val):
        self.data[key] = val
        if key == 'curtask':
           task = val
           pgd = task['mm']['pgd'] if task['mm'] else self['swapaddr']
           self.data['pgd'] = pgd

    def pidsel(self,pid):
        self['curtask'] = self['tasks'][pid][0]
        pgd = self['pgd']
        gdb.execute("monitor setpgd %#x\n" % ulong(pgd))

    def modules24(self):
      mod = gdb.lookup_symbol('module_list')[0].value()
      while mod and mod['next']:
       s=os.popen("./tools/load-module24" + (".mips64" if (longsize==8) else "") + "\tmodules/%s.o %#x" %(mod['name'].string(), ulong(mod))).read()
       print(s)
       gdb.execute(s)
       mod=mod['next']

    def modules0(self, modpath='modules'):
      modules = gdb.lookup_symbol('modules')[0].value().address
      #modules = gdb.Value(0xffffffff80b533c8).cast(gdb.lookup_symbol('modules')[0].type.pointer())
      modlst = modules['next']
      modtype=gdb.lookup_type('struct module').pointer()
      mcnt=0
      while modlst != modules:
        mod= gdb.Value(int(modlst)-int(gdb.Value(0).cast(modtype)['list'].address)).cast(modtype)
        s= "add-symbol-file\t" + modpath +"/%s.ko 0 " % (mod['name'].string())
        attrs=mod['sect_attrs']
        nsections = mod['sect_attrs']['nsections']
        n=0
        while n<nsections:
         attrsn = mod['sect_attrs']['attrs'][n]
         s+= "-s %s %#x " % ( attrsn['name'].string(),ulong(attrsn['address']))
         n=n+1
        gdb.execute(s)
        modlst = modlst['next']
    
    def modules(self,modpath='modules'):
      kallsyms = gdb.lookup_global_symbol('module_kallsyms_lookup_name')!=None
      modules = gdb.lookup_symbol('modules')[0].value().address
      modlst = modules['next']
      modtype=gdb.lookup_type('struct module').pointer()
      while modlst != modules:
        mod= gdb.Value(int(modlst)-int(gdb.Value(0).cast(modtype)['list'].address)).cast(modtype)
        s=os.popen("./tools/load-module\tmodules/%s.ko %#x %#lx %d" %(mod['name'].string(), ulong(mod['module_core']), ulong(mod['module_init']), self['linuxver']<0x2061f and kallsyms)).read()
        gdb.execute(s)
        modlst = modlst['next']
    def vma(self,pid=-1):
      if pid == -1: task = self['curtask']
      else : task = self['tasks'][pid][1]
      if task['mm']:
        t=task['mm']['mmap']
        while t:
          gdb.write("%#x-%#x %c%c%c%c %x " % ( ulong(t['vm_start']),ulong(t['vm_end']), t['vm_flags']&1 and 'r' or '-',t['vm_flags']&2 and 'w' or '-',t['vm_flags']&4 and 'x' or '-',t['vm_flags']&8 and 's' or '-',ulong(t['vm_pgoff'])*16*1024))
          if t['vm_file']:
            inode = t['vm_file']['f_path']['dentry']['d_inode']
            gdb.write("%02x:%02x %lu %s"%( ulong(inode['i_sb']['s_dev'])>>20, ulong(inode['i_sb']['s_dev'])&0xfffff, inode['i_ino'],self.fullpath(t['vm_file']['f_path']['dentry'])))
          gdb.write('\n')
          t = t['vm_next']
      gdb.write('\n')
    def fds(self):
      task = self['curtask']
      files = task['files']
      fdt = files['fdt']
      for i in range(int(fdt['max_fds'])):
        file = fdt['fd'][i]
        if file:
          gdb.write('%d\t->\t%s\n'%(i,self.fullpath(file['f_path']['dentry']))) 

    def interrupts(self):
      kernelsp = gdb.lookup_global_symbol('kernelsp').value()
      irq_desc = gdb.lookup_global_symbol('irq_desc').value()
      __per_cpu_offset = gdb.lookup_global_symbol('__per_cpu_offset')
      cpus = kernelsp.type.sizeof//kernelsp[0].type.sizeof
      nr_cpu_ids = gdb.lookup_global_symbol('nr_cpu_ids')
      if nr_cpu_ids: cpus = int(nr_cpu_ids.value())
      nirqs = irq_desc.type.sizeof//irq_desc[0].type.sizeof
      for i in range(nirqs):
        if not irq_desc[i]['action'] or not irq_desc[i]['kstat_irqs']: continue
        gdb.write('%d: ' % i)
        for j in range(cpus):
          if __per_cpu_offset:
            per_cpu_offset = __per_cpu_offset.value()[j]
            kstat_irqs = gdb.Value(int(irq_desc[i]['kstat_irqs'])+per_cpu_offset).cast(irq_desc[i]['kstat_irqs'].type).dereference()
            gdb.write('%d ' % kstat_irqs)
          else :
            gdb.write('%d ' % irq_desc[i]['kstat_irqs'][j])
        if self['linuxver'] >= 0x30000:
          gdb.write('%s %s\n' %(irq_desc[i]['irq_data']['chip']['name'].string(), irq_desc[i]['action']['name'].string()))
        else :
          gdb.write('%s %s\n' %(irq_desc[i]['chip']['name'].string(), irq_desc[i]['action']['name'].string()))

    def regs_save(self):
     if self['_regs']: return

     fm = gdb.newest_frame()
     regs={}
     for i in range(32):
       regs[i] = fm.read_register(str(i))
     regs['pc'] = fm.read_register('pc')
     self['_regs'] = regs

    def regs_restore(self):
     if not self['_regs']: return
     for i in range(32):
       gdb.execute('set $r%d=%#x'%(i,ulong(self['_regs'][i]))) 
     gdb.execute('set $pc=%#x'%(ulong(self['_regs']['pc']))) 
     self['_regs'] = None

    def switch_panic(self):
     self.regs_save()
     regs={}
     names={0:'zero',1:'at',2:'v0',3:'v1',4:'a0',5:'a1',6:'a2',7:'a3',8:'a4',9:'a5',10:'a6',11:'a7',12:'t0',
     13:'t1',14:'t2',15:'t3',16:'s0',17:'s1',18:'s2',19:'s3',20:'s4',21:'s5',22:'s6',23:'s7',24:'t8',25:'t9',26:'k0',27:'k11',
     28:'gp',29:'sp',30:'s8',31:'ra',37:'pc'}
     
     l=sys.stdin.readline()
     while l:
       m=re.search('(\d+)\s+:',l)
       if m:
          idx=int(m.groups()[0], 0)
          m=re.findall('[0-9a-f]{8,}',l)
          for i in m:
           regs[idx] = i
           idx +=1
       m=re.search('epc\s+:\s+([0-9a-f]{8,})',l)
       if m:
          regs[37] = m.groups()[0]
     
       l = sys.stdin.readline()
     
     for i in names:
        if i in regs:
           print('set $' +names[i]+'=0x'+regs[i])
           #gdb.execute('set $' +names[i]+'=0x'+regs[i])


    def switch_exception(self, pregs):
     self.regs_save()
     kr={1:'at',2:'v0',3:'v1',4:'a0',5:'a1',6:'a2',7:'a3',8:'a4',9:'a5',10:'a6',11:'a7',12:'t0',13:'t1',14:'t2',15:'t3',16:'s0',17:'s1',18:'s2',19:'s3',20:'s4',21:'s5',22:'s6',23:'s7',24:'t8',25:'t9',26:'k0',27:'k11',28:'gp',29:'sp',30:'s8',31:'ra',37:'pc'}

     pt_regs = gdb.parse_and_eval('((struct pt_regs *)%#x)'%pregs)

     for i in range(1,32):
       gdb.execute('set $%s=%#x'%(kr[i],ulong(pt_regs['regs'][i]))) 
     gdb.execute('set $pc=%#x'%(ulong(pt_regs['cp0_epc']))) 

    def switch_kernel(self):
     self.regs_save()
     kr = {31:'pc',29:'sp',30:'s8',23:'s7',22:'s6',21:'s5',20:'s4',19:'s3',18:'s2',17:'s1',16:'s0'}

     for i in kr:
       gdb.execute('set $%s=%#x'%(kr[i],ulong(self['curtask']['thread']['reg'+str(i)]))) 

    def switch_user(self):
     self.regs_save()
     pid=int(self['curtask']['pid'])
     uregs = self['tasks'][pid][2]

     for i in range(32):
       gdb.execute('set $r%d=%#x'%(i,ulong(uregs['regs'][i]))) 
     gdb.execute('set $pc=%#x'%(ulong(uregs['cp0_epc']))) 

    def cmdline(self):
      mm = self['curtask']['mm']
      if not mm: return
      arg_start = mm['arg_start']
      arg_end = mm['arg_end']
      arg_len = arg_end - arg_start

      env_start = mm['env_start']
      env_end = mm['env_end']
      env_len = env_end - env_start
      va=self.accessvm(arg_start)
      if not va['valid']: return

      ve=self.accessvm(env_start)
      if not ve['valid']: return

      inf = gdb.selected_inferior()
      m = inf.read_memory(va['addr'], arg_len)
      buf = m.tobytes()
      gdb.write('args:\n'+str(buf)+'\n')
      m=inf.read_memory(ve['addr'], env_len)
      buf = m.tobytes()
      gdb.write('envs:\n'+str(buf)+'\n')


try:
  from gdb.unwinder import Unwinder
  
  class FrameId(object):
  
      def __init__(self, sp, pc):
          self._sp = sp
          self._pc = pc
  
      @property
      def sp(self):
          return self._sp
  
      @property
      def pc(self):
          return self._pc
  
  
  class TestUnwinder(Unwinder):
  
      def __init__(self):
          Unwinder.__init__(self, "test unwinder")
          self.char_ptr_t = gdb.lookup_type("unsigned char").pointer()
          self.char_ptr_ptr_t = self.char_ptr_t.pointer()
  
      def _read_word(self, address):
          return address.cast(self.char_ptr_ptr_t).dereference()
  
      def __call__(self, pending_frame):
          """Test unwinder written in Python.
  
          This unwinder can unwind the frames that have been deliberately
          corrupted in a specific way (functions in the accompanying
          py-unwind.c file do that.)
          This code is only on AMD64.
          On AMD64 $RBP points to the innermost frame (unless the code
          was compiled with -fomit-frame-pointer), which contains the
          address of the previous frame at offset 0. The functions
          deliberately corrupt their frames as follows:
                       Before                 After
                     Corruption:           Corruption:
                  +--------------+       +--------------+
          RBP-8   |              |       | Previous RBP |
                  +--------------+       +--------------+
          RBP     + Previous RBP |       |    RBP       |
                  +--------------+       +--------------+
          RBP+8   | Return RIP   |       | Return  RIP  |
                  +--------------+       +--------------+
          Old SP  |              |       |              |
  
          This unwinder recognizes the corrupt frames by checking that
          *RBP == RBP, and restores previous RBP from the word above it.
          """
          try:
              # NOTE: the registers in Unwinder API can be referenced
              # either by name or by number. The code below uses both
              # to achieve more coverage.
              if not mycmd['_bt_kernel'] and not mycmd['_bt_user']:
                 return None
              mycmd['_bt_kernel'] = None
              pc = mycmd['curtasks']['thread']['reg31']
              sp = mycmd['curtasks']['thread']['reg29']
              s8 = mycmd['curtasks']['thread']['reg30']
              s7 = mycmd['curtasks']['thread']['reg23']
              s6 = mycmd['curtasks']['thread']['reg22']
              s5 = mycmd['curtasks']['thread']['reg21']
              s4 = mycmd['curtasks']['thread']['reg20']
              s3 = mycmd['curtasks']['thread']['reg19']
              s2 = mycmd['curtasks']['thread']['reg18']
              s1 = mycmd['curtasks']['thread']['reg17']
              s0 = mycmd['curtasks']['thread']['reg16']
  
  
              frame_id = FrameId(
                  pending_frame.read_register('sp'),
                  pending_frame.read_register('pc'))
              unwind_info = pending_frame.create_unwind_info(frame_id)
              unwind_info.add_saved_register('pc', pc)
              unwind_info.add_saved_register('sp', sp)
              unwind_info.add_saved_register('s8', s8)
              unwind_info.add_saved_register('s7', s7)
              unwind_info.add_saved_register('s6', s6)
              unwind_info.add_saved_register('s5', s5)
              unwind_info.add_saved_register('s4', s4)
              unwind_info.add_saved_register('s3', s3)
              unwind_info.add_saved_register('s2', s2)
              unwind_info.add_saved_register('s1', s1)
              unwind_info.add_saved_register('s0', s0)
              return unwind_info
          except (gdb.error, RuntimeError):
              return None
  
  gdb.unwinder.register_unwinder(None, TestUnwinder(), True)
except Exception:
  pass


class MycmdCommand_ls(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd ls', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.ls(arg,from_tty)

class MycmdCommand_get(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd get', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.get(arg,from_tty)

class MycmdCommand_task(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd task', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.task()

class MycmdCommand_tasks(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd tasks', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.tasks()

class MycmdCommand_modules(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd modules', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.modules()

class MycmdCommand_modules0(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd modules0', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.modules0()

class MycmdCommand_modules24(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd modules24', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.modules24()

class MycmdCommand_vma(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd vma', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.vma()

class MycmdCommand_pidsel(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd pidsel', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)
    def invoke(self, arg, from_tty):
        self.pidsel(int(arg,0))

class MycmdCommand_fds(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd fds', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.fds()

class MycmdCommand_interrupts(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd interrupts', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.interrupts()

class MycmdCommand_bt_panic(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd bt_panic', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.switch_panic()
        gdb.execute('bt')

class MycmdCommand_bt_exception(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd bt_exception', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        if arg:
          self.switch_exception(int(arg,0))
          gdb.execute('bt')

class MycmdCommand_bt_kernel(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd bt_kernel', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        if not arg:
          self.switch_kernel()
          gdb.execute('bt')
        else :
          for i in self['tasks']:
            self.pidsel(i)
            print('%d: %s\n' % (i,self['curtask']['comm'].string()))
            self.switch_kernel()
            gdb.execute('bt')

class MycmdCommand_bt_user(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd bt_user', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        if not arg:
          self.switch_user()
          gdb.execute('bt')
        else :
          for i in self['tasks']:
            self.pidsel(i)
            print('%d: %s\n' % (i,self['curtask']['comm'].string()))
            self.switch_user()
            gdb.execute('bt')

class MycmdCommand_save(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd save', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.regs_save()

class MycmdCommand_restore(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd restore', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.regs_restore()


class MycmdCommand_cmdline(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd cmdline', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        self.cmdline()

class MycmdCommand_vm(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd vm', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        v=self.accessvm(int(arg,0)) 
        for i in v:
          gdb.write(i+':0x%x, ' % v[i])
        gdb.write('\n')

class MycmdCommand_savevm(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd savevm', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        argv = gdb.string_to_argv(arg)
        argc = len(argv)
        if argc!=3: return
        vaddr = int(argv[1],0)
        vlen = int(argv[2],0)

        f=open(argv[0],'wb')
        inf = gdb.selected_inferior()
        
        while vlen: 
         v=self.accessvm(vaddr) 
         if not v['valid']: break
         once = min(v['len'], vlen); 
         m = inf.read_memory(v['addr'], once)
         f.write(m.tobytes())
         vlen -= once
         vaddr += once
        f.close()

class MycmdCommand_probe_pagesize(MycmdCommand):
    def __init__(self,mycmd):
        self.data = mycmd.data
        gdb.Command.__init__(self, 'mycmd probe_pagesize', gdb.COMMAND_DATA,
                             gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        if 'pagesize' in mycmd.data:
          del mycmd.data['pagesize']
        print('pagesize is ' + str(mycmd['pagesize']))

def cont_handler(event):
    print('cont')
    if 'tasks' in mycmd.data:
      mycmd.regs_restore()
      del mycmd.data['tasks']
      del mycmd.data['curtasks']

mycmd = MycmdCommand()
MycmdCommand_ls(mycmd)
MycmdCommand_get(mycmd)
MycmdCommand_task(mycmd)
MycmdCommand_tasks(mycmd)
MycmdCommand_modules(mycmd)
MycmdCommand_modules0(mycmd)
MycmdCommand_modules24(mycmd)
MycmdCommand_vma(mycmd)
MycmdCommand_pidsel(mycmd)
MycmdCommand_fds(mycmd)
MycmdCommand_interrupts(mycmd)
MycmdCommand_bt_panic(mycmd)
MycmdCommand_bt_exception(mycmd)
MycmdCommand_bt_kernel(mycmd)
MycmdCommand_bt_user(mycmd)
MycmdCommand_save(mycmd)
MycmdCommand_restore(mycmd)
MycmdCommand_cmdline(mycmd)
MycmdCommand_vm(mycmd)
MycmdCommand_savevm(mycmd)
MycmdCommand_probe_pagesize(mycmd)
gdb.events.cont.connect(cont_handler)

