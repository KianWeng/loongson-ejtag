#!/usr/bin/python
import sys
import gzip
import StringIO
import tempfile
tmpdir=None
f=open(len(sys.argv)>1 and sys.argv[1] or 'vmlinux','rb')
m=f.read()
i=0
while i<len(m):
 i=m.find('\x1f\x8b\x08',i)
 if i<0 : break
 gz=gzip.GzipFile(fileobj=StringIO.StringIO(m[i:]))
 try:
  buf=''
  buf=gz.read()
 except Exception,e:
  pass
 finally:
  if buf or gz.extrasize:
   if not tmpdir:
     tmpdir = tempfile.mkdtemp()+'/'
   fname=tmpdir+str(i)+'-'+str(gz.extrasize+len(buf))
   print(fname)
   f=open(fname,'wb')
   if buf:
     f.write(buf)
   if gz.extrasize:
     f.write(gz.extrabuf)
   f.close()

   i+=gz.fileobj.tell()
  else :
   i+=3
 

