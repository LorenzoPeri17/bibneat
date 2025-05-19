#include "cliParser.hpp"

std::pair<bool, bool> CliParser::getFlag(const std::string optionName) const{
    if(!(KEY_EXISTS(this->optionNames, optionName))){return std::make_pair(false, false);}
    BaseCliOption* baseOption = this->optionNames.at(optionName).get();
    if(baseOption->getCliType()!=CLI_FLAG){return std::make_pair(false, false);}
    CliFlag* option = dynamic_cast<CliFlag*>(baseOption);
    return std::make_pair(option->getValue(), true);
}
