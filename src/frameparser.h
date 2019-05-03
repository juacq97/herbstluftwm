#pragma once

#include <X11/X.h>
#include <memory>
#include <string>

#include "framedata.h"

/*! A 'pure' parser a layout description generated by the 'dump' command.
 *
 * Pure means that it does not affect the actual tiling tree. The purpose is to
 * first parse and validate the input string. The application of this parsing
 * result to the actual frame tree is done by the respective command in the
 * FrameTree class.
 *
 * Hence, the result of this parsing process is a 'raw tree', meaning a tree of
 * RawFrameNode objects, i.e. actually of RawFrameLeaf, RawFrameSplit. All the
 * members are already validated, so e.g. invalid window ids are already
 * filtered.
 *
 */

class RawFrameLeaf;
class RawFrameSplit;

class RawFrameNode : public std::enable_shared_from_this<RawFrameNode> {
protected:
    RawFrameNode() {};
    virtual ~RawFrameNode() {};
public:
    virtual std::shared_ptr<RawFrameLeaf> isLeaf() { return {}; };
    virtual std::shared_ptr<RawFrameSplit> isSplit() { return {}; };
};

class RawFrameLeaf : public RawFrameNode,
                     public FrameDataLeaf {
public:
    friend class FrameParser;
    friend class FrameTree;
    std::shared_ptr<RawFrameLeaf> isLeaf() override;
};

class RawFrameSplit : public RawFrameNode,
                      public FrameDataSplit<RawFrameNode> {
public:
    friend class FrameParser;
    friend class FrameTree;
    std::shared_ptr<RawFrameSplit> isSplit() override;
};

/*! the FrameParser is actually only a interface to access the parsing result
 * and possible error messages, because the parser methods are private member
 * functions. The parsing starts already in the constructor.
 */
class FrameParser {
public:
    //! a list of tokens and their positions
    using Token = std::pair<size_t,std::string>;
    using Tokens = std::vector<Token>;

    FrameParser(std::string buf);
    //! the parsing result
    std::shared_ptr<RawFrameNode> root_;
    //! a possible error message and error token
    std::shared_ptr<std::pair<Token,std::string>> error_;
    std::vector<std::pair<Token,Window>> unknownWindowIDs_;
private:
    void parse(std::string buf);
    //! Split a string into tokens. The tokens are defined in the sense that it
    //is always allowed to insert spaces between tokens. Hence in (a (b c)) the
    //two closing brackets are separate tokens because (a (b c) ) is
    //equivalent; however the Leaf-args string "vertical:0" is a single token
    //because "vertical: 0" is not of valid syntax.
    Tokens tokenize(std::string buf);

    //! the next token to process by buildTree()
    Tokens::const_iterator nextToken;
    Tokens::const_iterator endToken;
    //! build a RawFrameNode-Tree from the token list
    std::shared_ptr<RawFrameNode> buildTree();
    void expectTokens(std::vector<std::string> token);

    Token eofToken;

    //! tells whether the given char is contained in the string
    static bool contained_in(char c, std::string s);
};