//Goals:
//1, implement a non blocking IO manager, this will be trivial
//2, implement a pipe manager to figure out which entry and exit process to call
//3, implement a telnet server, again trivial

//subprocess = require ( 'child_process' )
addon = require('./addon')
net = require ( 'net' )
http = require ('http')
os = require ( 'os' )
fs = require ( 'fs' )
bf = require ('buffer')
stream = require( 'stream' )
async = require ('async')
express= require ('express')
//imports
PORTNUM=47606
HTTPPORT=3030
HOST=''
//EXEPATH='/home/dock/workspace/yuliu/codebase/mycodes/ids/fgc_proc.exe'
//TMPPATH='/tmp/fgc'
PAGEPATH='/home/dock/workspace/yuliu/codebase/mycodes/ids/portal.html'
FORMATEXECPATH='/home/dock/workspace/yuliu/codebase/mycodes/ids/format.py'
DBPATH='/var/preserve/genemapping/db/'
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
//fs.exists(EXEPATH, function (exists){
    //if (!exists){
        //process.exit(-1);
        //throw Error("Coluld not find executable!");
    //}
//})
//fs.exists(TMPPATH, function (exists){
    //if (!exists){
        //console.log("temp dir not created, creating");
        //fs.mkdir(TMPPATH);
    //}
//})
fs.readdir(DBPATH,function (err, files){
    for (var idx=0; idx<files.length; idx++){
        if (/.kch/.test(files[idx])){
            new fgc(files[idx]);
        }
    }
})
//globals
function fgc (binname) {
    // main class handling the service
    if (!/kch/.test(binname)){
        console.error('Error loading', binname)
        return null;
    }
    var self=this;//???
    var db =  new addon.KCHash(DBPATH+binname);
    var nametmp = binname.split('.')[0].split('__');
    this.from = nametmp[1];
    this.to = nametmp[nametmp.length-1];
    //start the process

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

    this.format = function (key){
        key=key.toLowerCase();
        for (var x in TRANSTABLE){
            if ((new RegExp(x)).test(key)){
                key = key.replace(x,TRANSTABLE[x]);
            }
        }
        return key;
    };

    this._in = function (msg,callback){
        var outs = [];
        msgs=msg.split("\n");
        for (var i = 0 ; i < msgs.length; i++)
        {
            var key = msgs[i];
            var val = db.get(key);
            if (val=='NA') outs.push("N/A");
            else outs.push(val);
        }
        return callback(outs.join("\n"));
    };

    this._v = function (msg, callback){
        var outs = [];
        msgs=msg.split("\n");
        for (var i = 0 ; i < msgs.length; i++)
        {
            var key = msgs[i];
            var val = db.get(key);
            if (val=='NA') outs.push("N/A");
            else outs.push(key);
        }
        return callback(outs.join("\n"));
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
        if (self.from=="OFFICIAL_GENE_SYMBOL") msg=self.format(msg);
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

    console.log('Now added a child process, from ' + this.from + ' to '
                + this.to + ' type ' + this.order);
}
console.log('begin service at '+ process.pid)
server = net.createServer(function(c){
    console.log('Connection established at ', c.address())
    //c.setEncoding('utf-8')
    //get strings
    var BUF=[];
    var len=-1;
    c.on('data',function(data){
        BUF.push(data);
    })
    //get the data
    var aTimer = setInterval(function(){
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
        var source='OFFICIAL_GENE_SYMBOL'
        var target='UNIPROT_ACCESSION'
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
            //if (target == 'uniprot'){
                //if (/mouse/.test(lines[0])) target ='mouse';
                //else target = 'human';
            //}
            lines.splice(0,1);
        }
        console.log('source target is ', source ,target)
        var total=lines.length;
        c.emit('ready', source, target , lines);
        c.emit('count', total)
    })
    var callback = function (recall, line, data){
        delete _data, tempdata;
        var _data=[];
        //c.write(line+","+data);
        //return recall(null);
        if (data.trim()==""){
            c.write(line+","+"N/A\n");
            return recall(null);
        }
        //if (/\n/.test(data)){
        var tempdata=data.split('\n');
        //console.log('split is '+data)
        for (var i=0; i<tempdata.length; i++){
            //console.log(data[i])
            if (tempdata[i].trim() == "")
                continue;
            if (/\f/.test(tempdata[i])){
                //console.log(data[i].split('\f'))
                _data=_data.concat(tempdata[i].split('\f'));
                //console.log("_data is ", _data)
            }
            else
                _data.push(tempdata[i]);
        }
        //}         //console.log("_data is ", _data)

        //truncate output to one only, can be changed
        try{
            for (var j = 0; j<_data.length; j ++){
                //console.log('iterating at '+ _data[j]);
                if (_data[j].trim()== "") continue;
                c.write(line+','+_data[j]+'\n');
            }
            return recall(null);
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
                    //console.log('going to',ext.from, ext.to)
                    flag=false;
                }
            }
            if (flag) for (var i=0; i< MIDDLES.length; i++) {
                var mid=MIDDLES[i];
                //if we are already at some exit node
                if (mid.from==source){
                    //console.log('going to',mid.from, mid.to)
                    mid.map(target, callback.bind(null,recall, line), line )
                    flag=false;
                }
            }
            if (flag) for (var i=0; i< ENTRIES.length; i++) {
                var ent=ENTRIES[i];
                if (ent.from == source){
                    //console.log('going to',ent.from, ent.to)
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
//a separate http server that does the following:
//send out static http
//listen for POST
//send message to and back from TCP server
//print info
app = express()
var WelcomeMSG="Hello, this page is currently empty.";
WelcomeMSG= fs.readFileSync(PAGEPATH,'utf8');
app.use(express.compress());
app.use(express.bodyParser());
//app.use(express.csrf())
app.get('/',function(req,res){
    console.log("HTTP client logged.")
    res.send(WelcomeMSG);
})
app.get('/which',function(req,res){
    res.send("The tcp server is on localhost:"+PORTNUM);
})
app.post('/post_action',function(req,res){
    var tempcmd="from "+req.body.from + " to "+ req.body.to +req.body.species+ "\n";
    //console.log(req.files)
    if (req.files==undefined || req.files.fcontent["size"]==0){
        var tsymbols = req.body.content.split("\n");
        tsymbols=tsymbols.concat(["FIN"]);
    } else {
        var tsymbols=fs.readFileSync(req.files.fcontent.path,'utf8').split("\n");
    }
    var symbols=[];
    for (var i = 0; i<tsymbols.length; i++){
        symbols=symbols.concat(tsymbols[i].trim().split("\t"));
    }
    symbols=symbols.join("\n")
    var content = tempcmd + symbols;
    var client = net.connect ({port:PORTNUM},function (){
        client.write(content);
    });
    var msg=""
    client.on('data',function(data){
        msg+=data;
    })
    client.on('end', function (){
        res.set('Content-Type','text/plain');
        if (msg=="")
            res.send('Transaction terminated with no results, check your settings.');
        else
            res.send(msg);
    })
})
app.listen(HTTPPORT)
