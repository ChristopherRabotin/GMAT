#include "TestApp.hpp"

#include <iostream>
#include <string>
#include "Cartesian.hpp"
#include "Rvector3.hpp"

#include "Moderator.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "ConsoleAppException.hpp"



int main(int argc, char *argv[])
{
   int retval = 0;
   try
   {
      std::cout << "\n*************************************\n"
                << "*** GMAT Library Interface Tester ***\n" 
                << "*************************************\n";

      std::string message;
      StringArray msgArray;

      ConsoleMessageReceiver *theMessageReceiver = 
            ConsoleMessageReceiver::Instance();
      MessageInterface::SetMessageReceiver(theMessageReceiver);

      // Build a core object
      Cartesian *myCart;
      myCart = new Cartesian;

      message = "Initial state data is \n   ";
      std::cout << message 
                << myCart->GetPosition(0) << ", "
                << myCart->GetPosition(1) << ", "
                << myCart->GetPosition(2) << ", "
                << myCart->GetVelocity(0) << ", "
                << myCart->GetVelocity(1) << ", "
                << myCart->GetVelocity(2) << "\n";
   
      Rvector3 pos, vel;
      pos(0) = 7000.0;
      pos(1) = 1000.0;
      pos(2) = 0.0;
      vel(0) = 0.0;
      vel(1) = -0.5;
      vel(2) = 7.5;

      myCart->SetPosition(pos);
      myCart->SetVelocity(vel);

      std::cout << "\nAfter setting, state data is \n   " 
                << myCart->GetPosition(0) << ", "
                << myCart->GetPosition(1) << ", "
                << myCart->GetPosition(2) << ", "
                << myCart->GetVelocity(0) << ", "
                << myCart->GetVelocity(1) << ", "
                << myCart->GetVelocity(2) << "\n";

      std::cout << "\nAccessing the Moderator singleton\n";
      Moderator *mod = Moderator::Instance();
      std::cout << "\nInitializing the Moderator\n";
      mod->Initialize("C:\\TS_Code\\VS2010\\GmatDevelopment\\build\\GmatVS2010\\Debug\\gmat_startup_file.txt");
      std::cout << "\nReady to roll\n";

      try
      {
         if (!mod->InterpretScript("Ex_TRMM.script")) 
               std::cout << "\n***Could not read script.***\n\n";

         if (mod->RunMission() != 1)
            throw ConsoleAppException("Moderator::RunMission failed\n");
      }
      catch (BaseException &oops)
      {
         std::cout << "ERROR!!!!!! ---- " << oops.GetFullMessage() << std::endl;
      }

      std::cout << "Success!!!\n";


      delete myCart;
      std::cout << "\n*************************************\n"
                << "***       Finished Testing!       ***\n"
                << "*************************************\n" << std::endl;
   }
   catch (BaseException &ex)
   {
      std::cout << "Gmat exception thrown: " << ex.GetFullMessage() << std::endl;
   }
   catch (...)
   {
      std::cout << "Unhandled exception thrown" << std::endl;
   }

   return retval;
} 
