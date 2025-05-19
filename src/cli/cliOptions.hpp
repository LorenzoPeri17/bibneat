#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <functional>
#include <type_traits>

#include <iostream>

#include  "../utils/debug.hpp"
#include  "../utils/attributes.hpp"

enum CliType{
    CLI_OPT,
    CLI_FLAG,
    CLI_VEC,
};

INLINE static std::string return_str(const std::string& s){return s;}
INLINE static long to_long(const std::string& s){return std::stol(s);}
INLINE static int to_int(const std::string& s){return std::stoi(s);}

class BaseCliOption {
public:
    virtual ~BaseCliOption() = default;
    virtual const std::string& getOptionName() const = 0;
    virtual const std::string& getHelpText() const = 0;
    virtual const std::string& getLongName() const = 0;
    virtual const std::string& getShortName() const = 0;

    virtual void setValue(const std::string& s) = 0;
    virtual CliType getCliType() const = 0;
};

template <typename T>
class CliOption : public BaseCliOption {
public:

    explicit CliOption(
            const bool isRequired,
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::function<T(const std::string)> converter,
            const std::optional<T> defaultValue
        ) : 
        cliType(CLI_OPT),
        isRequired(isRequired),
        optionName(optionName),
        helpText(helpText),
        longName(longName),
        shortName(shortName),
        converter(converter),
        defaultValue(defaultValue)
        {

        this->value = std::nullopt;
        this->hasDefault = this->defaultValue.has_value();

        this->hasLongName = !(this->longName.empty());
        this->hasShortName = !(this->shortName.empty());
        ASSERTMSG((this->hasLongName||this->hasShortName),"Cli Option " + this->optionName + " has no way to be invoked");
    }

    explicit CliOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName,
        std::function<T(const std::string)> converter
    ) : CliOption(isRequired, optionName, helpText, longName, shortName, converter, std::nullopt){}

    explicit CliOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName,
        const std::optional<T> defaultValue
    ) requires (std::is_same_v<T, int>) :
    CliOption(isRequired, optionName, helpText, longName, shortName, to_int, defaultValue){}

    explicit CliOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName,
        const std::optional<T> defaultValue
    ) requires (std::is_same_v<T, long>) :
    CliOption(isRequired, optionName, helpText, longName, shortName, to_long, defaultValue){}

    explicit CliOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName,
        const std::optional<T> defaultValue
    ) requires (std::is_same_v<T, std::string>) :
    CliOption(isRequired, optionName, helpText, longName, shortName, return_str, defaultValue){}

    virtual ~CliOption(){};
    const CliType cliType;

    const std::string& getOptionName() const override { return optionName; }
    const std::string& getHelpText() const override { return helpText; }
    const std::string& getLongName() const override { return longName; }
    const std::string& getShortName() const override { return shortName; }
    CliType getCliType() const override { return cliType; }

    // void setValue(const std::string& s)override;
    void setValue(const std::string& s)override {this->value = std::optional(this->converter(s));}
    // void setValue(const std::string& s)requires (std::is_same_v<T, int>) override {this->value = std::stoi(s);}
    // void setValue(const std::string& s)requires (std::is_same_v<T, long>) override {this->value = std::stol(s);}
    // void setValue(const std::string& s)requires (std::is_same_v<T, std::string>) override {this->value = s;}
    
    std::pair<std::shared_ptr<T>,bool> getValue(){
        this->setRealValue();
        if(this->hasValue){
            return std::make_pair(std::make_shared<T>(this->realValue), true);
        } else {
            return std::make_pair(nullptr, false);
        }
    }
    
    bool hasDefault;
    bool isRequired;
    bool hasLongName;
    bool hasShortName;
    bool hasValue;

    const std::string optionName;
    const std::string helpText;

    const std::string longName;
    const std::string shortName;

private:
    std::function<T(const std::string)> converter;

    const std::optional<T> defaultValue;
    std::optional<T> value;
    T realValue;

    void setRealValue(){
        if (this->value.has_value()){
            this->realValue = this->value.value();
            this->hasValue = true;
        } else if (this->defaultValue.has_value()){
            this->realValue = this->defaultValue.value();
            this->hasValue = true;
        } else{
            this->hasValue = false;
        }
    }
};


class CliFlag : public BaseCliOption {
public:
    explicit CliFlag(
        const std::string optionName,
        const std::string helpText,
        const bool defaultValue,
        const std::string longName,
        const std::string shortName
    );
    virtual ~CliFlag(){};
    const CliType cliType;

    const std::string& getOptionName() const override { return optionName; }
    const std::string& getHelpText() const override { return helpText; }
    const std::string& getLongName() const override { return longName; }
    const std::string& getShortName() const override { return shortName; }
    CliType getCliType() const override { return cliType; }

    const bool hasDefault=true;
    const bool hasValue=true;
    const bool isRequired = false;
    bool hasLongName;
    bool hasShortName;

    void setValue(const std::string& s) override;
    bool getValue() const;

private:
    const std::string optionName;
    const std::string helpText;
    const std::string longName;
    const std::string shortName;
    bool defaultValue;
    bool value;
};

template <typename T>
class CliVectorOption : public BaseCliOption {
public:
    explicit CliVectorOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName,
        std::function<T(const std::string& )> converter
    ) : cliType(CLI_VEC),
        isRequired(isRequired),
        optionName(optionName),
        helpText(helpText),
        longName(longName),
        shortName(shortName),
        converter(converter) 
    {

        this->value = std::make_shared<std::vector<T>>();
        this->hasValue = false;

        this->hasLongName = !(this->longName.empty());
        this->hasShortName = !(this->shortName.empty());
        ASSERTMSG((this->hasLongName||this->hasShortName),"Cli Option " + this->optionName + " has no way to be invoked");
    }

    explicit CliVectorOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName
    ) requires (std::is_same_v<T, int>) :
    CliVectorOption(isRequired, optionName, helpText, longName, shortName, to_int){}

    explicit CliVectorOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName
    ) requires (std::is_same_v<T, long>) :
    CliVectorOption(isRequired, optionName, helpText, longName, shortName, to_long){}

    explicit CliVectorOption(
        const bool isRequired,
        const std::string optionName,
        const std::string helpText,
        const std::string longName,
        const std::string shortName
    ) requires (std::is_same_v<T, std::string>) :
    CliVectorOption(isRequired, optionName, helpText, longName, shortName, return_str){}

    virtual ~CliVectorOption(){};
    const CliType cliType;

    const std::string& getOptionName() const override { return optionName; }
    const std::string& getHelpText() const override { return helpText; }
    const std::string& getLongName() const override { return longName; }
    const std::string& getShortName() const override { return shortName; }
    CliType getCliType() const override { return cliType; }

    const bool hasDefault=false;
    bool isRequired;
    bool hasValue;
    bool hasLongName;
    bool hasShortName;

    void setValue(const std::string& s)override {this->value->emplace_back(this->converter(s));}
    // void setValue(const std::string& s)requires (std::is_same_v<T, int>) override {this->value.push_back(std::stoi(s));}
    // void setValue(const std::string& s)requires (std::is_same_v<T, long>) override {this->value.push_back(std::stol(s));}
    // void setValue(const std::string& s)requires (std::is_same_v<T, std::string>) override {this->value.push_back(s);}
    
    std::shared_ptr<std::vector<T>> getValue() {return this->value;}

private:
    const std::string optionName;
    const std::string helpText;
    const std::string longName;
    const std::string shortName;
    std::function<T(const std::string)> converter;
    std::shared_ptr<std::vector<T>> value;
};