#include "math_parser.h"
#include <math.h>
#include <stdio.h>
#include<stdlib.h>
#include<iostream>

#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>
#include "calc_measure.h"
#include <psm/general_access.h>


CMathParser::CMathParser(const string& szExpression)
{
    Expression = szExpression;
}

CMathParser::~CMathParser()
{
}

TValue CMathParser::parse()
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

//  cout << "begin parse: Expression = " << Expression << endl;

    push(FirstToken);
    TokenType = next_token();

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
                shift(TokenType);
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
                reduce(ATTCH_STATE);
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
                shift(TokenType);
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
            if (low_priority(State))
            {
                reduce(State+TS_COUNT);
            }
            else
            {
                shift(TokenType);
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
                shift(TokenType);
            }
            break;

        case TS_FUNC:
            reduce(TO_FUNC);
            break;

        default://Token state is operator
            if ((TokenType == TT_OPAREN))
            {
                shift(TT_OPAREN);
            }
            else if ((TokenType == TT_NUM) && (State != TT_FUNC))
            {
                shift(TT_NUM);
            }
            else if ((TokenType == TT_FUNC) && (State != TT_FUNC))
            {
                shift(TT_FUNC);
            }
            else if ((State == TT_CPAREN))
            {
                reduce(TO_PAREN);
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

short CMathParser::next_token()
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
        if (is_func("ABS")||is_func("ATAN")||is_func("COS")||is_func("EXP")||is_func("ROUND")||is_func("BOOL")||
            is_func("SIN")||is_func("SQRT")||is_func("SQR")||is_func("NOT")||is_func("TRUNC")||is_func("SGN")||is_func("LN"))

        {
            ResultNextToken = TT_FUNC;
            return ResultNextToken ;
        }
        else if (is_var(CurrToken.Value))
        {
            ResultNextToken = TT_NUM;
            return ResultNextToken;
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

void CMathParser::pop(TokenRec &Token)
{
    Token = Stack[--StackTop];

//  cout << "pop TokenRec: TokenRec.State = " << Token.State;
//  cout << "; TokenRec.Value = " << Token.Value << endl;
}

void CMathParser::push(TokenRec Token)
{
//  cout << "push TokenRec: TokenRec.State = " << Token.State;
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

void CMathParser::shift(short State)
{
    CurrToken.State = State;
    push(CurrToken);
    TokenType = next_token();

//  cout << "next_token: TokenType = " << TokenType << endl;
}

void CMathParser::reduce(short Reduction)
{
//  cout << "reduce: Reduction = " << Reduction << endl;

	TokenRec Token1, Token2;

	switch(Reduction)
	{
    case TO_OR:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        CurrToken.Value = (round(Token1.Value) | round(Token2.Value));
        break;

    case TO_EXPO: 
        pop(Token1);
        pop(Token2);
        pop(Token2);
        CurrToken.Value = (round(Token1.Value) ^ round(Token2.Value));
        break;

    case TO_AND:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        CurrToken.Value = (round(Token1.Value) & round(Token2.Value));
        break;

    case TO_PLUS:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        CurrToken.Value = Token1.Value + Token2.Value;
        break;

    case TO_MINUS:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        CurrToken.Value = Token2.Value - Token1.Value;
        break;

    case TO_TIMES:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        CurrToken.Value = Token1.Value * Token2.Value;
        break;

    case TO_DIVIDE:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        if (fabs(Token1.Value) < 0.000001){
            ParseError = true;
        }
        else {
            CurrToken.Value = Token2.Value / Token1.Value;
        }
        break;

    case TO_MOD:
        pop(Token1);
        pop(Token2);
        pop(Token2);
        if (fabs(Token1.Value) < 0.000001){
            ParseError = true;
        }
        else {
            CurrToken.Value = round(Token2.Value) % round(Token1.Value);
        }
        break;


    case TO_PAREN:
        pop(Token1);
        pop(CurrToken);
        pop(Token2);
        break;

    case TO_FUNC:
        pop(CurrToken);
        pop(Token1);

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
                CurrToken.Value = round(CurrToken.Value);
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
        pop(CurrToken);
		break;
	}

    short  ResultGotoState = Stack[StackTop-1].State + TT_COUNT;
    CurrToken.State = ResultGotoState;
//  cout << "ResultGotoState = " << ResultGotoState << endl;
    push(CurrToken);
}

bool CMathParser::is_func(const string& szFuncName)
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

bool CMathParser::is_var(TValue& Value)
{
    string VarName = "";

    while ((Position < Expression.length()) &&
           ((Expression[Position] >= 'A' && Expression[Position] <= 'Z') ||
            (Expression[Position] >= 'a' && Expression[Position] <= 'z') ||
            (Expression[Position] >= '0' && Expression[Position] <= '9') ||
            (Expression[Position] == '_')||
            (Expression[Position] == '#')))// in ['A'..'Z',  'a'..'z', '0'..'9', '_', '#']) do
    {
        VarName = VarName + Expression[Position];
        Position++;
    }

    return get_var(VarName, Value);
}

bool CMathParser::get_var(const string& szName, TValue& Value)
{
    size_t Pos = szName.find("MID");
    if (Pos  == string::npos)
        return false;

    string szMeasureId = szName;
    szMeasureId.erase(0, 3);

    int nMeasureId = strtol(szMeasureId.c_str(), NULL, 10);
    if ((nMeasureId >= ci_minMeasureIdMonsb) && (nMeasureId < ci_maxMeasureIdMonsb))
    {
        MonsbMeasure* pMonsb = MonsbManager::measure(nMeasureId);
        if (pMonsb != NULL)
        {
            Value = pMonsb->value();
            return true;
        }
    }
    else if ((nMeasureId >= ci_minMeasureIdYcadd) && (nMeasureId < ci_maxMeasureIdYcadd))
    {
        YcaddMeasure* pYcadd = YcaddManager::measure(nMeasureId);
        if (pYcadd != NULL)
        {
            Value = pYcadd->value();
            return true;
        }
    }
    else if ((nMeasureId >= ci_minMeasureIdCalc) && (nMeasureId < ci_maxMeasureIdCalc))
    {
        CalcMeasure* pCalc = CalcManager::measure(nMeasureId);
        if (pCalc != NULL)
        {
            Value = pCalc->value();
            return true;
        }
    }

    return false;
}

int CMathParser::round(double value)
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

bool CMathParser::low_priority(short state)
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
