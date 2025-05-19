#pragma once

#include <unordered_map>
#include <sstream>
#include <cctype>
#include <iostream>
#include <ostream>

#include  "cliOptions.hpp"

#include  "../utils/attributes.hpp"
#include  "../utils/umap.hpp"

#define N_OPTS_RESERVE 20

class CliParser{
public:

    explicit CliParser(
        const std::string binName
    ) : binName(binName){
        options.reserve(N_OPTS_RESERVE);
        shortNames.reserve(N_OPTS_RESERVE);
        longNames.reserve(N_OPTS_RESERVE);
        optionNames.reserve(N_OPTS_RESERVE);
    }

    virtual ~CliParser(){}

    /* Generic */
    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::function<T(const std::string)> converter,
            const std::optional<T> defaultValue
        ){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption>(isRequired, optionName, helpText, longName, shortName, converter, defaultValue);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::function<T(const std::string)> converter
        ){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption>(isRequired, optionName, helpText, longName, shortName, converter);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* int */
    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::optional<T> defaultValue
        )requires (std::is_same_v<T, int>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption<int>>(isRequired, optionName, helpText, longName, shortName, defaultValue);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName
        )requires (std::is_same_v<T, int>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption<int>>(isRequired, optionName, helpText, longName, shortName, std::nullopt);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* long */
    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::optional<T> defaultValue
        )requires (std::is_same_v<T, long>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption<long>>(isRequired, optionName, helpText, longName, shortName, defaultValue);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName
        )requires (std::is_same_v<T, long>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption<long>>(isRequired, optionName, helpText, longName, shortName, std::nullopt);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* std::string */
    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::optional<T> defaultValue
        )requires (std::is_same_v<T, std::string>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption<std::string>>(isRequired, optionName, helpText, longName, shortName, defaultValue);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    template<typename T>
    void addOption(bool isRequired, 
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName
        )requires (std::is_same_v<T, std::string>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliOption<std::string>>(isRequired, optionName, helpText, longName, shortName, std::nullopt);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* flag */
    void addFlag(
        const std::string optionName,
        const std::string helpText,
        const bool defaultValue,
        const std::string longName,
        const std::string shortName);

    /* Generic */
    template<typename T>
    void addVectorOption(const bool isRequired,
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName,
            const std::function<T(const std::string)> converter
        ){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliVectorOption>(isRequired, optionName, helpText, longName, shortName, converter);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* int */
    template<typename T>
    void addVectorOption(const bool isRequired,
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName
        ) requires (std::is_same_v<T, int>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliVectorOption<int>>(isRequired, optionName, helpText, longName, shortName);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* long */
    template<typename T>
    void addVectorOption(const bool isRequired,
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName
        ) requires (std::is_same_v<T, long>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliVectorOption<long>>(isRequired, optionName, helpText, longName, shortName);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }

    /* std::string */
    template<typename T>
    void addVectorOption(const bool isRequired,
            const std::string optionName,
            const std::string helpText,
            const std::string longName,
            const std::string shortName
        ) requires (std::is_same_v<T, std::string>){
        std::shared_ptr<BaseCliOption> option = std::make_shared<CliVectorOption<std::string>>(isRequired, optionName, helpText, longName, shortName);
        this->index(option);
        this->options.emplace_back(std::move(option));
    }
    
    /* help */
    void printHelp(std::ostream* stream = &std::cout) const;
    void printUsage(std::ostream* stream = &std::cerr) const;
    void makeCategory(const std::string cat);
    void addToCategory(const std::string cat, const std::string optionName);
    
    /* parsing */
    void printError() const;
    bool parseArgs(int argc, char**argv);

    /* getting values */
    template<typename T>
    std::pair<std::shared_ptr<T>,bool> getValue(const std::string optionName) const{
        if(!(KEY_EXISTS(this->optionNames, optionName))){return std::make_pair(nullptr, false);}
        BaseCliOption* baseOption = this->optionNames.at(optionName).get();
        if(baseOption->getCliType()!=CLI_OPT){return std::make_pair(nullptr, false);}
        CliOption<T>* option = dynamic_cast<CliOption<T>*>(baseOption);
        return option->getValue();
    }

    template<typename T>
    std::pair<std::shared_ptr<std::vector<T>>,bool> getVector(const std::string optionName) const{
        if(!(KEY_EXISTS(this->optionNames, optionName))){return std::make_pair(nullptr, false);}
        BaseCliOption* baseOption = this->optionNames.at(optionName).get();
        if(baseOption->getCliType()!=CLI_VEC){return std::make_pair(nullptr, false);}
        CliVectorOption<T>* option = dynamic_cast<CliVectorOption<T>*>(baseOption);
        return std::make_pair(option->getValue(), true); 
    }

    std::pair<bool, bool> getFlag(const std::string optionName) const;

    std::string errorMsg;
    std::string usageString;
    const std::string binName;
    std::vector<std::string> extraArgs;

    std::vector<std::shared_ptr<BaseCliOption>> options;
    std::unordered_map<std::string,std::shared_ptr<BaseCliOption>> shortNames;
    std::unordered_map<std::string,std::shared_ptr<BaseCliOption>> longNames;
    std::unordered_map<std::string,std::shared_ptr<BaseCliOption>> optionNames;
    std::unordered_map<std::string,std::vector<std::shared_ptr<BaseCliOption>>> categories;
    std::vector<std::string> categoryOrder;
private:
    void index(std::shared_ptr<BaseCliOption> option);
    bool handleArg(const std::string& arg, BaseCliOption** option, std::string* value);
    void printOption(std::ostream* stream, std::shared_ptr<BaseCliOption> option) const;
    void getMiscCategory(std::vector<std::shared_ptr<BaseCliOption>>* misc) const;
};