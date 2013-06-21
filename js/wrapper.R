doconvertIDList2<- function(input, fromType="OFFICIAL_GENE_SYMBOL", toType="UNIPROT_ACCESSION",
    host="nebula-3.ics.uci.edu", port=47606){
    #Note: Species is either HUMAN, MOUSE or as default.
    if (toType=="UNIPROT_ACCESSION"){
        msg<-paste0("from ",fromType, " to ",toType,"_SPECIES");
        #only uniprot conversion will contain species information
    } else {
        msg<-paste0("from ",fromType, " to ",toType);
    }
    c<-c(msg,input);
    con<-socketConnection(host=host, port=port, server=F, open="r+", blocking=T);
    writeLines(c, con);
    output<-readLines(con);
    close(con);
    accum=c()
    cnt = 0;
    for (i in 1:length(output)){
        s<-unlist(strsplit(output[i], ","));
        if (length(s)>=3 && toType=="UNIPROT_ACCESSION"){
            if (s[3]=="10090") {
                s[3]="MOUSE";
            }
            if (s[3]=="9606") {
                s[3]="HUMAN";
            }
        }
        if (s[2]=="N/A") {
            accum <- c(accum, s[1],s[2],"N/A")
            cnt = cnt +1;
        } else {
            accum <- c(accum,s)
        }
    }
    accum<-unlist(accum);
    if (toType=="UNIPROT_ACCESSION"){
        accum<-matrix(accum,nrow=length(output),ncol=3,byrow=TRUE,dimnames=list(c(),c("From","To","Species")));
    } else {
        accum<-matrix(accum,nrow=length(output),ncol=2,byrow=TRUE,dimnames=list(c(),c("From","T")));
    }
    if (cnt > 0.5*length(output)){
        return (convertIDList(input, fromType, toType));
    } else {
        return (accum);
    }
}
convertIDList2<- function(input, fromType="OFFICIAL_GENE_SYMBOL", toType="UNIPROT_ACCESSION",
        host="nebula-3.ics.uci.edu", port=47606){
    tryCatch(doconvertIDList2(input,fromType,toType,host,port),
            error = function(e) return (convertIDList(input,fromType,toType))
            )
}
