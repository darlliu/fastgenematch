#!/auto/igb-libs/linux/centos/6.x/x86_64/pkgs/python/2.6.8/bin/python
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
import os, sys, socket, subprocess,time
PORTNUM=47606
HOST=''
FGCDIR="/home/dock/workspace/yuliu/codebase/mycodes/ids/"
BINDIR="/home/dock/workspace/yuliu/codebase/mycodes/ids/bin"
FGCEXE=FGCDIR+"fgc_proc.exe"
if not os.path.exists(FGCEXE):
    raise IOError

TRANSTABLE={
    u"¦Á":"a",
    u"¦Â":"b",
    u"¦Ã":"g",
    u"¦Ä":"d",
    u"¦Å":"e",
    u"¦Æ":"t",
    u"¦Ç":"e",
    u"¦È":"theta",
    u"¦Ê":"k",
    u"¦Ë":"l",
    u"¦Ì":"mu",
    u"¦Í":"nu",
    u"¦Î":"xi",
    u"¦¨":"theta",
    u"¦Ð":"pi",
    u"¦Ó":"t",
    u"¦Õ":"phi",
    u"¦µ":"phi",
    u"¦Ö":"chi",
    u"¦×":"psi",
    u"¦¸":"o",
    u"¦Ø":"o",
    u"_":"",
    u"-":""
}

class fgc(object):
    def __init__(self, execname="fgc.exe", binfilename=None, log=None):
        if not binfilename:
            raise IOError
        self.format1=-1;
        self.format2=-1;
        self.proc=subprocess.Popen(["bind", binfilename], bufsize=4096, executable=FGCEXE,
                                   stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=log)
        #opens a slave process with relatively large bufsize
        self.pollread()
        self.which()
        #make sure process is alive and sane
        return
    def __del__(self):
        self.kill();

    def which(self):
        """
        returns the format code of the bin bucket
        """
        self.proc.stdin.write("tell")
        msgs=self.pollread().split("\n")
        #read cleanly
        if msgs==None:
            raise IOError
            self.kill();
        return (int(msgs[0]), int(msgs[1]))
    def pollread(self):
        """
        simple polling function
        """
        t1=time.time()
        msg=None
        while True:
            t2=time.time()
            if t2-t1>15:
                return None
            msg=self.proc.stdout.read()
            if len (msg.strip())>1:
                break;
            else:
                time.sleep(0.05)
        return msg;
    def pipe (self, another):
        msg=self.pollread();
        if msg==None:
            raise IOError
        another.get(msg)
        return another

    def __or__(self, another):
        return self.pipe(another)

    def give (self, msg=None):
        """
        give output in list
        """
        msg=self.pollread()
        return msg.split("\n")
    def get (self, msg=None):
        """
        get input as a string
        """
        self.proc.stdin.write(msg)
        self.proc.stdin.flush()
        return
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
        if ("fgc_geneid_genesym" not in toload or
        "fgc_geneid_uniprot" not in toload or
        "fgc_uniprot_geneid" not in toload or
        "fgc_swissprot_swissprot" not in toload):
            raise IOError
        else:
            self.exit_uniprot=fgc("fgc_geneid_uniprot")
            self.exit_genesym=fgc("fgc_geneid_genesym")
            self.exit_swissprot=fgc("fgc_swissprot_swissprot")
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
        self.MSG="--Sending--\n"
        con,add = self.socket.accept(1)
        if not con:
            return
        self.data=con.recv(4096).strip().split("\n")
        commands = self.process_data()
        #do something
        con.send(self.MSG)
        return

    def process_data(self):
        commands= self.data[0].split()
        goodcmd=[]
        self.format1=None
        self.format2=None
        if len(commands)>1:
            doform=False
            if "validate" in commands:
                goodcmd+=["VALIDATE"]
            if "from" in commands:
                self.format1=commands[commands.index("from")+1];
                if self.format1=="genesym":
                    doform=True
            if "to" in commands:
                self.format2=commands[commands.index("to")+1]
            if "format" in commands:
                doform=True;
            if doform:
                #begin format routine
                for i in xrange(1,len(commands)):
                    raw=commands[i]
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
                                o=u.encode("ascii","ignore")
                            out+=o;
                        out="".join([i for i in out if i.isalnum()])
                        #these are stripped clean to alphanumericals
                    else:
                        out="".join([i for i in raw if i.isalnum()]).lower()
                    commands[i]=out;
            goodcmd+=["DO"]
            commands = goodcmd + commands[1:]+[""]
        else:
            goodcmd=["DO"]
            commands=goodcmd+commands+[""]
        if not self.format2:
            self.format2="swissprot"
        return commands
        #we have assembled the input now
        #now simply choose which process to try








