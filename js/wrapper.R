convertIDList2<- function(input, fromType="OFFICIAL_GENE_SYMBOL", toType="UNIPROT_ACCESSION",
    species="", host="nebula-3.ics.uci.edu", port=47606){
    #Note: Species is either HUMAN, MOUSE or as default.
    msg<-paste0("from ",fromType, " to ",toType,species);
    c<-c(msg,input);
    con<-socketConnection(host=host, port=port, server=F, open="r+", blocking=T);
    writeLines(c, con);
    output<-readLines(con);
    close(con);
    accum<-unlist(strsplit(output,","))
    if (species==""){
        accum<-matrix(accum,nrow=length(output),ncol=2,byrow=TRUE,dimnames=list(c(),c("From","To")));
    } else {
        accum<-matrix(accum,nrow=length(output),ncol=2,byrow=TRUE,dimnames=list(c(),c("From",species)));
    }
    return (accum);
}

