#pragma once

#include "Utils/Types/Containers/Vector.h"

#include "Utils/Types/String.h"

namespace o2
{
	// ---------------------------------------------------------------------------------
	// Basic log stream. Contains interfaces of outing data, parent and children streams
	// ---------------------------------------------------------------------------------
	class LogStream
	{
	protected:
		typedef Vector<LogStream*> LogSteamsVec;

		LogStream*   mParentStream; // Parent stream. NULL if no parent
		WString      mId;           // Name of log stream
		LogSteamsVec mChildStreams; // Child streams

	public:
		// Default constructor
		LogStream();

		// Constructor with id
		LogStream(const WString& id);

		// Destructor
		virtual ~LogStream();

		// Returns name of stream
		const WString& GetId() const;

		// Binds child stream
		void BindStream(LogStream* stream);

		// Unbinds child stream
		void UnbindStream(LogStream* stream);

		// Unbinds and destroy child stream
		void UnbindAndReleaseStream(LogStream* stream);

		// Unbinds and destroy all child streams
		void UnbindAllStreams();

		// Returns parent stream. Null if no parent
		LogStream* GetParentStream() const;

		// Outs with low level log
		void Out(WString format, ...);

		// Outs error message
		void Error(WString format, ...);

		// Outs warning message
		void Warning(WString format, ...);

		// Outs string to current stream and parent stream
		void OutStr(const WString& str);

		// Outs error to current stream and parent stream
		void ErrorStr(const WString& str);

		// Outs warning to current stream and parent stream
		void WarningStr(const WString& str);

	protected:
		// Outs string to stream
		virtual void OutStrEx(const WString& str) {}

		// Outs error to stream
		virtual void OutErrorEx(const WString& str);

		// Outs warning to stream
		virtual void OutWarningEx(const WString& str);
	};
}
