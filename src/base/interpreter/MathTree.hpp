#ifndef MATHTREE_HPP_
#define MATHTREE_HPP_

#include "gmatdefs.hpp"


class GmatBase;         // Only need the reference here

class MathTree
{
public:
   MathTree();
   virtual ~MathTree();
   MathTree(const MathTree& mt);
   MathTree&         operator=(const MathTree& mt);
   
   Real              Evaluate();
   
protected:
   std::string       label;
   GmatBase          *object;
   Real              value;
   MathTree          *left;
   MathTree          *right;
};

#endif /*MATHTREE_HPP_*/
