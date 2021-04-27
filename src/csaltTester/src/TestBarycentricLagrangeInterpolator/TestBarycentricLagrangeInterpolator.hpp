#include <iostream>
#include "BarycentricLagrangeInterpolator.hpp"
#ifndef TestBarycentricLagrangeInterpolator_hpp
#define TestBarycentricLagrangeInterpolator_hpp

class TestBarycentricLagrangeInterpolator
{
public:
   static int RunTests()
   {
      std::cout << "\nLagrange Interpolator Unit-Tester: begins" << std::endl;
      // the following is for indVar test of strictly-increasing-ness
      Rvector indVar(5);
   
      for (int idx = 0; idx < indVar.GetSize(); ++idx)
         indVar(idx) = -1;

      BarycentricLagrangeInterpolator BLI;
   
      /*
      std::cout << "indVar test: strictly-increasing-ness test" << std::endl;
      std::cout << "                                                  " << std::endl;
      BLI.SetIndVarVec(&indVar);
      if (BLI.IsIndVarVecDefined() == false)
      {
         std::cout << "indVar test: strictly-increasing-ness test succeed" << std::endl;
         std::cout << "                                                  " << std::endl;
      }
      else
      {
         std::cout << "indVar test: strictly-increasing-ness test failed" << std::endl;
         std::cout << "                                                  " << std::endl;
      }

      indVar.SetSize(1);
      std::cout << "new indVar is" << indVar << std::endl;

      std::cout << "indVar test: size test " << std::endl;
      std::cout << "                                                  " << std::endl;
   
      BLI.SetIndVarVec(&indVar);
      if (BLI.IsIndVarVecDefined() == false)
      {
         std::cout << "indVar test: size test succeed" << std::endl;
         std::cout << "                                                  " << std::endl;
      }
      else
      {
         std::cout << "indVar test: size test failed" << std::endl;
         std::cout << "                                                  " << std::endl;
      }
      */
      indVar.SetSize(10);
      for (int idx = 0; idx < 10; idx++)
      {
         indVar(idx) = 0.1*idx;
      }

      std::cout << "\nindVar test: setting indvar vector test" << std::endl;
      BLI.SetIndVarVec(&indVar);
      if (BLI.IsIndVarVecDefined() == true)
      {
         std::cout << "setting indVar succeed" << std::endl;
      }
      else
      {
         std::cout << "setting indVar failed" << std::endl;
      }
      if ((BLI.GetNumIndVarVec() == 10)
	      && (BLI.IsIndVarVecDefined() == true))
      {
	     std::cout << "GetNumIndVar and IsIndVarVecDefined method tests succeed" <<std::endl;
      }
      Rvector interpPointVec(2);
      interpPointVec(0) = 1;
      interpPointVec(1) = -1;
   
   
      std::cout << "interpPoint test: setting interpPointVec test" << std::endl;
   
      //BLI.SetInterpPointVec(&interpPointVec);

      interpPointVec.SetSize(10);

      interpPointVec(0) = 0.13;   interpPointVec(1) = 0.16;
      interpPointVec(2) = 0.19;   interpPointVec(3) = 0.34;
      interpPointVec(4) = 0.38;   interpPointVec(5) = 0.45;
      interpPointVec(6) = 0.73;   interpPointVec(7) = 0.76;
      interpPointVec(8) = 0.79;   interpPointVec(9) = 0.85;

      BLI.SetInterpPointVec(&interpPointVec);
      if (BLI.IsInterpPointVecDefined() == true)
      {
         std::cout << "setting interpPointVec succeed" << std::endl;
      }
      else
      {
         std::cout << "setting interpPointVec failed" << std::endl;
      }

      //interpPointVec(0) = 0.1;

      /*
      std::cout << "interpPoint test: coincidence with indVar test" << std::endl;
      std::cout << "                                                  " << std::endl;
      BLI.SetInterpPointVec(&interpPointVec);
      if (BLI.IsInterpPointVecDefined() == false)
      {
         std::cout << "coincidence with indVar test succeed" << std::endl;
         std::cout << "                                                  " << std::endl;
      }
      else
      {
         std::cout << "coincidence with indVar test failed" << std::endl;
         std::cout << "                                                  " << std::endl;
      }
      */
      interpPointVec(0) = 0.13;


      Rvector funcValueVec(10);
      Rvector truethData(10);

      for (int idx = 0; idx < 10; idx++)
      {
         funcValueVec(idx) = sin(indVar(idx));
	      truethData(idx) = sin(interpPointVec(idx));
      }

      unsigned int numResultVec = interpPointVec.GetSize();

      Rvector resultVec;
      BLI.Interpolate(&funcValueVec, &interpPointVec, resultVec);

      double error = 0;
      
      for (int idx = 0; idx < 10; idx++)
      {
         error += abs(resultVec[idx] - truethData[idx]);
      }
      std::cout << "Total interpolator error is:" << error << std::endl;

   
      for (int idx = 0; idx < 10; idx++)
      {
         funcValueVec(idx) = tan(indVar(idx));
         truethData(idx) = tan(interpPointVec(idx));
      }

      BLI.Interpolate(&funcValueVec, &interpPointVec, resultVec);

      error = 0;
      
      for (int idx = 0; idx < 10; idx++)
      {
         error += abs(resultVec[idx] - truethData[idx]);
      }
      std::cout << "Total interpolator error is:" << error << std::endl;   

   
      for (int idx = 0; idx < 10; idx++)
      {
         funcValueVec(idx) = exp(indVar(idx));
         truethData(idx) = exp(interpPointVec(idx));
      }

      BLI.Interpolate(&indVar, &funcValueVec, &interpPointVec, resultVec);

      error = 0;
      
      for (int idx = 0; idx < 10; idx++)
      {
         error += abs(resultVec[idx] - truethData[idx]);
      }
      std::cout << "Total interpolator error is:" << error << std::endl;

      //


      BarycentricLagrangeInterpolator bli1, bli2;
      Rvector testVec3;

      testVec3.SetSize(5);

      testVec3(0) = -1;
      testVec3(1) = -0.5;
      testVec3(2) = 0;
      testVec3(3) = 0.5;
      testVec3(4) = 1;

      bli1.SetIndVarVec(&testVec3);
      bli1.SetInterpPointVec(&interpPointVec);


      bli2 = BarycentricLagrangeInterpolator(bli1);

      if ((bli1.IsInterpPointVecDefined() == bli2.IsInterpPointVecDefined()) &&
            (bli1.IsIndVarVecDefined() == bli2.IsIndVarVecDefined()) &&
            (bli1.GetNumIndVarVec() == bli2.GetNumIndVarVec()) &&
            (bli1.GetNumInterpPointVec() == bli2.GetNumInterpPointVec()))
      {
         std::cout << "copy constructor test succeed" << std::endl;
      }
      else
      {
         std::cout << "copy constructor test failed" << std::endl;
      }

      return 0; 
   }
};

#endif // TestBarycentricLagrangeInterpolator