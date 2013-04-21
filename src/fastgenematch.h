#ifndef FASTGENEMATCH
#define FASTGENEMATCH
#include "../smhasher-read-only/MurmurHash3.h"
#include "utils.h"
namespace fastgenematch
{
    /*
     *Fundamental format types
     */
    typedef enum
    {
        allowed=-2,
        unknown=-1,
        genesym=0,
        emsemble_id,
        unigene_id,
        uniprot,
        swissprot,
        uniprot_crick,
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

#if defined(_M_X64)
                MurmurHash3_x64_128 ( (const void*) key.c_str(), key.size(), \
                        (uint32_t) seed, (void*) out.t);
#else
                MurmurHash3_x86_128 ( (const void*) key.c_str(), key.size(), \
                        (uint32_t) seed, (void*) out.t);
#endif

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
            typedef
            std::shared_ptr<std::unordered_map<std::string,std::string,\
                Hashcaller>> hashtable;
            typedef std::unordered_map<std::string,std::string,\
                Hashcaller> _hashtable;
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
            uint32_t setseed(const uint32_t& in )
            {
                Hashcaller::seed=in;
            };
            //std::string format(const std::string &);
            //size_t* hashes();
            void serialize() {serialize ("fgcdefault.bin");};
            void serialize(const std::string&);
            void load(const std::string&);
            void load() {load ("fgcdefault.bin");};

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
                settings.convert=false;
                settings.verbose=false;
                settings.validate=false;
                settings.fname="";
                settings.binname="";
                settings.filein=false;
                settings.fileout=false;
                settings.pair=false;
            };

            /* ====================  ACCESSORS     ======================================= */
            void print_help();
            void print_settings();
            void validate();
            void match();
            void match_pair();

            /* ====================  MUTATORS      ======================================= */
            bool read(char** argv);
            std::istream& feedin();
            std::ostream& feedout();

            /* ====================  DATA MEMBERS  ======================================= */
            struct params
            {
                bool bin;
                bool pair;
                bool verbose;
                bool validate;
                bool filein;
                std::string fname;
                std::string binname;
                bool convert;
                bool fileout;
            };
            params settings;
            std::istringstream iss;
            std::ostringstream oss;
    }; /* -----  end of class Genematcher  ----- */

};

#endif
