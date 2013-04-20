#ifndef FASTGENEMATCH
#define FASTGENEMATCH
#include "../smhasher-read-only/MurmurHash3.h"
#include "utils.h"
namespace fastgenematch
{

    /*
     * =====================================================================================
     *        Class:  Hashcaller
     *  Description:  utilitery wrapper for murmurhash3
     * =====================================================================================
     */
    class Hashcaller
    {
#define SEED 991413003
        public:
            /* ====================  LIFECYCLE     ======================================= */
            Hashcaller():
                seed(SEED), out(-1)
			{
                std::random_device rd;
                return;
            };                             /* constructor */

            size_t operator()(std::string key)
            {

#if defined(_M_X64)
                MurmurHash3_x64_128 ( (void*) key.c_str(), sizeof(key), \
                        (uint32_t) &seed, (void*) &out);
#else
                MurmurHash3_x86_128 ( (void*) key.c_str(), sizeof(key), \
                        (uint32_t) &seed, (void*) &out);
#endif
                return out;
            };
        protected:
            uint32_t seed;
            size_t out;

    }; /* -----  end of class Hashcaller  ----- */

    /*
     * =====================================================================================
     *        Class:  Geneobject
     *  Description:  object class with the unordered map data container
     * =====================================================================================
     */
    class Geneobject
    {
        public:
            /* ====================  LIFECYCLE     ======================================= */
			Geneobject ():genesym(false){};                             /* constructor */
        struct genesym
            //if key type is genesym, format first
            //and convert all greek letters to latin
        {
            std::string asis;
            std::string formatted;
        };
        void format();
        protected:
            std::unordered_map<std::string,std::string,\
                Hashcaller> data_default;
            bool genesym;


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

    protected:
        /* ====================  METHODS       ======================================= */
        struct params
        {
            std::string inputname;
            std::string outputfname;
            std::string inputformat;
            std::string outputformat;
        };
        /* ====================  DATA MEMBERS  ======================================= */


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
        void main_routine();

    protected:
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

}; /* -----  end of class Genematcher  ----- */

};

#endif
