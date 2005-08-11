//$Header$
//------------------------------------------------------------------------------
//                               ScriptInterpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/09/11
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class definition for the ScriptInterpreter
 */
//------------------------------------------------------------------------------


#ifndef SCRIPTINTERPRETER_HPP
#define SCRIPTINTERPRETER_HPP

#include "Interpreter.hpp" // inheriting class's header file

// Longest allowed single line in the script file
#define         MAXLINELENGTH       4096


/**
 * The ScriptInterpreter translates script files into GMAT objects and saves 
 * GMAT objects in script files
 */
class ScriptInterpreter : public Interpreter
{
	public:
        // class constructor
        static ScriptInterpreter*   Instance();

        virtual bool                Interpret();
        virtual bool                Interpret(const std::string &scriptfile);
        virtual bool                Build();
        virtual bool                Build(const std::string &scriptfile);
        
    protected:
        /// The script interpreter singleton
        static ScriptInterpreter    *instance;
        /// Name of the current script file
        std::string                 filename;
        /// Toggle for the portion of the script -- Initialization or sequence
        bool                        sequenceStarted;
        
        bool                        ReadScript();
        bool                        ReadLine();
        bool                        Parse();
        
        bool                        WriteScript();

        bool                        ConfigureCommand(GmatCommand *);
        bool                        ConfigureMathematics();
        
        ScriptInterpreter();
        virtual ~ScriptInterpreter();
};

#endif // SCRIPTINTERPRETER_HPP

