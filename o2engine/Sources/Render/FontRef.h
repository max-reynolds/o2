#pragma once

#include "Render/Font.h"

namespace o2
{
	// --------------
	// Font reference
	// --------------
	class FontRef
	{
	public:
		// Default constructor
		FontRef();

		// Constructor
		FontRef(Ptr<Font> font);

		// Constructor
		FontRef(const String& fileName);

		// Copy-constructor
		FontRef(const FontRef& other);

		// Destructor
		~FontRef();

		// Assign operator
		FontRef& operator=(const FontRef& other);

		// Font pointer operator
		Font* operator->();

		// Constant font pointer operator
		const Font* operator->() const;

		// Check equal operator
		bool operator==(const FontRef& other) const;

		// Check not equal operator
		bool operator!=(const FontRef& other) const;

		// Returns true if texture isn't null
		bool IsValid() const;

		// Returns true if texture isn't null
		operator bool() const;

	protected:
		Ptr<Font> mFont; // Font pointer

		friend class Render;
		friend class VectorFontAsset;
	};
}
