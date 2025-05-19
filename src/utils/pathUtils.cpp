#include "pathUtils.hpp"

void replaceTilde(std::string& path){
    const std::string home = std::getenv("HOME");
    replaceOnce(path, "~",  home);
}