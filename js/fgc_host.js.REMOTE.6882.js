//Goals:
//1, implement a non blocking IO manager, this will be trivial
//2, implement a pipe manager to figure out which entry and exit process to call
//3, implement a telnet server, again trivial

subprocess = require ( 'child_process' )
net = require ( 'net' )
http = require ('http')
os = require ( 'os' )
fs = require ( 'fs' )
bf = require ('buffer')
stream = require( 'stream' )
async = require ('async')
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
fs.exists(EXEPATH, function (exists){
    if (!exists){
        process.exit(-1);
        throw Error("Coluld not find executable!");
    }
})
fs.readdir(BINPATH,function (err, files){
    for (var idx=0; idx<files.length; idx++){
        if (/.bin/.test(files[idx])){
            new fgc(files[idx]);
        }
    }
})


//globals
function fgc (binname) {
    // main class handling the service
    if (!/bin/.test(binname)){
        console.error('Error loading', binname)
        return null;
    }
    var self=this;//???
    var nametmp = binname.split('.')[0].split('_');
    var bound=false;
    this.from = nametmp[1];
    this.to = nametmp[nametmp.length-1];
    //start the process
    var proc= subprocess.execFile(EXEPATH,[],{maxBuffer: 50000*1024});
    this.pid=proc.pid;
    proc.on('error', function(err){
        throw error('Error spawning', proc.pid);
    })
    var __init_proc = function () {
        proc.stdin.write('bind\n');
        proc.stdin.write(BINPATH+binname+'\n');
        var callback = function (data){
            console.log('STDERR:',binname,''+data+'\n');
            if(/BOUND/.test(''+data)){
                console.log('Sucessfully bound', binname);
                bound = true;
                proc.stderr.removeListener('data', callback);
                proc.stdout.setEncoding('utf8')
            }
        }
        proc.stderr.on('data', callback)
    }; //construct once
    __init_proc();

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
        key=key.toLowerCase();
        for (var x in TRANSTABLE){
            if ((new RegExp(x)).test(key)){
                key = key.replace(x,TRANSTABLE[x]);
            }
        }
        return key;
    };
    proc.stdout.on('error',function(){
        throw Error("STDOUT ERROR AT " + binname)
    });

    proc.stdin.on('error',function(){
        throw Error("STDIN ERROR AT " + binname)
    });
    this._in = function (msg,callback){
        //block the next attemp
        //console.log('msg is', 'DO\n'+msg+'\n\n')
        //console.log('Calling child at ', binname)
        if (proc.stdin.write('DO\n'+msg+'\n\n')){
        } else {
            console.log('write failed')
            proc.stdin.on('drain', function(){
                processoc.stdin.write('DO\n'+msg+'\n\n')
            })
        }
        proc.stdout.once('data',callback)
        return null;
    };

    this._v = function (msg, callback){
        //console.log('validating at',binname)
        if (proc.stdin.write('VALIDATE\nDO\n'+msg+'\n\n')){
        } else {
            console.log('write failed at ', self.pid)
            proc.stdin.on('drain', function(){
                proc.stdin.write('VALIDATE\nDO\n'+msg+'\n\n');
            })
        }
        proc.stdout.once('data',callback)
        return null;
    };

    this.map = function (target, callback, msg) {
        //console.log('mapping at',binname)
        //console.log('msg is', msg)
        //console.log('target is', target)
        //console.log('callback is', callback)
        //note: this assumes that the source ID of this object matches the msg
        target = typeof target !== 'undefined' ? target : 'human';
        callback = typeof callback !== 'undefined' ? callback : CALLBACK;
        msg = typeof msg !== 'undefined' ? msg : "N/A";
        if (self.from=="genesym") msg=self.format(msg);
        msg=msg.trim().split('\f').join('\n')
        if (target == self.to){
            return self._in(msg, callback);
            //global call back return
        } else if (target == this.from){
            return self._v(msg, callback);
            //global call back return
        } else {
            for (var i=0; i<EXITS.length; i++) {
                out=EXITS[i];
                //console.log('this is ', self.to, 'next is ', out.from)
                if (out.from != self.to) continue;
                if (out.to == target){
                    return self._in(msg,out.map.bind(out, target, callback));
                    break;
                }
            }   //first check if we map to a target
            if (self.order == 'entry') for (var i=0; i< MIDDLES.length; i++) {
                mid=MIDDLES[i];
                //console.log('this is ', self.to, 'next is ', mid.from)
                if (mid.from==self.to)
                    {
                        //console.log('calling a midpoint')
                        return self._in(msg,mid.map.bind(mid, target, callback));
                        break;
                    }
                    // it is now mid's job to do the above
            }
            //if all fails...
            console.log("Could not map", callback);
            return callback('N/A');
        }
    };
}
console.log('begin service at '+ process.pid)
server = net.createServer(function(c){
    console.log('Connection established at ', c.address())
    //c.setEncoding('utf-8')
    //get strings
    var BUF=[];
    len=-1;
    c.on('data',function(data){
        BUF.push(data);
    })
    //get the data
    aTimer= setInterval(function(){
        if (len==BUF.length)
            //if no more buffer gets read
            {
                clearInterval(aTimer);
                c.emit('received')
            }
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
        var buffer = bf.Buffer.concat(BUF);
        var lines=buffer.toString()
        lines=lines.split('\n');
        lines.splice(-1,1);//remove FIN pacakge
        console.log('First line is this', lines[0])
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
        console.log('source target is ', source ,target)
        var total=lines.length;
        c.emit('ready', source, target , lines);
        c.emit('count', total)
    })
    var callback = function (recall, line, data){
        data=data.split('\n')[0].split('\f')[0];
        //truncate output to one only, can be changed
        try{
            c.write(line+','+data+'\n');
        }
        catch (err){
            return recall(Error("Write Error"));
        }
        //the mapping in the middle is blocking
        return recall(null);
        //signals that the run is finished
    }
    c.once ('ready', function (source, target, lines){
        console.log('Initializing one map')
            //pass a callback closure
        var iter = function (line, recall){
            var flag=true;
            if (flag) for (var i=0; i< EXITS.length; i++) {
                var ext=EXITS[i];
                //if we are already at some exit node
                if (ext.from==source && ext.to==target){
                    ext.map(target, callback.bind(null,recall, line), line )
                    flag=false;
                }
            }
            if (flag) for (var i=0; i< MIDDLES.length; i++) {
                var mid=MIDDLES[i];
                //if we are already at some exit node
                if (mid.from==source && mid.to==target){
                    mid.map(target, callback.bind(null,recall, line), line )
                    flag=false;
                }
            }
            if (flag) for (var i=0; i< ENTRIES.length; i++) {
                var ent=ENTRIES[i];
                if (ent.from == source){
                    ent.map(target, callback.bind(null,recall, line), line )
                    flag= false;
                }
            }
            if (flag) {
                callback(recall,line, 'N/A')
            }
        }
        async.eachSeries(lines,iter,function(err){
            if (err==null){
                console.log('\n--Transaction complete--\n')
                c.emit('end')
            }
            else{
                console.log('Error occurred at the connection')
                c.destroy();
                c.emit('end');
            }
        })
    })

    c.on('error',function(err){
        c.emit('end')
    })
    c.once('end', function (){
        console.log('Client disconnected')
        c.end()
    })
    //end the connection immediately when got signal
})
server.listen(PORTNUM,function(){
    console.log('server running')
});
server.on('error', function(e){
    console.log('Error running server')
    console.log('end service')
})
