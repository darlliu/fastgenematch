#ifndef FASTGENEMATCH
#define FASTGENEMATCH
#include "../smhasher-read-only/MurmurHash3.h"
#include "utils.h"
namespace fastgenematch
{

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
            Hashcaller():
                seed(SEED)
			{
                return;
            };                             /* constructor */

            union out128
            {
                size_t s;
                uint64_t t[2];
                //overfilled the hash
            };

            void reseed()
            {
                std::random_device rd;
                seed=rd();
            };
            uint32_t setseed(uint32_t in)
            {
                seed=in;
            };
            uint32_t getseed()
            {
                return seed;
            };
            size_t operator()(std::string key)
            {

#if defined(_M_X64)
                MurmurHash3_x64_128 ( (void*) key.c_str(), key.size(), \
                        (uint32_t) &seed, (void*) out.t);
#else
                MurmurHash3_x86_128 ( (void*) key.c_str(), key.size(), \
                        (uint32_t) &seed, (void*) out.t);
#endif

                return out.s;
            };
        protected:
            uint32_t seed;
            union out128 out;

    }; /* -----  end of class Hashcaller  ----- */

    typedef enum
    {
        genesym=0,
        emsemble_id,
        unigene_id,
        uniprot,
        swissprot,
        uniprot_crick,
        string
    } format;
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
            /* ====================  LIFECYCLE     ======================================= */
            Geneobject ():isgenesym(false){};                             /* constructor */
            struct genesym
                //if key type is genesym, format first
                //and convert all greek letters to latin
            {
                std::string asis;
                std::string formatted;
            };
            std::string operator[](std::string key);
            std::string operator[](size_t idx);
            void rehash();
            void format();
            size_t* hashes();
            void serialize();
        protected:
            std::unordered_map<std::string,std::string,\
                Hashcaller> data;
            bool isgenesym;
            size_t length;
            std::pair<int,int> formats;

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

            /* ====================  ACCESSORS     ======================================= */

            /* ====================  MUTATORS      ======================================= */

            /* ====================  OPERATORS     ======================================= */

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

            /* ====================  MUTATORS      ======================================= */

            void save();
            void load();
            void main_routine();

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
