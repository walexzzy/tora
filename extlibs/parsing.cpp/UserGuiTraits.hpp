#ifndef	_T_GUI_TRAITS_H
#define	_T_GUI_TRAITS_H

// First thing we always do is include the ANTLR3 generated files, which
// will automatically include the antlr3 runtime header files.
#include <antlr3.hpp>
#include <algorithm>
#include <queue>
#include <boost/algorithm/string/predicate.hpp>

#include "TokenAttr.hpp"

namespace Antlr3Impl {
	class PLSQLGuiLexer;
	class MySQLGuiLexer;
	class OraclePLSQLLexer;
	class OraclePLSQLParser;
};

using namespace SQLLexer;

namespace Antlr3Impl {
//code for overriding
template<class ImplTraits>
class UserTraits : public antlr3::CustomTraitsBase<ImplTraits>
{
public:
	//for using the token stream which deleted the tokens, once it is reduced to a rule
	//but it leaves the start and stop tokens. So they can be accessed as usual
	//static const bool TOKENS_ACCESSED_FROM_OWNING_RULE = true;

	static void displayRecognitionError(const std::string& str)
	{
		antlr3::CustomTraitsBase<ImplTraits>::displayRecognitionError(str);
	}
	
	template<class StreamType>
	class RecognizerType : public antlr3::BaseRecognizer<ImplTraits, StreamType>
	{
	public:
		typedef antlr3::BaseRecognizer<ImplTraits, StreamType> BaseType;
		typedef typename antlr3::BaseRecognizer<ImplTraits, StreamType>::RecognizerSharedStateType RecognizerSharedStateType;
		RecognizerType(ANTLR_UINT32 sizeHint, RecognizerSharedStateType* state)
			: BaseType(sizeHint, state)
		{}

		ANTLR_INLINE void reportError()
		{
			BaseType::reportError();
		}

		// This recover method is probably only called from Parser
		// ANTLR_INLINE void recover()
		// {
		// 	BaseType::recover();
		// }
	};

	class BaseLexerType : public antlr3::Lexer<ImplTraits>
	{
	public:
		typedef antlr3::Lexer<ImplTraits> super;
		typedef typename antlr3::Lexer<ImplTraits>::RecognizerSharedStateType RecognizerSharedStateType;
		typedef typename antlr3::Lexer<ImplTraits>::InputStreamType InputStreamType;

		BaseLexerType(ANTLR_UINT32 sizeHint, RecognizerSharedStateType* state)
			: super(sizeHint, state)
		{}
		BaseLexerType(ANTLR_UINT32 sizeHint, InputStreamType* input, RecognizerSharedStateType* state)
			: super(sizeHint, input, state)
		{}
		
		ANTLR_INLINE void recover()
		{
			//super::recover();
			this->get_lexstate()->set_type(213);
			this->emit();
		}
	};
  
	class ToraToken : public antlr3::CommonToken<ImplTraits>
	{
		typedef antlr3::CommonToken<ImplTraits> super;
		typedef typename antlr3::CommonToken<ImplTraits>::TOKEN_TYPE TOKEN_TYPE;
	public:
		ToraToken() : m_block_context(BlkCtx::NONE), super() {};
		ToraToken( ANTLR_UINT32 type) : super(type), m_block_context(BlkCtx::NONE)  {};
		ToraToken( TOKEN_TYPE type) : super(type), m_block_context(BlkCtx::NONE)  {};
		ToraToken( const ToraToken& ctoken ) : super(ctoken), m_block_context(ctoken.m_block_context) {};

		ToraToken& operator=( const ToraToken& other ) { super::operator=(other); m_block_context = other.m_block_context; return *this; };

		void setBlockContext(BlkCtx::BlockContextEnum bc) { m_block_context = bc; }
		BlkCtx::BlockContextEnum getBlockContext() const { return m_block_context; }

	private:
		BlkCtx::BlockContextEnum m_block_context;
	};

	typedef ToraToken CommonTokenType;

	//Similarly, if you want to override the nextToken function. write a class that
	//derives from antlr3::TokenSource and override the nextToken function. But name the class
	//as TokenSourceType
	class TokenSourceType : public antlr3::TokenSource<ImplTraits>
	{
		typedef typename antlr3::TokenSource<ImplTraits> super;
		typedef typename antlr3::TokenSource<ImplTraits>::TokenType TokenType;
	public:
		ANTLR_INLINE TokenType*  nextToken()
		{
			if( tokenBuffer.empty())
				return super::nextToken();
			TokenType* retval = tokenBuffer.front();
			tokenBuffer.pop();
			usedTokens.push(retval);
			return retval;
		}

		ANTLR_INLINE void recover()
		{
			super::recover();
		}

		ANTLR_INLINE void enqueueToken(TokenType *t)
		{
			tokenBuffer.push(t);
		}

		ANTLR_INLINE void enqueueToken(std::auto_ptr< TokenType> &t)
		{
			tokenBuffer.push(t.release());
		}

		~TokenSourceType()
		{
			while(!usedTokens.empty())
			{
				TokenType* token = usedTokens.front();
				delete token;
				usedTokens.pop();
			}
		}
	private:
		// buffer (queue) to hold the emit()'d tokens
		std::queue<TokenType*> tokenBuffer;
		std::queue<TokenType*> usedTokens;
	};	
};

class EmptyParser {};

typedef antlr3::Traits<PLSQLGuiLexer, EmptyParser, UserTraits>               PLSQLGuiLexerTraits;
typedef antlr3::Traits<MySQLGuiLexer, EmptyParser, UserTraits>               MySQLGuiLexerTraits;

};

#endif