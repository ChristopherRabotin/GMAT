#include "SpaceObjectException.hpp"

SpaceObjectException::SpaceObjectException(std::string details) :
    BaseException           ("SpaceObject Exception Thrown: ", details)
{}


SpaceObjectException::~SpaceObjectException()
{}


SpaceObjectException::SpaceObjectException(const SpaceObjectException &soe) :
    BaseException       (soe)
{}

