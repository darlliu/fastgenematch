//Goals:
//1, implement a non blocking IO manager, this will be trivial
//2, implement a pipe manager to figure out which entry and exit process to call
//3, implement a telnet server, again trivial

subprocess = require ( 'child_process' )
net = require ( 'net' )
http = require ('http')
os = require ( 'os' )
fs = require ( 'fs' )
//imports
PORTNUM=47606
HOST=''
EXEPATH='/home/dock/workspace/yuliu/codebase/mycodes/ids/fgc_proc.exe'
FORMATEXECPATH='/home/dock/workspace/yuliu/codebase/mycodes/ids/format.py'
BINPATH='/home/dock/workspace/yuliu/codebase/mycodes/ids/bin/'
ENTRIES=[];
MIDDLES=[];
EXITS=[];
TRANSTABLE={
    "α":"a",
    "β":"b",
    "γ":"c",
    "δ":"d",
    "ε":"e",
    "ζ":"t",
    "η":"e",
    "θ":"theta",
    "κ":"k",
    "λ":"l",
    "μ":"mu",
    "ν":"nu",
    "ξ":"xi",
    "Θ":"theta",
    "π":"pi",
    "τ":"t",
    "φ":"phi",
    "Φ":"phi",
    "χ":"chi",
    "ψ":"psi",
    "Ω":"o",
    "ω":"o",
    "alpha":"a",
    "beta":"b",
    "kappa":"k",
    "lambda":"l",
    "delta":"d",
    "gamma":"c",
    "_":"",
    "-":""
}

CALLBACK = function (data){
    console.log('GLOBAL DUMMY CALLBACK', ''+data);
    return
};

//globals
function fgc (binname) {
    // main class handling the service
    if (!/bin/.test(binname)){
        console.error('Error loading', binname)
        return null;
    }
    var nametmp = binname.split('.')[0].split('_');
    var bound=false;
    this.from = nametmp[1];
    this.to = nametmp[nametmp.length-1];
    //start the process
    var proc= subprocess.execFile(EXEPATH);
    this.pid=proc.pid;
    proc.on('error', function(err){
        throw error('Error spawning', proc.pid);
    })
    __init_proc = function () {
        proc.stdin.write('bind\n');
        proc.stdin.write(BINPATH+binname+'\n');
        var callback = function (data){
            console.log('STDERR:',binname,''+data+'\n');
            if(/BOUND/.test(''+data)){
                console.log('Sucessfully bound', binname);
                bound = true;
                proc.stderr.removeListener('data', callback);
            }
        }
        proc.stderr.on('data', callback)
        console.log('Waiting...')
    }() ; //construct once

    if (/entry/.test(binname)){
        this.order = 'entry';
        ENTRIES.push(this);
    } else if ( /exit/.test(binname) ){
        this.order = 'exit';
        EXITS.push(this);
    } else {
        this.order = 'middle';
        MIDDLES.push(this);
    } //add to global stores
    console.log('Now added a process, from ' + this.from + ' to '
                + this.to + ' type ' + this.order);

    this.format = function (key){
        for (var x in TRANSTABLE){
            if ((new RegExp(x)).test(key)){
                return key.replace(x,TRANSTABLE[x]);
            }
        }
        return key;
    };

    this._in = function (msg,cb){
        proc.stdin.write('DO\n'+msg+'\n\n');
        var callback = function (data){
            //now we expect the stderr to output something
            return cb(data);
        };
        proc.stdout.once('data', callback);
        return null;
    };

    this._v = function (msg, cb){
        proc.stdin.write('VALIDATE\nDO\n'+msg+'\n\n');
        var callback = function (data){
            //now we expect the stderr to output something
            return cb(data);
        };
        proc.stdout.once('data', callback);
        return null;
    };

    this.map = function (msg, target, callback) {
        //note: this assumes that the source ID of this object matches the msg
        target = typeof target !== 'undefined' ? target : 'human';
        callback = typeof callback !== 'undefined' ? callback : CALLBACK;
        if (this.from=="genesym") msg=this.format(msg);
        if (target == this.to){
            return this._in(msg, callback);
            //global call back return
        } else if (target == this.from){
            return this._v(msg, callback);
            //global call back return
        } else {
            for (var out in EXITS) {
                if (out.from != this.to) continue;
                if (out.to == target)
                    return self._in(msg,out.map.bind(null, target, callback));
            }   //first check if we map to a target
            if (this.order == 'entry') for (var mid in MIDDLES) {
                if (mid.from==this.to)
                    return self._in(msg,mid.map.bind(null, target, callback));
                    // it is now mid's job to do the above
            }
            //if all fails...
            console.log("Could not map", msg);
            return callback(null);
        }
    };
}
console.log('begin service')
server = net.createServer(function(c){
    console.log('Connection established at ', c.address())
    c.setEncoding('utf-8')
    //get strings
    BUF='';
    len=-1;
    c.on('data',function(data){

        BUF+=data;
    })
    //get the data
    setInterval(function(){
        if (len==BUF.length)
            //if no more buffer gets read
            c.emit('received')
        else len=BUF.length;
    }, 100);
    //make sure all is got, intermittance is allowed to be 100ms
    //which should be plenty
    c.setTimeout(function(){
        c.emit('end');
    }, 30000)
    //set a global timeout of 30s
    c.once('received',function(){
        var source='uniprot'
        var target='human'
        //default params
        var lines=BUF.split('\n')
        //chop up by lines
        if (/from/.test(lines[0]) || /to/.test(lines[0])){
            var commands=lines[0].split(' ')
            //now go ahead and read the commands
            var idf=commands.indexOf('from'), idt=commands.indexOf('to')
            source = idf==-1 ? source : commands[idf+1];
            target = idt==-1 ? target : commands[idt+1];
            if (target == 'uniprot'){
                if (/mouse/.test(lines[0])) target ='mouse';
                else target = 'human';
            }
            lines.splice(0,1);
        }
        total=lines.length;
        accum=0;
        for (var line in lines){
            var callback = function (data){
                c.write(line+','+data);
                accum++;
                if (accum==total) c.emit('end');
            }
            flag=true;
            for (var ext in EXITS){
                //do something
                if (ext.from==source && ext.to==target)
                    ext.map(line, target, callback )
                flag=false;
            }
            if (flag) for (var ent in ENTRIES ){
                if (ent.from == source)
                    ent.map(line, target, callback )
                flag= false;
            }
            if (flag){
                c.write(line+','+'N/A')
                accum++;
                if (accum==total) c.emit('end');
            }
        }
    })
    c.once('end', function (){
        console.log('Client disconnected')
        c.end()
    })
    //end the connection
})
server.listen(PORTNUM,function(){
    console.log('server running')
});
server.on('error', function(e){
    console.log('Error running server')
    console.log('end service')
})
