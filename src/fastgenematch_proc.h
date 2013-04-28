#ifndef FASTGENEMATCH_PROC_H
#define FASTGENEMATCH_PROC_H
#include "fastgenematch.h"

namespace fastgenematch
{

    /*
     * =====================================================================================
     *        Class:  Fgc_proc
     *  Description:  a standalone process based genematcher. With only slight modification
     *                of IO
     * =====================================================================================
     */
    class Fgc_proc: public Genematcher
    {
        public:
            /* ====================  LIFECYCLE     ======================================= */
            Fgc_proc ():
                Genematcher(),
                binset(false)
            {
                reset();
            };                             /* constructor */

            /* ====================  ACCESSORS     ======================================= */
            void report()
            {
                std::cout<<table.formats.first<<"\t"<<table.formats.second<<std::endl;
            };

            /* ====================  MUTATORS      ======================================= */
            void reset()
            {
                settings.hold=true;
                settings.pair=false;
                settings.bin=true;
                settings.validate=false;
                settings.filein=false;
                settings.convert=false;
                //cannot do convert in this mode
                settings.fileout=false;
                //all IO via pipe
            };
            void bind (const std::string& name)
            {
                settings.binname=name;
                std::ifstream f(name, std::ios::binary);
                if (!f.good()) throw (ErrMsg("Bad bin file!\n"));
                table.load(settings.binname);
                binset=true;
            };

            void main ()
            {
                std::string line("");
                do
                {
                    line.clear();
                    std::getline(std::cin,line);
                    std::cin.sync();
                    if (line=="TERMINATETERMINATE") return;
                    std::clog<<"---*---"<<std::endl;
                    if (line=="bind")
                        //rebind
                    {
                        std::getline(std::cin,line);
                        bind(line);
                        std::cin.sync();
                        std::clog<<"--BOUND--"<<std::endl;
                    }
                    else if (line == "validate")
                    {
                        settings.validate=true;
                        std::cin.sync();
                        std::clog<<"--SETVALIDATE--"<<std::endl;
                    }
                    else if (line == "tell")
                    {
                        report();
                        std::cin.sync();
                    }
                    else if (line== "DO")
                    {
                        std::cin.sync();
                        //clear whatever has been typed
                        if (!binset) throw(ErrMsg("Bin file not loaded!"));
                        if (settings.validate)
                        {
                            validate();
                        }else{
                            match();
                        }
                        reset();
                    }
                    else ;
                    std::clog<<"---*---"<<std::endl;
                }
                while (true);
            };

        private:
            /* ====================  DATA MEMBERS  ======================================= */
            bool binset;

    }; /* -----  end of class Fgc_proc  ----- */


}

#endif
