#include "fastgenematch.h"

int test()
{
	fastgenematch::Geneobject g1,g2;
    g1("apple")="delicious";
    g1("orange")="nutritious";
    std::cout<<" Lookup 1 for key apple "<<g1["apple"]<<" orange "<<g1["orange"]<<std::endl;
    std::cout<<" Missing Lookup for key pear "<<g1["pear"]<<std::endl;
    g1.serialize();
    g2.load();
    std::cout<<" Lookup 1 for key apple "<<g2["apple"]<<" orange "<<g2["orange"]<<std::endl;
    std::cout<<" Missing Lookup for key pear "<<g2["pear"]<<std::endl;
    fastgenematch::Genematcher G;
	std::istringstream iss(std::string("A\tB\n"));
    G<<iss;
    std::ostringstream os;
    G>>os;
    std::cout<<os.str();
    G.print_help();
	G.print_settings();
    return 1;
};


int main(int argc, char**  argv)
{
    fastgenematch::Genematcher G;
    //std::clog<<"Initiating main routines"<<argc<<argv<<std::endl;
    switch(argc)
    {
        case 1:
            G.print_help();
            return 0;
        case 2:
            if (argv[1][1]=='V')
            {
                G.settings.verbose=true;
                G.print_help();
                G.print_settings();
                return 0;
            }else{
                return 1;
            }
        default:
            break;
    }
    try
    {
        //std::clog<<"Into main loop"<<std::endl;
        if( G.main(argc, argv) )
        return 1;
        else return 2;
    }
    catch(...)
    {
        std::cerr<<"Undefined exception occurred!"<<std::endl;
    }
}
