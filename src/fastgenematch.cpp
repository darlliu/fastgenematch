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
        param.outputname="fgc_"+param.inputformat+"_"+param.outputformat+".bin";
        table.serialize(param.outputname);
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

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  print_help,info
     *  Description:  print help info
     * =====================================================================================
     */

    void
    Genematcher::print_help()
    {
        std::string info=" Fast Gene Converter v0.0 -- Converts or validates one ID to another\n\
                          -C -[V fi fo] inputformat outputformat [outputfile] (inputfile|inputstream)\n\
                          -[v V fi fo] [outputfile] (inputfile|inputstream)\n\
                          To view more, type (thisexe) -V\
						  ";
        std::string detail= "Global Options:\n\
                            -V Verbose, shows more information and a summary.\n\
                            -fo File out, output to file instead of standard output\n\
                                -> outputfile required filename when -fo is selected.\n\
                            -fi File in, inputs file name instead of text content \n\
                                -> inputfile required input file name when -fi is selected.\n\
                            inputstream contents from stdin to be used to convert or match/validate.\n\
                            \n\
                            -C convert mode, without this part defaults to match/validate mode.\n\
                            Note: In this mode, the input must be two tab separated pairs, from key to value, and each pair separated by lines\n\
                            Options under -C only:\n\
                                inputformat, outputformat, string of formats for conversion, currently available.\n\
                            \n\
                            Options not under -C:\n\
                                -v Validate mode, not available in convert mode, validate the input symbols,\
                                return the same IDs if the IDs match up and return empty string otherwise\n\
                            Note: In this mode, the input must be line separated IDs to be matched/validated.\
							";
        if (settings.verbose)
            std::cout<<info<<std::endl<<detail<<std::endl;
        else std::cout<<info<<std::endl;
    }

    void
    Genematcher::print_settings (  )
    {
        std::clog<<"Current settings"<<std::endl;
        std::clog<<"Convert mode: "<<std::boolalpha<<settings.convert<<std::endl;
        std::clog<<"Verbose mode: "<<std::boolalpha<<settings.verbose<<std::endl;
        std::clog<<"Get input from file: "<<std::boolalpha<<settings.filein<<std::endl;
        std::clog<<"Write output to file: "<<std::boolalpha<<settings.fileout<<std::endl;
        std::clog<<"Input format: "<<param.inputformat<<std::endl;
        std::clog<<"Output format: "<<param.outputname<<std::endl;
        std::clog<<"Input filename: "<<settings.fname<<std::endl;
        std::clog<<"Automatically generated output name: "<<param.outputname<<std::endl;
        return ;
    }		/* -----  end of function print_info  ----- */

}
