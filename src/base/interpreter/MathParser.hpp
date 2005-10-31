#ifndef MATHPARSER_HPP_
#define MATHPARSER_HPP_


#include "gmatdefs.hpp"


class MathParser
{
public:
	MathParser();
	virtual ~MathParser();
   MathParser(const MathParser& mp);
   MathParser&       operator=(const MathParser& mp);
   
   void              Build(std::string expression);
   Real              Evaluate();
};

#endif /*MATHPARSER_HPP_*/
