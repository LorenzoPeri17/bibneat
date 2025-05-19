#include "cliParser.hpp"

void CliParser::makeCategory(const std::string cat){
    this->categories.try_emplace(cat, std::vector<std::shared_ptr<BaseCliOption>>());
    this->categoryOrder.push_back(cat);
}

void CliParser::addToCategory(const std::string cat, const std::string optionName){
    if(!(KEY_EXISTS(this->categories, cat))){
        this->makeCategory(cat);
    }
    this->categories.at(cat).push_back(this->optionNames.at(optionName));
}

void CliParser::printUsage(std::ostream* stream) const{
    *(stream) << "USAGE" << std::endl;
    *(stream) << "  " << this->binName << this->usageString << std::endl << std::endl;
    *(stream) << "  " << this->binName << " --help for information on the available options"<< std::endl;
}

void CliParser::printError() const{
    std::cerr << this->errorMsg << std::endl << std::endl;
    this->printUsage(&std::cerr);
}
void CliParser::printOption(std::ostream* stream, std::shared_ptr<BaseCliOption> option) const{

    *(stream) << "  ";

    if (!option->getShortName().empty()){
        *(stream) << "-" + option->getShortName();
    }

    if (!option->getLongName().empty()){
        if(!option->getShortName().empty()){*(stream)<<",";}
        *(stream) << "--" + option->getLongName();
    } else{
        *(stream) << "\t";
    }

    *(stream) << "\t" << option->getHelpText() << std::endl;
}

void CliParser::printHelp(std::ostream* stream) const{

    std::vector<std::shared_ptr<BaseCliOption>> misc;
    misc.clear();

    *(stream) << "USAGE" << std::endl;
    *(stream) << "  " << this->binName << this->usageString << std::endl << std::endl;

    *(stream) << "OPTIONS" << std::endl;
    for(auto& cat : this->categoryOrder){
        *(stream) << cat << std::endl;
        auto options = this->categories.at(cat);
        for(auto& option: options){
            this->printOption(stream, option);
        }
    }

    this->getMiscCategory(&misc);
    if(!(misc.empty())){
        *(stream) << "MISC" << std::endl;
    }
    for(auto& option: misc){
        this->printOption(stream, option);
    }
    *(stream) << std::endl;
}


void CliParser::getMiscCategory(std::vector<std::shared_ptr<BaseCliOption>>* misc) const{
    for(auto& [optName, option]:this->optionNames){
        bool foundCat = false;
        for(auto& [cat, catOptions] : this->categories){
            bool found = false;
            for (auto& catOption: catOptions){
                if (catOption->getOptionName() == option->getOptionName()) {
                    found = true;
                    break;
                }
            }
            if(found){
                foundCat=true;
                break;
            }
        }
        if(!foundCat){
            misc->push_back(option);
        }
    }
}