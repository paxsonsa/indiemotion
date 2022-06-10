#pragma once

namespace indiemotion
{
    enum class Error {
        BadMessage = 0
    };

    /**
	 * An Exception Type that contains information for transport to and from client.
     */
    struct Exception : public std::exception
    {
        /// The type of error that this exception represents
        std::string type;
        /// A user-friendly description that describe the error.
        std::string description;
        /// Mark this Exception as Fatal meaning that the session will shut down immediately.
        bool is_fatal = false;

        /**
		 * Construct an Exception from the type and description
		 * @param type A string representing the error type.
		 * @param message A user friendly description describing the description.
         */
        Exception(const std::string &type, const std::string &message, bool fatal = false) noexcept
            : type(type), description(message), is_fatal(fatal)
        {
            _m_full_message = type;
            _m_full_message += ": " + message;
        }

        /**
		 * Returns exception string
		 * @return a string which is a combination of the type and description.
         */
         [[ nodiscard ]]
        const char* what() const noexcept override
        {
            return _m_full_message.c_str();
        }

      private:
        std::string _m_full_message;
    };
}