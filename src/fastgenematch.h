#ifndef FASTGENEMATCH
#define FASTGENEMATCH
#include "utils.h"
namespace fastgenematch
{
    typedef std::string ErrMsg;
    /*
     *Fundamental format types
     */
    typedef enum
    {
        allowed=-2,
        unknown=-1,
        geneid=0,
        entry=0,
        exit=0,
        swissprot=1,
        uniprot,
        genesym,
        refseq,
        embl,
        emsemble,
        unigene,
        string
    } format_types;

    /*
     * =====================================================================================
     *        Class:  Geneconverter
     *  Description:  simple class to format gene symbols
     * =====================================================================================
     */
/*
 *    class Geneconverter
 *    {
 *        public:
 *            [> ====================  LIFECYCLE     ======================================= <]
 *            Geneconverter ();                             [> constructor <]
 *
 *            [> ====================  ACCESSORS     ======================================= <]
 *            std::string format (const std::string& in);
 *            std::unordered_map<std::string,std::string> greeks;
 *
 *    }; [> -----  end of class Geneconverter  ----- <]
 */

    /*
     * =====================================================================================
     *        Class:  Hashcaller
     *  Description:  utility wrapper for murmurhash3
     * =====================================================================================
     */
    class Hashcaller
    {
#define SEED 991413003
        public:
            /* ====================  LIFECYCLE     ======================================= */
            Hashcaller()
			{
                return;
            };                             /* constructor */

            union out128
            {
                size_t s;
                uint64_t t[2];
                //overfilled the hash
                //s is usually 64 bit and thus only t[0]
                //but in some cases it may be extended to above or below
            };
            static uint32_t seed;

            size_t operator() (const std::string& key) const
            {

                Murmur3( (const void*) key.c_str(), key.size(), \
                        (uint32_t) seed, (void*) out.t);

                return out.s;
            };

        protected:
            union out128 out;

    }; /* -----  end of class Hashcaller  ----- */

    /*
     * =====================================================================================
     *        Class:  Geneobject
     *  Description:  object class with the unordered map data container
     *                wraps the unordered map with murmurhash3 and stores
     *                synonymous gene symbols
     * =====================================================================================
     */
    class Geneobject
    {
        public:
            typedef std::unordered_map<std::string,std::string,\
                Hashcaller> _hashtable;
            typedef
            std::shared_ptr< _hashtable > hashtable;
            /* ====================  LIFECYCLE     ======================================= */
            Geneobject ():
                empty(""), data(new _hashtable), length(2000)
            {
                data->reserve(length);
                formats.first=-1;
                formats.second=-1;
            };                             /* constructor */
            /*
             *struct genesym
             *    //if key type is genesym, format first
             *    //and convert all greek letters to latin
             *{
             *    std::string asis;
             *    std::string formatted;
             *};
             */
            /* ====================  ACCESSORS     ======================================= */
            std::string& operator[](const std::string& key);
            std::string& operator()(const std::string& key);
            uint32_t getseed()
            {
                return Hashcaller::seed;
            };
            void rehash();

            /* ====================  MUTATORS      ======================================= */
            void reseed()
            {
                std::random_device rd;
                Hashcaller::seed=rd();
            };
            void setseed(const uint32_t& in )
            {
                Hashcaller::seed=in;
            };
            //std::string format(const std::string &);
            //size_t* hashes();
            void serialize() {serialize ("fgcdefault.bin");};
            void serialize(const std::string&);
            void load(const std::string&);
            void load() {load ("fgcdefault.bin");};
            void todb(const std::string&);
            void clear(){
                data.reset(new _hashtable);
                data->reserve(length);
            };

			std::pair<int,int> formats;
            hashtable data;
        protected:
            size_t length;
            std::string empty;
            uint32_t seed;
    }; /* -----  end of class Geneobject  ----- */

    /*
     * =====================================================================================
     *        Class:  Genematch_converter
     *  Description:  Converts and outputs proper data containers
     * =====================================================================================
     */
    class Genematch_converter
    {
        public:
            /* ====================  LIFECYCLE     ======================================= */
            Genematch_converter ()
            {
                param.outputname="";
                param.inputformat="other";
                param.outputformat="other";
                initialize();
            };                             /* constructor */


            /* ====================  MUTATORS      ======================================= */
            void initialize();
            void do_convert();

            /* ====================  OPERATORS     ======================================= */
            std::istream& operator<<(std::istream&);
            std::ostream& operator>>(std::ostream&);

            /* ====================  METHODS       ======================================= */
            struct params
            {
                std::string outputname;
                std::string inputformat;
                std::string outputformat;
            };
            /* ====================  DATA MEMBERS  ======================================= */
            params param;
            Geneobject table;
            std::string title;
            std::unordered_map <std::string, format_types> lookup;

    }; /* -----  end of class Genematch_converter  ----- */
    /*
     * =====================================================================================
     *        Class:  Genematcher
     *  Description:  a matcher that converts from one input to another output given the formats
     * =====================================================================================
     */
    class Genematcher: public Genematch_converter
    {
        public:
            /* ====================  LIFECYCLE     ======================================= */
            Genematcher():
                Genematch_converter()
            {
                settings.bin=false;
                settings.db=false;
                settings.hold=false;
                settings.convert=false;
                settings.verbose=false;
                settings.validate=false;
                settings.fname="";
                settings.binname="";
                settings.dbname="";
                settings.filein=false;
                settings.fileout=false;
                settings.pair=false;
                settings.give_keys=false;
                settings.give_vals=false;
                settings.give_pairs=false;
            };

            /* ====================  ACCESSORS     ======================================= */
            void print_help();
            void print_settings();
            inline void print_version()
            {
                std::clog<<"Fast Gene Match v"<<(float)FGCVERSION/10<<std::endl;
            };
            void validate();
            void match();
            void match_pair();

            /* ====================  MUTATORS      ======================================= */
            bool read(int argc, char** argv);
            const std::string feedin();
            void feedout(const std::ostringstream&);
            bool main(int argc, char** argv);

            /* ====================  DATA MEMBERS  ======================================= */
            struct params
            {
                bool hold;
                bool bin;
                bool db;
                bool pair;
                bool verbose;
                bool validate;
                bool filein;
                bool give_keys;
                bool give_vals;
                bool give_pairs;
                std::string fname;
                std::string binname;
                std::string dbname;
                bool convert;
                bool fileout;
            };
            params settings;
    }; /* -----  end of class Genematcher  ----- */

};

#endif
