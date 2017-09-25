#ifndef ORIENTAL_MOTOR_EXCEPTIONS
#define ORIENTAL_MOTOR_EXCEPTIONS

#include <stdexcept>
#include <exception>


class MMErrorCodeException : public std::runtime_error {

	public: 

		MMErrorCodeException( int errCode, std::string msg ) : runtime_error( "Runtime Error" ), errCode_( errCode ), msg_(msg) {}

		virtual const char* what() const throw()
		{

			return msg_.c_str();
		}

		virtual int getErrCode() const throw() 
		{
			return errCode_;
		}

	private:

		int errCode_;
		std::string msg_;


};

class CommunicationException : public std::runtime_error {

	public:

	CommunicationException( ) : runtime_error( "Error Communicating with " ) {}

	virtual const char* what() const throw()
	{

		return "Thrown";

	}

};

class ValueReadException : public std::runtime_error {

	public:
		ValueReadException() : runtime_error( "The Value was Read Inappropriately to be " ) {}

		virtual const char* what() const throw()
		{

			return "Thrown";

		}

};


#endif