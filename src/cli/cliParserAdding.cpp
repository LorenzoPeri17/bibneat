#include "cliParser.hpp"

void CliParser::index(std::shared_ptr<BaseCliOption> option){
    ASSERTMSG((!(KEY_EXISTS(this->optionNames, option->getOptionName()))), "Option "+option->getOptionName()+" willbe overwritten");
    this->optionNames.insert_or_assign(option->getOptionName(),option);
    this->longNames.insert_or_assign(option->getLongName(),option);
    this->shortNames.insert_or_assign(option->getShortName(),option);
}

/* flag */
void CliParser::addFlag(
    const std::string optionName,
    const std::string helpText,
    const bool defaultValue,
    const std::string longName,
    const std::string shortName){
    std::shared_ptr<BaseCliOption> option = std::make_shared<CliFlag>(optionName, helpText, defaultValue, longName, shortName);
    this->index(option);
    this->options.emplace_back(std::move(option));
}
