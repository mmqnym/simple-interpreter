// StyleCheckType vector, stack

# include <iostream>
# include <iomanip>
# include <cstring>
# include <cctype>
# include <vector>
# include <stack>
# include <cstdlib>
# include <cstdio>
# include <cmath>

using namespace std ;

typedef char Str100[100] ;
typedef char Str30[30] ;

struct Token
{
  Str100 name ;
  Str30 type ;
} ; // struct Token

struct Id
{
  Str100 name ;
  Str30 type ; // int or float
  int valueInt ;
  float valueFloat ;
} ; // struct Id

typedef Token * TokenPtr ;
typedef Id * IdPtr ;
typedef vector<TokenPtr> * TokenPtrVecPtr ;
typedef vector<IdPtr> * IdPtrVecPtr ;

void InitBuf( char * buf, int size ) ;
void InitBuf100( Str100 buf ) ;
bool IsDelimiter( const char & ch ) ;
bool IsWhite( const char & ch ) ;
bool IsFloatStartWith( const char & ch ) ;
void Scanner( bool & end, bool & readIllegalorEnd, bool & readSemicolon, TokenPtrVecPtr & tokenBuf ) ;
void SkipCommand() ;
void DeleteTokenBuf( TokenPtrVecPtr & tokenBuf ) ;
void DeleteIdTable( IdPtrVecPtr & idTable ) ;
void Tokenizer( TokenPtrVecPtr & tokenBuf, char * statement ) ;
void StrToIdToken( char * statement, int & startIdx, TokenPtr idToken ) ;
void StrToNumToken( char * statement, int & startIdx, bool & startwithDot, TokenPtr numToken ) ;
void StrToDelimiterToken( char * statement, int & startIdx, TokenPtr DelimiterToken ) ;
bool Parser( TokenPtrVecPtr & tokenBuf ) ;
bool IsArithExp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsTerm( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsFactor( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsIDlessArithExpOrBexp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsBooleanOperator( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsNOTIDStartArithExpOrBexp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsNOTIDStartArithExp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsNOTIDStartTerm( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;
bool IsNOTIDStartFactor( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx ) ;

int GetOPPriority( Str100 tokenName ) ;
bool Has( TokenPtrVecPtr & tokenBuf, const char * target, int & position ) ;
void Calculate( TokenPtrVecPtr & tokenBuf, IdPtrVecPtr & idTable ) ;
void MergeSign( TokenPtrVecPtr & tokenBuf, TokenPtrVecPtr & mergeBuf,
                const int startIdx, const int endIdx ) ;
bool GetIdRecord( IdPtrVecPtr & idTable, Str100 target, IdPtr & idRecord ) ;
void TransferToPostfix( TokenPtrVecPtr & tokenBuf, TokenPtrVecPtr & postfixBuf, 
                        const int startIdx, const int endIdx ) ;
int CalculatePostfixForInt( TokenPtrVecPtr & postfixBuf, IdPtrVecPtr & idTable, bool & err ) ;
float CalculatePostfixForFloat( TokenPtrVecPtr & postfixBuf, IdPtrVecPtr & idTable, bool & err ) ;
float Rounding3( float num ) ;
void DefineId( IdPtrVecPtr & idTable, IdPtr & idSource ) ;
bool IsFloatOperation( TokenPtrVecPtr & postfixBuf, IdPtrVecPtr & idTable ) ;
bool Compare( float num1, float num2, TokenPtr & cmpOp ) ;


// ---> start in here
int main()
{
  int uTestNum = cin.get() ;
  cin.get() ; // skip '\n'

  cout << "Program starts...\n" ;

  vector<TokenPtr> * tokenBuf = new vector<TokenPtr>() ;
  tokenBuf->reserve( 10 ) ;
  
  bool end = false, readSemicolon = false, readIllegal = false ;
  
  vector<IdPtr> * idTable = new vector<IdPtr>() ;
  idTable->reserve( 10 ) ;

  do
  { 
    // read statement and transfer them to token set.
    Scanner( end, readIllegal, readSemicolon, tokenBuf ) ; 
    
    // will delete this block
    /*
    if ( !readIllegal )
    {
      for ( int i = 0 ; i < tokenBuf->size() ; i++ )
          cout << tokenBuf->at( i )->type << ", " << tokenBuf->at( i )->name << endl ;
    }
    */

    if ( ( tokenBuf->size() > 0 && !strcmp( tokenBuf->at( 0 )->name, ";" ) ) 
         || readIllegal )
    {
      // ignore this statement, then pass and flush token buffer
      tokenBuf->clear() ;
    } // if ( only ';' in statement )

    else if ( readSemicolon )
    {

      // check syntax && excute statement, then flush token buffer
      bool isOk = Parser( tokenBuf ) ;

      if ( isOk ) // excute
      {
        // cout << "** " << "Pass" << " **\n" ;
        Calculate( tokenBuf, idTable ) ;
        
        // for test
        /*
        cout << "table:\n" ;
        for ( int i = 0 ; i < idTable->size() ; i++ )
          cout << idTable->at(i)->name << ", " << idTable->at(i)->type << ", " 
          << idTable->at(i)->valueInt << ", " << idTable->at(i)->valueFloat << endl ;
        
        cout << "\n<end>\n" ; 
        */
      } // if

      else
      {
        // cout << "** " << "Unpass" << " **\n" ;
        tokenBuf->clear() ;
      } // else

      tokenBuf->clear() ;
    } // else if ( read ';' )

    else if ( !end )
    {
      // "just" check

      TokenPtr idToken = new Token ;
      strcpy( idToken->name, "x;" ) ;
      strcpy( idToken->type, "TMP_COMMAND_END" ) ;
      tokenBuf->push_back( idToken ) ;

      bool isOk = Parser( tokenBuf ) ;

      tokenBuf->pop_back() ;
      
      if ( !isOk )
        tokenBuf->clear() ;
      
    } // else if => read '\n'

    readSemicolon = false ;
    readIllegal = false ;

  } while ( !end ) ;

  DeleteTokenBuf( tokenBuf ) ;
  DeleteIdTable( idTable ) ;
  tokenBuf->clear() ;
  idTable->clear() ;
  delete tokenBuf ;
  delete idTable ;
  cout << "> Program exits..." << endl ;
  return 0 ;
} // main()

void InitBuf( char * buf, int size )
{
  for ( int i = 0 ; i < size ; i++ )
    buf[i] = '\0' ;
} // InitBuf()

void InitBuf100( Str100 buf )
{
  for ( int i = 0 ; i < 100 ; i++ )
    buf[i] = '\0' ;
} // InitBuf100()

bool IsWhite( const char & ch )
{
  return ( ch == ' ' || ch == '\t' ) ;
} // IsWhite()

bool IsDelimiter( const char & ch )
{
  // ';' will be checked outside alone for more easier to check the statement is end.
  // '/' also be checked outside alone for more easier to check line commands.
  return ( ch == '+' || ch == '-' || ch == '*' || ch == '.' ||
           ch == '(' || ch == ')' || ch == '<' || ch == '>' || 
           ch == ':' || ch == '=' || ch == '_' ) ;
} // IsDelimiter()

bool IsFloatStartWith( const char & ch )
{
  return ( ch == '.' || isdigit( ch ) ) ;
} // IsFloatStartWith()

void SkipCommand()
{
  char ch = cin.get() ;

  while ( ch != '\n' )
    ch = cin.get() ;

} // SkipCommand()

void Scanner( bool & end, bool & readIllegal, bool & readSemicolon, TokenPtrVecPtr & tokenBuf )
{
  bool prevIsSlash = false ;

  char * statementBuf = new char[400] ; // record temp string until ';' or '\n' is read
  InitBuf( statementBuf, 400 ) ;

  int state_idx = 0 ;
  char ch = cin.get() ;

  while ( ch != EOF )
  {
    if ( statementBuf[0] == 'q' && statementBuf[1] == 'u' && statementBuf[2] == 'i'
         && statementBuf[3] == 't' )
    {
      delete statementBuf ;
      end = true ;
      return ;
    } // if

    if ( IsWhite( ch ) )
      prevIsSlash = false ;

    else if ( ch == '\n' )
    {
      if ( statementBuf[0] != '\0' )
      {
        Tokenizer( tokenBuf, statementBuf ) ; // change legal statement to token set
        delete statementBuf ;
        return ;
      } // if
    } // else if

    else if ( isdigit( ch ) || isalpha( ch ) || IsDelimiter( ch ) )
    {
      prevIsSlash = false ;
      statementBuf[state_idx] = ch ;
      state_idx++ ;
    } // else if
    
    else if ( prevIsSlash && ch == '/' ) // is command
    {
      SkipCommand() ;
      prevIsSlash = false ;
      state_idx -= 1 ;
      statementBuf[state_idx] = '\0' ;
      
    } // else if

    else if ( ch == '/' ) // get 1 '/', maybe command or division
    {
      prevIsSlash = true ;
      statementBuf[state_idx] = ch ;
      state_idx++ ;
    } // else if

    else if ( ch == ';' ) // the statement end
    {
      statementBuf[state_idx] = ch ;
      Tokenizer( tokenBuf, statementBuf ) ; // change legal statement to token set
      delete statementBuf ;
      readSemicolon = true ;

      return ;
    } // else if

    else // illeagal character
    {
      // skip this line
      cout << "> Unrecognized token with first char : " << "\'" << ch << "\'" << endl ;
      ch = cin.get() ;

      while ( ch != EOF && ch != ';' && ch != '\n' )
        ch = cin.get() ;
      delete statementBuf ;
      readIllegal = true ;

      if ( ch == EOF )
        end = true ;
        
      return ;
    } // else
    
    ch = cin.get() ;
  } // while ( ch != EOF )

  delete statementBuf ;
  end = true ;
  return ;
} // Scanner()

void DeleteTokenBuf( TokenPtrVecPtr & tokenBuf )
{
  if ( tokenBuf->size() == 0 )
    return ;
  
  for ( int i = tokenBuf->size() - 1 ; i >= 0 ; i-- )
  {
    TokenPtr tmp = tokenBuf->at( i ) ;
    delete tmp ;
  } // for
} // DeleteTokenBuf()

void DeleteIdTable( IdPtrVecPtr & idTable )
{
  if ( idTable->size() == 0 )
    return ;
  
  for ( int i = idTable->size() - 1 ; i >= 0 ; i-- )
  {
    IdPtr tmp = idTable->at( i ) ;
    delete tmp ;
  } // for
} // DeleteIdTable()

void Tokenizer( TokenPtrVecPtr & tokenBuf, char * statement )
{
    // QUIT is the word 'quit', an IDENT starts with a letter and is followed by digits or letters or
    // underlines
    // (however, an IDENT cannot be the word 'quit' ), a NUM is either an integer or a float number
    // (e.g., 35, 7, 43.8, 43., .35, 1.0 and 07)
    // and SIGN is either '+' or '-'.

  int idx = 0 ;

  while ( statement[idx] != '\0' )
  {
    char ch = statement[idx] ;

    if ( isalpha( ch ) ) // id
    {
      TokenPtr idToken = new Token ;

      StrToIdToken( statement, idx, idToken ) ;
      tokenBuf->push_back( idToken ) ;
    } // if ( alpha )

    else if ( ch == ';' ) // end
    {
      TokenPtr idToken = new Token ;
      strcpy( idToken->name, ";" ) ;
      strcpy( idToken->type, "COMMAND_END" ) ;
      tokenBuf->push_back( idToken ) ;
      idx++ ;
    } // else if ( end )
    
    else if ( isdigit( ch ) || ch == '.' ) // num (int or float)
    {
      bool startwithDot = false ;
      TokenPtr numToken = new Token ;

      if ( ch == '.' )
        startwithDot = true ;

      StrToNumToken( statement, idx, startwithDot, numToken ) ;
      tokenBuf->push_back( numToken ) ;

    } // else if ( num )
    
    else if ( IsDelimiter( ch ) || ch == '/' ) // delimiter
    {
      TokenPtr delimiterToken = new Token ;
      StrToDelimiterToken( statement, idx, delimiterToken ) ;
      tokenBuf->push_back( delimiterToken ) ;
    } // else if ( delimiter )

  } // while ( statement[idx] != '\0' )

} // Tokenizer()

void StrToIdToken( char * statement, int & startIdx, TokenPtr idToken )
{
  Str100 tmpId ;
  InitBuf100( tmpId ) ;

  char ch = statement[startIdx] ;
  int tmpIdIdx = 0 ;

  while ( isdigit( ch ) || isalpha( ch ) || ch == '_' )
  {
    tmpId[tmpIdIdx] = ch ;
    tmpIdIdx++ ;

    startIdx++ ;
    ch = statement[startIdx] ;
  } // while ( isdigit( ch ) || isalpha( ch ) || ch == '_' )

  strcpy( idToken->type, "ID" ) ;
  strcpy( idToken->name, tmpId ) ;
} // StrToIdToken()

void StrToNumToken( char * statement, int & startIdx, bool & startwithDot, TokenPtr numToken )
{
  char ch = statement[startIdx] ;
  Str100 tmpNum ;
  InitBuf100( tmpNum ) ;

  if ( startwithDot ) // is float
  {
    tmpNum[0] = ch ; // '.'
    startIdx++ ;
    ch = statement[startIdx] ;

    int digitsIdx = 1 ;
    
    while ( isdigit( ch ) )
    {
      tmpNum[digitsIdx] = ch ;
      digitsIdx++ ;
      startIdx++ ;

      ch = statement[startIdx] ;
    } // while ( isdigit( statement[startIdx] ) )

    strcpy( numToken->type, "NUM_FLOAT" ) ;
    strcpy( numToken->name, tmpNum ) ;
  } // if ( isfloat )

  else // maybe int or float
  {
    int dotCount = 0 ;
    int digitsIdx = 0 ;

    while ( ( isdigit( ch ) || ch == '.' ) && dotCount <= 1 )
    {  
      tmpNum[digitsIdx] = ch ;
      digitsIdx++ ;
      startIdx++ ;

      ch = statement[startIdx] ;

      if ( ch == '.' )
        dotCount++ ;
    } // while

    if ( dotCount == 0 )
      strcpy( numToken->type, "NUM_INT" ) ;
    else
      strcpy( numToken->type, "NUM_FLOAT" ) ;

    strcpy( numToken->name, tmpNum ) ;
  } // else ( is int or float )
} // StrToNumToken()

void StrToDelimiterToken( char * statement, int & startIdx, TokenPtr DelimiterToken )
{
  char ch = statement[startIdx] ;
  Str100 tmpBuf ;
  InitBuf100( tmpBuf ) ;

  if ( ch == ':' ) // maybe ":=" or just ':'
  {
    tmpBuf[0] = ch ;
    startIdx++ ;
    ch = statement[startIdx] ;

    if ( ch == '=' )
    {
      tmpBuf[1] = ch ;

      strcpy( DelimiterToken->name, tmpBuf ) ;
      strcpy( DelimiterToken->type, "DEFINE" ) ;

      startIdx++ ;
    } // if

    else
    {
      strcpy( DelimiterToken->name, tmpBuf ) ;
      strcpy( DelimiterToken->type, "NOTUSED" ) ;
    } // else

  } // if ( maybe ":=" or just ':' )

  else if ( ch == '+' || ch == '-' || ch == '*' || ch == '/' ) // arithmetic operator
  {
    tmpBuf[0] = ch ;
    strcpy( DelimiterToken->name, tmpBuf ) ;
    strcpy( DelimiterToken->type, "OPERATOR" ) ;

    startIdx++ ;
  } // else if ( arithmetic operator )

  // "=" "<" "<=" "<>" ">" ">="
  else if ( ch == '=' || ch == '<' || ch == '>' ) // compare operator
  {
    
    if ( ch == '=' )
    {
      tmpBuf[0] = ch ;
      startIdx++ ;
    } // if ( = )

    else if ( ch == '<' )
    {
      tmpBuf[0] = ch ;
      startIdx++ ;
      ch = statement[startIdx] ;

      if ( ch == '=' || ch == '>' )
      {
        tmpBuf[1] = ch ;
        startIdx++ ;
      } // if ( "<=" | "<>" )
      
    } // else if ( '<' | "<=" | "<>" )

    else if ( ch == '>' )
    {
      tmpBuf[0] = ch ;
      startIdx++ ;
      ch = statement[startIdx] ;

      if ( ch == '=' )
      {
        tmpBuf[1] = ch ;
        startIdx++ ;
      } // if ( "<=" | "<>" )
      
    } // else if ( '>' | ">=" )

    strcpy( DelimiterToken->name, tmpBuf ) ;
    strcpy( DelimiterToken->type, "CMP_OPERATOR" ) ;

  } // else if ( compare operator )

  else if ( ch == '(' || ch == ')' )
  {
    Str100 tmpBuf ;
    InitBuf100( tmpBuf ) ;
    tmpBuf[0] = ch ;
    strcpy( DelimiterToken->name, tmpBuf ) ;
    strcpy( DelimiterToken->type, "PARENTHESES" ) ;

    startIdx++ ;
  } // else if ( parentheses )

} // StrToDelimiterToken()

bool Parser( TokenPtrVecPtr & tokenBuf )
{
  bool isOk = false ;

  int startIdx = 0 ;
  int lastIdx = tokenBuf->size() - 1 ;

  if ( !strcmp( tokenBuf->at( 0 )->type, "ID" ) )
  {
    if ( tokenBuf->size() > 1 ) // ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
    {
      if ( !strcmp( tokenBuf->at( 1 )->name, ":=" ) )
      {
        startIdx = 2 ;
        isOk = IsArithExp( tokenBuf, startIdx, lastIdx - 1 ) ; // token_vector, startIdx, endIdx(exclude ';')
      } // if
      else
      {
        startIdx = 1 ;
        isOk = IsIDlessArithExpOrBexp( tokenBuf, startIdx, lastIdx - 1 ) ;
      } // else

      if ( isOk && ( startIdx == lastIdx ) )
        return true ;
      else
      {
        if ( !strcmp( tokenBuf->at( startIdx )->type, "NUM_INT" ) ||
             !strcmp( tokenBuf->at( startIdx )->type, "NUM_FLOAT" ) )
          cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx )->name << "\'\n" ;
        return false ;
      } // else
      
    } // if ( ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';' )

    else // only "ID"
    {
      return true ;
    } // else ( only "ID" )
  } // if ( start with "ID" )
  
  else // <NOT_IDStartArithExpOrBexp> ';'
  {
    int startIdx = 0 ;
    bool isOk = IsNOTIDStartArithExpOrBexp( tokenBuf, startIdx, lastIdx - 1 ) ;

    if ( isOk )
      return true ; // temp
    else
      return false ; 
  } // else ( <NOT_IDStartArithExpOrBexp> )

  
} // Parser()

bool IsArithExp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // <Term> { '+' <Term> | '-' <Term> }
  bool isOk = IsTerm( tokenBuf, startIdx, endIdx ) ;

  if ( isOk )
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) || 
         !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
    {
      // term with +- term
      startIdx++ ;
      bool isExpectedChar = true ;

      while ( startIdx <= endIdx && isExpectedChar )
      {
        isOk = IsTerm( tokenBuf, startIdx, endIdx ) ;

        if ( !isOk )
          return false ;
        
        if ( isOk && ( startIdx - 1 == endIdx ) )
          return true ;

        if ( startIdx <= endIdx )
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) ||
               !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
          {
            isExpectedChar = true ;
            startIdx++ ;
          } // if

          else
            return true ;
        } // if
      } // while
      
      return true ;
    } // if

    else
      return true ;
    
  } // if ( isOk )

  return false ;
} // IsArithExp()

bool IsTerm( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // <Factor> { '*' <Factor> | '/' <Factor> }

  bool isOk = IsFactor( tokenBuf, startIdx, endIdx ) ;
  
  if ( isOk )
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "*" ) || 
         !strcmp( tokenBuf->at( startIdx )->name, "/" ) )
    {
      // term with +- term
      startIdx++ ;
      bool isExpectedChar = true ;

      while ( startIdx <= endIdx && isExpectedChar )
      {
        isOk = IsFactor( tokenBuf, startIdx, endIdx ) ;

        if ( !isOk )
          return false ;
        
        if ( isOk && ( startIdx - 1 == endIdx ) )
          return true ;

        if ( startIdx <= endIdx )
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "*" ) ||
               !strcmp( tokenBuf->at( startIdx )->name, "/" ) )
          {
            isExpectedChar = true ;
            startIdx++ ;
          } // if

          else
            return true ;

        } // if
      } // while
        
    } // if

    else
      return true ;
    
  } // if ( isOk )

  return false ;
} // IsTerm()

bool IsFactor( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // ID | [ SIGN ] NUM | '(' <ArithExp> ')'

  if ( !strcmp( tokenBuf->at( startIdx )->type, "ID" ) )
  {
    startIdx++ ;
    return true ;
  } // if ( "ID" )

  else if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) ||
            !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
  {
    startIdx++ ;

    if ( startIdx <= endIdx )
    {
      if ( !strcmp( tokenBuf->at( startIdx )->type, "NUM_INT" ) ||
           !strcmp( tokenBuf->at( startIdx )->type, "NUM_FLOAT" ) )
      {
        startIdx++ ;
        return true ;
      } // if

      else
      {
        if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
          return true ;

        cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx )->name << "\'\n" ;
        return false ;
      } // else
    } // if

    else // only '+' or '-' in the end
    {
      if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
        return true ;

      cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx - 1 )->name << "\'\n" ;
      return false ;
    } // else
  } // else if ( "SIGN"  "NUM" )

  else if ( !strcmp( tokenBuf->at( startIdx )->type, "NUM_INT" ) ||
            !strcmp( tokenBuf->at( startIdx )->type, "NUM_FLOAT" ) )
  {
    startIdx++ ;
    return true ;
  } // else if  ( "NUM" )

  else if ( !strcmp( tokenBuf->at( startIdx )->name, "(" ) )
  {
    startIdx++ ;

    if ( startIdx < endIdx )
    {
      bool isOk = IsArithExp( tokenBuf, startIdx, endIdx ) ;

      if ( isOk )
      {
        if ( startIdx > endIdx )
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
            return true ;
          cout << "> Unexpected token : " << "\'" << "(" << "\'\n" ;
          return false ;
        } // if

        if ( !strcmp( tokenBuf->at( startIdx )->name, ")" ) )
        {
          startIdx++ ;
          return true ;
        } // if
        else
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
            return true ;
          cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx - 1 )->name << "\'\n" ;
          return false ;
        } // else
        
      } // if
      
      return false ;
    } // else if

    else // last token is current '('
    {
      if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
        return true ;
      cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx - 1 )->name << "\'\n" ;
      return false ;
    } // else
  } // else if ( '(' <ArithExp> ')' )

  else // not allowed token
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
      return true ;
    cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx )->name << "\'\n" ;
    return false ;
  } // else
} // IsFactor()

bool IsIDlessArithExpOrBexp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  while ( startIdx <= endIdx )
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) ||
         !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
    {
      startIdx++ ;

      bool isOk = IsTerm( tokenBuf, startIdx, endIdx ) ;

      if ( !isOk )
        return false ;
    } // if

    else if ( !strcmp( tokenBuf->at( startIdx )->name, "*" ) ||
              !strcmp( tokenBuf->at( startIdx )->name, "/" ) )
    {
      startIdx++ ;

      bool isOk = IsFactor( tokenBuf, startIdx, endIdx ) ;

      if ( !isOk )
        return false ;
    } // else if

    else if ( IsBooleanOperator( tokenBuf, startIdx, endIdx ) )
    {
      // if checked, escape this func

      bool isOk = IsArithExp( tokenBuf, startIdx, endIdx ) ;

      if ( !isOk )
        return false ;
      else
        return true ;

    } // else if

    else
    {
      if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
        return true ;
      
      return false ;
    } // else

  } // while

  return true ;

} // IsIDlessArithExpOrBexp()

bool IsNOTIDStartArithExpOrBexp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]

  bool isOk = IsNOTIDStartArithExp( tokenBuf, startIdx, endIdx ) ;

  if ( isOk && startIdx > endIdx ) // only <NOT_ID_StartArithExp>
    return true ;

  else if ( isOk && IsBooleanOperator( tokenBuf, startIdx, endIdx ) )
  {
    return IsArithExp( tokenBuf, startIdx, endIdx ) ;
  } // else if ( isOk and do comparison )

  else
    return false ;
    
} // IsNOTIDStartArithExpOrBexp()

bool IsBooleanOperator( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  if ( !strcmp( tokenBuf->at( startIdx )->name, "=" ) ||
       !strcmp( tokenBuf->at( startIdx )->name, "<>" ) ||
       !strcmp( tokenBuf->at( startIdx )->name, ">" ) ||
       !strcmp( tokenBuf->at( startIdx )->name, "<" ) ||
       !strcmp( tokenBuf->at( startIdx )->name, ">=" ) ||
       !strcmp( tokenBuf->at( startIdx )->name, "<=" ) )
  {
    startIdx++ ;
    return true ;
  } // if

  else
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
      return true ;
    cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx )->name << "\'\n" ;
    return false ;
  } // else
} // IsBooleanOperator()

bool IsNOTIDStartArithExp( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }
  bool isOk = IsNOTIDStartTerm( tokenBuf, startIdx, endIdx ) ;

  if ( isOk )
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) || 
         !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
    {
      // term with +- term
      startIdx++ ;
      bool isExpectedChar = true ;

      while ( startIdx <= endIdx && isExpectedChar )
      {
        isOk = IsTerm( tokenBuf, startIdx, endIdx ) ;

        if ( !isOk )
          return false ;
        
        if ( isOk && ( startIdx - 1 == endIdx ) )
          return true ;

        if ( startIdx <= endIdx )
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) ||
               !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
          {
            isExpectedChar = true ;
            startIdx++ ;
          } // if

          else
            return true ;
        } // if
      } // while

      return true ;
        
    } // if

    else
      return true ;
    
  } // if ( isOk )

  return false ;
} // IsNOTIDStartArithExp()

bool IsNOTIDStartTerm( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }

  bool isOk = IsNOTIDStartFactor( tokenBuf, startIdx, endIdx ) ;
  
  if ( isOk )
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "*" ) || 
         !strcmp( tokenBuf->at( startIdx )->name, "/" ) )
    {
      // term with +- term
      startIdx++ ;
      bool isExpectedChar = true ;

      while ( startIdx <= endIdx && isExpectedChar )
      {
        isOk = IsFactor( tokenBuf, startIdx, endIdx ) ;

        if ( !isOk )
          return false ;
        
        if ( isOk && ( startIdx - 1 == endIdx ) )
          return true ;

        if ( startIdx <= endIdx )
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "*" ) ||
               !strcmp( tokenBuf->at( startIdx )->name, "/" ) )
          {
            isExpectedChar = true ;
            startIdx++ ;
          } // if

          else
            return true ;

        } // if
      } // while
        
    } // if

    else
      return true ;
    
  } // if ( isOk )

  return false ;

} // IsNOTIDStartTerm()

bool IsNOTIDStartFactor( TokenPtrVecPtr & tokenBuf, int & startIdx, const int endIdx )
{
  // [ SIGN ] NUM | '(' <ArithExp> ')'
  if ( !strcmp( tokenBuf->at( startIdx )->name, "+" ) ||
       !strcmp( tokenBuf->at( startIdx )->name, "-" ) )
  {
    startIdx++ ;

    if ( startIdx <= endIdx )
    {
      if ( !strcmp( tokenBuf->at( startIdx )->type, "NUM_INT" ) ||
           !strcmp( tokenBuf->at( startIdx )->type, "NUM_FLOAT" ) )
      {
        startIdx++ ;
        return true ;
      } // if

      else
      {
        if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
          return true ;

        cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx )->name << "\'\n" ;
        return false ;
      } // else
    } // if

    else
    {
      if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
        return true ;
      cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx - 1 )->name << "\'\n" ;
      return false ;
    } // else
  } // if ( "SIGN"  "NUM" )

  else if ( !strcmp( tokenBuf->at( startIdx )->type, "NUM_INT" ) ||
            !strcmp( tokenBuf->at( startIdx )->type, "NUM_FLOAT" ) )
  {
    startIdx++ ;
    return true ;
  } // else if  ( "NUM" )

  else if ( !strcmp( tokenBuf->at( startIdx )->name, "(" ) )
  {
    startIdx++ ;

    if ( startIdx < endIdx )
    {
      bool isOk = IsArithExp( tokenBuf, startIdx, endIdx ) ;

      if ( isOk )
      {
        if ( startIdx > endIdx )
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
            return true ;

          cout << "> Unexpected token : " << "\'" << "(" << "\'\n" ;
          return false ;
        } // if

        if ( !strcmp( tokenBuf->at( startIdx )->name, ")" ) )
        {
          startIdx++ ;
          return true ;
        } // if
        else
        {
          if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
            return true ;

          cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx - 1 )->name << "\'\n" ;
          return false ;
        } // else
        
      } // if
      
      return false ;
    } // else if

    else // last token is current '('
    {
      if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
        return true ;

      cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx - 1 )->name << "\'\n" ;
      return false ;
    } // else
  } // else if ( '(' <ArithExp> ')' )

  else // not allowed token
  {
    if ( !strcmp( tokenBuf->at( startIdx )->name, "x;" ) )
      return true ;

    cout << "> Unexpected token : " << "\'" << tokenBuf->at( startIdx )->name << "\'\n" ;
    return false ;
  } // else
} // IsNOTIDStartFactor()

int GetOPPriority( Str100 tokenName )
{
  if ( !strcmp( tokenName, "*" ) ||
       !strcmp( tokenName, "/" ) )
    return 1 ;
  else
    return 0 ;
} // GetOPPriority()

bool Has( TokenPtrVecPtr & tokenBuf, const char * target, int & position )
{
  // has a target token
  
  for ( int i = 0 ; i < tokenBuf->size() ; i++ )
  {
    if ( !strcmp( tokenBuf->at( i )->type, target ) )
    {
      position = i ;
      return true ;
    } // if
  } // for

  return false ;

} // Has()

bool IsFloatOperation( TokenPtrVecPtr & postfixBuf, IdPtrVecPtr & idTable )
{
  for ( int i = 0 ; i < postfixBuf->size() ; i++ )
  {
    if ( !strcmp( postfixBuf->at( i )->type, "NUM_FLOAT" ) )
    {
      return true ;
    } // if
    else if ( !strcmp( postfixBuf->at( i )->type, "ID" ) )
    {
      IdPtr idRecord = NULL ;
      bool found = GetIdRecord( idTable, postfixBuf->at( i )->name, idRecord ) ;
       
      if ( found && !strcmp( idRecord->type, "NUM_FLOAT" ) )
        return true ;
    } // else if
  } // for

  return false ;
} // IsFloatOperation()

bool Compare( float num1, float num2, TokenPtr & cmpOp )
{
  if ( !strcmp( cmpOp->name, "=" ) )
  {
    float diff = abs( num1 - num2 ) ;
    
    if ( diff <= 0.0001 )
      return true ;

    return false ;
  } // if ( equal )

  else if ( !strcmp( cmpOp->name, "<>" ) )
  {
    float diff = abs( num1 - num2 ) ;
    
    if ( diff > 0.0001 )
      return true ;

    return false ;
  } // else if ( not equal )

  else if ( !strcmp( cmpOp->name, ">" ) )
  {
    if ( num1 > num2 + 0.0001 )
      return true ;

    return false ;
  } // else if ( more than )

  else if ( !strcmp( cmpOp->name, "<" ) )
  {
    if ( num1 + 0.0001 < num2 )
      return true ;
      
    return false ;
  } // else if ( small than )

  else if ( !strcmp( cmpOp->name, ">=" ) )
  {
    if ( num1 >= num2 + 0.0001 )
      return true ;

    return false ;
  } // else if ( more than or equal to )

  else // "<="
  {
    if ( num1 + 0.0001 <= num2 )
      return true ;
      
    return false ;
  } // else if ( small than or equal to )
} // Compare()

void Calculate( TokenPtrVecPtr & tokenBuf, IdPtrVecPtr & idTable )
{
  vector<TokenPtr> * postfixBuf = new vector<TokenPtr>() ;
  postfixBuf->reserve( 10 ) ;
  int position = -1 ;
  bool err = false ;

  // compare
  if ( Has( tokenBuf, "CMP_OPERATOR", position ) )
  {
    // using float to compare every formula
    TransferToPostfix( tokenBuf, postfixBuf, 0, position - 1 ) ;

    float num1 = CalculatePostfixForFloat( postfixBuf, idTable, err ) ;
    
    if ( err )
      return ;
    
    postfixBuf->clear() ;

    TransferToPostfix( tokenBuf, postfixBuf, position + 1, tokenBuf->size() - 2 ) ;

    float num2 = CalculatePostfixForFloat( postfixBuf, idTable, err ) ;

    if ( err )
      return ;
    
    TokenPtr cmpOp = tokenBuf->at( position ) ;

    if ( Compare( num1, num2, cmpOp ) )
      cout << "> true" << endl ;
    else
      cout << "> false" << endl ;
  } // if                  

  // assign
  else if ( Has( tokenBuf, "DEFINE", position ) )
  {
    // trasfer formula after define token, and calculate formula.
    // if that id exist, replace; if not, adding it.

    IdPtr id = new Id ;
    strcpy( id->name, tokenBuf->at( position - 1 )->name ) ;
    id->valueInt = 0 ;
    id->valueFloat = 0.0 ;
    TransferToPostfix( tokenBuf, postfixBuf, position + 1, tokenBuf->size() - 2 ) ;

    if ( !IsFloatOperation( postfixBuf, idTable ) )
    {
      int num = CalculatePostfixForInt( postfixBuf, idTable, err ) ;

      if ( err )
        return ;
      strcpy( id->type, "NUM_INT" ) ;
      id->valueInt = num ;
      DefineId( idTable, id ) ;

      cout << "> " << num << endl ;
    } // if

    else
    {
      float num = CalculatePostfixForFloat( postfixBuf, idTable, err ) ;

      if ( err )
        return ;
      strcpy( id->type, "NUM_FLOAT" ) ;
      id->valueFloat = num ;
      DefineId( idTable, id ) ;

      num = Rounding3( num ) ;
      cout << "> " << fixed << setprecision( 3 ) << num << endl ;
    } // else
  } // else if
  
  // pure calculate
  else
  {
    TransferToPostfix( tokenBuf, postfixBuf, 0, tokenBuf->size() - 2 ) ; 

    if ( !IsFloatOperation( postfixBuf, idTable ) )
    {
      int num = CalculatePostfixForInt( postfixBuf, idTable, err ) ;
      
      if ( err )
        return ;
      cout << "> " << num << endl ;
    } // if

    else
    {
      float num = CalculatePostfixForFloat( postfixBuf, idTable, err ) ;

      if ( err )
        return ;
      num = Rounding3( num ) ;
      cout << "> " << fixed << setprecision( 3 ) << num << endl ;
    } // else
    
  } // else

  delete postfixBuf ;
} // Calculate()

void MergeSign( TokenPtrVecPtr & tokenBuf, TokenPtrVecPtr & mergeBuf,
                const int startIdx, const int endIdx )
{
  bool end = false ;
  int newStartIdx = -1 ;

  for ( int i = startIdx ; i <= endIdx && !end ; i++ )
  {
    // find first number, test it has sign or not
    // if yes, merge it.

    if ( !strcmp( tokenBuf->at( i )->type, "NUM_INT" ) || 
         !strcmp( tokenBuf->at( i )->type, "NUM_FLOAT" ) )
    {
      if ( i - 1 >= startIdx && !strcmp( tokenBuf->at( i - 1 )->name, "+" ) )
      {
        mergeBuf->at( mergeBuf->size() - 1 ) = tokenBuf->at( i ) ;
      } // if

      else if ( i - 1 >= startIdx && !strcmp( tokenBuf->at( i - 1 )->name, "-" ) )
      {
        if ( !strcmp( tokenBuf->at( i )->type, "NUM_INT" ) )
        {
          int newValue = -1 * atoi( tokenBuf->at( i )->name ) ;
          Str100 tmp ;
          sprintf( tmp, "%d", newValue ) ;
          strcpy( mergeBuf->at( mergeBuf->size() - 1 )->name, tmp ) ;
        } // if int

        else
        {
          float newValue = -1 * atof( tokenBuf->at( i )->name ) ;
          Str100 tmp ;
          sprintf( tmp, "%f", newValue ) ;
          strcpy( mergeBuf->at( mergeBuf->size() - 1 )->name, tmp ) ;
        } // else float

        strcpy( mergeBuf->at( mergeBuf->size() - 1 )->type, tokenBuf->at( i )->type ) ;
      } // else if

      else
      {
        mergeBuf->push_back( tokenBuf->at( i ) ) ;
      } // else
      
      end = true ;
      newStartIdx = i + 1 ;
    } // if

    else
    {
      mergeBuf->push_back( tokenBuf->at( i ) ) ;

      if ( !strcmp( tokenBuf->at( i )->type, "ID" ) )
      {
        newStartIdx = i + 1 ;
        end = true ;
      } // if
    } // else 
  } // for

  // merge rear part

  for ( int i = newStartIdx ; i <= endIdx ; i++ )
  {
    // check op is sign or not, if yes, merge it. 
    bool mayHasSign = false ;
    int j = i - 1 ;
    end = false ;

    if ( ( !strcmp( tokenBuf->at( i )->type, "NUM_INT" ) ||
           !strcmp( tokenBuf->at( i )->type, "NUM_FLOAT" ) ) &&
         ( !strcmp( tokenBuf->at( j )->name, "+" ) ||
           !strcmp( tokenBuf->at( j )->name, "-" ) )
       )
    {

      for ( int k = j - 1 ; k >= startIdx && !end ; k-- )
      {
        // find first number or sign, if it's number, push them to buf, else merge it
        if ( !strcmp( tokenBuf->at( k )->type, "NUM_INT" ) ||
             !strcmp( tokenBuf->at( k )->type, "NUM_FLOAT" ) )
        {
          mergeBuf->push_back( tokenBuf->at( i ) ) ;
          end = true ;
        } // if

        else if ( !strcmp( tokenBuf->at( k )->name, "+" ) ||
                  !strcmp( tokenBuf->at( k )->name, "-" ) ||
                  !strcmp( tokenBuf->at( k )->name, "(" ) )
        {
          if ( !strcmp( tokenBuf->at( j )->name, "-" ) )
          {
            if ( !strcmp( tokenBuf->at( i )->type, "NUM_INT" ) )
            {
              int newValue = -1 * atoi( tokenBuf->at( i )->name ) ;
              Str100 tmp ;
              sprintf( tmp, "%d", newValue ) ;
              strcpy( mergeBuf->at( mergeBuf->size() - 1 )->name, tmp ) ;
            } // if int

            else
            {
              float newValue = -1 * atof( tokenBuf->at( i )->name ) ;
              Str100 tmp ;
              sprintf( tmp, "%f", newValue ) ;
              strcpy( mergeBuf->at( mergeBuf->size() - 1 )->name, tmp ) ;
            } // else float

            strcpy( mergeBuf->at( mergeBuf->size() - 1 )->type, tokenBuf->at( i )->type ) ;
            end = true ;
          } // if

          else // '+'
          {
            mergeBuf->at( mergeBuf->size() - 1 ) = tokenBuf->at( i ) ;
            end = true ;
          } // else
        } // else if

        else if ( !strcmp( tokenBuf->at( k )->name, "*" ) ||
                  !strcmp( tokenBuf->at( k )->name, "/" ) )
        {
          mergeBuf->push_back( tokenBuf->at( i ) ) ;
          end = true ;
        } // else if

        else if ( !strcmp( tokenBuf->at( k )->type, "ID" ) )
        {
          mergeBuf->push_back( tokenBuf->at( i ) ) ;
          end = true ;
        } // else if

      } // for
    } // if

    else // not number
    {
      mergeBuf->push_back( tokenBuf->at( i ) ) ;
    } // else
  } // for
  
} // MergeSign()

void TransferToPostfix( TokenPtrVecPtr & tokenBuf, TokenPtrVecPtr & postfixBuf, 
                        const int startIdx, const int endIdx )
{
  // merge sign
  vector<TokenPtr> * mergeBuf = new vector<TokenPtr>() ;
  mergeBuf->reserve( tokenBuf->size() ) ;
  MergeSign( tokenBuf, mergeBuf, startIdx, endIdx ) ;

  stack<TokenPtr> * stk = new stack<TokenPtr>() ;

  for ( int i = 0 ; i < mergeBuf->size() ; i++ )
  {
    if ( !strcmp( mergeBuf->at( i )->type, "OPERATOR" ) ||
         !strcmp( mergeBuf->at( i )->name, "(" ) )
    {
      if ( !stk->empty() && !strcmp( stk->top()->type, "OPERATOR" ) &&
           strcmp( mergeBuf->at( i )->name, "(" ) &&
           GetOPPriority( stk->top()->name ) >= GetOPPriority( mergeBuf->at( i )->name ) )
      {
        postfixBuf->push_back( stk->top() ) ;
        stk->pop() ;
        stk->push( mergeBuf->at( i ) ) ;
      } // if

      else
        stk->push( mergeBuf->at( i ) ) ;
    } // if
    
    else if ( !strcmp( mergeBuf->at( i )->name, ")" ) )
    {
      // output until "("

      while ( !stk->empty() && strcmp( stk->top()->name, "(" ) )
      {
        postfixBuf->push_back( stk->top() ) ;
        stk->pop() ;
      } // while

      if ( !stk->empty() )
        stk->pop() ; // drop "("
    } // else if

    else // operand
      postfixBuf->push_back( mergeBuf->at( i ) ) ;
  } // for

  while ( !stk->empty() )
  {
    postfixBuf->push_back( stk->top() ) ;
    stk->pop() ;
  } // while
  
  delete mergeBuf ;
  delete stk ;


} // TransferToPostfix()

bool GetIdRecord( IdPtrVecPtr & idTable, Str100 target, IdPtr & idRecord )
{
  for ( int i = 0 ; i < idTable->size() ; i++ )
  {
    if ( !strcmp( idTable->at( i )->name, target ) )
    {
      idRecord = idTable->at( i ) ;
      return true ;
    } // if
  } // for

  return false ;
} // GetIdRecord()

int CalculatePostfixForInt( TokenPtrVecPtr & postfixBuf, IdPtrVecPtr & idTable, bool & err )
{
  if ( postfixBuf->size() == 1 )
  {
    if ( !strcmp( postfixBuf->at( 0 )->type, "ID" ) )
    {
      IdPtr idRecord = NULL ;

      if ( GetIdRecord( idTable, postfixBuf->at( 0 )->name, idRecord ) )
      {
        return idRecord->valueInt ;
      } // if

      else
      {
        cout << "> Undefined identifier : \'" <<  postfixBuf->at( 0 )->name << "\'" << endl ;
        err = true ;
        return -1 ;
      } // else
    } // if

  } // if ( only 1 operand )

  stack<TokenPtr> * stk = new stack<TokenPtr>() ;

  for ( int i = 0 ; i < postfixBuf->size() ; i++ )
  {
    if ( strcmp( postfixBuf->at( i )->type, "OPERATOR" ) )
      stk->push( postfixBuf->at( i ) ) ;
    else
    {
      int arg1 = 0, arg2 = 0 ;

      if ( !strcmp( stk->top()->type, "ID" ) )
      {
        IdPtr idRecord = NULL ;

        if ( GetIdRecord( idTable, stk->top()->name, idRecord ) )
        {
          arg2 = idRecord->valueInt ;
        } // if

        else
        {
          cout << "> Undefined identifier : \'" << stk->top()->name << "\'" << endl ;
          err = true ;
          return -1 ;
        } // else
      } // if
      else // is number
        arg2 = atoi( stk->top()->name ) ;

      TokenPtr arg1Ptr = NULL, arg2Ptr = NULL ;

      if ( !strcmp( stk->top()->type, "TMP_NUM" ) )
        arg1Ptr = stk->top() ;

      stk->pop() ;
      
      if ( !strcmp( stk->top()->type, "ID" ) )
      {
        IdPtr idRecord = NULL ;

        if ( GetIdRecord( idTable, stk->top()->name, idRecord ) )
        {
          arg1 = idRecord->valueInt ;
        } // if

        else
        {
          cout << "> Undefined identifier : \'" << stk->top()->name << "\'" << endl ;
          err = true ;
          return -1 ;
        } // else
      } // if
      else if ( !stk->empty() )// is number
        arg1 = atoi( stk->top()->name ) ;

      if ( !strcmp( stk->top()->type, "TMP_NUM" ) )
        arg2Ptr = stk->top() ;

      stk->pop() ;

      if ( arg1Ptr != NULL )
        delete arg1Ptr ;

      if ( arg2Ptr != NULL )
        delete arg2Ptr ;

      int result = 0 ;

      if ( !strcmp( postfixBuf->at( i )->name, "+" ) )
        result = arg1 + arg2 ;
      else if ( !strcmp( postfixBuf->at( i )->name, "-" ) ) 
        result = arg1 - arg2 ;
      else if ( !strcmp( postfixBuf->at( i )->name, "*" ) ) 
        result = arg1 * arg2 ;
      else
      {
        if ( arg2 == 0 )
        {
          cout << "> Error" << endl ;
          err = true ;
          return -1 ;
        } // if

        result = arg1 / arg2 ;
      } // else
      
      Str100 buf ;
      InitBuf100( buf ) ;
      sprintf( buf, "%d", result ) ;

      TokenPtr tmp = new Token ;
      strcpy( tmp->name, buf ) ;
      strcpy( tmp->type, "TMP_NUM" ) ;

      stk->push( tmp ) ;
      
    } // else

  } // for

  int finalResult = atoi( stk->top()->name ) ;
  TokenPtr tmp = stk->top() ;
  delete tmp ;
  delete stk ;
  stk = NULL ;

  return finalResult ;
} // CalculatePostfixForInt()

float CalculatePostfixForFloat( TokenPtrVecPtr & postfixBuf, IdPtrVecPtr & idTable, bool & err )
{
  if ( postfixBuf->size() == 1 )
  {
    if ( !strcmp( postfixBuf->at( 0 )->type, "ID" ) )
    {
      IdPtr idRecord = NULL ;

      if ( GetIdRecord( idTable, postfixBuf->at( 0 )->name, idRecord ) )
      {
        float num = idRecord->valueFloat ;
        cout << num << endl ;

        if ( num == 0.0 ) // origin is int
          num = ( float ) idRecord->valueInt ;
        
        return num ;
      } // if

      else
      {
        cout << "> Undefined identifier : \'" <<  postfixBuf->at( 0 )->name << "\'" << endl ;
        err = true ;
        return -1.0 ;
      } // else
    } // if

  } // if ( only 1 operand )

  stack<TokenPtr> * stk = new stack<TokenPtr>() ;

  for ( int i = 0 ; i < postfixBuf->size() ; i++ )
  {
    if ( strcmp( postfixBuf->at( i )->type, "OPERATOR" ) )
      stk->push( postfixBuf->at( i ) ) ;
    else
    {
      float arg1 = 0.0, arg2 = 0.0 ;

      if ( !strcmp( stk->top()->type, "ID" ) )
      {
        IdPtr idRecord = NULL ;

        if ( GetIdRecord( idTable, stk->top()->name, idRecord ) )
        {
          arg2 = idRecord->valueFloat ;

          if ( arg2 == 0.0 ) // origin is int
            arg2 = ( float ) idRecord->valueInt ;
        } // if

        else
        {
          cout << "> Undefined identifier : \'" << stk->top()->name << "\'" << endl ;
          err = true ;
          return -1.0 ;
        } // else
      } // if
      else // is number
        arg2 = atof( stk->top()->name ) ;

      TokenPtr arg1Ptr = NULL, arg2Ptr = NULL ;

      if ( !strcmp( stk->top()->type, "TMP_NUM" ) )
        arg1Ptr = stk->top() ;

      stk->pop() ;
      
      if ( !strcmp( stk->top()->type, "ID" ) )
      {
        IdPtr idRecord = NULL ;

        if ( GetIdRecord( idTable, stk->top()->name, idRecord ) )
        {
          arg1 = idRecord->valueFloat ;

          if ( arg1 == 0.0 ) // origin is int
            arg1 = ( float ) idRecord->valueInt ;
        } // if

        else
        {
          cout << "> Undefined identifier : \'" << stk->top()->name << "\'" << endl ;
          err = true ;
          return -1 ;
        } // else
      } // if
      else // is number
        arg1 = atof( stk->top()->name ) ;

      if ( !strcmp( stk->top()->type, "TMP_NUM" ) )
        arg2Ptr = stk->top() ;

      stk->pop() ;

      if ( arg1Ptr != NULL )
        delete arg1Ptr ;

      if ( arg2Ptr != NULL )
        delete arg2Ptr ;

      float result = 0.0 ;

      if ( !strcmp( postfixBuf->at( i )->name, "+" ) )
        result = arg1 + arg2 ;
      else if ( !strcmp( postfixBuf->at( i )->name, "-" ) ) 
        result = arg1 - arg2 ;
      else if ( !strcmp( postfixBuf->at( i )->name, "*" ) ) 
        result = arg1 * arg2 ;
      else
      {
        if ( arg2 == 0.0 )
        {
          err = true ;
          return -1.0 ;
        } // if

        result = arg1 / arg2 ;
      
      } // else

      Str100 buf ;
      InitBuf100( buf ) ;

      sprintf( buf, "%f", result ) ;

      TokenPtr tmp = new Token ;
      strcpy( tmp->name, buf ) ;
      strcpy( tmp->type, "TMP_NUM" ) ;

      stk->push( tmp ) ;
      
    } // else

  } // for

  float finalResult = atof( stk->top()->name ) ;
  TokenPtr tmp = stk->top() ;
  delete tmp ;
  delete stk ;
  stk = NULL ;

  return finalResult ;
} // CalculatePostfixForFloat()

float Rounding3( float num )
{
  bool isNegative = false ;

  if ( num < 0 )
  {
    isNegative = true ;
    num = -num ;
  } // if

  num = ( int ) ( num * 1000 + 0.5 ) / 1000.0 ;

  if ( isNegative )
    num = -num ;

  return num ;
} // Rounding3()

void DefineId( IdPtrVecPtr & idTable, IdPtr & idSource )
{
  for ( int i = 0 ; i < idTable->size() ; i++ )
  {
    if ( !strcmp( idTable->at( i )->name, idSource->name ) )
    {
      idTable->at( i ) = idSource ;
      return ;
    } // if
  } // for

  // not found
  idTable->push_back( idSource ) ;

} // DefineId()
