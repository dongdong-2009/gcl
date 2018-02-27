#include "MathParser.h"
#include <math.h>
#include <stdio.h>
#include<stdlib.h>
#include<iostream>


CMathParser::CMathParser(const string& szExpression)
{
    Expression = szExpression;
}

CMathParser::~CMathParser()
{
}

TValue CMathParser::Parse()
{
    bool Accepted;
    short State;
    TokenRec FirstToken;

    Position = 0;
    StackTop = 0;
    ParseError = false;
    Accepted = false;
	FirstToken.State = 0;
	FirstToken.Value = 0;

//  cout << "begin Parse: Expression = " << Expression << endl;

	Push(FirstToken);
	TokenType = NextToken();

	do
	{
        if ((TokenType == TT_BAD) || (TokenType == TT_ERR))
        {
            ParseError = true;
            Accepted = true;
            break;
        }

        State = Stack[StackTop-1].State;

        switch(State)
		{
		case 0:   
            if ((TokenType == TT_NUM) ||
                (TokenType == TT_FUNC)||
                (TokenType == TT_OPAREN))
            {
                Shift(TokenType);
            }
            else
            {
                ParseError = true;
                Accepted = true;
            }
            break;

        case TT_NUM://Token state is number
            if ((TokenType == TT_NUM) ||
                (TokenType == TT_FUNC) ||
                (TokenType == TT_OPAREN))
            {
                ParseError = true;
                Accepted = true;
            }
            else
            {
                Reduce(ATTCH_STATE);
            }
            break;

        case TS_NULL:
            if (TokenType == TT_EOL)
            {
                Accepted = true;
            }
            else if ((TokenType == TT_FUNC) || (TokenType == TT_CPAREN))
            {
                ParseError = true;
                Accepted = true;
            }
            else
            {
                Shift(TokenType);
            }
            break;

        case TS_OR:
        case TS_EXPO:
        case TS_AND:
        case TS_PLUS:
        case TS_MINUS:
        case TS_TIMES:
        case TS_DIVIDE:
        case TS_MOD:
            if (LowPriority(State))
            {
                Reduce(State+TS_COUNT);
            }
            else
            {
                Shift(TokenType);
            }
            break;

        case TS_OPAREN:
            if ((TokenType == TT_NUM) || (TokenType == TT_FUNC))
            {
                ParseError = true;
                Accepted = true;
            }
            else
            {
                Shift(TokenType);
            }
            break;

        case TS_FUNC:
            Reduce(TO_FUNC);
            break;

        default://Token state is operator
            if ((TokenType == TT_OPAREN))
            {
                Shift(TT_OPAREN);
            }
            else if ((TokenType == TT_NUM) && (State != TT_FUNC))
            {
                Shift(TT_NUM);
            }
            else if ((TokenType == TT_FUNC) && (State != TT_FUNC))
            {
                Shift(TT_FUNC);
            }
            else if ((State == TT_CPAREN))
            {
                Reduce(TO_PAREN);
            }
            else
            {
                ParseError = true;
                Accepted = true;
            }
            break;
        }

    }while((!Accepted) && (!ParseError));

    return ParseError ? 0 : Stack[StackTop-1].Value;
}

short CMathParser::NextToken()
{
    string NumString;
    WORD TLen, NumLen;
    char Ch;
    bool Decimal;
    short ResultNextToken;

    while ((Position < Expression.length()) && (Expression[Position] == ' '))
    {
        Position++;
    }

    if (Position >= Expression.length())
    {
        ResultNextToken = TT_EOL;
        return ResultNextToken;
    }

    Ch = Expression[Position];
    if (Ch == '!')
    {
        ResultNextToken = TT_ERR;
        return ResultNextToken ;
    }

    if((Ch >= '0' && Ch <= '9') || Ch == '.')
    {
        NumString = "";
        TLen = Position;
        Decimal = false;

        while ((TLen < Expression.length()) &&
               ((Expression[TLen] >= '0' && Expression[TLen] <= '9' ) ||
                ((Expression[TLen] == '.') && (!Decimal))))
        {
            NumString = NumString + Expression[TLen];
            if (Ch == '.')
                Decimal = true;
            TLen++;
        }

        if ((TLen == 2) && (Ch == '.'))
        {
            ResultNextToken = TT_BAD;
            return ResultNextToken;
        }

        if ((TLen < Expression.length()) && ((Expression[TLen]) == 'E'))
        {
            NumString = NumString + 'E';
            TLen++;

            if( Expression[TLen] == '+' || Expression[TLen] == '-')
            {
                NumString[TLen] = Expression[TLen];
                TLen++;
            }

            NumLen = 1;
            while ((TLen < Expression.length()) &&
                   (Expression[TLen] >= '0' && Expression[TLen] <= '9') &&
                   (NumLen <= MaxExpLen))
            {
                NumString = NumString + Expression[TLen];
                NumLen++;
                TLen++;
            }
        }

        if (NumString[0] == '.')
            NumString = '0' + NumString;

        CurrToken.Value = strtod(NumString.c_str(), NULL);
        {
            ResultNextToken = TT_NUM;
            Position = Position + NumString.length();
        }

        return ResultNextToken;
    }
    else if ((Ch>='a' && Ch <= 'z')	|| (Ch>='A' && Ch <= 'Z'))
    {
        if (IsFunc("ABS")||IsFunc("ATAN")||IsFunc("COS")||IsFunc("EXP")||IsFunc("ROUND")||IsFunc("BOOL")||
            IsFunc("SIN")||IsFunc("SQRT")||IsFunc("SQR")||IsFunc("NOT")||IsFunc("TRUNC")||IsFunc("SGN")||IsFunc("LN"))

        {
            ResultNextToken = TT_FUNC;
            return ResultNextToken ;
        }
        else
        {
            ResultNextToken = TT_BAD;
            return ResultNextToken ;
        }
    }
    else
    {
        switch(Ch)
        {

            case  '+' : ResultNextToken = TT_PLUS; break;
            case  '-' : ResultNextToken = TT_MINUS; break;
            case  '*' : ResultNextToken = TT_TIMES; break;
            case  '/' : ResultNextToken = TT_DIVIDE; break;
            case  '^' : ResultNextToken = TT_EXPO; break;
            case  '&' : ResultNextToken = TT_AND; break;
            case  '|' : ResultNextToken = TT_OR; break;
            case  '%' : ResultNextToken = TT_MOD; break;
            case  '(' : ResultNextToken = TT_OPAREN; break;
            case  ')' : ResultNextToken = TT_CPAREN; break;
            default:
                ResultNextToken = TT_BAD;
                return ResultNextToken;
        }

        Position++;

        return ResultNextToken;
    }
}

void CMathParser::Pop(TokenRec &Token)
{
    Token = Stack[--StackTop];

//  cout << "Pop TokenRec: TokenRec.State = " << Token.State;
//  cout << "; TokenRec.Value = " << Token.Value << endl;
}

void CMathParser::Push(TokenRec Token)
{
//  cout << "Push TokenRec: TokenRec.State = " << Token.State;
//  cout << "; TokenRec.Value = " << Token.Value << endl;

    if (StackTop == ParserStackSize-1)
    {
        ParseError = true;
    }
    else
    {
        Stack[StackTop++] = Token;
    }
}

void CMathParser::Shift(short State)
{
    CurrToken.State = State;
    Push(CurrToken);
    TokenType = NextToken();

//  cout << "NextToken: TokenType = " << TokenType << endl;
}

void CMathParser::Reduce(short Reduction)
{
//  cout << "Reduce: Reduction = " << Reduction << endl;

	TokenRec Token1, Token2;

	switch(Reduction)
	{
    case TO_OR:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        CurrToken.Value = (Round(Token1.Value) | Round(Token2.Value));
        break;

    case TO_EXPO: 
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        CurrToken.Value = (Round(Token1.Value) ^ Round(Token2.Value));
        break;

    case TO_AND:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        CurrToken.Value = (Round(Token1.Value) & Round(Token2.Value));
        break;

    case TO_PLUS:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        CurrToken.Value = Token1.Value + Token2.Value;
        break;

    case TO_MINUS:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        CurrToken.Value = Token2.Value - Token1.Value;
        break;

    case TO_TIMES:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        CurrToken.Value = Token1.Value * Token2.Value;
        break;

    case TO_DIVIDE:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        if (fabs(Token1.Value) < 0.000001){
            ParseError = true;
        }
        else {
            CurrToken.Value = Token2.Value / Token1.Value;
        }
        break;

    case TO_MOD:
        Pop(Token1);
        Pop(Token2);
        Pop(Token2);
        if (fabs(Token1.Value) < 0.000001){
            ParseError = true;
        }
        else {
            CurrToken.Value = Round(Token2.Value) % Round(Token1.Value);
        }
        break;


    case TO_PAREN:
        Pop(Token1);
        Pop(CurrToken);
        Pop(Token2);
        break;

    case TO_FUNC:
        Pop(CurrToken);
        Pop(Token1);

        if( Token1.FuncName == "ABS"){
            CurrToken.Value = fabs(CurrToken.Value);
        }
        else if (Token1.FuncName == "ATAN"){
            CurrToken.Value = atan(CurrToken.Value);
        }
        else if (Token1.FuncName == "COS")
        {
            if ((CurrToken.Value < -9E18) || (CurrToken.Value > 9E18)){
                ParseError = true;
            }
            else{
                CurrToken.Value = cos(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "EXP")
        {
            if ((CurrToken.Value < -ExpLimit) || (CurrToken.Value > ExpLimit)){
                ParseError = true;
            }
            else{
                CurrToken.Value = exp(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "LN")
        {
            if (CurrToken.Value <= 0){
                ParseError = true;
            }
            else{
                CurrToken.Value = log(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "ROUND")
        {
            if ((CurrToken.Value < -1E9) || (CurrToken.Value > 1E9)){
                ParseError = true;
            }
            else{
                CurrToken.Value = Round(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "SIN")
        {
            if( (CurrToken.Value < -9E18) || (CurrToken.Value > 9E18)){
                ParseError = true;
            }
            else{
                CurrToken.Value = sin(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "SQRT")
        {
            if( CurrToken.Value < 0 ){
                ParseError = true;
            }
            else{
                CurrToken.Value = sqrt(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "SQR")
        {
            if ((CurrToken.Value < -SQRLIMIT) || (CurrToken.Value > SQRLIMIT)){
                ParseError = true;
            }
            else{
                CurrToken.Value = sqrt(CurrToken.Value);
            }
        }
        else if (Token1.FuncName == "TRUNC")
        {
            if ((CurrToken.Value < -1E9) || (CurrToken.Value > 1E9)){
                ParseError = true;
            }
            else{
                CurrToken.Value = (int)(CurrToken.Value);
            }
        }
        else if(Token1.FuncName == "NOT")
        {
            if (CurrToken.Value < 0.001 && CurrToken.Value > -0.001){
                CurrToken.Value = 1.0;
            }
            else{
                CurrToken.Value = 0.0;
            }
        }
        else if(Token1.FuncName == "BOOL")
        {
            if (CurrToken.Value < 0.001 && CurrToken.Value > -0.001){
                CurrToken.Value = 0.0;
            }
            else{
                CurrToken.Value = 1.0;
            }
        }
        else if(Token1.FuncName == "SGN")
        {
            if (CurrToken.Value > 0.0){
                CurrToken.Value = 1.0;
            }
            else{
                CurrToken.Value = 0;
            }
        }
        break;

    case ATTCH_STATE:
		Pop(CurrToken);
		break;
	}

    short  ResultGotoState = Stack[StackTop-1].State + TT_COUNT;
    CurrToken.State = ResultGotoState;
//  cout << "ResultGotoState = " << ResultGotoState << endl;
	Push(CurrToken);
}

bool CMathParser::IsFunc(const string& szFuncName)
{
    WORD Pos = Position;
    bool IsFunc = false;
    string FuncName = "";

    while ((Pos < Expression.length()) &&
           ((Expression[Pos] >= 'A' && Expression[Pos] <= 'Z') ||
            (Expression[Pos] >= 'a' && Expression[Pos] <= 'z') ||
            (Expression[Pos] >= '0' && Expression[Pos] <= '9') ||
            (Expression[Pos] == '_')))// in ['A'..'Z',  'a'..'z', '0'..'9', '_']) do
    {
        FuncName = FuncName + Expression[Pos];
        Pos++;
    }

    if (FuncName == szFuncName)
    {
        IsFunc = true;

        CurrToken.FuncName = szFuncName;
        Position =  Position + szFuncName.length();
    }

    return IsFunc;
}

int CMathParser::Round(double value)
{
    int result;

    if(value > 0.0){
        result = (int)(value+0.5);
    }
    else{
        result = (int)(value-0.5);
    }

    return result;
}

bool CMathParser::LowPriority(short state)
{
    short StateTmp = state;

    if (StateTmp == TS_PLUS)
    {
        StateTmp = TS_MINUS;
    }
    else if ((StateTmp == TS_TIMES) || (StateTmp == TS_DIVIDE))
    {
        StateTmp = TS_MOD;
    }

    if (TokenType == TT_CPAREN)
    {
        return true;
    }
    else  if (TokenType == TT_EOL)
    {
        return true;
    }
    else if (TokenType <= (StateTmp-TT_COUNT))
    {
        return true;
    }

    return false;
}
