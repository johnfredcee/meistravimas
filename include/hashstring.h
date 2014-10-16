#ifndef HASHSTRING_H
#define HASHSTRING_H


namespace venk
{
    /** Reduce a string to a 32 bit hash value, keeping track of it in debug. */
    class HashString
    {
    private:
	/** The crc value */
	Uint32 crc;
	/** The seed for the hash algorithm */
	static Uint32 hval;
#ifndef NDEBUG
	/** The original string, preserved in debug. */
	std::string originalString;
#endif
	/**
	 * Compute the hash.
	 * @param  str String to compute hash of
	 * @return Computed hash
	 */
	Uint32 Hash(const char *str);
	HashString();
public:
	/**
	 * Construct a hash value from a c-style string
	 * @param  str - string to hash
	 */
	HashString(const char *str);
	/**
	 * Construct a string from a C++ style string.
	 * @param str String to compute hash of
	 */
	HashString(const std::string& str);
	/**
	 * Function to return hash value
	 * @return hash value of string
	 */
	Uint32 getValue();
	/**
	 * Type conversion operator - we want to use this as a 
	 * proxy for the hash value
	 */
	operator Uint32();
#ifndef NDEBUG
	/**
	 * Return the value of the original string for debugging
	 * @return the original string.
	 */
	std::string getString();
#endif	
    /** Non virtual destructor. This should only ever be a base. */
	~HashString();
};

}

#endif
