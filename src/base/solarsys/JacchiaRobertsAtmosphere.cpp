//$Id$
//------------------------------------------------------------------------------
//                              JacchiaRobertsAtmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Waka A. Waktola
// Created: 2004/05/11
//
/**
 * The Jacchia-Roberts atmosphere.  The implementation here is derived from the
 * Windows Swingby code.
 */
//------------------------------------------------------------------------------

#include "JacchiaRobertsAtmosphere.hpp"
#include "AtmosphereException.hpp"
#include "SolarSystem.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"

// #define DEBUG_JR_DRAG 1




//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// JacchiaRobertsAtmosphere(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * GMAT's JR model uses default values for the acceleration due to gravity at
 * the surface of the Earth, g_zero, and for Avagadro's number, avagadro, that
 * were picked up from legacy code.  These values, while not in agreement with
 * references found in the literature and on the Internet, are tuned to make the
 * model perform  in agreement with other tested systems, and should not be
 * changed in this  code without testing of propagation in all if the
 * atmospheric regimes that  apply to Jacchia-Roberts drag (0-100 km,
 * 100-125 km, 125 - 500 km, 500 - 2500 km).
 *
 * @param name Name of the drag object (optional)
 */
//------------------------------------------------------------------------------
JacchiaRobertsAtmosphere::JacchiaRobertsAtmosphere(const std::string &name) :
   AtmosphereModel      ("JacchiaRoberts", name),
   cbPolarRadius        (6356.766),       // @todo - check effect of using default from GmatDefaults
   cbPolarSquared       (40408473.978756),
   rho_zero             (3.46e-9),
   tzero                (183.0),
   g_zero               (9.80665),
   gas_con              (8.31432),
   avogadro             (6.022045e23)
{
   LoadConstants();
}


//------------------------------------------------------------------------------
// ~JacchiaRobertsAtmosphere()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
JacchiaRobertsAtmosphere::~JacchiaRobertsAtmosphere()
{
   if (fileReader)
   {
      if (fileReader->CloseSolarFluxFile(solarFluxFile))
         fileRead = false;
      else
         throw AtmosphereException("Error closing JacchiaRoberts data file.\n");
   }
}

//------------------------------------------------------------------------------
// JacchiaRobertsAtmosphere(const JacchiaRobertsAtmosphere& jr)
//------------------------------------------------------------------------------
/**
 * Copy Constructor.
 * 
 * @param jr The JacchiaRoberts instance used to set parameters for this clone.
 */
//------------------------------------------------------------------------------
JacchiaRobertsAtmosphere::JacchiaRobertsAtmosphere(const JacchiaRobertsAtmosphere& jr) :
    AtmosphereModel     (jr),
    root1               (jr.root1),
    root2               (jr.root2),
    x_root              (jr.x_root),
    y_root              (jr.y_root),
    t_infinity          (jr.t_infinity),
    tx                  (jr.tx),
    sum                 (jr.sum),
    cbPolarRadius       (jr.cbPolarRadius),
    cbPolarSquared      (jr.cbPolarSquared),
    rho_zero            (jr.rho_zero),
    tzero               (jr.tzero),
    g_zero              (jr.g_zero),
    gas_con             (jr.gas_con),
    avogadro            (jr.avogadro)
{
   LoadConstants();
}

//------------------------------------------------------------------------------
// JacchiaRobertsAtmosphere& operator=(const JacchiaRobertsAtmosphere& jr)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param jr JacchiaRobertsAtmosphere instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
JacchiaRobertsAtmosphere& JacchiaRobertsAtmosphere::operator=
                                            (const JacchiaRobertsAtmosphere& jr)
{
    if (this == &jr)
        return *this;
        
    AtmosphereModel::operator=(jr);   
    
    root1            = jr.root1;
    root2            = jr.root2;
    x_root           = jr.x_root;
    y_root           = jr.y_root;
    t_infinity       = jr.t_infinity;
    tx               = jr.tx;
    sum              = jr.sum;
    cbPolarRadius    = jr.cbPolarRadius;
    cbPolarSquared   = jr.cbPolarSquared;

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the DragForce.
 *
 * @return clone of the atmosphere model.
 */
//------------------------------------------------------------------------------
GmatBase* JacchiaRobertsAtmosphere::Clone() const
{
   return (new JacchiaRobertsAtmosphere(*this));
}

//------------------------------------------------------------------------------
//  bool Density(Real *pos, Real *density, Real epoch, Integer count)
//------------------------------------------------------------------------------
/**
 *  Calculates the density at each of the states in the input vector, using the 
 *  JacchiaRoberts atmosphere model.
 * 
 *  @param pos       The input vector of spacecraft states (6 elements)
 *  @param density   The array of output densities
 *  @param epoch     The current TAIJulian epoch
 *  @param count     The number of spacecraft contained in pos 
 *
 *  @return true on success
 */
//------------------------------------------------------------------------------
bool JacchiaRobertsAtmosphere::Density(Real *pos, Real *density, Real epoch, 
                                       Integer count)
{
   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("JacchiaRobertsAtmosphere::Density() epoch=%g, sc count=%d\n", epoch,
          count);
   #endif
   Real height;
   Real utc_time;
   A1Mjd a1mjd_time(epoch);

   // Don't offset by half a day here:
   utc_time = (Real)a1mjd_time.ToUtcMjd();// - 0.5;
   
   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("   UTC time = %lf\n", utc_time);
   #endif

   for (Integer i=0; i<count; i++)
   {
      height = CalculateGeodetics(&pos[i*6], epoch, true);
      if (epoch != wUpdateEpoch)
         AtmosphereModel::BuildAngularVelocity(epoch);
      
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Calculated height = %lf\n", height);
      #endif
      // Obtain density of atmosphere at spacecraft height

      //  For heights in the air, use JacchiaRoberts to calculate the density.
      //  For heights at or below the surface of the Earth, use the constant
      //  value used for heights below 90 KM.
      if (height > 0.0)
      {         
         // Output density in units of kg/m3
         density[i] = 1.0e3 * JacchiaRoberts(height, &pos[i*6], sunVector,
               utc_time, newFile);
      }
      else
      {
         // Output density in units of kg/m3
         density[i] = 1.0e3 * rho_zero;
      }

      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("JRDensity(): sat %d pos = %g, %g, %g, height = %g, density = %g\n",
             i, pos[i*6], pos[i*6+1], pos[i*6+2], height, density[i]);
      #endif
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void SetCentralBodyVector(Real *cv)
//------------------------------------------------------------------------------
/**
 * Sets the central body, obtaining locally needed constants
 *
 * @param cb The new central body
 */
//------------------------------------------------------------------------------
void JacchiaRobertsAtmosphere::SetCentralBody(CelestialBody *cb)
{
   AtmosphereModel::SetCentralBody(cb);

   cbPolarRadius  = mCentralBody->GetPolarRadius();
   cbPolarSquared = cbPolarRadius*cbPolarRadius;
}


//------------------------------------------------------------------------------
//  Private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Real JacchiaRoberts(Real height, Real space_craft[3], Real sun[3], 
//                     Real a1_time, bool new_file)
//------------------------------------------------------------------------------
/**
 * Obtain atmospheric density using the Jacchia-Roberts model
 *  
 *  
 * Modifications:
 * Name                Date      Description
 * ------------------  --------  ------------------------------------------
 * D. Ginn             01/26/94  DSPSE OPS:  Added new_file argument, logic
 * W. Waktola          06/08/04  Code 583: Renamed to JacchiaRoberts()
 * W. Waktola          12/28/04  Code 583: Removed FILE* tkptr
 *
 * @param height       Spacecraft height (km)
 * @param space_craft  Spacecraft position (km, TOD GCI)
 * @param sun          Sun unit vector (TOD GCI)
 * @param a1_time      Reduced julian date (days)
 * @param new_file     If true, flush static data for file
 *
 * @return The density
 */  
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::JacchiaRoberts(Real height, Real space_craft[3], 
          Real sun[3], Real a1_time, bool new_file)
{
   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("JacchiaRobertsAtmosphere::JacchiaRoberts(%15lf, [%15lf %15lf %15lf],"
          "\n   [%15lf %15lf %15lf], %15lf, %s\n",
          height, space_craft[0], space_craft[1], space_craft[2],
          sun[0], sun[1], sun[2], a1_time, (new_file ? "true" : "false") );
   #endif

   GEOPARMS geo;
   Real density, temperature, t_500, sun_dec, geo_lat;
   FILE *tkptr;

   if (new_file)
   {
      if (!fileRead)
      {  
         tkptr = fileReader->OpenSolarFluxFile(fileName);
         solarFluxFile = tkptr;
         fileRead = true;
         
         if (tkptr != NULL)
         {  
            // Read minimum temperature and geomagnetic indices
            Integer status = fileReader->LoadSolarFluxFile(a1_time, tkptr, new_file, &geo);
            // Following Swingby code's procedure, return density = 0.0
            // if values read from file are not accurate.
            if (status != 0)
            {
                  throw AtmosphereException("Error loading in JacchiaRoberts data file."
                                            "Density has be set to 0.0\n");
               Real dValue = 0.0;
               return dValue; 
            }    
         }
         else
            throw AtmosphereException("Error opening JacchiaRoberts data file.\n");
      }       
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Using file\n   K_p     = %15.10le\n   exoTemp = %lf\n", geo.tkp,
             geo.xtemp);
      #endif
   }
   else
   {
      geo.xtemp = 379.0 + 3.24 * nominalF107a + 1.3 * (nominalF107 - nominalF107a);
      geo.tkp   = nominalKp;
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Using constant values\n   F10.7   = %lf\n"
             "   F10.7a  = %lf\n   K_p     = %lf\n   exoTemp = %lf\n",
             nominalF107, nominalF107a, geo.tkp, geo.xtemp);
      #endif
   }
   // Compute declination of the sun
   sun_dec = atan2(sun[2], sqrt(sun[0]*sun[0] + sun[1]*sun[1]));
   // Geodetic latitude of spacecraft, in radians
   geo_lat = geoLat * GmatMathConstants::RAD_PER_DEG;

   // Compute height dependent density
   if (height<=90.0)
   {
      density = rho_zero;
   }
   else if (height < (Real) 100.0)
   {
      temperature = exotherm(space_craft, sun, &geo, height, sun_dec, geo_lat);
      density = rho_100(height, temperature);
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Evaluating for height < 100\n"
             "   temp = %lf\n   density = %15.10le\n",
             temperature, density);
      #endif
   }
   else if (height <= (Real) 125.0)
   {
      temperature = exotherm(space_craft, sun, &geo, height, sun_dec, geo_lat);
      density = rho_125(height, temperature);
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Evaluating for 100 < height < 125\n"
             "   temp = %lf\n   density = %15.10le\n",
             temperature, density);
      #endif
   }
   else if (height <= (Real) 2500.0)
   {
      t_500 = exotherm(space_craft, sun, &geo, 500.0, sun_dec, geo_lat);
      temperature = exotherm(space_craft, sun, &geo, height, sun_dec, geo_lat);
 
      density = rho_high(height, temperature, t_500, sun_dec, geo_lat);
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Evaluating for 125 < height < 2500\n"
             "   temp = %lf\n   t_500 = %lf\n   density = %15.10le\n",
             temperature, t_500, density);
      #endif
   }
   else
   {
      density = 0.0; 
      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("   Evaluating for height > 2500\n"
             "   density = %15.10le\n", density);
      #endif
   }
   
   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("   Corrected density = %15.10le\n",
          density * rho_cor(height, a1_time, geo_lat, &geo));
   #endif
   return density * rho_cor(height, a1_time, geo_lat, &geo);
}


//------------------------------------------------------------------------------
//  Real exotherm(Real space_craft[2], Real sun[3], GEOPARMS *geo, Real height, 
//                Real sun_dec, Real geo_lat)
//------------------------------------------------------------------------------
/**
 *  Compute the temperature of earth's atmosphere and auxiliary temperature 
 *  related quantities at a given altitude.
 *
 *   global variables -
 *      con_c                Series expansion coefficients for height < 125 km
 *                           (in jr_drag header file)
 *      con_l                Series expansion coefficients for height > 125 km
 *                           (in jr_drag header file)
 *
 *  @param space_craft[2]  Spacecraft position (km)
 *  @param sun             Sun unit vector
 *  @param geo             Geomagnetic index and minimum global exospheric
 *                         temperature (from tables)
 *  @param height          Spacecraft height (km)
 *
 *  @return exotemp, the local exospheric temperature used in the model
 */ 
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::exotherm(Real space_craft[2], Real sun[3], 
                         GEOPARMS *geo, Real height, Real sun_dec, Real geo_lat)
{
   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("   JacchiaRobertsAtmosphere::exotherm([%15lf, %15lf],\n"
          "      [%15lf, %15lf, %15lf], geo = {%15lf, %15lf},\n"
          "      %15lf, %15lf, %15lf)\n",
          space_craft[0], space_craft[1], sun[0], sun[1], sun[2],
          geo->tkp, geo->xtemp, height, sun_dec, geo_lat);
   #endif
   
   Real exotemp, expkp, hour_angle;
   Real theta, eta, tau, th22, t1, sun_denom;
   Real c_star[5], aux[4][2];
   Integer i;
   const Integer na=5;

   // Compute hour angle of the sun
   sun_denom = sqrt(sun[0]*sun[0] + sun[1]*sun[1]);
   hour_angle = ( (sun[0]*space_craft[1]-sun[1]*space_craft[0])/
                  fabs(sun[0]*space_craft[1]-sun[1]*space_craft[0]))
      *acos((sun[0]*space_craft[0]+sun[1]*space_craft[1])/
            (sun_denom * sqrt(space_craft[0]*space_craft[0] +
                              space_craft[1]*space_craft[1])));

   // Compute sun and spacecraft position dependent part of temperature
   theta = 0.5 * fabs(geo_lat + sun_dec);
   eta = 0.5 * fabs(geo_lat - sun_dec);
   tau = hour_angle - 0.64577182325 + 0.10471975512 *
      sin(hour_angle + 0.75049157836);
   if (tau < - GmatMathConstants::PI)
      tau += 2 * GmatMathConstants::PI;
   else if (tau > GmatMathConstants::PI)
      tau -= 2 * GmatMathConstants::PI;
   th22 = pow(sin(theta), 2.2);
   t1 = (Real)geo->xtemp * (1.0 + 0.3*(th22 +pow(cos(0.5*tau),3.0) *
                                         (pow(cos(eta),2.2) - th22)));
   expkp = exp(geo->tkp);

   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("      Calculated data:\n         sun_denom = %15lf\n"
          "         hour_angle = %15lf\n         theta = %15lf\n"
          "         eta = %15lf\n"
          "         tau = %15lf\n         th22 = %15lf\n"
          "         t1 = %15lf\n         expkp = %15lf\n",
          sun_denom, hour_angle, theta, eta, tau, th22, t1, expkp);
   #endif

   // Compute the infinity based on altitude
   if (height < 200.0)
   {
      t_infinity =   t1 + 14.0 * geo->tkp + 0.02 * expkp;
   }
   else
   {
      t_infinity = t1 + 28.0 * geo->tkp + 0.03 * expkp;
   }
   tx = 371.6678 + 0.0518806 * t_infinity -294.3505 *
        exp(-0.0021622 * t_infinity);

   // If the spacecraft altitude is below 125 km then
   if (height < 125.0)
   {
      // Compute height dependent polynomial
      for (sum = con_c[4], i = 3; i >= 0; i--)
      {
         sum = con_c[i] + sum*height;
      }
      // Compute temperature
      exotemp = tx + (tx - tzero)*sum/1.500625e6;
   }
   // else if spacecraft altitude is above 125 km then
   else if (height > 125.0)
   {
      // Compute temperature dependent polynomial
      for (sum = con_l[4], i = 3; i >= 0; i--)
      {
         sum = con_l[i] + sum*t_infinity;
      }
      // Compute temperature
      exotemp = t_infinity - (t_infinity - tx) *
         exp(-(tx-tzero)/(t_infinity - tx) *
             (height - 125.0)/35.0 *
             sum/(cbPolarRadius + height) );
   }
   else
      exotemp = tx;

   if (height <= 125.0)
   {
      // Obtain coefficients of polynomial for auxiliary quantities
      // required for heights less than 125 km
      for (c_star[0]=con_c[0] + 1500625.0 * tx/(tx-tzero), i = 1; i <= 4; i++)
      {
         c_star[i] = con_c[i];
      }

      aux[0][0] = 125.0;            // get 1st real root
      aux[0][1] = 0.0;
      roots(c_star, na, aux, 1);    
      root1 = aux[0][0];

      deflate_polynomial(c_star, na, root1, c_star);

      aux[0][0] = 200.0;            // get 2nd real root
      aux[0][1] = 0.0;
      roots(c_star, na-1, aux, 1);    
      root2 = aux[0][0];

      deflate_polynomial(c_star, na-1, root2, c_star);

      aux[0][0] = 10.0;            // get remaining roots
      aux[0][1] = 125.0;
      roots(c_star, na-2, aux, 1);    
      x_root = aux[0][0];
      y_root = fabs(aux[0][1]);
   }
   return exotemp;
   
}


//------------------------------------------------------------------------------
//  Real rho_100(Real height, Real temperature)
//------------------------------------------------------------------------------
/**
 *  Compute density of the atmosphere between 90 and 100 km using the 
 *  Jacchia-Roberts density model.
 *  
 *  @param height      Spacecraft altitude (km)
 *  @param temperature Exospheric temperature at spacecraft altitude(degrees K)
 *
 *  @return The density
 */
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::rho_100(Real height, Real temperature)
{
   Real m_poly, s_poly, f2, p1, p2, p3, p4, p5, p6, b[6];
   Real x_star, u[2], w[2], v, factor_k, roots_2, log_f1;
   Integer i;

   // Compute M(z) polynomial
   for (m_poly = m_con[6], i = 5; i >= 0; i--)
   {
      m_poly = m_poly * height + m_con[i];
   }

   // Compute temperature dependent coefficients
   for (i = 0; i <= 5; i++)
   {
      b[i] = s_con[i] + s_beta[i] * tx/(tx-tzero);
   }

   // Compute functions of auxiliary temperature values
   roots_2 = x_root*x_root + y_root*y_root;
   x_star = -2.0* root1 * root2 * cbPolarRadius * (cbPolarSquared +
         2.0*cbPolarRadius*x_root + roots_2);
   v = (cbPolarRadius + root1)*(cbPolarRadius +root2) * (cbPolarSquared +
         2.0*cbPolarRadius*x_root + roots_2);
   u[0] = (root1 - root2) * (root1 + cbPolarRadius) * (root1 + cbPolarRadius) *
        (root1*root1 - 2.0*root1*x_root + roots_2);
   u[1] = (root1 - root2) * (root2 + cbPolarRadius) * (root2 + cbPolarRadius) *
        (root2*root2 - 2.0*root2*x_root + roots_2);
   w[0] = root1 * root2 * cbPolarRadius * (cbPolarRadius + root1) *
         (cbPolarRadius + roots_2/root1);
   w[1] = root1 * root2 * cbPolarRadius * (cbPolarRadius + root2) *
         (cbPolarRadius + roots_2/root2);

   // Compute S(z) polynomial for z = root1
   for (s_poly = b[5], i=4; i>=0; i--)
   {
      s_poly = s_poly * root1 + b[i];
   }
   // Compute power of second quantity in f1 function
   p2 = s_poly/u[0];

   // Compute S(z) polynomial for z = root2
   for (s_poly = b[5], i=4; i>=0; i--)
   {
      s_poly = s_poly * root2 + b[i];
   }
   // Compute power of third quantity in f1 function
   p3 = -s_poly/u[1];
   // Compute S(z) polynomial for z = negative earth radius
   for (s_poly = b[5], i=4; i>=0; i--)
   {
      s_poly = -s_poly * cbPolarRadius + b[i];
   }
   // Compute p5 factor in f2 function
   p5 = s_poly/v;

   // Compute power of fourth quantity in f1 function
   p4 = (b[0] - root1*root2*cbPolarSquared*(b[4] + b[5]*
       (2.0*x_root + root1 + root2 - cbPolarRadius)) + w[0]*p2 + w[1]*p3
       - root1*root2*b[5]*cbPolarRadius*roots_2
       + root1*root2*(cbPolarSquared - roots_2)*p5)/x_star;

   // Compute power of first quantity in f1 function
   p1 = b[5] - 2*p4 - p3 - p2;

   // Compute p6 factor in f2 function
   p6 = b[4] + b[5]*(2.0*x_root + root1 + root2 - cbPolarRadius) - p5
       - 2.0*(x_root + cbPolarRadius)*p4 - (root2 + cbPolarRadius)*p3
       - (root1 + cbPolarRadius)*p2;

   // Compute natural log of f1 function
   log_f1 =   p1 * log( (height + cbPolarRadius)/(90.0 + cbPolarRadius))
         +   p2 * log( (height - root1)/(90.0 - root1))
         +   p3 * log( (height - root2)/(90.0 - root2))
         +   p4 * log( (height*height - 2.0*x_root*height + roots_2)
             /(8100.0 - 180.0*x_root + roots_2));

   // Compute f2 function
   f2 =    (height - 90.0) * (m_con[6] + p5/((height + cbPolarRadius)*
         (90.0 + cbPolarRadius)))
       + p6 * atan(y_root * (height - 90.0)/(
        y_root*y_root + (height - x_root)*(90.0 - x_root))) / y_root;

   // Compute f1 power
   factor_k = -1500625.0*g_zero*cbPolarSquared/(gas_con*con_c[4]*(tx-tzero));

   return rho_zero * tzero * m_poly * exp(factor_k*(log_f1 + f2)) /
       (mzero * temperature);
}


//------------------------------------------------------------------------------
//  Real rho_125(Real height, Real temperature)
//------------------------------------------------------------------------------
/**
 *  Compute density of the atmosphere between 100 and 125 km
 *  using the Jacchia-Roberts density model.
 *  
 *  @param height       Spacecraft altitude (km)
 *  @param temperature  Exospheric temperature at spacecraft (degrees K)
 *
 *  @return The density
 */
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::rho_125(Real height, Real temperature)
{
   Real f4, q1, q2, q3, q4, q5, q6;
   Real rho_prime, x_star, u[2], w[2], v, factor_k, t_100;
   Real rho_sum, rhoi, roots_2, log_f3;
   Integer i;

   // Compute base density polynomial
   for (rho_prime = zeta_con[6], i=5; i>=0; i--)
   {
      rho_prime = rho_prime * t_infinity + zeta_con[i];
   }

   // Compute base temperature
   t_100 = tx + omega * (tx - tzero);

   // Compute functions of auxiliary temperature values
   roots_2 = x_root*x_root + y_root*y_root;
   x_star = -2.0* root1 * root2 * cbPolarRadius * (cbPolarSquared + 2.0 *
         cbPolarRadius*x_root + roots_2);
   v = (cbPolarRadius + root1)*(cbPolarRadius +root2) * (cbPolarSquared + 2.0 *
         cbPolarRadius*x_root + roots_2);
   u[0] = (root1 - root2) * (root1 + cbPolarRadius) * (root1 + cbPolarRadius) *
       (root1*root1 - 2.0*root1*x_root + roots_2);
   u[1] = (root1 - root2) * (root2 + cbPolarRadius) * (root2 + cbPolarRadius) *
       (root2*root2 - 2.0*root2*x_root + roots_2);
   w[0] = root1 * root2 * cbPolarRadius * (cbPolarRadius + root1) *
         (cbPolarRadius + roots_2/root1);
   w[1] = root1 * root2 * cbPolarRadius * (cbPolarRadius + root2) *
         (cbPolarRadius + roots_2/root2);

   // Compute power of second quantity in f3 function
   q2 = 1.0/u[0];

   // Compute power of third quantity in f3 function
   q3 = -1.0/u[1];

   // Compute q5 factor in f4 function
   q5 = 1.0/v;

   // Compute power of fourth quantity in f3 function
   q4 = (1.0 + w[0]*q2 + w[1]*q3
       + root1*root2*(cbPolarSquared - roots_2)*q5)/x_star;

   // Compute power of first quantity in f3 function
   q1 = - 2*q4 - q3 - q2;

   // Compute q6 factor in f4 function
   q6 = - q5 - 2.0*(x_root + cbPolarRadius)*q4 - (root2 + cbPolarRadius)*q3 -
         (root1 +cbPolarRadius)*q2;

   // Compute log of f3 function
   log_f3 =  q1 * log( (height + cbPolarRadius)/(100.0 + cbPolarRadius))
        +  q2 * log( (height - root1)/(100.0 - root1))
        +  q3 * log( (height - root2)/(100.0 - root2))
        +  q4 * log( (height*height - 2.0*x_root*height + roots_2)
            /(1.0e4 - 200.0*x_root + roots_2));

   // Compute f4 function
   f4 =    (height - 100.0) * q5 / ((height + cbPolarRadius) *
         (100.0 + cbPolarRadius))
       + q6 * atan(y_root * (height - 100.0)/(
       y_root*y_root + (height - x_root)*(100.0 - x_root)))/ y_root;

   // Compute f3 power
   factor_k = -1500625.0*g_zero*cbPolarSquared/(gas_con*con_c[4]*(tx-tzero));

   // Compute mass-dependent sum
   for (rho_sum = 0.0, i=0; i<=4; i++)
   {
      rhoi = mol_mass[i] * num_dens[i] *
          exp(mol_mass[i] * factor_k * (f4 + log_f3));

      if (i == 2)
      {
        rhoi *= pow(t_100/temperature, -0.38);
      }
      rho_sum += rhoi;
   }
   
   return rho_sum * rho_prime * t_100 / temperature;
}


//------------------------------------------------------------------------------
//  Real rho_cor(Real height, Real a1_time, Real geo_lat, GEOPARMS *geo)
//------------------------------------------------------------------------------
/**
 * Calculates the density correction factor
 *
 * @param height       Spacecraft altitude (km)                 I
 * @param a1_time      The A.1 epoch
 * @param geo_lat      Geodetic latitude of spacecraft (radians)
 * @param geo          The geomagnetic parameters used in the model
 *
 * @return The atmospheric density at the spacecraft's location */
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::rho_cor(Real height, Real a1_time, Real geo_lat, 
                                       GEOPARMS *geo)
{
   Real geo_cor, semian_cor, slat_cor, f, g, day_58, tausa, alpha;
   Real sin_lat, eta_lat;

   // Compute geomagnetic activity correction
   if (height < 200.0)
   {
      geo_cor = 0.012 * geo->tkp + 0.000012 * exp(geo->tkp);
   }
   else
   {
      geo_cor = 0.0;
   }

   // Compute semiannual variation correction
   f = (5.876e-7 * pow(height, 2.331) + 0.06328) *
      exp(-0.002868 * height);
   day_58 = (a1_time - 6204.5)/365.2422; // @todo - should this use GmatConstants value?
   tausa = day_58 + 0.09544 * (
         pow( 0.5*(1.0 + sin(2*GmatMathConstants::PI*day_58 +6.035)), 1.65 )  - 0.5);
   alpha = sin(4.0*GmatMathConstants::PI*tausa + 4.259);
   g = 0.02835 + (0.3817 + 0.17829 * sin(2*GmatMathConstants::PI*tausa + 4.137)) *
         alpha;
   semian_cor = f * g;

   // Compute seasonal latitudinal variation
   sin_lat = sin(geo_lat);
   eta_lat = sin(2.0*GmatMathConstants::PI*day_58 + 1.72) * sin_lat * fabs(sin_lat);
   slat_cor = 0.014 * (height - 90.0) * eta_lat *
      exp(-0.0013 * (height - 90.0) * (height - 90.0));

   return pow(10.0, geo_cor + semian_cor + slat_cor);
}


//------------------------------------------------------------------------------
//  Real rho_high(Real height, Real temperature, Real t_500, Real sun_dec, 
//                Real geo_lat)
//------------------------------------------------------------------------------
/**
 * Compute density of the atmosphere between 125 and 2500 km using the 
 * Jacchia-Roberts density model.
 *  
 * @param height       Spacecraft altitude (km)                 I
 * @param temperature  Exospheric temperature at spacecraft altitude(degrees K)
 * @param t_500        Exospheric temperature at altitude of 500 km (degrees K)
 * @param sun_dec      Declination of the sun in TOD coordinates (radians)
 * @param geo_lat      Geodetic latitude of spacecraft (radians)
 *
 * @return rho_out, the atmospheric density at the spacecraft's location
 */
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::rho_high(Real height, Real temperature, 
                                        Real t_500, Real sun_dec, Real geo_lat)
{
   Real f, log_di, gamma, exp1, rho_out, r;
   Real di = 0;
   Integer i, j;

   #ifdef DEBUG_JR_DRAG
      MessageInterface::ShowMessage
         ("   Performing rho_high calculations\n");
   #endif

   for (rho_out = 0.0, i = 0; i <= 5; i++)
   {
      // Compute constituent density sum for this atmospheric component
      if (i <= 4)       // (skip 5 => hydrogen)
      {
         for (log_di=con_den[i][6], j=5; j>=0; j--)
         {
            log_di = log_di * t_infinity + con_den[i][j];
         }
         di = pow(10.0, log_di) / avogadro;
      }

      // Compute second exponent in density expression for this component
      Real polar125 = cbPolarRadius + 125.0;
      gamma =   35.0 * mol_mass[i] * g_zero * cbPolarSquared * (t_infinity - tx) /
         ( gas_con * sum * t_infinity * (tx - tzero) * polar125);

      // Compute first exponent in density expression for this component 
      exp1 = 1.0 + gamma;

      // A factor which is non-unity only for helium
      f = 1.0;

      // Compute corrections for helium
      if (i == 2)
      {
         exp1 -= 0.38;
         f = 4.9914 * fabs(sun_dec) * (pow(sin(0.25 * GmatMathConstants::PI - 0.5 * geo_lat *
                                               sun_dec / fabs(sun_dec)), 3)
                                       - 0.35355) / GmatMathConstants::PI;
         f = pow(10.0, f);
      }

      // Add corrections to rho_out, skip hydrogen unless above 500 km
      if (height > 500.0 && i == 5)
      {
         r = mol_mass[5] * pow(10.0, 73.13 - (39.4 - 5.5 * log10(t_500))
                               * log10(t_500))
            * pow(t_500/temperature, exp1)
            * pow((t_infinity - temperature) / (t_infinity - t_500),
                  gamma)
            /avogadro;
         rho_out += r;

      }
      else if (i <= 4)
      {
            
         r = f * mol_mass[i] * di * pow(tx/temperature, exp1)
            * pow((t_infinity - temperature)/(t_infinity - tx), gamma);
         rho_out += r;
      }

      #ifdef DEBUG_JR_DRAG
         MessageInterface::ShowMessage
            ("      i = %d, rho_out = %15.10le\n", i, rho_out);
      #endif
   }
   return rho_out;
   
}


//------------------------------------------------------------------------------
//  void roots(Real a[], Integer na, Real croots[][2], Integer irl)
//------------------------------------------------------------------------------
/**
 * Finds the roots(s) of a polynomial
 *
 *  To compute any desired complex roots (zeroes) of a given polynomial if 
 *  approximations to the desired roots are known.
 * 
 * Modification -
 * Name              Date             Description
 * ----              ----             -----------
 * M. Zwick          12/1/92          Original C code converted from
 *                                    preexisting Fortran code.
 * 
 *                                    (Release 1, SMR 23)
 * 
 * Notes:  This module was converted from preexisting Fortran code
 *        written by J. P. Molineaux of CSC.  Mr. Molineaux based
 *        the original code on Newton's method described in the
 *        following reference:
 * 
 *        Henrici, P. "ELEMENTS OF NUMERICAL ANALYSIS", NEW YORK,
 *        WILEY, 1965, P. 84.
 * 
 *        Curt Anderson of CSC provided mathematical formulas for
 *        conversion of Fortran COMPLEX*16 variables to C formats.
 *
 *  @param a      An array of non-imaginary polynomial coefficients, starting
 *                with the lowest power.
 *  @param na     The number of coefficients, one greater than the degree of
 *                the polynomial.
 *
 *  @param croots A 2-dimensional array of 'na' rows by 2 columns, containing
 *                the starting approximations to the desired roots on input
 *                and the solutions on output.
 *
 *  @param irl    The number of roots (zeroes) desired to be solved for.
 *
 */
//------------------------------------------------------------------------------
void JacchiaRobertsAtmosphere::roots(Real a[], Integer na, Real croots[][2], 
                                     Integer irl)
{
   Integer i, ir, n1, n2, j;
   Real z[2], zs[2], cb[2], cc[2], dif, denom, temp;

   ir = 0;     // ir counts roots solved to this point
   n1 = na - 1;
   n2 = n1 - 1;
   dif = 0.0l;

   // While the number of roots solved to this point (ir) has not exceeded
   // the number passed to this function (irl), perform the following
   // iterations. 

   while (ir < irl)
   {
      z[0] = croots[ir][0];       // z is the currently iterated value
      z[1] = croots[ir][1];       // of the root

      do                          // Iterate while the convergence criterion
      {                           // is greater than the defined limit
         cb[0] = a[n1];           // Real component of complex number
         cb[1] = 0;               // Imaginary component of complex number
         cc[0] = a[n1];           // Real component of complex number
         cc[1] = 0;               // Imaginary component of complex number

         // cc and cb are the values of the polynomial (P(z))
         // and its' derivative (P'(z)).

         for (i = 0; i <= n2; ++i)
         {
            j = n2 - i;
            temp = (z[0]*cb[0] - z[1]*cb[1]) + a[j];
            cb[1] =  z[0]*cb[1] + z[1]*cb[0];
            cb[0] = temp;
            if (j != 0)
            {
               temp = (z[0]*cc[0] - z[1]*cc[1]) + cb[0];
               cc[1] = (z[0]*cc[1] + z[1]*cc[0]) + cb[1];
               cc[0] = temp;
            }
         }
         zs[0] = z[0];             // Result of last iteration saved in zs
         zs[1] = z[1];

         // Newton's Method: Z(I) = Z(I-1) - P(Z(I-1)) / P'(Z(I-1))
         // tends towards a root of the polynomial P(Z).

         denom =   cc[0]*cc[0] + cc[1]*cc[1];
         z[0] -= ((cb[0]*cc[0] + cb[1]*cc[1]) / denom);
         z[1] += ((cb[0]*cc[1] - cb[1]*cc[0]) / denom);

         // The convergence criterion (dif) is the sum of the relative
         // changes in the real and imaginary parts of the root (z).

         dif = fabs( (zs[0] - z[0]) / zs[0]);
         if(zs[1] != 0.0E0)
         {
            dif += fabs( (zs[1] - z[1]) / zs[1]);
         }
      }
      while (dif > 1.0E-14);  // If convergence criterion is too large
                              // iterate again

      // Convergence criterion is complete.  Store the answer in the
      // output array 'croots' and start on the next root.

      croots[ir][0] = z[0];
      croots[ir][1] = z[1];
      ++ir;                   // Increment roots solved counter
   }
   
}


//------------------------------------------------------------------------------
//  void deflate_polynomial(Real c[], Integer n, Real root, Real c_new[])
//------------------------------------------------------------------------------
/**
 * Reduces the order of a polynomial by division
 *
 * Given a polynomial P(z) of order n-1 specified by its real coefficients
 * c[n-1] to c[0], along with a single real root of the polynomial, this
 * function divides the polynomial by (z - root) to arrive at a new set of
 * coefficients, c_new[n-2] to c_new[0]. This routine is designed to allow
 * calls where c[] and c_new[] are the same array, e.g.,
 * deflate_polynomial(c, 5, 3.5, c); Note however that only c_new[0] to
 * c_new[n-2] are touched, c_new[n-1] is not, so that if c[] and c_new[]
 * are the same, the highest order coefficient of c[] is not zeroed out.
 *
 * Reference: Numerical Recipes in C, section 5.3.
 *                                                                         
 *  Modification -
 *  Name              Date             Description
 *  ----              ----             ----------- 
 *  D. Ginn           93/8/31          Created unit.
 *
 *  @param c The real coefficients of the polynomial
 *  @param n The order + 1 of the polynomial
 *  @param root A single real root of the polynomial
 *  @param c_new The array that is filled with new polynomial coefficients
 */                                                                                     
//------------------------------------------------------------------------------
void JacchiaRobertsAtmosphere::deflate_polynomial(Real c[], Integer n, 
                                                  Real root, Real c_new[])
{
    short i;
    Real sum, save;

    sum = c[n-1];

    for(i = n - 2; i >= 0; i--)
    {
        save = c[i];
        c_new[i] = sum;
        sum = save + sum * root;
    }
    
}


//------------------------------------------------------------------------------
// Real length_of(Real v[3])
//------------------------------------------------------------------------------
/**
 * Calculates the norm of a 3-vector
 *
 *  @param v The 3-vector
 *
 *  @return the norm of the 3-vector, |v[3]|
 */
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::length_of(Real v[3])
{
   return( sqrt( dot_product(v,v) ) );
}


//------------------------------------------------------------------------------
// Real dot_product(Real a[3] , Real b[3])
//------------------------------------------------------------------------------
/**
 * Calculates a dot product
 *
 *  @param a[3] The first vector in the product
 *  @param b[3] The second vector in the product
 *
 *  @return a dot b
 */
//------------------------------------------------------------------------------
Real JacchiaRobertsAtmosphere::dot_product(Real a[3] , Real b[3])
{
    return(a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}


//------------------------------------------------------------------------------
// void LoadConstants()
//------------------------------------------------------------------------------
/**
 * Loads the constants used inthe model
 *
 * (These constants were formerly defined as file level globals)
 */
//------------------------------------------------------------------------------
void JacchiaRobertsAtmosphere::LoadConstants()
{
   con_c[0] = -89284375.0;
   con_c[1] =   3542400.0;
   con_c[2] =    -52687.5;
   con_c[3] =       340.5;
   con_c[4] =        -0.8;

   con_l[0] = 0.1031445e5;
   con_l[1] = 0.2341230e1;
   con_l[2] = 0.1579202e-2;
   con_l[3] = -0.1252487e-5;
   con_l[4] = 0.2462708e-9;

   mzero = 28.82678;

   m_con[0] = -435093.363387;      // of M(z) function
   m_con[1] = 28275.5646391;       // (1/km)
   m_con[2] = -765.33466108;       // (1/km**2)
   m_con[3] = 11.043387545;        // (1/km**3)
   m_con[4] = -0.08958790995;      // (1/km**4)
   m_con[5] = 0.00038737586;       // (1/km**5)
   m_con[6] = -0.000000697444;     // (1/km**6)

   s_con[0] = 3144902516.672729;   // of S(z) function
   s_con[1] = -123774885.4832917;  // (1/km)
   s_con[2] = 1816141.096520398;   // (1/km**2)
   s_con[3] = -11403.31079489267;  // (1/km**3)
   s_con[4] = 24.36498612105595;   // (1/km**4)
   s_con[5] = 0.008957502869707995;// (1/km**5)

   s_beta[0] = -52864482.17910969; // of S(z) function - temperature part
   s_beta[1] = -16632.50847336828; // (1/km)
   s_beta[2] = -1.308252378125;    // (1/km**2)
   s_beta[3] = 0.0;                // (1/km**3)
   s_beta[4] = 0.0;                // (1/km**4)
   s_beta[5] = 0.0;                // (1/km**5)

   omega = -0.94585589;

   zeta_con[0] = 0.1985549e-10;
   zeta_con[1] = -0.1833490e-14;   // (1/deg)
   zeta_con[2] = 0.1711735e-17;    // (1/deg**2)
   zeta_con[3] = -0.1021474e-20;   // (1/deg**3)
   zeta_con[4] = 0.3727894e-24;    // (1/deg**4)
   zeta_con[5] = -0.7734110e-28;   // (1/deg**5)
   zeta_con[6] = 0.7026942e-32;    // (1/deg**6)

   mol_mass[0] = 28.0134;          // Nitrogen
   mol_mass[1] = 39.948;           // Argon
   mol_mass[2] = 4.0026;           // Helium
   mol_mass[3] = 31.9988;          // Molecular Oxygen
   mol_mass[4] = 15.9994;          // Atomic Oxygen
   mol_mass[5] = 1.00797;          // Hydrogen

   num_dens[0] = 0.78110;          // Nitrogen
   num_dens[1] = 0.93432e-2;       // Argon
   num_dens[2] = 0.61471e-5;       // Helium
   num_dens[3] = 0.161778;         // Molecular Oxygen
   num_dens[4] = 0.95544e-1;       // Atomic Oxygen

   // Nitrogen
   con_den[0][0] = 0.1093155e2;
   con_den[0][1] = 0.1186783e-2;   // (1/deg)
   con_den[0][2] = -0.1677341e-5;  // (1/deg**2)
   con_den[0][3] = 0.1420228e-8;   // (1/deg**3)
   con_den[0][4] = -0.7139785e-12; // (1/deg**4)
   con_den[0][5] = 0.1969715e-15;  // (1/deg**5)
   con_den[0][6] = -0.2296182e-19; // (1/deg**6)

   // Argon
   con_den[1][0] = 0.8049405e1;
   con_den[1][1] = 0.2382822e-2;   // (1/deg)
   con_den[1][2] = -0.3391366e-5;  // (1/deg**2)
   con_den[1][3] = 0.2909714e-8;   // (1/deg**3)
   con_den[1][4] = -0.1481702e-11; // (1/deg**4)
   con_den[1][5] = 0.4127600e-15;  // (1/deg**5)
   con_den[1][6] = -0.4837461e-19; // (1/deg**6)

   // Helium
   con_den[2][0] = 0.7646886e1;
   con_den[2][1] = -0.4383486e-3;  // (1/deg)
   con_den[2][2] = 0.4694319e-6;   // (1/deg**2)
   con_den[2][3] = -0.2894886e-9;  // (1/deg**3)
   con_den[2][4] = 0.9451989e-13;  // (1/deg**4)
   con_den[2][5] = -0.1270838e-16; // (1/deg**5)
   con_den[2][6] = 0.0;            // (1/deg**6)

   // Molecular Oxygen
   con_den[3][0] = 0.9924237e1;
   con_den[3][1] = 0.1600311e-2;   // (1/deg)
   con_den[3][2] = -0.2274761e-5;  // (1/deg**2)
   con_den[3][3] = 0.1938454e-8;   // (1/deg**3)
   con_den[3][4] = -0.9782183e-12; // (1/deg**4)
   con_den[3][5] = 0.2698450e-15;  // (1/deg**5)
   con_den[3][6] = -0.3131808e-19; // (1/deg**6)

   // Atomic Oxygen
   con_den[4][0] = 0.1097083e2;
   con_den[4][1] = 0.6118742e-4;   // (1/deg)
   con_den[4][2] = -0.1165003e-6;  // (1/deg**2)
   con_den[4][3] = 0.9239354e-10;  // (1/deg**3)
   con_den[4][4] = -0.3490739e-13; // (1/deg**4)
   con_den[4][5] = 0.5116298e-17;  // (1/deg**5)
   con_den[4][6] = 0.0;            // (1/deg**6)
}
