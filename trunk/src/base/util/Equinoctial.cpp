
#include "Equinoctial.hpp"

using namespace GmatMathUtil;

const std::string Equinoctial::DATA_DESCRIPTIONS[NUM_DATA] =
{
	"SemiMajor",
	"Projection of eccentricity onto y_ep axis",
	"Projection of eccentricity onto x_ep axis",
	"Projection of N onto y_ep axis",
	"Projection of N onto y_ep axis",
	"Mean Longitude"
};

Equinoctial::Equinoctial() :
   semiMajor(0.0),
   projEccY(0.0),
   projEccX(0.0),
   projNY(0.0),
   projNX(0.0),
   meanLongitude(0.0)
{
}

Equinoctial::Equinoctial(const Rvector6& state) :
   semiMajor(state[0]),
   projEccY(state[1]),
   projEccX(state[2]),
   projNY(state[3]),
   projNX(state[4]),
   meanLongitude(state[5])
{
}

Equinoctial::Equinoctial(const Real& ma, const Real& pEY, const Real& pEX, const Real& pNY, const Real& pNX, const Real& ml) :
	semiMajor(ma),
	projEccY(pEY),
	projEccX(pEX),
	projNY(pNY),
	projNX(pNX),
	meanLongitude(ml)
{
}

Equinoctial::Equinoctial(const Equinoctial &eq) :
	semiMajor(eq.semiMajor),
	projEccY(eq.projEccY),
	projEccX(eq.projEccX),
	projNY(eq.projNY),
	projNX(eq.projNX),
	meanLongitude(eq.meanLongitude)
{
}

Equinoctial& Equinoctial::operator=(const Equinoctial &eq)
{
	if (this != &eq)
	{
	   semiMajor = eq.semiMajor;
	   projEccY = eq.projEccY;
	   projEccX = eq.projEccX;
	   projNY = eq.projNY;
	   projNX = eq.projNX;
	   meanLongitude = eq.meanLongitude;
	}
	return *this;
}
	  
	
Equinoctial::~Equinoctial()
{
}

//  Friend functions
std::ostream& operator<<(std::ostream& output, const Equinoctial& eq)
{
    Rvector v(6, eq.semiMajor, eq.projEccY, eq.projEccX, eq.projNY, eq.projNX, eq.meanLongitude);

    output << v << std::endl;

    return output;
}

std::istream& operator>>(std::istream& input, Equinoctial& eq) {
    input >> eq.semiMajor >> eq.projEccY
    	  >> eq.projEccX >> eq.projNY
    	  >> eq.projNX >> eq.meanLongitude;

    return input;
}

Rvector6 CartesianToEquinoctial(const Rvector6& cartesian, const Real& grav)
{
	Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
	Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
	Real r = pos.GetMagnitude();
	Real v = vel.GetMagnitude();

	Real sm, pEY, pEX, pNY, pNX, ml;
	
	sm = 1/( (2/r) - ((v*v)/grav) );     // eqn 4.49
	
	Rvector3 eccVec = -pos.GetUnitVector() - Cross(Cross(pos, vel), vel)/grav; // eqn 4.50
	
	Rvector3 angMom = Cross(pos, vel).GetUnitVector();     // eqn 4.51
	
//	Real i = ACos(angMom.Get(2));
	Integer j = 1;
	
	// Unit basis vectors
	Real f[3];
	f[0] = 1.0 - Pow(angMom.Get(0),2)/(1 + Pow(angMom.Get(2),j));     // 4.52
	f[1] = -(angMom.Get(0) * angMom.Get(1))/(1 + Pow(angMom.Get(2),j));     // eqn 4.53
	f[2] = -Pow(angMom.Get(0),j);     // eqn 4.54
	
	Rvector3 fVec(f[0], f[1], f[2]);
	fVec = fVec.GetUnitVector();
	Rvector3 gVec = Cross(angMom, fVec).GetUnitVector();     // eqn 4.55

	pEY =  eccVec * gVec;     // eqn 4.56
	pEX =  eccVec * fVec;     // eqn 4.57
	pNY =  angMom.Get(0) / (1 + Pow(angMom.Get(2), j));     // eqn 4.58
	pNX = -angMom.Get(1) / (1 + Pow(angMom.Get(2), j));     // eqn 4.59
	
	Real x1 = pos * fVec;     // eqn 4.60
	Real y1 = pos * gVec;     // eqn 4.61

	Real beta = 1/(1 + Sqrt(1.0 - pEY*pEY - pEX*pEX));  // eqn 4.36
	
	Real cosF, sinF, trueLong;
	
	// eqn 4.62
	cosF = pEX + ((1 - pEX*pEX*beta)*x1 - pEY*pEX*beta*y1)/(sm*Sqrt(1 - pEY*pEY - pEX*pEX));
	// eqn 4.63
	sinF = pEY + ((1 - pEY*pEY*beta)*y1 - pEY*pEX*beta*x1)/(sm*Sqrt(1 - pEY*pEY - pEX*pEX));
	//eqn 4.64
	trueLong = ATan(sinF, cosF);
	if (trueLong < 0)
	   trueLong += TWO_PI;
	   
	ml = trueLong + pEY * cosF - pEX * sinF;  // eqn 4.65

	ml *= DEG_PER_RAD;
	
	Rvector6 eq(sm, pEY, pEX, pNY, pNX, ml);
	return eq;
}

Rvector6 EquinoctialToCartesian(const Rvector6& equinoctial, const Real& grav)
{
	Real sm = equinoctial[0],   // semi major axis
	    pEY = equinoctial[1],   // projection of eccentricity vector onto y
	    pEX = equinoctial[2],   // projection of eccentricity vector onto x
	    pNY = equinoctial[3],   // projection of N onto y
	    pNX = equinoctial[4],   // projection of N onto x
	     ml = equinoctial[5]*RAD_PER_DEG;   // mean longitude
	     
	Real prevTrueLong, f, f_;
	Real trueLong = ml;		 // first guess is mean longitude
	do {
		prevTrueLong = trueLong;
		f = prevTrueLong + pEY*Cos(prevTrueLong) - pEX*Sin(prevTrueLong) - ml;
		f_ = 1 - pEY*Sin(prevTrueLong) - pEX*Cos(prevTrueLong);
		trueLong = prevTrueLong - f/f_;
	} while (Abs(trueLong-prevTrueLong) > 1E-10);

    if (trueLong < 0)
       trueLong += TWO_PI;

	Real beta = 1/(1 + Sqrt(1 - pEY*pEY - pEX*pEX));  // eq 4.36

	Real n = Sqrt(grav/(sm*sm*sm));   // eq 4.37
	Real r = sm*(1 - pEX*Cos(trueLong) - pEY*Sin(trueLong));  // eq 4.38

	// eqns 4.39 - 4.42
	Real x1  = sm*((1 - pEY*pEY*beta)*Cos(trueLong) + pEY*pEX*beta*Sin(trueLong) - pEX),
	     y1  = sm*((1 - pEX*pEX*beta)*Sin(trueLong) + pEY*pEX*beta*Cos(trueLong) - pEY),
	     _x1 = ((n*sm*sm)/r)*(pEY*pEX*beta*Cos(trueLong) - (1 - pEY*pEY*beta)*Sin(trueLong)),
	     _y1 = ((n*sm*sm)/r)*((1 - pEX*pEX*beta)*Cos(trueLong) - pEY*pEX*beta*Sin(trueLong));

	// assumption in conversion from equinoctial to cartesian
	Integer j = 1;
	   
	Rmatrix33 Q = SetQ(pNY, pNX, j);   // eq 4.46

	// eq 4.45
	Rmatrix33 _Q = (1/(1+pNY*pNY+pNX*pNX))*Q;
	Rvector3 fVec = _Q * Rvector3(1,0,0);
	Rvector3 gVec = _Q * Rvector3(0,1,0);

	Rvector3 pos = x1*fVec + y1*gVec;    // eq 4.43
	Rvector3 vel = _x1*fVec + _y1*gVec;  // eq 4.44
	
	Rvector6 cart( pos.Get(0),
	               pos.Get(1),
	               pos.Get(2),
	               vel.Get(0),
	               vel.Get(1),
	               vel.Get(2) );

	return cart;
}

// public methods
Rvector6 Equinoctial::GetState()
{
	return Rvector6(semiMajor, projEccY, projEccX, projNY, projNX, meanLongitude);
}

void Equinoctial::SetState(const Rvector6& state)
{
   semiMajor = state[0];
   projEccY = state[1];
   projEccX = state[2];
   projNY = state[3];
   projNX = state[4];
   meanLongitude = state[5];
}

Integer Equinoctial::GetNumData() const
{
	return NUM_DATA;
}

const std::string* Equinoctial::GetDataDescriptions() const
{
	return DATA_DESCRIPTIONS;
}

std::string* Equinoctial::ToValueStrings()
{
   std::stringstream ss("");

   ss << semiMajor;
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << projEccY;
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << projEccX;
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << projNY;
   stringValues[3] = ss.str();

   ss.str("");
   ss << projNX;
   stringValues[4] = ss.str();

   ss.str("");
   ss << meanLongitude;
   stringValues[5] = ss.str();
   
   return stringValues;
}


Rmatrix33 SetQ(Real p, Real q, Real j) {
	// eqn 4.46
	return Rmatrix33( 1-p*p+q*q, 	   2*p*q*j,			 2*p,
						  2*p*q, (1+p*p-q*q)*j,			-2*q,
						 -2*p*j,		   2*q, (1-p*p-q*q)*j );
}
