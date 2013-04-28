#!/auto/igb-libs/linux/centos/6.x/x86_64/pkgs/python/2.6.8/bin/python
# -*- coding: utf-8 -*-
# host for fgc.exe
# listens to port PORTNUM and do the following
# 0, maintains multiple subprocesses of fgc.exe each loaded
#    some bin files
# 1, listens for verify or match command, defaults to match
#    also listens for input output command of match
# 2, listens for a list of ID
# 3, formats the ID if necessary, get targeted input/output
# 4, If formats specified and found convert as is.
# 5, otherwwise checks the first few words and automatically convert if found
import os, sys, socket, subprocess, time, select
PORTNUM=47606
HOST=''
FGCDIR="/home/dock/workspace/yuliu/codebase/mycodes/ids/"
BINDIR="/home/dock/workspace/yuliu/codebase/mycodes/ids/bin/"
FGCEXE=FGCDIR+"fgc_proc.exe"
if not os.path.exists(FGCEXE):
    raise IOError

TRANSTABLE={
    u"α":"a",
    u"β":"b",
    u"γ":"g",
    u"δ":"d",
    u"ε":"e",
    u"ζ":"t",
    u"η":"e",
    u"θ":"theta",
    u"κ":"k",
    u"λ":"l",
    u"μ":"mu",
    u"ν":"nu",
    u"ξ":"xi",
    u"Θ":"theta",
    u"π":"pi",
    u"τ":"t",
    u"φ":"phi",
    u"Φ":"phi",
    u"χ":"chi",
    u"ψ":"psi",
    u"Ω":"o",
    u"ω":"o",
    u"_":"",
    u"-":""
}

class fgc(object):
    def __init__(self, binfilename=None, log=None):
        if not binfilename:
            raise IOError
        self.format1=-1;
        self.format2=-1;
        self.set_timeout()
        self.proc=subprocess.Popen([FGCEXE], shell=True,
                                   stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=log)
        #opens a slave process with relatively large bufsize
        self.proc.stdin.write("bind\n")
        self.proc.stdin.write(BINDIR+binfilename+"\n")
        self.ping()
        #make sure process is alive and sane
        return
    def __del__(self):
        self._get("TERMINATETERMINATE\n")
        self._get("TERMINATETERMINATE\n")
        self.proc.kill();

    def ping(self):
        """
        returns the format code of the bin bucket
        serves as a sanity check tool
        """
        #read cleanly
        msgs=None
        t1=time.time()
        while (msgs==None):
            time.sleep(1)
            self.proc.stdin.write("tell\n")
            self.proc.stdin.flush()
            msgs=self.pollread()
            t2=time.time()
            if t2-t1>self.timeout:
                break
        if msgs==None:
            sys.exit()
            #forcibly exit when sanity is not guaranteed
        msgs=msgs.split("\n")[0].split("\t")
        self.format1,self.format2=msgs[0],msgs[1]
        return (int(msgs[0]), int(msgs[1]))
    def set_timeout(self,timeout=10):
        self.timeout=timeout
    def pollread(self):
        """
        simple polling function
        from nest of heliopolis
        """
        msg=None
        time.sleep(0.1)
        #we wait a little while for output to be available
        def read():
            poll=select.poll()
            poll.register(self.proc.stdout.fileno(), select.POLLIN or select.POLLPRI)
            fd = poll.poll(self.timeout)
            if len (fd):
                f=fd[0]
                if f[1]>0:
                    return self.proc.stdout.read(1)
        c=read()
        if c != None:
            msg=""
            while c != None:
                msg+=str(c)
                c=read()
        print repr(msg)
        return msg
    def pipe (self, another):
        msg=self.pollread();
        if msg==None:
            msg=""
        another._get(msg)
        return another

    def __or__(self, another):
        return self.pipe(another)

    def give (self, msg=None):
        msg=self.pollread()
        if not msg:
            return ""
        else:
            return msg
    def get(self,msg=None):
        self.proc.stdin.write("\n".join(["DO"]+msg)+"\n\n")
        self.proc.stdin.flush()
        return

    def _get (self, msg=None):
        self.proc.stdin.write("DO\n"+msg+"\n\n")
        #pad the message so it goes through
        #note that this does not affect the validate
        #routine
        self.proc.stdin.flush()
        return

    def verify (self, entries=None):
        """
        issue a validate command with entries
        """
        msg="\n".join(entries)
        self.proc.stdin.write("VALIDATE\n"+msg+"\n\n")
        out=self.pollread()
        if out:
            return out.split("\n")
        else:
            return ""

    def kill(self):
        self.proc.kill()
        return

class host(object):
    """
    simple tcp ip host for the converter
    """
    def __init__(self):
        self.entries=[];
        self.exits=[]
        self.entry_uniprot=None
        self.entry_genesym=None
        self.exit_uniprot=None
        self.exit_genesym=None
        self.exit_swissprot=None
        #these three are held in memory running because they
        #are typical exit nodes, if some other conversion is needed
        #that will be done on an ondemand basis
        try:
            self.socket=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error as emsg:
            print repr(emsg)
            return
        try:
            self.socket.bind((HOST,PORTNUM))
        except socket.error as emsg:
            print repr(emsg)
            return
        toload=[]
        for fname in os.listdir(BINDIR):
            if ".bin" in fname:
                toload.append(fname)
        print toload
        if ("fgc_geneid_genesym.bin" not in toload or
        "fgc_geneid_uniprot.bin" not in toload or
        "fgc_uniprot_geneid.bin" not in toload or
        "fgc_genesym_geneid.bin" not in toload or
        "fgc_swissprot_swissprot.bin" not in toload):
            raise IOError
        else:
            self.exit_uniprot=fgc("fgc_geneid_uniprot.bin")
            self.exit_genesym=fgc("fgc_geneid_genesym.bin")
            self.exit_swissprot=fgc("fgc_swissprot_swissprot.bin")
            self.entry_genesym=fgc("fgc_genesym_geneid.bin")
            self.entry_uniprot=fgc("fgc_uniprot_geneid.bin")
        for some in toload:
            if "entry" in some:
                self.entries.append(some)
            elif "exit" in some:
                self.exits.append(some)
        return;

    def do(self):
        """
        main routine
        """
        self.socket.listen(1)
        self.MSG="--Sending--\n"
        con,add = self.socket.accept()
        if not con:
            return
        self.data=con.recv(4096).strip().split("\n")
        commands = self.process_data()
        print commands, len(commands)
        #do something

        #if input is set, grab input if unless it is geneid
        #if not found or input not set, try out
        proc=None
        if self.format1:
            if self.format1=="geneid":
                proc=None
                #no need to pipe
            else:
                if self.format1=="uniprot":
                    proc=self.entry_uniprot
                elif self.format1=="genesym":
                    proc=self.entry_genesym
                for sth in self.entries:
                    if self.format1 in sth:
                        proc=fgc(sth)
                        break
                if not proc:
                    proc,_=self.tryinput(commands)
        else:
            proc=self.entry_uniprot
            #if nothing is found, use uniprot anyway
        if self.validate:
            if not proc:
                out=self.exit_swissprot.verify(commands)
                self.MSG+="\n".join(out)
                self.MSG+="--Done--"
                con.send(self.MSG)
                return;
            else:
                out=proc.verify(commands)
                self.MSG+="\n".join(out)
                self.MSG+="--Done--"
                con.send(self.MSG)
                return;

        proc2=None
        if self.format2:
            if self.format2=="genesym":
                proc2=self.exit_genesym
            elif self.format2=="swissprot" or self.format2=="uniprot":
                pass;
            else:
                for sth in self.exits:
                    if self.format2 in sth:
                        proc2=fgc(sth)
                        break
        #now if output format is not set, assume swissprot

        #now do the conversion finally!
        if not proc:
            if proc2:
                proc2.get(commands)
                self.MSG+=proc2.give()
            else:
                self.exit_uniprot.get(commands)
                self.MSG+=(self.exit_uniprot|self.exit_swissprot).give()
        else:
            proc.get(commands)
            if proc2:
                self.MSG+=(proc|proc2).give()
            else:
                self.MSG+=(proc|self.exit_uniprot|self.exit_swissprot).give()
        self.MSG+="--Done--"
        con.send(self.MSG)
        print proc, proc2
        con.close()
        return

    def tryinput(self, commands):
        for i in self.entries:
            with fgc(i) as proc:
                output=proc.verify(commands)
                if len(output)>1:
                    return (proc, output)
                else:
                    continue;
        return (None, None)

    def __del__(self):
        """
        make sure connection is closed, this should be handled gracefully
        """
        self.entry_genesym.kill()
        self.entry_uniprot.kill()
        self.exit_genesym.kill()
        self.exit_uniprot.kill()
        self.exit_uniprot.kill()
        self.socket.close()
        return
    def process_data(self):
        commands= self.data[0].split()
        self.validate=False
        self.format1=None
        self.format2=None
        if len(commands)>1:
            doform=False
            if "validate" in commands:
                self.validate=True
            if "from" in commands:
                self.format1=commands[commands.index("from")+1];
                if self.format1=="genesym":
                    doform=True
            if "to" in commands:
                self.format2=commands[commands.index("to")+1]
            if "format" in commands:
                doform=True;
                self.format1="genesym"
            if doform:
                #begin format routine
                for j in xrange(1,len(self.data)):
                    raw=self.data[j]
                    out=""
                    u=None
                    try:
                        u=raw.decode("utf-8")
                    except:
                        try:
                            u=raw.decode("utf-16")
                        except:
                            u=None
                    if u:
                        for char in u:
                            if char in TRANSTABLE.keys():
                                o=TRANSTABLE[char]
                            else:
                                o=char.encode("ascii","ignore")
                            out+=o;
                        out="".join([i for i in out if i.isalnum()])
                        #these are stripped clean to alphanumericals
                    else:
                        out="".join([i for i in raw if i.isalnum()]).lower()
                    self.data[j]=out;
                self.data[0]=","
        else:
            pass
        if not self.format2:
            self.format2="swissprot"
        return self.data
        #we have assembled the input now
        #now simply choose which process to try


if __name__ == "__main__":
    a=host()
    while (True):
        a.do()

