//$Id$
//------------------------------------------------------------------------------
//                           GmatConsole driver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Program entry point for GmatConsole.
 */
//------------------------------------------------------------------------------


#include "driver.hpp" 

#include <fstream>

#include "BaseException.hpp"
#include "ConsoleAppException.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "Moderator.hpp"

#include "CommandFactory.hpp"
#include "PointMassForce.hpp"
#include "PrintUtility.hpp"


//------------------------------------------------------------------------------
//  void ShowHelp()
//------------------------------------------------------------------------------
/**
 * Lists the commands available for the application.
 */
//------------------------------------------------------------------------------
void ShowHelp()
{
   std::cout << "Usage: One of the following\n"  
             << "   GmatConsole\n"
             << "   GmatConsole ScriptFileName\n"
             << "   GmatConsole <option> <string>\n\n"
             << "The first selection runs an interactive session.\n"
             << "The second runs the input script once and then exits.\n"
             << "The third selection executes specific testing scenarios.\n\n" 
             << "Valid options are:\n"
             << "   --help               Shows available options\n"
             << "   --save               Saves current script (interactive "
             << "mode only)\n"
             << "   --summary            Writes command summary (interactive "
             << "mode only)\n"
             << "   --batch <filename>   "
             << "Runs multiple scripts listed in specified file\n"
             << "   --verbose <on/off>   "
             << "Toggles display of command sequence prior to a run\n"
             << "                        This option is set on the startup line\n"
             << "                        (default is on)\n"
             << std::endl;   
}


//------------------------------------------------------------------------------
// void RunScriptInterpreter(std::string script, int verbosity, bool batchmode)
//------------------------------------------------------------------------------
/**
 * Executes a script.
 * 
 * @param <script> The script file that is run.
 * @param <verbosity> Toggles the display of the command list (from the script)
 *                    on or off.  Likely to become more robust over time.
 * @param <batchmode> Flag indicating if the script is part of a batch or a
 *                    single script.
 */
//------------------------------------------------------------------------------
void RunScriptInterpreter(std::string script, int verbosity, bool batchmode)
{
   static bool moderatorInitialized = false;
   
   std::ifstream fin(script.c_str());
   if (!(fin)) {
      std::string errstr = "Script file ";
      errstr += script;
      errstr += " does not exist";
      if (!batchmode) {
         std::cout << errstr << std::endl;
         return;
      }
      else
         throw  ConsoleAppException(errstr);
   }
   else
      fin.close();

   ConsoleMessageReceiver *theMessageReceiver = 
      ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(theMessageReceiver);
   
   Moderator *mod = Moderator::Instance();
   
   if (!moderatorInitialized) {
      if (!mod->Initialize()) {
         throw ConsoleAppException("Moderator failed to initialize!");
      }
      
      moderatorInitialized = true;
   }
   
   try
   {
      if (!mod->InterpretScript(script)) {
         if (!batchmode) {
            std::cout << "\n***Could not read script.***\n\n";
            ShowHelp();
         }
         else
            throw ConsoleAppException("Script file did not parse");
         return;
      }
   }
   catch (BaseException &oops)
   {
      std::cout << "ERROR!!!!!! ---- " << oops.GetFullMessage();
   }
   // print out the sequence
   GmatCommand *top = mod->GetFirstCommand();
   if (verbosity != 0)
   {
      PrintUtility* pu = PrintUtility::Instance();
      pu->PrintEntireSequence(top);
   }
   
   // And now run it
   if (mod->RunMission() != 1)
      throw ConsoleAppException("Moderator::RunMission failed");
   
   // Success!
   if (!batchmode)
      std::cout << "\n\n*** GMAT Integration test "
                << "(Console version) successful! ***"
                << "\n\n";
}


//------------------------------------------------------------------------------
// Integer RunBatch(std::string& batchfilename)
//------------------------------------------------------------------------------
/**
 * Executes a collection of scripts.
 * 
 * @param <batchfilename> The file containing the list of script files to run.
 * 
 * @return The number of lines parsed from teh batch file.
 */
//------------------------------------------------------------------------------
Integer RunBatch(std::string& batchfilename)
{
   Integer count = 0, successful = 0, failed = 0, skipped = 0;
   std::string script;
   StringArray failedScripts;
   StringArray skippedScripts;
      
   std::cout << "Running batch file \"" << batchfilename << "\"" << std::endl;
   std::ifstream batchfile(batchfilename.c_str());
   
   if (!(batchfile)) {
      std::string errstr = "Batch file ";
      errstr += batchfilename;
      errstr += " does not exist";
      std::cout << errstr << std::endl;
         return 0;
   }
   
   batchfile >> script;

   while (!batchfile.eof()) {
      if (script == "--summary")
      {
         ShowCommandSummary();
      }
      else
      {
         ++count;
         if (script[0] != '%') {
            std::cout << "\n*************************************************\n*** " 
                      << count << ": \"" << script 
                      << "\"\n*************************************************\n" 
                      << std::endl;
            try {
               RunScriptInterpreter(script, 0, true);
               ++successful;
            }
            catch (BaseException &ex) {
               std::cout << "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << "!!!\n"
                         << "!!! Exception in script \"" << script << "\"\n"
                         << "!!!    \""
                         << ex.GetFullMessage() << "\"\n" 
                         << "!!!\n"
                         << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << std::endl;
                         
               ++failed;
               failedScripts.push_back(script);
            }
            catch (...)
            {
               std::cout << "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << "!!!\n"
                         << "!!! Unhandled Exception in script \"" << script << "\"\n"
                         << "!!!\n"
                         << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << std::endl;
                         
               ++failed;
               failedScripts.push_back(script);
            }
         }
         else {
            std::cout << "\n*************************************************\n*** " 
                      << count << ": Skipping script \"" << script.substr(1)
                      << "\"\n*************************************************\n" 
                      << std::endl;
            skippedScripts.push_back(script.substr(1));
            ++skipped;
         }
      }
      batchfile >> script;
   }
   
   std::cout << "\n\n**************************************\n*** "
             << "Batch Run Statistics:"
             <<               "\n***   Successful scripts:  " 
             << successful << "\n***   Failed Scripts:      "
             << failed     << "\n***   Skipped Scripts:     "
             << skipped    << "\n**************************************\n";
             
   if (failed > 0) {
      std::cout << "\n**************************************\n" 
                << "***   Scripts that failed:\n";
      for (StringArray::iterator i = failedScripts.begin(); 
           i != failedScripts.end(); ++i)
         std::cout << "***      " << *i << "\n";
      std::cout << "**************************************\n";
   }
   if (skipped > 0) {
      std::cout << "\n**************************************\n"<< "***   Scripts that were skipped:\n";
      for (StringArray::iterator i = skippedScripts.begin(); 
           i != skippedScripts.end(); ++i)
         std::cout << "***      " << *i << "\n";
      std::cout << "**************************************\n\n";
   }

   return count;
}


//------------------------------------------------------------------------------
// void SaveScript(std::string filename)
//------------------------------------------------------------------------------
/**
 * Saves the current script to a file
 * 
 * @param <filename> The name of the script file.
 */
//------------------------------------------------------------------------------
void SaveScript(std::string filename)
{
    Moderator *mod = Moderator::Instance();
    mod->SaveScript(filename);
    std::cout << "\n\n";
}


//------------------------------------------------------------------------------
// void ShowCommandSummary(std::string filename)
//------------------------------------------------------------------------------
/**
 * Displays the command summary, either on screen or writing to a file
 * 
 * @param <filename> The name of the summary file.
 */
//------------------------------------------------------------------------------
void ShowCommandSummary(std::string filename)
{
   Moderator *mod = Moderator::Instance();
   GmatCommand *cmd = mod->GetFirstCommand();
   if (cmd->GetTypeName() == "NoOp")
      cmd = cmd->GetNext();
   
   if (cmd == NULL)
   {
      std::cout << "Command stream is empty.\n\n";
      return;
   }
   
   if (filename == "")
   {
      std::cout << "\n\n";
      std::string summary = cmd->GetStringParameter("MissionSummary");
      std::cout << summary << "\n\n";
   }
   else
   {
      std::cout << "File output for command summaries is not yet available\n\n";
   }
}


//------------------------------------------------------------------------------
// void TestSyncModeAccess(std::string filename)
//------------------------------------------------------------------------------
/**
 * Tests the propsync script.
 * 
 * @note This looks like old code that should be removed.
 * 
 * @param <filename> The name of the script file.  (Not used)
 */
//------------------------------------------------------------------------------
void TestSyncModeAccess(std::string filwwename)
{
    Moderator *mod = Moderator::Instance();
    
    // First load up the Moderator with the propsync script
    RunScriptInterpreter("propsync.script", 1);
    std::cout << "\n\n";
    
    // Find the command entry point
    GmatCommand *cmd = mod->GetFirstCommand();
    StringArray props, sats;
    
    while (cmd) {
       if (cmd->GetTypeName() == "Propagate") {
          std::cout << "Found \"" << cmd->GetGeneratingString() << "\"\n";
          std::cout << "Current propagation mode is \"" 
                    << cmd->GetStringParameter("PropagateMode")
                    << "\"\n";
          props = cmd->GetStringArrayParameter("Propagator");
          for (Integer i = 0; i < (Integer) props.size(); ++i) {
             std::cout << "  Propagator: " << props[i] << "\n";
             sats = cmd->GetStringArrayParameter("Spacecraft", i);
             for (Integer i = 0; i < (Integer) sats.size(); ++i) {
                std::cout << "    SpaceObject: " << sats[i] << "\n";
             }
          }
          
          // Now try clearing this puppy
          std::cout << "*** Testing the \"Clear\" action\n";
          cmd->TakeAction("Clear");
          std::cout << "Current propagation mode is \"" 
                    << cmd->GetStringParameter("PropagateMode")
                    << "\"\n";
          props = cmd->GetStringArrayParameter("Propagator");
          for (Integer i = 0; i < (Integer) props.size(); ++i) {
             std::cout << "  Propagator: " << props[i] << "\n";
             sats = cmd->GetStringArrayParameter("Spacecraft", i);
             for (Integer i = 0; i < (Integer) sats.size(); ++i) {
                std::cout << "    SpaceObject: " << sats[i] << "\n";
             }
          }
          
          // Now add in some bogus data
          std::cout << "*** Testing the \"SetString\" method: \"\", "
                    << "\"Bogus\", \"Synchronized\"\n";
          cmd->SetStringParameter("PropagateMode", "");
          std::cout << "Current propagation mode is \"" 
                    << cmd->GetStringParameter("PropagateMode")
                    << "\"\n";
//          cmd->SetStringParameter("PropagateMode", "Bogus");
          std::cout << "Current propagation mode is \"" 
                    << cmd->GetStringParameter("PropagateMode")
                    << "\"\n";
          cmd->SetStringParameter("PropagateMode", "Synchronized");
          std::cout << "Current propagation mode is \"" 
                    << cmd->GetStringParameter("PropagateMode")
                    << "\"\n";

          std::cout << "Setting the stooges as the PropSetups\n";
          cmd->SetStringParameter("Propagator", "Moe");
          cmd->SetStringParameter("Propagator", "Curly");
          cmd->SetStringParameter("Propagator", "Larry");
          std::cout << "Setting the dwarfs as the Spacecraft\n";
          cmd->SetStringParameter("Spacecraft", "Dopey", 0);
          cmd->SetStringParameter("Spacecraft", "Sleepy", 1);
          cmd->SetStringParameter("Spacecraft", "Doc", 2);
          cmd->SetStringParameter("Spacecraft", "Happy", 0);
          cmd->SetStringParameter("Spacecraft", "Grumpy", 1);
          cmd->SetStringParameter("Spacecraft", "Bashful", 2);
          cmd->SetStringParameter("Spacecraft", "Sneezy", 0);
          
          props = cmd->GetStringArrayParameter("Propagator");
          for (Integer i = 0; i < (Integer) props.size(); ++i) {
             std::cout << "  Propagator: " << props[i] << "\n";
             sats = cmd->GetStringArrayParameter("Spacecraft", i);
             for (Integer i = 0; i < (Integer) sats.size(); ++i) {
                std::cout << "    SpaceObject: " << sats[i] << "\n";
             }
          }
       }
       
       cmd = cmd->GetNext();
    }
    
    std::cout << "\n\n";
}


//------------------------------------------------------------------------------
// void DumpDEData(double secsToStep)
//------------------------------------------------------------------------------
/**
 * Writes out the Earth and Moon position and velocity data for a set span to
 * the file EarthMoonDe.txt
 *
 * @param secsToStep The timestep to use
 * @param spanInSecs The time span in seconds
 */
//------------------------------------------------------------------------------
void DumpDEData(double secsToStep, double spanInSecs)
{
   double baseEpoch = 21545.0, currentEpoch = 21545.0;
   long step = 0;
   std::ofstream data("EarthMoonDe.txt");

   Moderator *mod = Moderator::Instance();
   if (!mod->Initialize()) {
      throw ConsoleAppException("Moderator failed to initialize!");
   }

   SolarSystem *sol = mod->GetSolarSystemInUse();
   if (sol == NULL)
      MessageInterface::ShowMessage("Oh no, the solar system is NULL!");

   CelestialBody *earth = sol->GetBody("Earth");
   CelestialBody *moon  = sol->GetBody("Luna");

   data << "Earth and Moon Position and Velocity from the DE file\n\n";

   data.precision(17);
   double targetEpoch = currentEpoch + spanInSecs / 86400.0;
   while (currentEpoch <= targetEpoch)
   {
      currentEpoch = baseEpoch + step * secsToStep / 86400.0;

      Rvector6 earthRV = earth->GetMJ2000State(currentEpoch);
      Rvector6 moonRV  = moon->GetMJ2000State(currentEpoch);
      Rvector3 moonAcc = moon->GetMJ2000Acceleration(currentEpoch);

      data << currentEpoch << " " << (step * secsToStep) << " "
//           << earthRV[0] << " " << earthRV[1] << " " << earthRV[2] << " "
//           << earthRV[3] << " " << earthRV[4] << " " << earthRV[5] << " | "
           << moonRV[0] << " " << moonRV[1] << " " << moonRV[2] << " "
           << moonRV[3] << " " << moonRV[4] << " " << moonRV[5]
           << moonAcc[0] << " " << moonAcc[1] << " " << moonAcc[2]
           << "\n";

      ++step;
   }
   data << std::endl;
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
/**
 * The program entry point.
 * 
 * @param <argc> The count of the input arguments.
 * @param <argv> The input arguments.
 * 
 * @return 0 on success.
 */
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try {
      std::string msg = "General Mission Analysis Tool\nConsole Based Version\n";

      msg += "Build Date: ";
      msg += __DATE__;
      msg += "  ";
      msg += __TIME__;
      
      std::cout << "\n********************************************\n"
                << "***  GMAT Console Application\n"
                << "********************************************\n\n"
                << msg << "\n\n"
                << std::endl;
      
      char scriptfile[1024];
      bool runcomplete = false;
      int verbosity = 1;
      std::string optionParm = "";
      StringArray parms;
      
      do {
         if (argc < 2)
         {
            std::cout << "Enter a script file, " 
                      << "q to quit, or an option:  ";
            
            std::cin >> scriptfile;
            // Drop the return character -- may be platform dependent
            std::cin.ignore(1);
            
            parms.clear();
            std::string chunk;
            // Integer start = 0, finish;
         }
         else
         {
            strcpy(scriptfile, argv[1]);
            if (argc == 3)
               optionParm = argv[2];
            if (optionParm != "")
               std::cout << "Optional parameter: \"" << optionParm << "\"\n";
         }
            
         if ((!strcmp(scriptfile, "q")) || (!strcmp(scriptfile, "Q")))
            runcomplete = true;
            
         if (scriptfile[0] == '-')
         {
            if (!strcmp(scriptfile, "--help"))
            {
               ShowHelp();
            }
            else if (!strcmp(scriptfile, "--batch"))
            {
               RunBatch(optionParm);
            }
            else if (!strcmp(scriptfile, "--save"))
            {
               SaveScript();
            }
            else if (!strcmp(scriptfile, "--summary"))
            {
               ShowCommandSummary();
            }
            else if (!strcmp(scriptfile, "--sync"))
            {
               TestSyncModeAccess();
            }
            else if (!strcmp(scriptfile, "--verbose"))
            {
               if (!strcmp(optionParm.c_str(), "off"))
                  verbosity = 0;
               std::cout << "Verbose mode is " 
                         << (verbosity == 0 ? "off" : "on")
                         << "\n";
               argc = 1;
            }
            // Options used for some detailed tests but hidden from casual users
            // (i.e. missing from the help messages)
            else if (!strcmp(scriptfile, "--DumpDEData"))
            {
               DumpDEData(0.001, 0.2);
            }
            else
            {
               std::cout << "Unrecognized option.\n\n";
               ShowHelp();
            }
         }
         else if (!runcomplete)
            RunScriptInterpreter(scriptfile, verbosity);
         
      } while ((!runcomplete) && (argc < 2));
   }
   catch (BaseException &ex) {
      std::cout << ex.GetFullMessage() << std::endl;
      exit(0);
   }
   
   Moderator::Instance()->Finalize();
   
   return 0;
}

