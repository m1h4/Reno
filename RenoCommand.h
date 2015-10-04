#pragma once

typedef BOOL (CCmdTarget::*RENO_COMMAND_FUNCTION)(const CStringPtrArray& commandElements);
typedef BOOL (CCmdTarget::*RENO_COMMAND_PARAMETER_FUNCTION)(const CStringPtrArray& commandElements,CStringPtrArray& parameterElements);

typedef struct
{
	LPCTSTR name;
	LPCTSTR description;
	RENO_COMMAND_FUNCTION function;
	RENO_COMMAND_PARAMETER_FUNCTION parameters;
} RENO_COMMAND;

#define DECLARE_COMMAND(name) \
	BOOL name(const CStringPtrArray& commandElements)

#define DECLARE_PARAMETER(name) \
	BOOL name(const CStringPtrArray& commandElements,CStringPtrArray& parameterElements)

#define DECLARE_COMMAND_MAP() \
	public: \
	static const RENO_COMMAND* PASCAL GetCommandMap();

#define BEGIN_COMMAND_MAP(theClass) \
	const RENO_COMMAND* PASCAL theClass::GetCommandMap() \
	{ \
		static const RENO_COMMAND commandEntries[] =  \
		{

#define END_COMMAND_MAP() \
		{0, 0, 0, 0} \
	}; \
		return commandEntries; \
	}

#define COMMAND(name, description, function, paramters) \
	{ name, description, static_cast<RENO_COMMAND_FUNCTION>(function), static_cast<RENO_COMMAND_PARAMETER_FUNCTION>(paramters) },

typedef BOOL (CCmdTarget::*RENO_REPLY_HANDLER)(const CString& prefix,const CString& command,const CStringPtrArray& parameters);

typedef struct
{
	LPCTSTR command;
	RENO_REPLY_HANDLER handler;
} RENO_REPLY;

#define DECLARE_REPLY(name) \
	BOOL name(const CString& prefix,const CString& command,const CStringPtrArray& parameters)

#define DECLARE_REPLY_MAP() \
	public: \
	static const RENO_REPLY* PASCAL GetReplyMap();

#define BEGIN_REPLY_MAP(theClass) \
	const RENO_REPLY* PASCAL theClass::GetReplyMap() \
	{ \
		static const RENO_REPLY replyEntries[] =  \
		{

#define END_REPLY_MAP() \
		{0, 0} \
	}; \
		return replyEntries; \
	}

#define REPLY(command, handler) \
	{ command, static_cast<RENO_REPLY_HANDLER>(handler) },