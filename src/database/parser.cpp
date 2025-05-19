#include "parser.hpp"

HOT inline void Parser::reset() noexcept{
    this->isInEntry = false;
    this->isInAttribute = false;
    this->attrOpenWithQuotes = false;
    this->isInMath= false;
    this->isEscaped = false;
    this->immediate = false;
    this->needKey = false;
    this->needOneWordAttr = false;
    this->curlyDepth = 0;
}

INLINE static bool isWhiteSpace(const char c) noexcept {
        return isspace(c) || c == '\n' || c == '\r';
    }

HOT void Parser::skipWhiteSpace(bool get = true) noexcept{
    this->LastChar = this->stream->get();
    while (isWhiteSpace(this->LastChar)){
        this->LastChar = this->stream->get();
    }
    if(!get){
        this->stream->unget();
    }
}

INLINE static bool isJunk(const char c) noexcept {
        return !(c=='@' || c==EOF || c==0) ;
    }

HOT void Parser::skipJunk(bool get) noexcept{
    this->LastChar = this->stream->get();
    while (isJunk(this->LastChar)){
        this->LastChar = this->stream->get();
    }
    if(!get){
        this->stream->unget();
    }
}

HOT Token Parser::getNext() noexcept {
    int tmpChar;

    if (this->isInAttribute){
        this->LastChar = this->stream->get();
    } else {
        this->skipWhiteSpace();
    }

    // skip junk
    if(!(this->isInEntry)){
        if(isJunk(this->LastChar)){
            this->skipJunk(true);
        }
    }

    if (this->LastChar == EOF)
        return TOK_EOF;
    if (this->LastChar == 0) // terminating null byte
        return TOK_EOF;
    if (this->immediate){
        if (this->LastChar == ',')[[unlikely]]{
            this->currentStr = "Bib entry with empty key or attribute";
            return TOK_ERROR;
        }
        this->currentStr = this->LastChar;
        this->LastChar = this->stream->get();
        while ((this->LastChar != ',') && (this->LastChar != '}')){
            this->currentStr += this->LastChar;
            this->LastChar = this->stream->get();
        }
        if(this->LastChar == '}'){this->stream->unget();} // not et the end of entry...
        this->immediate = false;
        if (this->needKey){
            this->needKey = false;
            return TOK_KEY;
        } else if (this->needOneWordAttr){
            this->isInAttribute=false;
            this->needOneWordAttr = false;
            return TOK_END_ATTR;
        }
        return TOK_STR;
    }

    if (this->isEscaped){
        this->currentStr += "\\";
        this->currentStr += this->LastChar;
        this->isEscaped = false;
        return TOK_KEEP_PARSING;
    }

    if (this->LastChar == '\\'){
        this->isEscaped = true;
        return TOK_KEEP_PARSING;
    }

    if(this->LastChar == '$'){
        this->currentStr += '$';
        if (this->stream->peek() == '$'){
            (void)this->stream->get(); // consume the second $
            this->currentStr += '$';
        }
        this->isInMath = !(this->isInMath);
        return TOK_KEEP_PARSING;
    }

    if(this->LastChar == '@'){
        if(!(this->isInEntry)){
            this->isInEntry = true;
            this->immediate = true; // we need a key
            this->needKey = true;
            this->currentStr = this->stream->get();
            while ((this->LastChar = this->stream->get()) != '{'){
                this->currentStr += this->LastChar;
            }
            return TOK_NEW_ENTRY;
        } else { // is this even allowed?
            this->currentStr += "@";
            return TOK_KEEP_PARSING;
        }
    }

    if(this->LastChar == '}'){
        if (this->curlyDepth > 0){
            this->curlyDepth--;
            this->currentStr += "}";
            return TOK_KEEP_PARSING;
        } else if (this->isInAttribute && !this->attrOpenWithQuotes) {
            this->isInAttribute = false;
            this->skipWhiteSpace(false);
            if ((this->stream->peek()) == ',') [[likely]] { // if there is a comma, might as well consume it
                (void)this->stream->get(); // casting to void tells the compiler to shut up
            }
            return TOK_END_ATTR;
        } else if (this->isInEntry) {
            this->isInEntry = false;
            return TOK_END_ENTRY;
        } else {
            this->currentStr = "Closing a curly that was never opened...";
            return TOK_ERROR;
        }
    } 

    if(this->LastChar == '"'){
        if (this->isInAttribute && this->attrOpenWithQuotes){
            this->isInAttribute = false;
            this->skipWhiteSpace(false);
            tmpChar = this->stream->peek();
            if (tmpChar == ',') [[likely]] { // if there is a comma, might as well consume it
                (void)this->stream->get(); // casting to void tells the compiler to shut up
            } else if (tmpChar == '#'){ // for some reason BibTex allows string concatenation...
                (void)this->stream->get(); // eat '#'
                this->skipWhiteSpace(false);
                if (this->stream->peek() == '"') [[likely]] {
                    (void)this->stream->get(); // eat '"'
                    this->isInAttribute = true;
                    return TOK_KEEP_PARSING;
                } else{
                    this->currentStr = "Concatenation is allowed only between \"-opened strings";
                    return TOK_ERROR;
                }
            }
            return TOK_END_ATTR;
        }
    }

    if (this->LastChar == '{'){
        // In theory we should only get here in an entry
        if (!this->isInEntry){
            this->currentStr = "Illegal opening of curly bracket";
            return TOK_ERROR;
        }
        this->curlyDepth++;
        this->currentStr += "{";
        return TOK_KEEP_PARSING;
    }

    if(!this->isInAttribute){
        this->currentStr = (char)this->LastChar;
        while ((this->LastChar = this->stream->get()) != '='){
            this->currentStr += this->LastChar;
        }
        this->skipWhiteSpace(false);
        this->isInAttribute = true;
        tmpChar=this->stream->peek();
        if (tmpChar != '{' && tmpChar != '"') {
            this->immediate = true; // one word attributes do not need {}
            this->needOneWordAttr = true;
            return TOK_ONEWORD_ATTR;
        } else { // it was a '{' or '"'
            this->LastChar = this->stream->get(); // consume the { or "
            this->attrOpenWithQuotes = (this->LastChar=='"');
            return TOK_NEW_ATTR;
        }
    }

    // parse the body of an attribute
    this->currentStr += this->LastChar;
    while (!this->specialChars.contains(this->LastChar = this->stream->get())){
        this->currentStr += this->LastChar;
    }
    this->stream->unget(); // we deal with it later
    return TOK_KEEP_PARSING;
}

HOT bool Parser::handleNewEntry() {

    toLower(&(this->currentStr));
    const std::string bibTypeName = this->currentStr;
    if(this->specialBibTypes.contains(this->currentStr)){
        this->handleSpecialEntry(bibTypeName);
        this->reset();
        return true;
    }
    this->nextToken(); // eats TOK_NEW_ENTRY
    ASSERTMSG((this->currentToken==TOK_KEY), "We should get a key after a new entry");
    const std::string bibKey = this->currentStr;

    std::unique_ptr<BibEntry> entry = std::make_unique<BibEntry>(bibTypeName, bibKey);

    std::string currentAttrName;
    currentAttrName.reserve(256);
    while((this->currentToken = this->getNext()) != TOK_END_ENTRY){
        switch (this->currentToken){
            case TOK_NEW_ATTR:
                FALLTHROUGH;
            case TOK_ONEWORD_ATTR:
                currentAttrName = this->currentStr;
                this->currentStr.clear();
                break;
            case TOK_KEEP_PARSING:
                FALLTHROUGH;
            case TOK_STR:
                break;
            case TOK_END_ATTR:
                entry->addAttribute(currentAttrName, this->currentStr);
                this->currentStr.clear();
                break;
            case TOK_ERROR:
                this->currentStr = "Error in entry " + bibKey + "\n" + this->currentStr;
                return false;
                break;
            default:
                ASSERTMSG((false), "We should never get here...");
                this->currentStr = "Malformed entry " + bibKey;
                return false;
                break;
        }
    }

    // new we hand the entry to the BibDB
    this->bibDB->addEntry(std::move(entry));
    if(this->verbose){
        std::cout << "Found entry " << bibKey <<std::endl;
    }
    this->reset();
    return true;
}

HOT void Parser::handleSpecialEntry(const std::string bibTypeName){
    this->curlyDepth = 0;
    this->currentStr.clear();
    this->LastChar=this->stream->get();
    while(!(this->LastChar=='}' && this->curlyDepth==0)){
        this->currentStr+=this->LastChar;
        if(this->LastChar=='{'){this->curlyDepth++;}
        if(this->LastChar=='}' && this->curlyDepth>0){this->curlyDepth--;}
        this->LastChar=this->stream->get(); // we do want to eat the last "}"
    }
    std::unique_ptr<SpecialEntry> entry = std::make_unique<SpecialEntry>(bibTypeName, this->currentStr);
    this->bibDB->addSpecialEntry(std::move(entry));
    
    if(this->verbose){
        std::cout << "Found special entry of type " << bibTypeName <<std::endl;
    }
}

HOT inline void Parser::nextToken() noexcept {
    this->currentToken = this->getNext();
}

bool Parser::parseLoop(){
    bool entrySuccess;
    while (stream->good()) {
        this->nextToken();
        switch (this->currentToken)
        {
            case TOK_EOF:
                break;
            case TOK_NEW_ENTRY:
                entrySuccess = this->handleNewEntry();
                if(!entrySuccess)[[unlikely]]{return false;}
                break;
            case TOK_ERROR:
                return false;
                break;
            default:
                break;
        }
    }
    return true;
}

bool Parser::parse(std::string fname){

    this->fname = fname;
    replaceTilde(fname);
    this->file = std::ifstream(fname, std::ios::in);
    this->stream=dynamic_cast<std::istream*>(&file);

    this->reset();

    if (!file.is_open()) {
        this->currentStr = "Unable to open file " + this->fname;
        return false;
    }

    bool success = this->parseLoop();

    file.close();
    return success;
}

bool Parser::parseString(const std::string bibString){

    this->fname = "String";
    std::istringstream s = std::istringstream(bibString, std::ios::in);
    this->stream=dynamic_cast<std::istream*>(&s);

    this->reset();

    bool success = this->parseLoop();

    return success;
}

