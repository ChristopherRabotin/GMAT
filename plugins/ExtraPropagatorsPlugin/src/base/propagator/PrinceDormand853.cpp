//
//------------------------------------------------------------------------------
//                                PrinceDormand853
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : PrinceDormand853.cpp
// *** Created   : July 1, 2013
// **************************************************************************
// ***  Developed By  :  Hwayeong Kim, CBNU                               ***
// **************************************************************************
// *** August 12, 2013 : add Coefficient 



#include "gmatdefs.hpp"
#include "PrinceDormand853.hpp"

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PrinceDormand853::PrinceDormand853()
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
PrinceDormand853::PrinceDormand853(const std::string &nomme) :
//   RungeKutta      (16, 8, "PrinceDormand853", nomme)
   RungeKutta      (12, 8, "PrinceDormand853", nomme)
{
}

//------------------------------------------------------------------------------
// PrinceDormand853::~PrinceDormand853()
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
PrinceDormand853::~PrinceDormand853()
{
}


//------------------------------------------------------------------------------
// PrinceDormand853::PrinceDormand853(const PrinceDormand853& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
PrinceDormand853::PrinceDormand853(const PrinceDormand853& rk) :
    RungeKutta      (rk)
{
}


//------------------------------------------------------------------------------
// PrinceDormand853 & PrinceDormand853::operator=(const PrinceDormand853& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
PrinceDormand853 & PrinceDormand853::operator=(const PrinceDormand853& rk)
{
    if (this == &rk)
        return *this;

    RungeKutta::operator=(rk);

    return *this;
}


//------------------------------------------------------------------------------
// Propagator* PrinceDormand853::Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
Propagator* PrinceDormand853::Clone() const
{
    return new PrinceDormand853(*this);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void PrinceDormand853::SetCoefficients()
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void PrinceDormand853::SetCoefficients()
{
    if ((ai == NULL) || (bij == NULL) || (cj == NULL) || (ee == NULL)) {
        isInitialized = false;
        return;
    }

    ai[0] = 0.0;
    ai[1] = 0.0526001519587677;
    ai[2] = 0.0789002279381515;
    ai[3] = 0.118350341907227;
    ai[4] = 0.281649658092772;
    ai[5] = 0.333333333333333;
    ai[6] = 0.25;
    ai[7] = 0.307692307692307;
    ai[8] = 0.651282051282051;
    ai[9] = 0.6;
    ai[10] = 0.857142857142857;
    ai[11] = 1.0; //0.0;      // Per Hairer, Norsett and Wanner, ai[11] = 1.0
                              // (eq 5.25b; their C_12 is GMAT's ai[11])

    if (stages == 16)
    {
       ai[12] =  0.0;
       ai[13] =  0.1;
       ai[14] =  0.2;
       ai[15] =  0.777777777777777;
    }

    bij[0][0] = 0.0;

    bij[1][0] = 0.0526001519587677;

    bij[2][0] = 0.0197250569845378;
    bij[2][1] = 0.05917517095361360000;

    bij[3][0] = 0.02958758547680680000;
    bij[3][1] = 0.0;
    bij[3][2] = 0.08876275643042050000;

    bij[4][0] = 0.24136513415926600000;
    bij[4][1] = 0.0;
    bij[4][2] = -0.88454947932828600000;
    bij[4][3] = 0.92483400326179200000;

    bij[5][0] = 0.03703703703703700000;
    bij[5][1] = 0.0;
    bij[5][2] = 0.0;
    bij[5][3] = 0.17082860872947300000;
    bij[5][4] = 0.12546768756682200000;

    bij[6][0] = 0.03710937500000000000;
    bij[6][1] = 0.0;
    bij[6][2] = 0.0;
    bij[6][3] = 0.17025221101954400000;
    bij[6][4] = 0.06021653898045590000;
    bij[6][5] = -0.01757812500000000000;   

    bij[7][0] = 0.03709200011850470000;
    bij[7][1] = 0.0;
    bij[7][2] = 0.0;
    bij[7][3] = 0.17038392571223900000;
    bij[7][4] = 0.10726203044637300000;
    bij[7][5] = -0.01531943774862440000;      
    bij[7][6] = 0.00827378916381402000;

    bij[8][0] = 0.62411095871607500000;
    bij[8][1] = 0.0;
    bij[8][2] = 0.0;
    bij[8][3] = -3.36089262944694000000;
    bij[8][4] = -0.86821934684172600000;
    bij[8][5] = 27.59209969944670000000;      
    bij[8][6] = 20.15406755047780000000;
    bij[8][7] = -43.48988418106990000000;

    bij[9][0] = 0.47766253643826400000;
    bij[9][1] = 0.0;
    bij[9][2] = 0.0;
    bij[9][3] = -2.48811461997166000000;
    bij[9][4] = -0.59029082683684200000;
    bij[9][5] = 21.23005144818110000000;
    bij[9][6] = 15.27923363288240000000;
    bij[9][7] = -33.28821096898480000000;
    bij[9][8] = -0.02033120170850860000;

    bij[10][0] = -0.93714243008598700000;
    bij[10][1] = 0.0;
    bij[10][2] = 0.0;
    bij[10][3] = 5.18637242884406000000;
    bij[10][4] = 1.09143734899672000000;
    bij[10][5] = -8.14978701074692000000;
    bij[10][6] = -18.52006565999690000000;
    bij[10][7] = 22.73948709935050000000;
    bij[10][8] = 2.49360555267965000000;
    bij[10][9] = -3.04676447189821000000;

    bij[11][0] = 2.27331014751653000000;
    bij[11][1] = 0.0;
    bij[11][2] = 0.0;
    bij[11][3] = -10.53449546673720000000;
    bij[11][4] = -2.00087205822486000000;
    bij[11][5] = -17.95893186311870000000;
    bij[11][6] = 27.94888452941990000000;
    bij[11][7] = -2.85899827713502000000;
    bij[11][8] = -8.87285693353062000000;
    bij[11][9] = 12.36056717579430000000;
    bij[11][10] = 0.64339274601576300000;
            
    if (stages == 16)
    {
       bij[12][0] = 0.0;
       bij[12][1] = 0.0;
       bij[12][2] = 0.0;
       bij[12][3] = 0.0;
       bij[12][4] = 0.0;
       bij[12][5] = 0.0;
       bij[12][6] = 0.0;
       bij[12][7] = 0.0;
       bij[12][8] = 0.0;
       bij[12][9] = 0.0;
       bij[12][10] = 0.0;
       bij[12][11] = 0.0;

       bij[13][0] = 0.05616750228304790000;
       bij[13][1] = 0.0;
       bij[13][2] = 0.0;
       bij[13][3] = 0.0;
       bij[13][4] = 0.0;
       bij[13][5] = 0.0;
       bij[13][6] = 0.25350021021662400000;
       bij[13][7] = -0.24623903747080200000;
       bij[13][8] = -0.12419142326381600000;
       bij[13][9] = 0.15329179827876500000;
       bij[13][10] = 0.00820105229563468000;
       bij[13][11] = 0.00756789766054569000;
       bij[13][12] = -0.00829800000000000000;

       bij[14][0] = 0.03183464816350210000;
       bij[14][1] = 0.0;
       bij[14][2] = 0.0;
       bij[14][3] = 0.0;
       bij[14][4] = 0.0;
       bij[14][5] = 0.02830090967236670000;
       bij[14][6] = 0.05354198830743850000;
       bij[14][7] = -0.05492374857139090000;
       bij[14][8] = 0.0;
       bij[14][9] = 0.0;
       bij[14][10] = -0.00010834732869724900;
       bij[14][11] = 0.00038257109083565800;
       bij[14][12] = -0.00034046500868740400;
       bij[14][13] = 0.14131244367463200000;

       bij[15][0] = -0.42889630158379100000;
       bij[15][1] = 0.0;
       bij[15][2] = 0.0;
       bij[15][3] = 0.0;
       bij[15][4] = 0.0;
       bij[15][5] = -4.69762141536116000000;
       bij[15][6] = 7.68342119606259000000;
       bij[15][7] = 4.06898981839711000000;
       bij[15][8] = 0.35672718745528100000;
       bij[15][9] = 0.0;
       bij[15][10] = 0.0;
       bij[15][11] = 0.0;
       bij[15][12] = -0.00139902416515901000;
       bij[15][13] = 2.94751478915277000000;
       bij[15][14] = -9.15095847217987000000;
    }
	
    cj[0] = 0.05429373411656870000;
    cj[1] =  0.0;
    cj[2] =  0.0;
    cj[3] =  0.0;
    cj[4] =  0.0;
    cj[5] = 4.45031289275240000000;
    cj[6] = 1.89151789931450000000;
    cj[7] = -5.80120396001058000000;
    cj[8] = 0.31116436695781900000;
    cj[9] = -0.15216094966251600000;
    cj[10] = 0.20136540080403000000;
    cj[11] = 0.04471061572777250000;

    if (stages == 16)
    {
       // Zero remaining coefficients
       cj[12] = cj[13] = cj[14] = cj[15] = 0.0;
    }

    ee[0] = 0.01312004499419480000;
    ee[1] =  0.0;
    ee[2] =  0.0;
    ee[3] =  0.0;
    ee[4] =  0.0;
    ee[5] = -1.22515644637620000000;
    ee[6] = -0.49575894965725000000;
    ee[7] = 1.66437718245498000000;
    ee[8] = -0.35032884874997300000;
    ee[9] = 0.33417911871301700000;
    ee[10] = 0.08192320648511570000;
    ee[11] = -0.02235530786388620000;

    if (stages == 16)
    {
       // Zero remaining coefficients
       ee[12] = ee[13] = ee[14] = ee[15] = 0.0;
    }
}
