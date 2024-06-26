# ifndef NETXPTO_H_ 
# define NETXPTO_H_ 

// ##################### Compatibility Issues #########################################################
// # 
// # Requires C++17, because of the type std::byte
// #
// ####################################################################################################

// ####################################################################################################
// #
// # STL header files
// #f
// ####################################################################################################

#ifdef _WIN32
# include <ctime>		
#endif

#ifdef __linux__
#define __STDC_WANT_LIB_EXT1__ 1 //for ip_tunnel_linux
# include <time.h>
#endif

# include <algorithm>	// copy(), find(), sort(), std::min()
# include <array>		// array	
# include <bitset>		//
# include <chrono>      // duration, time_point
# include <cmath>       // sqrt(), pow(), exp()
# include <complex>		// complex, sqrt(), pow()		
# include <cctype>      //
# include <fstream>		// fstream, ifstream, ofstream
# include <functional>	//
# include <filesystem>  //
//# include <experimenal/filesystem>  //
# include <iostream>    // cout, cin
# include <iomanip>		// setw
# include <locale>		//
# include <map>         // map, multimap
# include <random>      // default_random_engine, normal_distribution
# include <sstream>     // istream, ostream
# include <vector>      // vector
# include <string>      // string, basic_string
# include <tuple>		//


// ####################################################################################################
// #
// # Eigen header files
// #
// ###################################################################################################

#ifdef _WIN32
# include <Eigen/Sparse>
#endif 

#ifdef __linux__
# include "../eigen/Eigen/Sparse"
#endif

// ####################################################################################################
// #
// # Global constants
// #
// ####################################################################################################

const int MAX_NAME_SIZE = 256;						// Maximum size of names (file names,..)
const int MAX_BUFFER_LENGTH = 10000;				// Maximum signal buffer length
const int DEFAULT_BUFFER_LENGTH = 512;				// Default buffer length
const int MAX_TOPOLOGY_SIZE = 100;					// Maximum number of blocks (topology) 
const int MAX_TAPS = 1000;							// Maximum taps number
const double PI = 3.1415926535897932384;			// Value of pi
const double SPEED_OF_LIGHT = 299792458;			// Speed of light in vaccum (m/s)
const double ELECTRON_CHARGE = 1.60217662E-19;
const double BOLTZMANN_CONSTANT = 1.380649E-23;
const double PLANCK_CONSTANT = 6.626070040e-34;		// Plack constant (NIST) (m^2 kg / s)  
const int MAX_NUMBER_OF_PATHS = 2;

// ####################################################################################################
// #
// # NetXpto value types
// #
// ####################################################################################################

using t_void = void;

using t_bool = bool;

using t_raw = std::byte;

using t_char = char;

using t_unsigned_long = unsigned long int;

using t_unsigned = unsigned int;

using t_integer = int;

using t_binary = uint8_t;				//int;

using t_real = double;

using t_complex = std::complex<t_real>;

using t_index = size_t;

struct t_complex_xy 
{ 
	t_complex_xy() : x{ t_complex(0,0) }, y{ t_complex(0,0) }{};
	t_complex_xy(t_complex x_val, t_complex y_val) : x{ x_val }, y{ y_val }{};

	t_complex x;
	t_complex y;
};

struct t_complex_xy_mp 
{ 
	t_complex x;
	t_complex y;
	bool processed{ false };
};

using t_photon = t_complex;

using t_photon_xy = t_complex_xy;

using t_photon_mp = struct { t_complex_xy path[MAX_NUMBER_OF_PATHS]; };

using t_photon_mp_xy = struct { t_complex_xy_mp path[MAX_NUMBER_OF_PATHS]; };

using t_seed = unsigned;

using t_string = std::string;

// ################################## t_message type definition ###########################################

// ####################################################################################################################################################################################
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// | messageType (MESSAGE_TYPE_SIZE bytes) | messageDataLength (MESSAGE_DATA_LENGTH_SIZE bytes) | messageData (up to MAX_MESSAGE_SIZE-(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE)) |
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ####################################################################################################################################################################################

const size_t MESSAGE_TYPE_SIZE = 35;
const size_t MESSAGE_DATA_LENGTH_SIZE = 6;
const size_t MAX_MESSAGE_SIZE = 5000;

bool isNumber(const std::string& s);

size_t strToNumber(const std::string& s);

class t_message
{
public:

	t_message()
	{
		messageRaw.resize(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE);
	};

	void setMessageType(std::string mType)
	{
		mType.resize(MESSAGE_TYPE_SIZE,' ');
		messageRaw.replace(0, MESSAGE_TYPE_SIZE, mType);
	};

	std::string getMessageType() const
	{
		t_string str = messageRaw.substr(0, MESSAGE_TYPE_SIZE);
		return str.substr(0, str.find(' '));
	}

	void setMessageDataLength(std::string mDataLength)
	{
		if (isNumber(mDataLength) && (strToNumber(mDataLength)<= MAX_MESSAGE_SIZE - MESSAGE_TYPE_SIZE - MESSAGE_DATA_LENGTH_SIZE))
		{
			std::string aux;
			aux.resize(MESSAGE_DATA_LENGTH_SIZE,'0');
			aux.replace(MESSAGE_DATA_LENGTH_SIZE - mDataLength.size(), mDataLength.size(), mDataLength);
			messageRaw.replace(MESSAGE_TYPE_SIZE, MESSAGE_DATA_LENGTH_SIZE, aux); 
		}
		else
		{
			std::cerr << "Message Data Length must be a positive integer, smaller than " << MAX_MESSAGE_SIZE - MESSAGE_TYPE_SIZE - MESSAGE_DATA_LENGTH_SIZE << "std::endl";
		}
	};

	std::string getMessageDataLength() const
	{
		return messageRaw.substr(MESSAGE_TYPE_SIZE, MESSAGE_DATA_LENGTH_SIZE);
	}

	void setMessageData(std::string mData)
	{
		if (mData.size() > strToNumber(getMessageDataLength()))
			mData.resize(strToNumber(getMessageDataLength()));
		messageRaw.resize(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE);
		messageRaw.append(mData);
	};

	std::string getMessageData() const
	{
		return messageRaw.substr(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE);
	}

	size_t size()
	{
		return messageRaw.size();
	}

	size_t maxSize() { return MAX_MESSAGE_SIZE; };

	t_integer getMaxDataSize() const { return (MAX_MESSAGE_SIZE - (MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE)); };

	char& operator[](int index) { return messageRaw[index]; }

	friend std::ostream& operator<<(std::ostream& out, const t_message& ms)
	{
	out << ms.getMessageType() << "||" << ms.getMessageDataLength() << "||" << ms.getMessageData() << std::endl;
	return out;
	}
	
	t_string messageRaw;

};

// ####################################################################################################
// #
// # Message Handler Messages
// #
// ####################################################################################################

/* Header sizes, in Bytes */
const size_t DST_BLOCK_SIZE = 32;
const size_t SRC_BLOCK_SIZE = 32;
const size_t PRIORITY_SIZE = 1;
const size_t ID_SIZE = 4;
const size_t MSG_TYPE_SIZE = 35;
const size_t SRC_PORT_SIZE = 1;
const size_t MSG_DATA_LENGTH_SIZE = 6;
/* Data size, in Bytes*/
const size_t MAX_MSG_SIZE = 5000;

/*!
    This class is used to represent a Message Object. For that, it takes 
    advantage of the macros defined above
*/
class t_handler_message {
   public:
    //! Create and Allocate Max Size Message
    t_handler_message() { rawMessage.resize(HEADER_SIZE + MAX_MSG_SIZE, '\0'); }

    //! Instantiate already defined messages
    t_handler_message(t_string completeMessage) {
        rawMessage = completeMessage;
    }

    /* ##############################################################
     * ###################### Destination Block #####################
     * ############################################################## */

    void setDestinationBlock(t_string destinationBlock);

    t_string getDestinationBlock() const;

    /* ##############################################################
     * ######################## Source Block ########################
     * ############################################################## */

    void setSourceBlock(t_string sourceBlock);

    t_string getSourceBlock() const;

    /* ##############################################################
     * ######################## Priority ############################
     * ############################################################## */

    void setPriority(uint8_t priority);

    /*
     * Note that this function can launch an exception on converting from string to integer
     */
    uint8_t getPriority() const;

    /* ##############################################################
     * ##################### Sequential Id ##########################
     * ############################################################## */

    void setSequentialId(uint32_t Id);

    //! Note that this function can launch an exception on converting from string to integer
    uint32_t getSequentialId() const;

    /* ##############################################################
     * ######################## Message Type ########################
     * ############################################################## */

    void setMessageType(t_string mType);

    t_string getMessageType() const;

    /* ##############################################################
     * ###################### Source Port ###########################
     * ############################################################## */

    void setSourcePort(uint8_t sourcePort);

    //! Note that this function can launch an exception on converting from string to integer
    uint8_t getSourcePort() const;

    /* ##############################################################
     * ##################### Message Data Length ####################
     * ############################################################## */

    void setMessageDataLength(t_string mDataLength);

    t_string getMessageDataLength() const ;

    /* ##############################################################
     * ######################### Message Data #######################
     * ############################################################## */

    void setMessageData(t_string mData);

    t_string getMessageData() const;

    /* ##############################################################
     * ############################ Others ##########################
     * ############################################################## */

    size_t size() { return rawMessage.size(); }

    t_integer getMaxDataSize() const { return (MAX_MSG_SIZE - HEADER_SIZE); };

    void setHeader(t_string destinationBlock, t_string sourceBlock,
                   uint8_t priority, uint32_t Id, t_string mType,
                   uint8_t sourcePort, t_string mDataLength);

    t_string getRawMessage() const { return rawMessage; }
	void setRawMessage(t_string newMessage) { rawMessage.resize(HEADER_SIZE + MAX_MSG_SIZE, '\0'); rawMessage = newMessage; }

    char& operator[](int index) { return rawMessage[index]; }

    bool operator<(const t_handler_message& other) const;

    //!This function is used to asses if this and other class have equal values
    bool operator==(const t_handler_message& other) const;

    /*!
        This function is used to copy the contents of the other
        object to this class.
    */
    t_handler_message& operator=(const t_handler_message& other);

    //!Function used to print the contents of a message
    friend std::ostream& operator<<(std::ostream& out,
                                    const t_handler_message& ms) {
        out << ms.getDestinationBlock() << "||" << ms.getSourceBlock() << "||"
            << unsigned(ms.getPriority()) << "||" << ms.getSequentialId() << "||"
            << ms.getMessageType() << "||" << unsigned(ms.getSourcePort()) << "||"
            << ms.getMessageDataLength() << "||" << ms.getMessageData()
            << std::endl;
        return out;
    }

   private:
    static const uint32_t HEADER_SIZE =
        DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE + ID_SIZE +
        MSG_TYPE_SIZE + SRC_PORT_SIZE + MSG_DATA_LENGTH_SIZE;
    t_string rawMessage;
};


// #######################################################################################################
// #
// # Operator << overloading, for complex<T>
// #
// #######################################################################################################

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::complex<T>& cx) 
{ 
	out << cx.real() << " + i " << cx.imag();
	return out;
}



// ####################################################################################################
// #
// # NetXpto enumerate class types
// #
// ####################################################################################################

enum class value_type
{
	t_void,
	t_char,
	t_unsigned_long_int,
	t_int,
	t_double,
	t_string,
	t_bool
};

enum class signal_value_type {
	t_raw,
	t_char,
	t_binary,
	t_integer,
	t_real,
	t_complex,
	t_complex_xy,
	t_photon,
	t_photon_mp,
	t_photon_mp_xy,
	t_message,
	t_handler_message
};

enum class signal_type {
	Raw,									// this is just a sequence of bytes
	TimeDiscreteAmplitudeDiscreteInteger,
	TimeContinuousAmplitudeDiscreteInteger,
	TimeDiscreteAmplitudeDiscreteReal,
	TimeDiscreteAmplitudeContinuousReal,
	TimeContinuousAmplitudeContinuousReal,
	TimeContinuousAmplitudeDiscreteReal,
	TimeDiscreteAmplitudeContinuousComplex,
	TimeContinuousAmplitudeContinuousComplex,
	Binary,									// this is a uint8 type_value therefore can hold integers values from 0 to 255
	OpticalSignal,
	OpticalSignalXY,
	PhotonStreamXY,
	PhotonStreamMP,
	PhotonStreamMPXY,
	Message,
	HandlerMessage
};

//########################################################################################################################################################
//############################################################## CIRCULAR BUFFER #####################################################################
//########################################################################################################################################################

template<class type>
class CircularBuffer
{

public:

	explicit CircularBuffer() {}
	explicit CircularBuffer(t_index s) { data.resize(s); }

	void resize(size_t s) { posIn = 0; posOut = 0, empty = true; data.resize(s); } // resize clears the circular buffer
	int size() { return (int) data.size(); }

	int space() const
	{
		if (empty)
			return (int) data.size();
		if (posOut >= posIn)
			return posOut - posIn;
		else
			return (int) data.size() - posIn + posOut;
	}

	t_integer space(t_integer dataLength) 
	{
		if (space() < dataLength && !ready()) 
			resize(dataLength);
		return space();
	}
	

	int ready() const
	{
		if (empty)
			return 0;
		if (posIn > posOut)
			return posIn - posOut;
		else
			return ((int) data.size() - posOut + posIn);
	}
	
	void bufferPut(type v) { data[posIn] = v; posIn = (posIn+1) % (size()); empty = false;}
	type bufferGet() { auto d = data[posOut];  posOut = (posOut+1) % (size()); if (posOut == posIn) empty = true;  return d; }
	type bufferRead() const { auto d = data[posOut]; return d; }

	type operator[](size_t ind) { return data[ind]; };

private:

	std::vector<type> data;

	t_bool empty{ true };

	int posIn{ 0 };
	int posOut{ 0 };

};


//########################################################################################################################################################
//############################################################## SIGNALS DECLARATION #####################################################################
//########################################################################################################################################################

//########################################################## SIGNALS CLASS HIERARCHY #####################################################################
// The hierarchy of the signals are the following: the root is the class SIGNAL class,
// which includes all comum data and functions,
// the second level is a template where value type is defined.
// From this template a list of generic signals are defined using an alias.
// Specific signals are derived from the generic signals.
// All signals have associated a circular buffer, which is used as a temporary storage,
// to hold signal samples waiting for processing,
// and a file for permanente storage (the file is optional).
//#########################################################################################################################################################

// class Signal, this is an abstract class

class Signal 
{

public:

	enum class t_header_type {noHeader, fullHeader};
	enum class t_write_mode { Binary, Ascii };

	// Signals constructors
	explicit Signal() {};
	explicit Signal(std::string fileName) : fileName{ fileName }, saveSignal{ true } {};
	explicit Signal(std::string fileName, t_unsigned_long bLength) : fileName{ fileName }, bufferLength{ bLength }, saveSignal{ true } {};
	explicit Signal(std::string fileName, t_unsigned_long bLength, t_header_type hType, Signal::t_write_mode sWriteMode) : fileName{ fileName }, bufferLength{ bLength }, saveSignal{ true }, headerType{ hType }, signalWriteMode{ sWriteMode } {};
	explicit Signal(std::string fileName, t_unsigned_long bLength, Signal::t_write_mode sWriteMode) : fileName{ fileName }, bufferLength{ bLength }, saveSignal{ true }, signalWriteMode{ sWriteMode } {};
	explicit Signal(std::string fileName, std::string folderName) : fileName{ fileName }, folderName{ folderName }, saveSignal{ true } {};
	explicit Signal(std::string fileName, std::string folderName, t_unsigned_long bLength) : fileName{ fileName }, folderName{ folderName }, bufferLength{ bLength }, saveSignal{ true } {};
	explicit Signal(t_unsigned_long bLength) : bufferLength{ bLength } {};

	// Signal destructors
	virtual ~Signal() { if ( buffer != nullptr ) { delete(buffer); } };

	// Buffer manipulation funtions
	t_integer ready() const;										// Returns the number of samples in the circular buffer ready to be processed
	t_integer space() const { return (bufferLength - ready()); };   // Returns the signal circular buffer space 

	template<typename T>void bufferPut(T value) {
		
		(static_cast<T*>(buffer))[inPosition] = value;
		


		if (bufferFull)
			outPosition = (outPosition + 1) % bufferLength;

		inPosition = (inPosition + 1) % bufferLength;

		bufferEmpty = false;
		bufferFull = inPosition == outPosition;

		if (inPosition == 0)
		{
			if (saveSignal)
			{
				if (!headerWritten) writeHeader();

				if (firstValueToBeSaved < bufferLength)
				{
					char* ptr = (char*)buffer;
					ptr = ptr + firstValueToBeSaved * sizeof(T);
					if (signalWriteMode == Signal::t_write_mode::Binary)
					{
						std::ofstream fileHandler{ "./" + folderName + "/" + fileName, std::ios::out | std::ios::binary | std::ios::app };
						fileHandler.write(ptr, (bufferLength - firstValueToBeSaved) * sizeof(T));
						fileHandler.close();
					}
					else
					{
						std::ofstream fileHandler{ "./" + folderName + "/" + fileName, std::ios::out | std::ios::app };
						for (t_unsigned k = 0; k < (bufferLength - firstValueToBeSaved); k++)
						{
							auto vType = getValueType();
							switch (vType)
							{
							case signal_value_type::t_real:
								fileHandler << (static_cast<t_real*>(buffer))[k] << std::endl;
								break;
							case signal_value_type::t_complex:
								fileHandler << (static_cast<t_complex*>(buffer))[k];
								break;
							case signal_value_type::t_binary:
								fileHandler << std::to_string((static_cast<t_binary*>(buffer))[k]);
								break;
							case signal_value_type::t_integer:
								fileHandler << (static_cast<t_integer*>(buffer))[k];
								break;
							case signal_value_type::t_message:
								fileHandler << (static_cast<t_message*>(buffer))[k];
								break;
							default:
								fileHandler << (static_cast<t_char*>(buffer))[k];
							}
							if ((k + 1) % 180 == 0)	fileHandler << std::endl;
						}
						fileHandler << std::endl;
						fileHandler.close();
					}
					firstValueToBeSaved = 0;
				}
				else
				{
					firstValueToBeSaved = firstValueToBeSaved - bufferLength;
				}
			}
		}
	};		    // Puts a value in the buffer (at the end)

	void bufferPut(t_integer value);						// Puts an integer in the buffer (at the end), so some reason with int the template doesn't work
	void bufferPut(t_handler_message& value);						// Puts a message handler style message in the buffer (at the end)
	void bufferPut(t_message& value);						// Puts a message in the buffer (at the end)
	void bufferPut(t_message* value);						// Puts a message in the buffer (at the end)
	void bufferPut(std::byte* pValue);						// Puts a value in the buffer (at the end), the input parameter is a pointer for the first byte	
	void bufferPut(std::byte* pValue, size_t n){

		auto vTypeSizeOf = this->valueTypeSizeOf();
		for (size_t m = 0; m < n; m++)
			this->bufferPut((std::byte*)&(pValue[m * vTypeSizeOf]));
	};			// Puts n values in the buffer (at the end), the input parameter is a pointer for the first byte

	template<typename T> void bufferGet(T* valueAddr) {

		if (bufferEmpty) {
			*valueAddr = T();
			//std::cerr << "Silly point: bufferGet template!" << std::endl;
		}

		*valueAddr = static_cast<T*>(buffer)[outPosition];

		bufferFull = false;

		outPosition = (outPosition + 1) % bufferLength;

		bufferEmpty = outPosition == inPosition;

		return;
	};		// Returns a value from the buffer (from the begining)	
	void bufferGet(t_integer* valueAddr);
	void bufferGet(t_message* valueAddr);	
	void bufferGet(t_handler_message* valueAddr);				
	void bufferGet(std::byte* valueAddr, size_t n);			// Returns n values from the buffer (from the begining), works for any type	
	void bufferGet();										// Remove a value from the buffer (skips a value)

	template<typename T> void bufferRead(T* valueAddr) {

		if (bufferEmpty) {
			*valueAddr = T();
			//std::cerr << "Silly point: bufferGet template!" << std::endl;
		}

		*valueAddr = static_cast<T*>(buffer)[outPosition];

		return;
	};		// Reads a value from the buffer (from the begining) without removing it
	void bufferRead(t_integer* valueAddr);					// not that is only possible to read the first value, buffetGet(&value) = buffer(&value) + bufferGet()
	void bufferRead(t_message* valueAddr);



	// File manipulation
	void writeHeader();										// Opens the signal file in the default signals directory, \signals, and writes the signal header
	void writeHeader(std::string signalPath);				// Opens the signal file in the signalPath directory, and writes the signal header

	// Buffer and file manipulation
	void close();											// Close the signal, write the remaining samples in the file

	void reset() { inPosition = 0; outPosition = inPosition; bufferEmpty = true; bufferFull = false; }

	// Change signal parameters
	void setSaveSignal(bool sSignal) { saveSignal = sSignal; };

	void setSaveSignal(bool sSignal, Signal::t_write_mode sWriteMode) { saveSignal = sSignal; signalWriteMode = sWriteMode; };

	bool getSaveSignal() const { return saveSignal; };
	


	void setType(signal_type sType, signal_value_type vType);// { type = sType; valueType = vType; };
	//void setType(std::string sType) { type = sType; };
	std::string getType() const { return type; };

	/* This functions are used in the sampler block, are they really needed? */
	void setInPosition(int iPosition) { inPosition = iPosition; };
	int getInPosition() const { return inPosition; };

	void setOutPosition(int oPosition) { outPosition = oPosition; };
	int getOutPosition() const { return outPosition; };

	void setBufferEmpty(bool bEmpty) { bufferEmpty = bEmpty; };
	bool getBufferEmpty() const { return bufferEmpty; };
	
	void setBufferFull(bool bFull) { bufferFull = bFull; };
	bool getBufferFull() const { return bufferFull; };


	//void setValueType(signal_value_type vType) { valueType = vType; };
	signal_value_type getValueType() const { return valueType; };

	void setFileName(std::string fName) { fileName = fName; };
	std::string getFileName() const { return fileName; };

	void setSignalFolderName(std::string fName) { folderName = fName; };
	std::string getSignalFolderName() const { return folderName; };

	//void setBufferLength(t_unsigned_long bLength) { bufferLength = bLength; };
	int getBufferLength() const { return bufferLength; };

	void setFirstValueToBeSaved(t_unsigned_long fValueToBeSaved) { firstValueToBeSaved = fValueToBeSaved; };
	t_unsigned_long getFirstValueToBeSaved() const { return firstValueToBeSaved; };

	void setAddToFirstValueToBeSaved(t_unsigned_long aToFirstValueToBeSaved) { firstValueToBeSaved = getFirstValueToBeSaved()+ aToFirstValueToBeSaved; };
		
	void setNumberOfValuesToBeSaved(t_unsigned_long nOfValuesToBeSaved) { numberOfValuesToBeSaved = nOfValuesToBeSaved; };
	t_unsigned_long getNumberOfValuesToBeSaved() const { return numberOfValuesToBeSaved; };

	void setNumberOfValuesToBeSkipped(t_unsigned_long nOfValuesToBeSkipped) { numberOfValuesToBeSkipped = nOfValuesToBeSkipped; };
	void setAddToNumberOfValuesToBeSkipped(t_unsigned_long aToNumberOfValuesToBeSkipped) { numberOfValuesToBeSkipped = numberOfValuesToBeSkipped + aToNumberOfValuesToBeSkipped; };
	t_unsigned_long getNumberOfValuesToBeSkipped() const { return numberOfValuesToBeSkipped; };

	void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; samplesPerSymbol = symbolPeriod / samplingPeriod; };
	double getSymbolPeriod() const { return symbolPeriod; };

	void setSamplingPeriod(double sPeriod) { samplingPeriod = sPeriod; samplesPerSymbol = symbolPeriod / samplingPeriod; };
	double getSamplingPeriod() const { return samplingPeriod; };

	void setSamplesPerSymbol(double sPeriod) { samplesPerSymbol = sPeriod; symbolPeriod = samplesPerSymbol * samplingPeriod; samplingPeriod = symbolPeriod / samplesPerSymbol; };
	double getSamplesPerSymbol() const { return samplesPerSymbol; };

	void setNoHeader(bool nHeader) { headerType=t_header_type::noHeader; };

	virtual std::size_t valueTypeSizeOf()=0;
	template<typename t, signal_type sType, signal_value_type vType> friend class BaseSignal;

	void setCentralFrequency_Hz(double cFrequency_Hz) { centralFrequency_Hz = cFrequency_Hz; centralWavelength_m = SPEED_OF_LIGHT / centralFrequency_Hz; }
	double getCentralFrequency_Hz() const { return centralFrequency_Hz; };

	void setCentralWavelength_m(double cWavelength_m) { centralWavelength_m = cWavelength_m; centralFrequency_Hz = SPEED_OF_LIGHT / centralWavelength_m; }
	double getCentralWavelength_m() const { return centralWavelength_m; }

private:


	/* Circular buffer state variables */
	void* buffer{ nullptr };									// Pointer to buffer
	t_unsigned inPosition{ 0 };									// Next position for the buffer input values
	t_unsigned outPosition{ 0 };								// Next position for the buffer output values
	bool bufferEmpty{ true };									// Flag bufferEmpty
	bool bufferFull{ false };									// Flag bufferFull
	const t_unsigned bufferLength{ DEFAULT_BUFFER_LENGTH };		// Buffer length

	t_unsigned_long count{ 0 };									// Number of values that have already entered in the buffer

	bool saveSignal{ false };
	long int numberOfValuesToBeSaved{ -1 };						// Number of values to be saved, if -1 all values are going to be saved
	t_unsigned_long firstValueToBeSaved{ 0 };					// First value (>= 1) to be saved
	t_unsigned_long numberOfSavedValues{ 0 };					// Number of saved values
	t_unsigned_long numberOfValuesToBeSkipped{ 0 };				// This is only to be used after the simulation by the vizualization tool

	Signal::t_write_mode signalWriteMode{ Signal::t_write_mode::Binary };

	std::string type;											// Signal type

	signal_type signalType{ signal_type::Raw };
	signal_value_type valueType{ signal_value_type::t_raw };								// Signal samples type

	std::string fileName{ "" };									// Name of the file where data values are going to be saved
	std::string folderName{ "signals" };						// folder where signals are going to be saved by default
	bool headerWritten{ false };
	t_header_type headerType{ t_header_type::fullHeader };


	double symbolPeriod{ 1.0 };									// Signal symbol period (it is the inverse of the symbol rate)
	double samplingPeriod{ 1.0 };								// Signal sampling period (it is the time space between two samples)
	double samplesPerSymbol{ 1.0 };								// samplesPerSymbol = symbolPeriod / samplingPeriod;

	double centralWavelength_m{ 1550E-9 };
	double centralFrequency_Hz{ SPEED_OF_LIGHT / centralWavelength_m };

};

// This template defines a set of final classes (instantiable)
template<typename T, signal_type sType, signal_value_type vType>
class BaseSignal final: public Signal {

public:

	using value_type = T;

	BaseSignal() : Signal() { setBuffer(); }
	BaseSignal(std::string fileName) : Signal(fileName) { setBuffer(); }
	BaseSignal(std::string fileName, t_unsigned_long bLength) : Signal(fileName, bLength) { setBuffer(); }
	BaseSignal(std::string fileName, t_unsigned_long bLength, Signal::t_write_mode sWriteMode): Signal(fileName, bLength, sWriteMode) { setBuffer(); }
	BaseSignal(std::string fileName, t_unsigned_long bLength, t_header_type hType, Signal::t_write_mode sWriteMode) : Signal(fileName, bLength, hType, sWriteMode) { setBuffer(); }
	BaseSignal(std::string fileName, std::string folderName) : Signal(fileName, folderName) { setBuffer(); }
	BaseSignal(std::string fileName, std::string folderName, t_unsigned_long bLength) : Signal(fileName, folderName, bLength) { setBuffer(); }
	BaseSignal(t_unsigned_long bLength) : Signal(bLength) { setBuffer(); }

	value_type bufferGet()
		{
		if (bufferEmpty) return value_type();

		value_type value = static_cast<value_type *>(buffer)[outPosition];

		bufferFull = false;

		outPosition = (outPosition + 1) % bufferLength;

		bufferEmpty = outPosition == inPosition;

		return value;
	}

	value_type& operator[](int index) { return (static_cast<value_type*>(buffer)[(outPosition + index) % bufferLength]); }

	std::size_t valueTypeSizeOf() { return sizeof(T); }

private:

	void setBuffer() {
		setType(sType, vType);
		if (buffer == nullptr) buffer = (value_type*) (new value_type[getBufferLength()]());
	}
};

// #######################################################################################################
//
// Signal types
//
// #######################################################################################################

using Raw = BaseSignal< t_raw, signal_type::Raw, signal_value_type::t_raw>;

using Binary = BaseSignal< t_binary, signal_type::Binary, signal_value_type::t_binary> ;

using TimeDiscreteAmplitudeDiscreteInteger = BaseSignal<t_integer, signal_type::TimeDiscreteAmplitudeDiscreteInteger, signal_value_type::t_integer>;

using TimeContinuousAmplitudeDiscreteInteger = BaseSignal<t_integer, signal_type::TimeContinuousAmplitudeDiscreteInteger, signal_value_type::t_integer>;

using TimeDiscreteAmplitudeContinuousReal = BaseSignal<t_real, signal_type::TimeDiscreteAmplitudeContinuousReal, signal_value_type::t_real>;

using TimeDiscreteAmplitudeDiscreteReal = BaseSignal<t_real, signal_type::TimeDiscreteAmplitudeDiscreteReal, signal_value_type::t_real>;

using TimeContinuousAmplitudeContinuousReal = BaseSignal<t_real, signal_type::TimeContinuousAmplitudeContinuousReal, signal_value_type::t_real>;

using TimeContinuousAmplitudeDiscreteReal = BaseSignal<t_real, signal_type::TimeContinuousAmplitudeDiscreteReal, signal_value_type::t_real>;

using TimeDiscreteAmplitudeContinuousComplex = BaseSignal<t_complex, signal_type::TimeDiscreteAmplitudeContinuousComplex, signal_value_type::t_complex>;

using TimeContinuousAmplitudeContinuousComplex = BaseSignal<t_complex, signal_type::TimeContinuousAmplitudeContinuousComplex, signal_value_type::t_complex>;

using OpticalSignal = BaseSignal<t_complex, signal_type::OpticalSignal, signal_value_type::t_complex>;

using OpticalSignalXY = BaseSignal<t_complex, signal_type::OpticalSignalXY, signal_value_type::t_complex_xy>;

using PhotonStreamXY = BaseSignal<t_complex_xy, signal_type::PhotonStreamXY, signal_value_type::t_complex_xy>;

//using PhotonStreamMP = BaseSignal<t_photon_mp, signal_type::PhotonStreamMP, signal_value_type::t_photon_mp>;

using PhotonStreamMPXY = BaseSignal<t_photon_mp_xy, signal_type::PhotonStreamMPXY, signal_value_type::t_photon_mp_xy>;

using Message = BaseSignal<t_message*, signal_type::Message, signal_value_type::t_message>;

//! Type of the Signal declaration, using the t_handler_message class
using HandlerMessage =
    BaseSignal<t_handler_message*, signal_type::HandlerMessage,
               signal_value_type::t_handler_message>;

//########################################################################################################################################################
//########################################################## GENERIC BLOCK DECLARATIONS AND DEFINITIONS ##################################################
//########################################################################################################################################################

class Block
{

public:
	/* Methods */
	Block(){};
	Block(std::vector<Signal *> &InputSig, std::vector<Signal *> &OutputSig);
	Block(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig); // since C++11

	// void initializeBlock(std::vector<Signal*> InputSig, vector<Signal*> OutputSig);
	void initializeBlock();

	virtual void initialize(void){};

	virtual bool runBlock();

	void terminateBlock();
	virtual void terminate(void){};

	void closeOutputSignals();

	void setNumberOfInputSignals(int nOfInputSignal) { numberOfInputSignals = nOfInputSignal; };
	int getNumberOfInputSignals() { return numberOfInputSignals; };

	void setNumberOfOutputSignals(int nOfOutputSignal) { numberOfOutputSignals = nOfOutputSignal; };
	int getNumberOfOutputSignals() { return numberOfOutputSignals; };

	void setLogValue(bool lValue) { logValue = lValue; }
	bool getLogValue() { return logValue; }

	void setFirstRun(bool fRun) { firstRun = fRun; }
	bool getFirstRun() { return firstRun; }

	void setFirstTime(bool fTime) { firstTime = fTime; }
	bool getFirstTime() { return firstTime; }

	void setTerminated(bool t) { terminated = t; }
	bool getTerminated() { return terminated; }

	std::string getSignalsFolderName();

	void setVerboseMode(t_bool vMode) { verboseMode = vMode; }
	t_bool getVerboseMode(void) { return verboseMode; }

	void setVerboseFolderName(t_string vFolderName) { verboseFolderName = vFolderName; }
	t_string getVerboseFolderName(void) const { return verboseFolderName; }

	std::vector<Signal *> inputSignals;
	std::vector<Signal *> outputSignals;

private:
	bool logValue{true};
	bool firstRun{true}; // To be deleted, 2020/02/04, the name firstTime is more comum
	bool firstTime{true};
	bool terminated{false};

	t_bool verboseMode{true};

	int numberOfInputSignals{1};
	int numberOfOutputSignals{1};

	t_string verboseFolderName{"verbose"};
};

//########################################################################################################################################################
//####################################################################### CONSOLE ########################################################################
//########################################################################################################################################################

namespace ConsoleOutputCommon 
{
	void clrLine();
	void moveCursorTopLeft();
	void replaceLine(std::string s);
	void clrScreen();
	void prepareWindowsConsole();
	void clrRemainingScreen();
}

class BlockGetFunctionBase
{
public:

	double nviGetFunction(double* rValue) { return getFunction(rValue); };
	int nviGetFunction(int* rValue) { return getFunction(rValue); };
	unsigned long int nviGetFunction(unsigned long int* rValue) { return getFunction(rValue); };
	bool nviGetFunction(bool* rValue) { return getFunction(rValue); };
	std::string nviGetFunction(std::string* rValue) { return getFunction(rValue); };

private:

	virtual double getFunction(double* rValue) { return (*rValue); };
	virtual int getFunction(int* rValue) { return (*rValue); };
	virtual unsigned long int getFunction(unsigned long int* rValue) { return (*rValue); };
	virtual t_bool getFunction(t_bool* rValue) { return (*rValue); };
	virtual std::string getFunction(std::string* rValue) { return (*rValue); };

};

template<typename fT, typename bT, typename rT>
class BlockGetFunction : public BlockGetFunctionBase
{
public:
	BlockGetFunction(bT* pB, fT gF) : pBlk{ pB }, gFunction{ gF } {};

	fT gFunction;

	bT* pBlk;

	rT operator()() { return gFunction(*pBlk); };

	virtual rT getFunction(rT* ) override { return gFunction(*pBlk); };

};

class Console
{
public:
	Console() {};

	void addGetFunction(std::string name, BlockGetFunctionBase* gFunction, value_type vType) { getOrder[displayOrder++] = name; getFunctionsTable[name] = gFunction; getReturnType[name] = vType; };

	void setShowConsolePeriodicity(int sConsolePeriodicity) { showConsolePeriodicity = sConsolePeriodicity; }

	std::map<int, std::string>  getOrder = std::map<int, std::string>();
	std::map<std::string, BlockGetFunctionBase*>  getFunctionsTable = std::map<std::string, BlockGetFunctionBase*>(); //Maps the names of the variables to the addresses of the GetFunction
	std::map<std::string, value_type>  getReturnType = std::map<std::string, value_type>();

	int displayOrder{ 0 };

	int showConsolePeriodicity{ 0 };

	int display{ 0 };
};


//########################################################################################################################################################
//########################################################## GENERIC SYSTEM DECLARATIONS AND DEFINITIONS #################################################
//########################################################################################################################################################


class System {

public:

	//######################################################################################################

	System() {};
	void setSystem(std::initializer_list<Block *> MainSystem);

	System(std::initializer_list<Block *> MainSystem);
	System(std::initializer_list<Block*> MainSystem, std::string signalsFolderName);
	System(std::initializer_list<Block *> MainSystem, std::string signalsFolderName, std::vector<std::string> lInputParameters);

	bool run();
	bool run(std::string signalPath);
	void terminate();
	void terminateSuperBlock();
	
	void nviOpenConsole() { openConsole(); };
	void nviUpdateConsole() { openConsole(); updateConsole(); };
	void nviCloseConsole() { closeConsole(); };

	//########################################################################################################

	void setLogValue(bool value);
	bool getLogValue() { return logValue; };

	void setOpenFile(bool value);
	bool getOpenFile() { return openFile; };

	void setLogFileName(std::string newName);
	std::string getLogFileName() { return logFileName; };

	void setShowConsole(t_bool sConsole) { showConsole = sConsole; };
	t_bool getShowConsole() const { return showConsole; };

	void setShowConsolePeriodicity(int sConsolePeriodicity) { systemConsole->setShowConsolePeriodicity(sConsolePeriodicity); }

	void setSignalsFolderName(std::string newName);
	std::string getSignalsFolderName() { return signalsFolder; };

	void setVerboseFolderName(std::string vFolderName);
	std::string getVerboseFolderName() const { return verboseFolderName; };

	void setSaveSignals(bool sSignals);
	bool const getSaveSignals(void) { return saveSignals; };

	void setLoadedInputParameters(std::vector<std::string> loadedInputParams);
	std::vector<std::string> getLoadedInputParameters() { return loadedInputParameters; };
	size_t getSystemBlocksSize() { return SystemBlocks.size(); };
	std::vector<Block *> getSystemBlocks() { return SystemBlocks; };

	void setVerboseMode(t_bool vMode);
	t_bool getVerboseMode(void) { return verboseMode; }

	void setConsole(Console *c) { systemConsole = c; }

	Console* getConsole() const { return systemConsole; }

	void setMsgOpenConsole(std::string mOpenConsole) { msgOpenConsole = mOpenConsole; }

	//########################################################################################################

private:

	std::string signalsFolder{ "signals" };
	bool saveSignals{ true };

	char fileName[MAX_NAME_SIZE]="";  // Name of the file with system description (.sdf)
	char name[MAX_NAME_SIZE]="";  // System Name
	int numberOfBlocks{ 0 };  // Number of system Blocks
	int(*topology)[MAX_TOPOLOGY_SIZE];  // Relationship matrix
	std::vector<Block *> SystemBlocks;  // Pointer to an array of pointers to Block objects
								   //Log File Inputs
	std::string logFileName{ "log.txt" }; // The name of the file where the debug info will be written
	bool logValue{ true }; // Will write debug info if true
	bool openFile{ true };
	std::ofstream logFile;

	std::vector<std::string> loadedInputParameters;

	std::string verboseFolderName{ "verbose" };
	t_bool verboseMode{ true };

	t_bool showConsole{ false };
	Console* systemConsole{ nullptr };

	std::string msgOpenConsole{"System Console"};

	// #### NVI methods #####################
	virtual void openConsole();
	virtual void updateConsole();
	virtual void closeConsole();
};



//########################################################################################################################################################
//#################################################### GENERIC SUPER BLOCK DECLARATIONS AND DEFINITIONS ##################################################
//########################################################################################################################################################


class SuperBlock : public Block {

public:


	/* Methods */
	SuperBlock(std::initializer_list<Signal*> inputSignals, std::initializer_list<Signal*> outputSignals) : Block(inputSignals, outputSignals) { superBlockSystem.setLogValue(false); };

	void initializeSuperBlock();

	//virtual void initialize(void) {};

	virtual bool runBlock(std::string signalPath);

	void terminate(void);
	void terminateSuperBlock(void) { superBlockSystem.terminateSuperBlock(); };

	/* Set Methods */

	//void setBlocks(std::vector<Block*> blks) { blocks = blks; };
	void setSuperBlockSystem(std::initializer_list<Block*> blks);

	void setLogValue(bool lValue) { superBlockSystem.setLogValue(lValue);	};
	bool const getLogValue() { return superBlockSystem.getLogValue(); };

	void setOpenFile(bool value) { superBlockSystem.setOpenFile(value); };

	void setLogFileName(std::string lFileName) { superBlockSystem.setLogFileName(lFileName); };

	void setSuperBlockFolderName(std::string fName) { superBlockFolderName = fName; };
	std::string getSuperBlockFolderName() { return superBlockFolderName; };

	void setSignalsFolderName(std::string sFolderName) { superBlockSystem.setSignalsFolderName(sFolderName); };
	std::string getSignalsFolderName() { return superBlockSystem.getSignalsFolderName(); };

	void setVerboseFolderName(std::string vFolderName) { superBlockVerboseFolderName = vFolderName; };
	std::string getVerboseFolderName() const { return superBlockVerboseFolderName; };

	void setLoadedInputParameters(std::vector<std::string> loadedInputParams) { superBlockSystem.setLoadedInputParameters(loadedInputParams); };

	void setSaveInternalSignals(bool sInternalSignals) { saveInternalSignals = sInternalSignals;  superBlockSystem.setSaveSignals(saveInternalSignals); };
	bool const getSaveInternalSignals(void) { return saveInternalSignals; };

	/*void setSuperBlockSystemValues();*/

private:

	/* State Variables */

	std::vector<Block*> moduleBlocks;
	std::vector<Block*> blocks;
	System superBlockSystem;
	std::ofstream logFileSP;
	bool saveInternalSignals{ true };

	/* Input Parameters */

	std::string superBlockFolderName{ "signals/SuperBlock" };

	std::string superBlockVerboseFolderName{ "verbose/SuperBlock" };
	
};

//########################################################################################################################################################
//###################################################################### LINEAR FILTERS ##################################################################
//########################################################################################################################################################


class FIR_Filter : public Block {

public:

	/* State Variable */
	std::vector<t_real> impulseResponse;

	/* Methods */
	FIR_Filter(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};
	FIR_Filter(std::vector<Signal *> &InputSig, std::vector<Signal *> &OutputSig) :Block(InputSig, OutputSig) {};
	FIR_Filter() {};

	void initializeFIR_Filter(void);

	bool runBlock(void);

	void terminate(void);

	void setSaveImpulseResponse(bool sImpulseResponse) { saveImpulseResponse = sImpulseResponse; };
	bool getSaveImpulseResponse(void){ return saveImpulseResponse; };

	void setImpulseResponseLength(int iResponseLength) { impulseResponseLength = iResponseLength; };
	int const getImpulseResponseLength(){ return impulseResponseLength; }

	void setImpulseResponseFilename(std::string iResponseFilename) { impulseResponseFilename = iResponseFilename; }
	std::string getImpulseResponseFilename() const { return impulseResponseFilename; }

	void setBeginningOfImpulseResponseLength(int bOfImpulseResponseLength) { beginningOfImpulseResponseLength = bOfImpulseResponseLength; };
	int getBeginningOfImpulseResponseLength() const { return beginningOfImpulseResponseLength; };

	void setSeeBeginningOfImpulseResponse(bool sBeginning){ seeBeginningOfImpulseResponse = sBeginning; };
	bool const getSeeBeginningOfImpulseResponse(){ return seeBeginningOfImpulseResponse; };

private:

	/* State Variable */
	std::vector<t_real> delayLine;

	bool saveImpulseResponse{ true };
	std::string impulseResponseFilename{ "impulse_response.imp" };

	int impulseResponseLength{ 0 };							// filter order + 1 (filter order = number of delays)

	bool seeBeginningOfImpulseResponse{ false };

	int beginningOfImpulseResponseLength{ 0 };

};


class FD_Filter : public Block {
	
	/* State Variable */
	
	std::vector<t_real> inputBufferTimeDomain;
	std::vector<t_real> outputBufferTimeDomain;

	int inputBufferPointer{ 0 };
	int outputBufferPointer{ 0 };
	
	/* Input Parameters */
	bool saveTransferFunction{ true };
	std::string transferFunctionFilename{ "transfer_function.tfn" };
	int transferFunctionLength{ 128 };

	int inputBufferTimeDomainLength{ transferFunctionLength };
	int outputBufferTimeDomainLength{ transferFunctionLength };

public:
	/* State Variable */
	std::vector<t_complex> transferFunction;

	/* Methods */
	FD_Filter() {};
	FD_Filter(std::vector<Signal *> &InputSig, std::vector<Signal *> &OutputSig) :Block(InputSig, OutputSig) {};
	FD_Filter(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initializeFD_Filter(void);

	bool runBlock(void);

	void terminate(void) {};

	void setInputBufferTimeDomainLength(int iBufferTimeDomainLength) { inputBufferTimeDomainLength = iBufferTimeDomainLength; };
	int const getInputBufferTimeDomainLength() { return inputBufferTimeDomainLength; }

	void setOutputBufferTimeDomainLength(int oBufferTimeDomainLength) { outputBufferTimeDomainLength = oBufferTimeDomainLength; };
	int const getOutputBufferTimeDomainLength() { return outputBufferTimeDomainLength; }

	void setInputBufferPointer(int iBufferPointer) { inputBufferPointer = iBufferPointer; };
	int const getInputBufferPointer() { return inputBufferPointer; }

	void setOutputBufferPointer(int oBufferPointer) { outputBufferPointer = oBufferPointer; };
	int const getOutputBufferPointer() { return outputBufferPointer; }

	void setSaveTransferFunction(bool sTransferFunction) { saveTransferFunction = sTransferFunction; };
	bool getSaveTransferFunction(void) { return saveTransferFunction; };

	void setTransferFunctionLength(int iTransferFunctionLength) { transferFunctionLength = iTransferFunctionLength; };
	int const getTransferFunctionLength() { return transferFunctionLength; };

};

/*
// Generates a complex signal knowing the real part and the complex part.
class RealToComplex : public Block {
 public:
	 RealToComplex(std::vector<Signal *> &InputSig, std::vector<Signal *> &OutputSig);
	 bool runBlock(void);
 //private:
};
*/

// Separates the complex signal into two parts: real and imaginary.
/*class ComplexToReal : public Block {
 public:
	 ComplexToReal(std::vector<Signal *> &InputSig, vector<Signal *> &OutputSig);
	 bool runBlock(void);
 //private:
};*/





//########################################################################################################################################################
//############################################################### GENERIC DSP FUNCTIONS ##################################################################
//########################################################################################################################################################


/*class OverlapMethod
{

public:

	void overlapSaveSymComplexIn(std::vector<complex <double>> &v_in, vector<complex <double>> &v_out, vector<complex <double>> Hf, int NFFT);
	void overlapSaveSyRealIn(std::vector<double> &v_in, vector<double> &v_out, vector<double> Hf, int NFFT);
	void overlapSaveAsym(std::vector<double> &real_in, vector<double> &imag_in, vector<double> &real_out, vector<double> &imag_out, vector<double> h_real, vector<double> h_imag, int M, int L, int N);
	void overlapSaveSym(std::vector<double> &real_in, vector<double> &imag_in, vector<double> &real_out, vector<double> &imag_out, vector<double> h_real, vector<double> h_imag, int NFFT);
	void checkSize(std::vector<double> &real_in, vector<double> &imag_in, int L);

};




class Fft
{

public:
	vector<complex <double>> directTransformInReal(std::vector<double> real);

	vector<double> inverseTransformInCP(std::vector<complex <double>> &In);
	
	void directTransform(std::vector<double> &real, vector<double> &imag);
	void inverseTransform(std::vector<double> &real, vector<double> &imag);
	void transformRadix2(std::vector<double> &real, vector<double> &imag);
	void transformBluestein(std::vector<double> &real, vector<double> &imag);
	void convolve(const vector<double> &x, const vector<double> &y, vector<double> &out);
	void convolve(const vector<double> &xreal, const vector<double> &ximag, const vector<double> &yreal, const vector<double> &yimag, vector<double> &outreal, vector<double> &outimag);
	
	void Radix2(std::vector<double> &real, vector<double> &imag, int m);
	void Bluestein(std::vector<double> &real, vector<double> &imag, int m);
};*/


class ComplexMult
{
public:
	void CMultVector(std::vector<double> &v1_real, std::vector<double> &v1_imag, std::vector<double> v2_real, std::vector<double> v2_imag);
	void CMultVector_Loop(std::vector<double> &v1_real, std::vector<double> &v1_imag, std::vector<double> v2_real, std::vector<double> v2_imag);
	std::vector<std::complex <double>> CMultVectorInCP(std::vector<std::complex <double>> &v1_in, std::vector<std::complex <double>> &v2_in);
	void ComplexVect2ReImVect(std::vector<std::complex <double>> &v_in, std::vector<double> &v1_real, std::vector<double> &v1_imag);
	void CMultVector_InComplex(std::vector<std::complex <double>> &v1_in, std::vector<std::complex <double>> &v2_in);
	void ReImVect2ComplexVect(std::vector<double> &v1_real, std::vector<double> &v1_imag, std::vector<std::complex <double>> &v_out);

	std::vector<std::complex<double>> ReImVect2ComplexVector(std::vector<double> &v1_real, std::vector<double> &v1_imag);
	std::vector<std::complex <double>> complexVectorMultiplication(std::vector<std::complex <double>> &v1_in, std::vector<std::complex <double>> &v2_in);
};


///////////////////// Fast Fourier Transform ////////////////////////

class FourierTransform {

public:

	std::vector<std::complex<double>> fft(std::vector<std::complex<double> > &vec, int sign);
	std::vector<std::complex<double>> fft(std::vector<std::complex<double> > &vec);
	std::vector<std::complex<double>> ifft(std::vector<std::complex<double> > &vec);
	void transformRadix2(std::vector<std::complex<double> > &vec);
	void transformBluestein(std::vector<std::complex<double> > &vec);
	void convolve(
		const std::vector<std::complex<double> > &vecx,
		const std::vector<std::complex<double> > &vecy,
		std::vector<std::complex<double> > &vecout);
};

///////////////////// System Parameters ////////////////////////

class SystemInputParameters {
private:
	std::vector<std::string> loadedInputParameters;
	std::string inputParametersFileName{ "input_parameters_0.txt" }; //name of the file from where the input parameters will be read
	std::string outputFolderName{ "signals" };
	enum ParameterType { INT, LONG, DOUBLE, BOOL, STRING }; //types of parameters
											  //A parameter can only be of 1 type
	class Parameter {
	private:
		//Type of the parameter. this type should be hidden from the user
		ParameterType type;
		union { //pointers to the parameter
			int* i;
			long* l;
			double* d;
			bool* b;
			std::string* s;
		};

	public:
		//Changes the value of the parameter
		void setValue(int value);
		void setValue(long value);
		void setValue(double value);
		void setValue(bool value);
		void setValue(std::string value);

		ParameterType getType();
		//Constructor for parameter of type int
		Parameter(int* elem);
		//Constructor for parameter of type long
		Parameter(long* elem);
		//Constructor for parameter of type double
		Parameter(double* elem);
		//Constructor for parameter of type bool
		Parameter(bool* elem);
		//Constructor for parameter of type string
		Parameter(std::string* elem);
	};

	int parseInt(std::string str);
	long parseLong(std::string str);
	double parseDouble(std::string str);
	bool parseBool(std::string str);
	std::string parseString(std::string str);

	std::vector<std::string> split(const std::string& text, char sep);
	std::map<std::string, Parameter*> parameters = std::map<std::string, Parameter*>(); //Maps the names of the variables to the addresses of the parameters

public:

	std::string getInputParametersFileName() { return inputParametersFileName; }
	void setInputParametersFileName(std::string fname) { inputParametersFileName = fname; }
	std::string getOutputFolderName() { return outputFolderName; }
	std::vector<std::string> getLoadedInputParameters() { return loadedInputParameters; }
	void readSystemInputParameters();
	//Adds the parameter to the map
	void addInputParameter(std::string name, int* variable);
	void addInputParameter(std::string name, long* variable);
	void addInputParameter(std::string name, double* variable);
	void addInputParameter(std::string name, bool* variable);
	void addInputParameter(std::string name, std::string* variable);
	/* Default empty constructor. Initializes the map */
	SystemInputParameters() {}
	SystemInputParameters(int argc, char* argv[]);
	/* Deletes all heap memory occupied by the parameters */
	~SystemInputParameters();
};

//########################################################################################################################################################
//################################################################### RANDOM NUMBERS #####################################################################
//########################################################################################################################################################

namespace netxpto
{
	using random_engine = std::default_random_engine; 
	using t_seed = unsigned;   // to be deleted 2021/01/12, it is define in the global namespace

	enum class GeneratorValue
	{
		wrong_value,
		default_random_engine,
		minstd_rand,
		minstd_rand0,
		mt19937,
		mt19937_64,
		ranlux24_base,
		ranlux48_base,
		ranlux24,
		knuth_b
	};

	enum class DistributionValue
	{
		wrong_value,
		poisson
	};

	GeneratorValue stoGeneratorValue(std::string str);
	DistributionValue stoDistributionValue(t_string str);

	// Generates real numbers (double) uniformely distribuited in the interval [lo,hi)
	// by default the interval is (0,1)
	class UniformRandomRealNumbers {
	public:

		UniformRandomRealNumbers();
		UniformRandomRealNumbers(t_seed s);
		UniformRandomRealNumbers(t_seed s, GeneratorValue g_value);
		UniformRandomRealNumbers(t_seed s, std::string g_value_str);
		UniformRandomRealNumbers(double lo, double hi);
		UniformRandomRealNumbers(t_seed s, double lo, double hi);
		UniformRandomRealNumbers(t_seed s, GeneratorValue g_value, double lo, double hi);
		UniformRandomRealNumbers(t_seed s, std::string g_value_str, double lo, double hi);

		double operator()() const { return r(); };

		void setGeneratorValue(GeneratorValue g_value);
		void setGeneratorValue(std::string g_value_str);
		GeneratorValue getGeneratorValue() const { return generator_value; };

		void setSeed(t_seed s) { seed = s; setGeneratorValue(generator_value); };
		void setSeed(t_seed s, GeneratorValue g_value) { seed = s; setGeneratorValue(g_value); };
		t_seed getSeed() const { return seed; };

	private:
		double low{ std::nextafter(0, std::numeric_limits<double>::max()) };
		double high{ 1 };

		t_seed seed{ 0 };
		GeneratorValue generator_value{ GeneratorValue::default_random_engine };

		std::function<double()> r;
	};

	// Generates real numbers (double) with normal distribution
	class NormalRandomRealNumbers {
	public:

		NormalRandomRealNumbers();
		NormalRandomRealNumbers(t_seed s);
		NormalRandomRealNumbers(t_seed s, GeneratorValue g_value);
		NormalRandomRealNumbers(t_seed s, std::string g_value_str);

		double operator()() const { return r(); };

		void setGeneratorValue(GeneratorValue g_value);
		void setGeneratorValue(t_seed s, GeneratorValue g_value) { seed = s; setGeneratorValue(g_value); };
		void setGeneratorValue(std::string g_value_str);
		void setGeneratorValue(t_seed s, std::string g_value_str) { seed = s; setGeneratorValue(g_value_str); };
		GeneratorValue getGeneratorValue() const { return generator_value; };

		void setSeed(t_seed s) { seed = s; setGeneratorValue(generator_value); };
		void setSeed(t_seed s, GeneratorValue g_value) { seed = s; setGeneratorValue(g_value); };
		void setSeed(t_seed s, std::string g_value_str) { seed = s; setGeneratorValue(g_value_str); };
		t_seed getSeed() const { return seed; };

	private:
		double low{ std::nextafter(0, std::numeric_limits<double>::max()) };
		double high{ 1 };

		t_seed seed{ 0 };
		GeneratorValue generator_value{ GeneratorValue::default_random_engine };

		std::function<double()> r;
	};

	// Generates integer numbers uniformely distribuited in the closed interval [1,imax]
// r(bind(std::uniform_int_distribution<>(lo, hi)
	class UniformRandomIntegerNumbers {
	public:

		UniformRandomIntegerNumbers();
		UniformRandomIntegerNumbers(t_seed s);
		UniformRandomIntegerNumbers(t_seed s, GeneratorValue g_value);
		UniformRandomIntegerNumbers(t_seed s, std::string g_value_str);
		UniformRandomIntegerNumbers(int lo, int hi);
		UniformRandomIntegerNumbers(t_seed s, int lo, int hi);
		UniformRandomIntegerNumbers(t_seed s, GeneratorValue g_value, int lo, int hi);
		UniformRandomIntegerNumbers(t_seed s, std::string g_value, int lo, int hi);

		int operator()(int imax) { return ((r() % imax) + 1); }

		void setGeneratorValue(GeneratorValue g_value);
		void setGeneratorValue(std::string g_value_str) { setGeneratorValue(stoGeneratorValue(g_value_str)); };
		GeneratorValue getGeneratorValue() const { return generator_value; };

		void setSeed(t_seed s) { seed = s; setGeneratorValue(generator_value); };
		void setSeed(t_seed s, GeneratorValue g_value) { seed = s; setGeneratorValue(g_value); };
		void setSeed(t_seed s, std::string g_value_str) { seed = s; setGeneratorValue(stoGeneratorValue(g_value_str)); };
		t_seed getSeed() const { return seed; };

	private:
		int low{ 1 };
		int high{ std::numeric_limits<int>::max() };

		t_seed seed{ 0 };
		GeneratorValue generator_value{ GeneratorValue::default_random_engine };

		std::function<int()> r;

	};
	
	// Generates integer numbers following a Poisson distribution
	class PoissonRandomIntegerNumbers {
	public:

		PoissonRandomIntegerNumbers(t_real lamb);
		PoissonRandomIntegerNumbers(t_real lamb, t_seed s);
		PoissonRandomIntegerNumbers(t_real lamb, t_seed s, GeneratorValue g_value);
		PoissonRandomIntegerNumbers(t_real lamb, t_seed s, std::string g_value_str);

		t_integer operator()() const { return r(); };

		t_void setGeneratorValue(GeneratorValue g_value);
		t_void setGeneratorValue(t_seed s, GeneratorValue g_value) { seed = s; setGeneratorValue(g_value); };
		t_void setGeneratorValue(std::string g_value_str);
		t_void setGeneratorValue(t_seed s, std::string g_value_str) { seed = s; setGeneratorValue(g_value_str); };
		GeneratorValue getGeneratorValue() const { return generator_value; };

		void setSeed(t_seed s) { seed = s; setGeneratorValue(generator_value); };
		void setSeed(t_seed s, GeneratorValue g_value) { seed = s; setGeneratorValue(g_value); };
		void setSeed(t_seed s, std::string g_value_str) { seed = s; setGeneratorValue(g_value_str); };
		t_seed getSeed() const { return seed; };

	private:

		t_real lambda{ 1 };

		t_seed seed{ 0 };
		GeneratorValue generator_value{ GeneratorValue::default_random_engine };

		std::function<t_integer()> r;
	};



	void rng(t_seed s);

	void rng(t_seed s, GeneratorValue g_value);

	void rng(t_seed s, std::string g_value_str);

	void rng(std::string s_str);

	void rng(std::string s_str, GeneratorValue g_value);

	void rng(std::string s_str, std::string g_value_str);
	
	extern UniformRandomRealNumbers rand;
	extern NormalRandomRealNumbers randn;
	extern UniformRandomIntegerNumbers randi;

	t_integer random(t_string distribution, t_real parameter_1);

}

//########################################################################################################################################################
//############################################################### NUMERICAL WINDOWS ######################################################################
//########################################################################################################################################################

enum class WindowType { None, Hann, Hamming };

std::vector<double> getWindow(WindowType windowType, int windowSize);

# endif 
