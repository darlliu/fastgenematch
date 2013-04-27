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
import os, sys, socket, subprocess
PORTNUM=47606
HOST=''
FGCDIR="/home/dock/workspace/yuliu/codebase/mycodes/ids/"


TRANSTABLE={
        }

class fgc(object):
    def __init__(self, execname="fgc.exe", binfilename=None, argv=None):
        if not binfilename or not argv:
            raise IOError
        return
    def pipe (self, another):
        pass;
    def pass (self, msg=None):
        pass;
    def get (self, msg=None):
        pass;
    def kill(self):
        pass;

class host(object):
    def __init__(self):
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
        return;

    def do(self):
        """
        main routine
        """
        con,add = self.socket.accept(1)
        if not con:
            return
        self.data=con.recv(4096).strip().split("\n")
        commands= self.data[0].split()
        if len(commands)>1:
            goodcmd=[]
            if "validate" in commands:
                goodcmd+=["VALIDATE"]
            if "from" in commands:
                self.from=commands[commands.index("from")+1];
            if "to" in commands:






