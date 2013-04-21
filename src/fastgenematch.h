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
            void serialize();
            void load(std::ifstream);

        protected:
            hashtable data;
            size_t length;
            std::pair<int,int> formats;
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
            Genematch_converter ();                             /* constructor */


            /* ====================  MUTATORS      ======================================= */
            void initialize();
            void do_convert();

            /* ====================  OPERATORS     ======================================= */
            std::ostringstream operator<<(std::ifstream);
            std::ofstream operator>>(std::ofstream);

            /* ====================  METHODS       ======================================= */
            struct params
            {
                std::string inputname;
                std::string outputfname;
                std::string inputformat;
                std::string outputformat;
            };
            /* ====================  DATA MEMBERS  ======================================= */
            params param;
    }; /* -----  end of class Genematch_converter  ----- */
    /*
     * =====================================================================================
     *        Class:  Genematcher
     *  Description:  a matcher that converts from one input to another output given the formats
     * =====================================================================================
     */
    class Genematcher
    {
        public:
            /* ====================  LIFECYCLE     ======================================= */
            Genematcher ();                             /* constructor */
            ~Genematcher();

            /* ====================  ACCESSORS     ======================================= */
            bool validate (std::string,std::string);
            std::string validate(std::string);
            std::string feedout(std::string);

            /* ====================  MUTATORS      ======================================= */

            void initialize();
            void save();
            void load();

            /* ====================  DATA MEMBERS  ======================================= */
            struct params
            {
                bool once;
                bool verbose;
                bool validate;
                bool filein;
                std::string fname;
                bool convert;
                bool fileout;
            };
            params param;

    }; /* -----  end of class Genematcher  ----- */

};

#endif
