/*
Testing the CSALT tutorial

Noble Hatten
2019/07/03
*/

#include "TutorialPathObject.hpp"
#include "MessageInterface.hpp"

// Class constructor
TutorialPathObject::TutorialPathObject() :
	UserPathFunction(),
	gravity(-32.174)
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
TutorialPathObject::~TutorialPathObject()
{
}

// The EvaluateFunctions method
void TutorialPathObject::EvaluateFunctions()
{
	// Extract parameter data
	Rvector stateVec = GetStateVector();
	Rvector controlVec = GetControlVector();

	// Compute the ODEs
	Real u = controlVec(0);
	Real x = stateVec(0);
	Real y = stateVec(1);
	Real v = stateVec(2);
	Real xdot = v * sin(u);
	Real ydot = v * cos(u);
	Real vdot = gravity * cos(u);

	// Set the ODEs by calling SetFuntions with DYNAMICS enum value
	Rvector dynFunctions(3, xdot, ydot, vdot);
	SetFunctions(DYNAMICS, dynFunctions);

}

void TutorialPathObject::EvaluateJacobians()
{
	// Get state and control
	Rvector stateVec = GetStateVector();
	Rvector controlVec = GetControlVector();
	Real u = controlVec(0);
	Real x = stateVec(0);
	Real y = stateVec(1);
	Real v = stateVec(2);

	// The dynamics state Jacobian
	Real dxdot_dv = sin(u);
	Real dydot_dv = cos(u);
	Rmatrix dynState(3, 3,
		0.0, 0.0, dxdot_dv,
		0.0, 0.0, dydot_dv,
		0.0, 0.0, 0.0);

	// The dynamics control Jacobian
	Real dxdot_du = v * cos(u);
	Real dydot_du = -v * sin(u);
	Real dvdot_du = -gravity * sin(u);
	Rmatrix dynControl(3, 1, dxdot_du, dydot_du, dvdot_du);

	// The dynamics time Jacobian
	Rmatrix dynTime(3, 1, 0.0, 0.0, 0.0);

	// Set the Jacobians
	SetJacobian(DYNAMICS, STATE, dynState);
	SetJacobian(DYNAMICS, CONTROL, dynControl);
	SetJacobian(DYNAMICS, TIME, dynTime);
}
