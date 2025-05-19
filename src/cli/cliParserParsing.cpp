#include "cliParser.hpp"

#include <iostream>

bool CliParser::parseArgs(int argc, char**argv){

    bool foundArg = false;
    BaseCliOption* option = nullptr;
    std::string value;

    for(int i=1;i<argc;i++){
        const std::string arg(argv[i]);
        if(!foundArg){
            if(arg[0]!='-' || arg.size()==1){
                this->extraArgs.push_back(arg);
                continue;
            }
            foundArg = true;
            bool success = handleArg(arg, &option, &value);
            if (!success|| (option==nullptr)){
                this->errorMsg= "Option " + arg + " not recognized\n";
                return false;
            }
            if (!value.empty()||option->getCliType()==CLI_FLAG){
                if(option->getCliType()==CLI_FLAG&&!value.empty()){
                    this->errorMsg= "Option " + arg + " not recognized\n";
                    return false;
                }
                option->setValue(value);
                foundArg = false;
                option = nullptr;
            }
        } else{
            option->setValue(arg);
            foundArg = false;
            option = nullptr;
        }
    }
    return true;
}

bool CliParser::handleArg(const std::string& arg, BaseCliOption** option, std::string* value){
    value->clear();
    bool longArg = false;
    if(arg[1]=='-'){longArg=true;}
    if (longArg){
        for (auto& [lName, optSP]:this->longNames){
            if(lName.empty()){continue;}
            std::size_t found = arg.find(lName, 2);
            if (found==std::string::npos){
                continue;
            }
            *option = optSP.get();
            if(found+lName.size()<arg.size()){
                *value = arg.substr(found+lName.size());
            }
            return true;
        }
    } else {
        for (auto& [sName, optSP]:this->shortNames){
            if(sName.empty()){continue;}
            std::size_t found = arg.find(sName, 1);
            if (found==std::string::npos){
                continue;
            }
            *option = optSP.get();
            if(found+sName.size()<arg.size()){
                *value = arg.substr(found+sName.size());
            }
            return true;
        }
    }
    (void)option;
    this->errorMsg= "Option " + arg + " not recognized";
    return false;
}