/*
 * =====================================================================================
 *
 *       Filename:  fastgenematch.cpp
 *
 *    Description:  Source file matching header
 *
 *        Version:  0.0
 *        Created:  4/20/2013 4:17:34 PM
 *       Revision:  none
 *       Compiler:  gcc/clang/msvc
 *
 *         Author:  Yu Liu (yul13@uci.edu)
 *
 * =====================================================================================
 */
#include"fastgenematch.h"




namespace fastgenematch
{
    /* Not used
     *Geneconverter::Geneconverter()
     *{
     *    greeks["α"]="alpha";
     *    greeks["β"]="alpha";
     *    greeks["γ"]="gamma";
     *    greeks["δ"]="delta";
     *    greeks["ε"]="epsilon";
     *    greeks["ζ"]="tao";
     *    greeks["η"]="eta";
     *    greeks["θ"]="theta";
     *    greeks["κ"]="kappa";
     *    greeks["λ"]="lambda";
     *    greeks["μ"]="mu";
     *    greeks["ν"]="nu";
     *    greeks["ξ"]="xi";
     *    greeks["Θ"]="theta";
     *    greeks["π"]="pi";
     *    greeks["τ"]="tao";
     *    greeks["φ"]="phi";
     *    greeks["Φ"]="phi";
     *    greeks["χ"]="chi";
     *    greeks["ψ"]="psi";
     *    greeks["Ω"]="omega";
     *    greeks["ω"]="omega";
     *    greeks["_"]="";
     *    greeks["-"]="";
     *};
     *std::string
     *Geneconverter::format(const std::string& key)
     *{
     *    std::string out("");
     *    std::string temp(key);
     *    for (auto it:temp)
     *    {
     *        try
     *        {
     *            out+=greeks.at(it);
     *        }
     *        catch(...)
     *        {
     *            out+=it;
     *        }
     *    }
     *    return out;
     *}
     */
//decided this is for python wrapper

                            /*
                             *      Container
                             */

    uint32_t Hashcaller::seed=SEED;
    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  operator[],()
     *  Description:  access operator overloads, note the default behavior change for easy
     *                validation
     * =====================================================================================
     */
    std::string&
    Geneobject::operator[] (const std::string& key)
    {
        try
        {
            return data->at(key);
        }
        catch (std::out_of_range& err)
        {
            return empty;
        }
    };
    std::string&
    Geneobject::operator() (const std::string & key)
    {
        if (data->size()>=(size_t)length*0.8) rehash();
        //automatically rehash when adding stuff.
        return (*data)[key];
    }
    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  rehash
     *  Description:  rehash the table, doubling size and reseeding.
     *                this process is automatically done every so often
     * =====================================================================================
     */
    void
    Geneobject::rehash()
    {
        length*=2;
        reseed();
        hashtable temp(new _hashtable);
        temp->reserve(length);
        for (auto it:(*data))
        {
            (*temp)[it.first]=it.second;
        }
        data=temp;
    };

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  serialize, load
     *  Description:  serialize the hashtable
     * =====================================================================================
     */
    void
    Geneobject::serialize(const std::string& name)
    {
        std::ofstream f;
        f.open(name,std::ofstream::binary);
        if (f.good())
        {
            f.write((char*) &formats.first, sizeof(int));
            f.write((char*) &formats.second,sizeof(int));
            f.write((char*) &seed, sizeof(seed));
            f.write((char*) &length, sizeof(length));
            size_t s=data->size();
            f.write((char*) &(s), sizeof(size_t));
            for (auto it: *data)
            {
                size_t len1=it.first.size(),len2=it.second.size();
                f.write((char*) &len1, sizeof(size_t));
                f.write((char*) it.first.c_str(), len1+1);
                f.write((char*) &len2, sizeof(size_t));
                f.write((char*) it.second.c_str(), len2+1);
            }
            f.close();
        }
        else f.close();
    };

    void
    Geneobject::load(const std::string& name)
    {
        std::ifstream f;
        f.open(name,std::ifstream::binary);
        if (f.good())
        {
            f.read((char*) &formats.first, sizeof(int));
            f.read((char*) &formats.second, sizeof(int));
            f.read((char*) &seed, sizeof(seed));
            f.read((char*) &length, sizeof(length));
            data->reserve(length);
            size_t s;
            f.read((char*) &s, sizeof(size_t));
            for (size_t i=0; i<s; i++)
            {
                size_t len1,len2;
                char* first, *second;
                //get two holders;
                f.read((char*) &len1, sizeof(size_t));
                first=new char[len1+1];
                f.read((char*) first, len1+1);
                f.read((char*) &len2, sizeof(size_t));
                second=new char[len2+1];
                f.read((char*) second, len2+1);
                std::string key(first), value(second);
                (*data)[key]=value;
                delete first,second;
            }
            f.close();
        }
        else f.close();
    };

                        /*
                         *      Converter
                         */

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  initialize
     *  Description:  initialize class
     * =====================================================================================
     */
    void
    Genematch_converter::initialize()
    {
        param.inputname="";
        param.outputname="";
        param.inputformat="other";
        param.outputformat="other";
        lookup["genesym"]=genesym;
        lookup["emsemble_id"]=emsemble_id;
        lookup["emsemble"]=emsemble_id;
        lookup["unigene_id"]=unigene_id;
        lookup["uniprot"]=uniprot;
        lookup["swissprot"]=swissprot;
        lookup["uniprot_crick"]=uniprot_crick;
        lookup["crick"]=uniprot_crick;
        lookup["string"]=string;
        lookup["other"]=allowed;
        table.reseed();
    }

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  do_convert
     *  Description:  call convert routine
     * =====================================================================================
     */
    void
	Genematch_converter::do_convert()
    {
        try
        {
            table.formats.first=lookup[param.inputformat];
        }
        catch(std::out_of_range &err)
        {
            table.formats.first=-2;
        }
        try
        {
            table.formats.second=lookup[param.outputformat];
        }
        catch(std::out_of_range &err)
        {
            table.formats.second=-2;
        }
        title="fgc_"+param.inputformat+"_"+param.outputformat+".bin";
        table.serialize(title);
    }
    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  operator<<
     *  Description:  populate the table via input file and output formatted results
     *                this is slow but okay given the rare application
     * =====================================================================================
     */
    std::istream&
    Genematch_converter::operator<<(std::istream& in)
    {
        if (!in.good()) return in;
        char line[256],key[128],value[128];
        //impose line limit of 256, it really should be just around 25
        while (!in.eof())
        {
            in.getline(line,256);\
            if (line[0]=='\0') continue;
            //does not allow null char arrays
			std::istringstream ss(line);
            ss.getline(key,128,'\t');
            ss.getline(value,128,'\t');
            if (table[key]=="")
            {
                table(key)=value;
            }else{
                table(key)+=","+std::string(value);
                //accumulative
            }
        }
        do_convert();
        return in;
    };

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  operator>>
     *  Description:  output key,value pair;
     * =====================================================================================
     */
    std::ostream&
    Genematch_converter::operator>>(std::ostream& out)
    {
        for (auto it: *(table.data))
        {
            out<<it.first<<"\t"<<it.second<<std::endl;
        }
        return out;
    };
                        /*
                         *      Matcher
                         */
}
