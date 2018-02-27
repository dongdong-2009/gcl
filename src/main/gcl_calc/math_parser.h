#ifndef __MATHPARSER_H__
#define __MATHPARSER_H__


#define  ParserStackSize    1000
#define  MaxFuncNameLen     10
#define  ExpLimit           11356
#define  SQRLIMIT           1E246
#define  MaxExpLen          4
#define  TotalErrors        7
#define  ErrParserStack     1
#define  ErrBadRange        2
#define  ErrExpression      3
#define  ErrOperator        4
#define  ErrOpenParen       5
#define  ErrOpCloseParen    6
#define  ErrInvalidNum      7

#define TT_COUNT        20
#define TS_COUNT        20

#define TT_INDEX        0
#define TT_NUM          TT_INDEX+1
#define TT_OR           TT_INDEX+2
#define TT_EXPO         TT_INDEX+3
#define TT_AND          TT_INDEX+4
#define TT_PLUS         TT_INDEX+5
#define TT_MINUS        TT_INDEX+6
#define TT_TIMES        TT_INDEX+7
#define TT_DIVIDE       TT_INDEX+8
#define TT_MOD          TT_INDEX+9
#define TT_OPAREN       TT_INDEX+10
#define TT_CPAREN       TT_INDEX+11
#define TT_FUNC         TT_INDEX+12
#define TT_EOL          TT_INDEX+13
#define TT_BAD          TT_INDEX+14
#define TT_ERR          TT_INDEX+15

#define TS_INDEX        (TT_INDEX+TT_COUNT)
#define TS_NULL         TS_INDEX+0
#define TS_NUM          TS_INDEX+1
#define TS_OR           TS_INDEX+2
#define TS_EXPO         TS_INDEX+3
#define TS_AND          TS_INDEX+4
#define TS_PLUS         TS_INDEX+5
#define TS_MINUS        TS_INDEX+6
#define TS_TIMES        TS_INDEX+7
#define TS_DIVIDE       TS_INDEX+8
#define TS_MOD          TS_INDEX+9
#define TS_OPAREN       TS_INDEX+10
#define TS_CPAREN       TS_INDEX+11
#define TS_FUNC         TS_INDEX+12

#define TO_INDEX        (TS_INDEX+TS_COUNT)
#define TO_NUM          TO_INDEX+1
#define TO_OR           TO_INDEX+2
#define TO_EXPO         TO_INDEX+3
#define TO_AND          TO_INDEX+4
#define TO_PLUS         TO_INDEX+5
#define TO_MINUS        TO_INDEX+6
#define TO_TIMES        TO_INDEX+7
#define TO_DIVIDE       TO_INDEX+8
#define TO_MOD          TO_INDEX+9
#define TO_PAREN        TO_INDEX+10
//#define TO_OPAREN     TO_INDEX+10
//#define TO_CPAREN     TO_INDEX+11
#define TO_FUNC         TO_INDEX+12

#define ATTCH_STATE     100


#include <string>

using namespace std;


typedef double TValue;

typedef struct
{
    short State;
    TValue Value;
    string FuncName;
}TokenRec;


class  CMathParser
{
public:
    CMathParser(const string& szExpression);
	virtual ~CMathParser();
	
	
protected:
    string  Expression;
	TokenRec CurrToken;
	TokenRec Stack[ParserStackSize];
    int StackTop;   // : 0..ParserStackSize;
    short TokenType;

    unsigned short Position;
    bool ParseError;
    TValue ParseValue;
	
    short next_token();
    void push(TokenRec Token);
    void pop(TokenRec& Token);
    void shift(short State);
    void reduce(short Reduction);

    bool is_func(const string& szFuncName);

    bool is_var(TValue& Value);
    bool get_var(const string& szName, TValue& Value);

    int round(double value);
    bool low_priority(short state);
	
public://    { Public declarations }
    TValue parse();
    bool error(){ return ParseError; }
};

#endif//#ifndef __MATHPARSER_H__
