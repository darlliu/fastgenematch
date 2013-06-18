convertIDList2<- function(input, fromType="genesym", toType="human",
        host="nebula-3.ics.uci.edu", port=47606){
    msg<-paste0("from ",tolower(fromType), " to ",tolower(toType));
    c<-c(msg,input);
    con<-socketConnection(host=host, port=port, server=F, open="r+", blocking=T);
    writeLines(c, con);
    output<-readLines(con);
    close(con);
    return (output);
}


