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
                delete first;
                delete second;
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
            std::cout<<it.first<<"\t"<<it.second<<std::endl;
            std::cout<<"outputing converts"<<std::endl;
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
                          -C -[V i o] inputformat outputformat (inputfile|inputstream)\n\
                          -[v V i o p] [-b binfile] (inputfile|inputstream)\n\
                          To view more, type (thisexe) -V\
						  ";
        std::string detail= "Global Options:\n\
                            -V Verbose, shows more information and a summary.\n\
                            -i File in, inputs file name instead of text content \n\
                                -> inputfile required input file name when -i is selected.\n\
                            inputstream contents from stdin to be used to convert or match/validate.\n\
                            \n\
                            -C build hashtable (convert) mode, without this part defaults to match/validate mode.\n\
                            Options under -C only:\n\
                                Required: inputformat, outputformat, string of formats for conversion.\n\
                                Required: inputfile if -i or piped in tables\n\
                            Note: In this mode, the input must be a tab separated pair, key (tab) value, and each pair separated by lines\n\
                            \n\
                            Options not under -C:\n\
                                Required: inputfile if -i or piped in tables\n\
                                -b specify bin file (otherwise default is used), then requires bin file name of some pregenerated hashtable\n\
                                -p specify whether or not the output format should contain original key value\n\
                                -o File out, output to file instead of standard output\n\
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
        std::clog<<"Binary file provided: "<<std::boolalpha<<settings.bin<<std::endl;
        std::clog<<"Verbose mode: "<<std::boolalpha<<settings.verbose<<std::endl;
        std::clog<<"Get input from file: "<<std::boolalpha<<settings.filein<<std::endl;
        std::clog<<"Write output to file: "<<std::boolalpha<<settings.fileout<<std::endl;
        std::clog<<"Input format: "<<param.inputformat<<std::endl;
        std::clog<<"Output format: "<<param.outputformat<<std::endl;
        std::clog<<"Input filename: "<<settings.fname<<std::endl;
        std::clog<<"Automatically generated output name: "<<param.outputname<<std::endl;
        return ;
    }		/* -----  end of function print_info  ----- */


    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  read
     *  Description:  read input options
     * =====================================================================================
     */
    bool
    Genematcher::read ( int argc, char** argv )
    {
        char* argvv;
        std::string f="", iformat="",oformat="",bin="";
        for (int i=1; i<argc; i++)
        {
            argvv=argv[i];
            if (argvv[0]=='-')
            {
                std::clog<<"Got "<<argvv[0]<<argvv[1]<<std::endl;
                switch(argvv[1])
                {
                    case 'V':
                        settings.verbose=true;
                        break;
                    case 'v':
                        settings.validate=true;
                        break;
                    case 'C':
                        settings.convert=true;
                        break;
                    case 'i':
                        settings.filein=true;
                        break;
                    case 'o':
                        settings.fileout=true;
                        break;
                    case 'b':
                        settings.bin=true;
                        break;
                    case 'p':
                        settings.pair=true;
                        break;
                    default:
                        break;
                }
            }else{
                if (settings.convert)
                {
                    if (iformat=="") iformat=argvv;
                    else if (oformat=="") oformat=argvv;
                    else if (f=="") f=argvv;
                    else break;
                }else{
                    if (settings.bin)
                        if (bin=="")
                            bin=argvv;
                        else
                        {
                            f=argvv;
                            break;
                        }
                    else
                    {
                        f=argvv;
                        break;
                    }
                }
            }
        }
        //validation

        if (settings.bin)
        {
            if (bin=="") return false;
            else
            {
                std::ifstream fs(bin);
                if (!fs.good())
                {
                    std::cerr<<"Unable to open hashtable file!"<<std::endl;
                    fs.close();
                    return false;
                }
                else{
                    settings.binname=bin;
                    fs.close();
                }
            }
        }
        if (settings.convert)
        {
            if (settings.validate) return false;
            if (iformat=="" || oformat=="") return false;
            else
            {
                param.inputformat=iformat;
                param.outputformat=oformat;
            }
        }
        if (settings.filein)
        {
            if (f=="") return false;
            std::ifstream fs(f);
            if (!fs.good())
            {
                std::cerr<<"Unable to open input file!"<<std::endl;
                fs.close();
                return false;
            }else{
                fs.close();
                settings.fname=f;
            }
        }
        print_settings();
        return true;
    }		/* -----  end of function read  ----- */
    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  validate
     *  Description:  validates symbols, returns itself if correct, returns empty otherwise
     * =====================================================================================
     */
    void
    Genematcher::validate (  )
    {
        std::string key, value;
        std::stringstream trim;
        while (iss.good())
        {
            std::getline(iss,key);
            trim<<key;
            key.clear();
            trim>>key;
            //cleanup
            value=table[key];
            if (value=="") oss<<""<<std::endl;
            else oss<<key<<std::endl;
        }
        return ;
    }		/* -----  end of function validate  ----- */

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  match,match_pair
     *  Description:  match input to key,value pairs
     * =====================================================================================
     */
    void
    Genematcher::match()
    {
        std::string key, value;
        std::stringstream trim;
        while (iss.good())
        {
            std::getline(iss,key);
            trim<<key;
            key.clear();
            trim>>key;
            //cleanup
            value=table[key];
            if (value!="") oss<<value<<std::endl;
        }
        return ;
    }
    void
    Genematcher::match_pair()
    {
        std::string key, value;
        std::stringstream trim;
        while (iss.good())
        {
            std::getline(iss,key);
            trim<<key;
            key.clear();
            trim>>key;
            //cleanup
            value=table[key];
            oss<<key<<"\t"<<value<<std::endl;
        }
        return ;
    }

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  feedin,out
     *  Description:  connect from cin/out or file to rest of program
     * =====================================================================================
     */
    std::istream&
    Genematcher::feedin ()
    {
        if (settings.filein)
        {
            std::ifstream fs (settings.fname);
            if (!fs.good()) throw (ErrMsg("Failed to read!") );
            iss<<fs.rdbuf();
            fs.close();
        }
        else iss<<std::cin;
        return iss;
    }		/* -----  end of function feedin  ----- */
    std::ostream&
    Genematcher::feedout ()
    {
        if (settings.filein)
        {
            std::ofstream fs ("output.csv");
            if (!fs.good()) throw (ErrMsg("Failed to write!"));
            fs<<oss.str();
			fs.close();
        }
        else std::cout<<oss;
        return oss;
    }		/* -----  end of function feedin  ----- */

    /*
     * ===  FUNCTION  ======================================================================
     *         Name:  main
     *  Description:  main interface and scheduler
     * =====================================================================================
     */
    bool
    Genematcher::main (int argc, char** argv)
    {
        std::clog<<"Fast gene converter v0.0"<<std::endl;
        if (!read(argc, argv))
        {
            std::cerr<<"Error reading options!"<<std::endl;
            print_settings();
            return false;
        }else{
            feedin();
            if(settings.convert)
            {
                (*this)<<iss;
                (*this)>>oss;
            } else {
                if (settings.bin)
                    table.load(settings.binname);
                else{
                    std::ifstream ff("fgcdefault.bin");
                    if (!ff.good())
                    {
                        ff.close();
                        return false;
                    }else{
                        table.load();
                        ff.close();
                    }
                }
                if (settings.validate){
                    validate();
                }else if (settings.pair){
                    match_pair();
                }else {
                    match();
                }
            }
            feedout();
        }
        return true;
    }		/* -----  end of function main  ----- */
}
