#!/auto/igb-libs/linux/centos/6.x/x86_64/pkgs/python/2.6.8/bin/python
import os, sys, socket
PORTNUM=47606
HOST='nebula-2.ics.uci.edu'
def convertIDs ( fromType="OFFICIAL_GENE_SYMBOL", toType="UNIPROT_ACCESSION", species="",
        asType="pairs", ids=None):
    if ids is None:
        print "Please input list of ids or text of ids"
        return None
    if type(ids)==str:
        ids=[i.strip() for i in ids.split("\n") if i.strip()!=""]
        ids=[i.split("\t") for i in ids]
        ids = [i for j in ids for i in j if i!=""]
        #grabs formatted tsv
    if type (ids)==list:
        ids=[str(i) for i in ids if str(i)!=""]
    if species:
        cmd = "from "+ fromType + " to " + toType+"_"+species
    else:
        cmd = "from "+ fromType + " to " + toType
    msg = cmd+ "\n" + "\n".join(ids)+"\nFIN"
    try:
        sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.connect((HOST,PORTNUM))
        sock.sendall(msg)
        buf=""
        while True:
            tbuf=sock.recv(1024)
            if not tbuf:
                break
            buf+=tbuf
        if asType=="pairs":
            results = [(i.split(",")[0],i.split(",")[1]) for i in buf.split("\n") if "," in i]
        elif asType == "tabs":
            results = [i.split(",")[0]+"\t"+i.split(",")[1] for i in buf.split("\n") if "," in i]
            results = "\n".join(results)
        return results
    except socket.error as err:
        print "Error occured: " + repr(err)
        return None

if __name__ == "__main__":
    sys.stderr.write(
        "Usage: input from and to types (optional) and the actual message, get tsv back"
            +   "\nStart input please, terminate with CTRL-D"
        )
    ids=sys.stdin.read()
    if len (sys.argv)==2:
        sys.stderr.write("Wrong number of parameters.")
    elif len(sys.argv)==1:
        print convertIDs(ids=ids,asType="tabs")
    elif len(sys.argv)>=3:
        print convertIDs(fromType=sys.argv[1],toType=sys.argv[2],asType="tabs",ids=ids)
