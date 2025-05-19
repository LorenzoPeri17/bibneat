#include "cliOptions.hpp"

CliFlag::CliFlag(
    const std::string optionName,
    const std::string helpText,
    const bool defaultValue,
    const std::string longName,
    const std::string shortName
)
    : cliType(CLI_FLAG),
      optionName(optionName),
      helpText(helpText),
      longName(longName),
      shortName(shortName),
      defaultValue(defaultValue),
      value(defaultValue)

{
    this->hasLongName = !(this->longName.empty());
    this->hasShortName = !(this->shortName.empty());
    ASSERTMSG((this->hasLongName||this->hasShortName),"Cli Flag " + this->optionName + " has no way to be invoked");
}

void CliFlag::setValue(const std::string& s){
    (void)s;
    this->value=!(this->defaultValue);
}

bool CliFlag::getValue() const{
    return this->value;
}