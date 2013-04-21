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
    /*
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
        if ((format_types)formats.first==genesym)
        {
            return (*data)[key];
        }else{
            return (*data)[key];
        }
    }
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
    };

}
