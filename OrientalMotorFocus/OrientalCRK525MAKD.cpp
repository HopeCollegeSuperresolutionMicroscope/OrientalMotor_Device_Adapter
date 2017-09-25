#include "OrientalCRK525PMAKD.h"
#include "ftd2xx.h"
#include <type_traits>
#include <cstdint>
#include "ControllerStatusMonitorThread.h"
#include "OrientalMotorExceptions.h"

typedef enum exceptionData{

	invalid_fcn = 0x01,
	invalid_address = 0x02,
	invalid_data = 0x03,
	slave_error = 0x04

} exceptionData;

/* Virtual - Sets the current BaseAnglePartition from the one passed
*	Note:  This will be hard-coded for every new controller
*   @param baseAnglePartition - number that corresponds to an enumerated Value in the register
*   Returns - 0 if successful or errCode otherwise
*/
int OrientalCRK525MAKD::SetBaseAnglePartitionImpl( double baseAnglePartition ) 
{

	double currentBaseAngle = GetCurrentBaseAngle();
	AbstractRegisterBase* reg = GetCurrentBaseAngleRegister( );
	unsigned char serialVal[ MM::MaxStrLength ];
	int numSerialBytes;
	int ret;

	if( reg == nullptr )
	{
		AbstractControllerInterfaceFactory::LogMessage(  "Register is Nullptr" );
	}


	if( currentBaseAngle == .36 )
	{
		std::map< double, base_36DegStepAnglesEnum16Bit::EnumBaseType >::iterator it = _36DegBaseStepAnglesTypeMap_.find( baseAnglePartition );

		if( it == _36DegBaseStepAnglesTypeMap_.end() )
		{
			//Error, No such option
			return 1;
		}
		/*if( reg->isBigEndianCheck() )
		{
			numSerialBytes = ReadWrite< decltype( _36DegBaseStepAnglesTypeMap_[ baseAnglePartition ] ), true >::read( serialVal, sizeof(serialVal), _36DegBaseStepAnglesTypeMap_[ baseAnglePartition ] );
		}
		else
		{
			numSerialBytes = ReadWrite< decltype( _36DegBaseStepAnglesTypeMap_[ baseAnglePartition ] ), false >::read( serialVal, sizeof(serialVal), _36DegBaseStepAnglesTypeMap_[ baseAnglePartition ] );
		}*/
		AbstractControllerInterfaceFactory::LogMessage(  "Going to Serial Write Single Register " );

		if( ( ret = serialWriteSingleRegister( (*reg), _36DegBaseStepAnglesTypeMap_[ baseAnglePartition ] ) ) != 0 )
		{ 
			return ret;
		}

	}
	else if( currentBaseAngle == .72 )
	{

		std::map< double, base_72DegStepAnglesEnum16Bit::EnumBaseType >::iterator it = _72DegBaseStepAnglesTypeMap_.find( baseAnglePartition );

		if( it == _72DegBaseStepAnglesTypeMap_.end() )
		{
			//Error, No such option
			return 1;
		}
		/*if( reg->isBigEndianCheck() )
		{
			numSerialBytes = ReadWrite< decltype( _72DegBaseStepAnglesTypeMap_[ baseAnglePartition ] ), true >::read( serialVal, sizeof(serialVal), _72DegBaseStepAnglesTypeMap_[ baseAnglePartition ] );
		}
		else
		{
			numSerialBytes = ReadWrite< decltype( _72DegBaseStepAnglesTypeMap_[ baseAnglePartition ] ), false >::read( serialVal, sizeof(serialVal), _72DegBaseStepAnglesTypeMap_[ baseAnglePartition ] );
		}*/
		if( ( ret = serialWriteSingleRegister( (*reg), _72DegBaseStepAnglesTypeMap_[ baseAnglePartition ] ) ) != 0 )
		{ 
			return ret;
		}
		
	}
	else
	{
		return 1;
	}

	return 0;

}

/* Virtual - Returns the current BaseAngle Partition From the Controller
*	Note: This Function Should Read The Current SelectedBaseAngle Register (from GetCurrentBaseAngleRegister( address) )
*         It is the programmer's burden to make sure the baseAngleRegister is not subject to external manipulation
*	Returns - Read BaseAnglePartition value of Current BaseAngleRegister as a double or number > 360 if error
*         Note:  GetBaseAnglePartitionOptions() may be used to correlated Enumerated Values to doubles
*/
double OrientalCRK525MAKD::ReadCurrentBaseAnglePartition( void )
{
	//For the Sake of Time, We Will not Read The Register via RS-485
	AbstractRegisterBase* reg = GetCurrentBaseAngleRegister();

	if( reg == nullptr )
	{
		//Error, CurrentBaseAngle Is Corrupted
		return 400;
	}

	//Prepare Register Value for Evaluation
	unsigned char tempValBuf[ MM::MaxStrLength ];
	reg->read( tempValBuf, sizeof(tempValBuf) );

	//Base on Current Base Angles, Check Corresponding Maps
	double currentBaseAngle = GetCurrentBaseAngle();
	if( currentBaseAngle == .36 )
	{

		base_36DegStepAnglesEnum16Bit::EnumBaseType tempVal;
		if( reg->isBigEndianCheck() )
		{
			ReadWrite< base_36DegStepAnglesEnum16Bit::EnumBaseType, true >::write( tempValBuf, sizeof(tempVal), tempVal);
		}
		else
		{
			ReadWrite< base_36DegStepAnglesEnum16Bit::EnumBaseType, false >::write( tempValBuf, sizeof(tempVal), tempVal);
		}

		return matchBaseAnglePartitionRegisterValueToNumber( tempVal, _36DegBaseStepAnglesTypeMap_ );

	}
	else if( currentBaseAngle == .72 )
	{
		base_72DegStepAnglesEnum16Bit::EnumBaseType tempVal;
		if( reg->isBigEndianCheck() )
		{
			ReadWrite< base_72DegStepAnglesEnum16Bit::EnumBaseType, true >::write( tempValBuf, sizeof(tempVal), tempVal);
		}
		else
		{
			ReadWrite< base_72DegStepAnglesEnum16Bit::EnumBaseType, false >::write( tempValBuf, sizeof(tempVal), tempVal);
		}

		return matchBaseAnglePartitionRegisterValueToNumber( tempVal, _72DegBaseStepAnglesTypeMap_ );

	}

	//Unnecessary, but for any other code modification This may be used
	return 500;

}

/* Virtual Implementation - Returns a Standard Vector of double values indicative of all Base Angle Partitions for a given BaseAngle
*   Note: This will be hard-coded for every new controller
*   Note:  Used to display all available step Increments for a selected Base Angle
*   @param baseAngleOption - the current BaseAngle Being Queried
*   Returns - vector of doubles or an empty vector if there is a problem
*/
std::vector<double> OrientalCRK525MAKD::GetBaseAnglePartitionOptions( double baseAngleOption )
{
	vector< double > options;

	if( baseAngleOption == .36 )
	{
		return degOptions_36_;
	}
	else if ( baseAngleOption == .72 )
	{
		return degOptions_72_;
	}
	else
	{
		//Invalid Base Angle Passed! options.empty() == true
		return options;
	}
	return options;

}


/*Virtual Implementation - Initialize Physical Controller Via Serial Commands
*    Note:  NO Serial Communication should be done inside of controller constructor, due to user ability to correct errors later
*    Returns - 0 or errCode otherwise
*/
int OrientalCRK525MAKD::InitializePhysicalController() 
{

	int errCode;
	unsigned char multiRegValues[maxWritePacketbytes_];
	int currentByte = 0;

	//Verify Controller is connected or throw exception
	if( ( errCode = testConnection() ) != 0 )
	{
		return errCode;
	}
	//Switch over Command Control to RS-485 Communication
	AbstractControllerInterfaceFactory::LogMessage(  "Single Write" );
	//Start Input Mode
	if( (errCode = serialWriteSingleRegister( startInputModeReg, inputTypeEnum16Bit::RS485 ) ) != 0 )
	{
		return errCode;
	}

	if( (errCode = serialWriteSingleRegister( IOStopInputReg, genericEnableEnum16Bit::Disable) )  != 0 )
	{
		return errCode;
	}

	//Ensure ClockWise is + Direction First
	if( (errCode = serialWriteSingleRegister( motorRotationDirReg, spinDirectionsEnum16Bit::CounterClockWise, nullptr, true ) ) != 0 )
	{
		return errCode;
	}

	currentByte = 0;
	//Motor Excitation Mode
	currentByte += ReadWrite< decltype( motorExciteInputModeReg.getVal() ), motorExciteInputModeReg.isBigEndian_>::read( &multiRegValues[currentByte], sizeof(multiRegValues), inputTypeEnum16Bit::RS485 );
	//Home/Fwd/Rvs
	currentByte += ReadWrite< decltype( homeFwdRvsInputModeReg.getVal() ), homeFwdRvsInputModeReg.isBigEndian_>::read( &multiRegValues[currentByte], sizeof(multiRegValues), inputTypeEnum16Bit::RS485 );
	//Data No Input Mode
	currentByte += ReadWrite< decltype( dataNumInputModeReg.getVal() ), dataNumInputModeReg.isBigEndian_>::read( &multiRegValues[currentByte], sizeof(multiRegValues), inputTypeEnum16Bit::RS485 );

	std::ostringstream os;
	

	AbstractControllerInterfaceFactory::LogMessage(  "MultiWrite" );
	if( (errCode = serialWriteMultiRegister( motorExciteInputModeReg, 3, multiRegValues, currentByte ) ) != 0 )
	{
		os << "The errCode Was " << (int) errCode;
		AbstractControllerInterfaceFactory::LogMessage( os.str() );
		return errCode;
	}
	

	//AbstractControllerInterfaceFactory::LogMessage( "Checking Is Busy" );
	//IsMotorBusy();




	return 0;
}

/*  @Overload to virtual function testConnection
*   Sends a testConnection Packet Request to the Controller to verify Working Order
*		@param testValue - May be any 2 Byte number to test Byte integrity
*		Returns - 0 if successful, or errCode if not 
*/
int OrientalCRK525MAKD::testConnection( uint16_t testValue ){

	static const int packetByteSize = 8;
	unsigned char packet[packetByteSize];
	int currentByte = 0;
	int valueIndex;
	int numBytesWritten;
	int errCode = 0;

	//Controller Slave Address
	numBytesWritten = getAddressBuffer( packet, 1 );
	if( numBytesWritten == -1 )  
	{
		//The Buffer must have been too small
		return -1;
	}
	currentByte +=numBytesWritten;

	//Function Code
	packet[currentByte] =  functionCodes::diagnose;
	currentByte += sizeof( functionCodes::diagnose );

	//SubFunction Code
	packet[ currentByte ] = 0x00;
	packet[currentByte + 1 ] = 0x00;
	currentByte += 2;

	std::ostringstream os;
	os << "Value To Send is " << (int) testValue << "\n" ;

	//Add First Two Bytes of the passed value Big Endian Pass (Will not overflow)
	currentByte += ReadWrite< uint16_t, true >::read( &packet[ currentByte ], packetByteSize - currentByte, testValue );

	for( int i = 0; i < sizeof(testValue); i++ )
	{
		os << "packet [" << i - sizeof(testValue) + currentByte << "] = " << (int) packet[ currentByte + i -(currentByte + 1) ] << "\n";
	}

	AbstractControllerInterfaceFactory::LogMessage( os.str() );

	//Add CRCCheckValue
	numBytesWritten = appendCRCCheckValue( packet, sizeof(packet), currentByte );
	if( numBytesWritten == -1 )
	{
		//The Buffer Must have been too small
		return -1;
	}
	currentByte += numBytesWritten;

	//Communicate With Controller
	if( ( errCode = ( retrieveSerialCommHubPtr()->*retrieveSerialCommFuncPtr() )( packet, currentByte, this, false ) ) != DEVICE_OK )
	{
		//Register Was not Written
		return errCode;
	}

	return DEVICE_OK;

}

/*
* DataLengthLookup - Takes The Sampled Header and Parses it to Determine the length of the data packet expected
*  @param rxHeader[] - byte array of the header recieved from RX
*  @param headerLen - length of the header recieved
*  Returns - number of remaining bytes after header to read or -1 if there was an error in the dataspecifier
*/
int OrientalCRK525MAKD::dataLengthLookup( const unsigned char rxHeader[], const int rxHeaderLen )
{
	int remainingBytes = -1;

	std::ostringstream os;
	os << "the function code is " << (int) rxHeader[1];
	AbstractControllerInterfaceFactory::LogMessage( os.str() );

	//Check to see if the response is an exception Code
	//Choose Fixed Length
	if( (rxHeader[1] & g_exceptionBase) != 0 )
	{
		remainingBytes = 3;
	}
	else
	{
		switch( rxHeader[1] )
		{
			case functionCodes::diagnose:
			case functionCodes::multipleRegisterWrite:
			case functionCodes::registerWrite:
				remainingBytes = 6;
				break;
			case functionCodes::registerRead:
				//Either Read Next Value and pump it out
				//Or Have the Read Command Stored and then evaluated
				//Since Read remainingBytes = byte(num bytes) + 2xNum Regs + 2 byte error check
				AbstractControllerInterfaceFactory::LogMessage( "In Register Read HeaderLength" );
				if ( rxHeaderLen == 3 )
				{
					remainingBytes = rxHeader[2] + 2;
				}
				break;
			default:
				break;
		}
	}
	
	return remainingBytes;
}


/*
* Logic To Determine the Expected Header Length of the Response Message
*  @param txMsgBuffer[] = byte message that was transmitted
*  @param txMsgLen = length of defined Trasmit Message, used to detect undefined Index
*  Return = Number of Bytes in the Header or -1
*/
int OrientalCRK525MAKD::headerLengthLookup( const unsigned char txMsgBuffer[], const int txMsgLen)
{
	int headerBytes = -1;

	switch( txMsgBuffer[1] )
		{
			case functionCodes::diagnose:
			case functionCodes::multipleRegisterWrite:
			case functionCodes::registerWrite:
				headerBytes = 2;
				break;
			case functionCodes::registerRead:
				headerBytes = 3;
				break;
			default:
				break;
		}

	return headerBytes;
}

/*
*
* SameAddres - Check to ensure that the address in the header is the same as the controller recieving it
*  @param rxHeader[] - byte array of the header recieved from RX
*  @param headerLen - length of the header recieved
*  Return - true (sameAddress) or false ( not the same address/header passed is corrupted )
*
*Note:  headerLen is expected to match the length of the header not the whole message.  
*It is typically used after using a headerLengthLookup to sample the only the expected header
*Other uses would require the user to pass only the determined header Length is the whole Msg was sent
*/
bool OrientalCRK525MAKD::sameAddress( const unsigned char rxHeader[], const int rxHeaderLen )
{
	//Only two finite header lengths so might as well check them
	if( rxHeaderLen != 2 && rxHeaderLen != 3 )
	{
		return false;
	}

	OrientalCRK525MAKD::ControllerAddressType regAddress;
	GetAddress( regAddress );

	return ( rxHeader[0] == regAddress ) ? true : false;

}

/* Writes a Serialized Value Through the passed SerialCommFunction
*   @param serializedValue[] = Byte Array Passed From MMDevice Object for desired value
*   @param serializedValueLen =  Number of Defined Bytes in the array
*   @param valueIsBigEndian = Identifies The Way the Value was packaged into Bytes before being passed
*   @param *SerialCommFuncPtr = Pointer to the Desired Serial Communication Function Being Used
*/
int OrientalCRK525MAKD::WritePosBuffer( unsigned char serializedValue[], int serializedValueLen, bool valueIsBigEndian, AbstractControllerInterface::SerialCommFuncPtr serialCommFuncPtr )
{ 
	int errCode = 0;
	int typedValueIdx = 0;

	//Check to See if SerializedValueLen and Register Length Coincide
	if( serializedValueLen != posReg.getRegisterByteSize() )
	{
		typedValueIdx = serializedValueLen - posReg.getRegisterByteSize();
		if( typedValueIdx < 0 ) //Undersized Value Not Accepted
		{
			return -1;
		}

		//One Exception: if buffer is passed with padded 0's from something like a long
		for( int i = 0; i < typedValueIdx; i++ )
		{
			if( serializedValue[i] != 0 )
			{
				AbstractControllerInterfaceFactory::LogMessage("There is a type mismatch!");
				return -1;
			}
		}
	}

	if( ( errCode = serialWriteSingleRegister( cmd1Reg, cmd1BitsEnum16Bit::M0 | cmd1BitsEnum16Bit::COn, serialCommFuncPtr ) ) != 0 )
	{
		return errCode;
	}

	
	//Swap Values if The Data was Transported Differently Than in the Register
	//Little Endian -> Big Endian Or Big Endian -> Little Endian
	if( valueIsBigEndian != posReg.isBigEndianCheck() )
	{
		//OverSized Buffer That guarantees large transfer
		unsigned char tempArray[MM::MaxStrLength];
		for( int i = 0; i < serializedValueLen; ++i )
		{
			tempArray[i] = serializedValue[ serializedValueLen - (i+1) ];
		}

		//Copy Over to serializedValue
		for( int i = 0; i < serializedValueLen; ++i )
		{
			serializedValue[ i ] = tempArray[ i ];
		}

	}

	/*
	*   Send Position Packet
	*/

	//Create value of Register Basic Type
	/*decltype( posReg.getVal() ) val = 0;
	if( valueIsBigEndian )
	{
		ReadWrite< decltype( posReg.getVal() ), true>::write( &serializedValue[ typedValueIdx ], serializedValueLen, val );
	}
	else
	{
		ReadWrite< decltype( posReg.getVal() ), false>::write( &serializedValue[ typedValueIdx ], serializedValueLen, val );
	}*/
	/*
	std::ostringstream os;
	for( int i = 0; i < serializedValueLen; ++i )
	{
		os << "value[" << i << "] = " << (int) serializedValue[i];
	}

	AbstractControllerInterfaceFactory::LogMessage( os.str() );*/

	if( ( errCode = serialWriteMultiRegister( posReg, posReg.getRegisterByteSize()/baseRegisterByteSize_, serializedValue, serializedValueLen ) ) != 0 )
	{
		return errCode;
	}


	/* 
	*  Send Start Command 
	*/

	if( ( errCode = serialWriteSingleRegister( cmd1Reg, cmd1BitsEnum16Bit::Start | cmd1BitsEnum16Bit::M0 | cmd1BitsEnum16Bit::COn, serialCommFuncPtr ) ) != 0 )
	{
		return errCode;
	}
	
	//Check Code Will Be implemented in WritePosBuffer()

	return 0; 

};

/* Vitrual Implementation -Writes a Serialized Value For Step Speed to Motor Step Speed Register
*   Note:  Assumes SerializedSpeedValue is BigEndian and expects WriteStepSpeed() to be public Implementation
*   @param serializedSpeedValue[] = Byte Array Passed From MMDevice Object for desired value
*   @param serializedValueLen =  Number of Defined Bytes in the array
*/
int OrientalCRK525MAKD::WriteStepSpeedBuffer( unsigned char serializedSpeedValue[], unsigned int speedValueLen ) {

	int errCode = 0;
	int typedValueIdx = 0;
	int val = 0;

	//Check to See if SerializedValueLen and Register Length Coincide
	if( speedValueLen != operatingSpeedReg.getRegisterByteSize() )
	{
		typedValueIdx = speedValueLen - operatingSpeedReg.getRegisterByteSize();
		if( typedValueIdx < 0 ) //Undersized Value Not Accepted
		{
			return -1;
		}

		//One Exception: if buffer is passed with padded 0's from something like a long
		for( int i = 0; i < typedValueIdx; i++ )
		{
			if( serializedSpeedValue[i] != 0 )
			{
				AbstractControllerInterfaceFactory::LogMessage("There is a type mismatch!");
				return -1;
			}
		}
	}

	//Produce the value
	ReadWrite< decltype( posReg.getVal() ), true>::write( &serializedSpeedValue[ typedValueIdx ], speedValueLen, val );

	//Write The speed register
	if( ( errCode = serialWriteSingleRegister( operatingSpeedReg, val ) ) != 0 )
	{
		return errCode;
	}

	return 0;
}

/* Virtual - Sets the current energized state in the inheriting controller logic and hardware communication
*	Note:  This will be hard-coded for every new type of controller
*	  @param energyOn - whether or not the motor should be energized (true = On)
*   Returns - 0 if successful or errCode otherwise  (errCode is used to determine if the intention should be stored in isHardwareEnergized_)
*   Throws - Exceptions for Communication Errors
*/
void OrientalCRK525MAKD::SetRestingHardwareEnergizedImpl( bool energyOn ) {

	int errCode = 0;

	cmd1BitsEnum16Bit::EnumBaseType cOnBit = static_cast<cmd1BitsEnum16Bit::EnumBaseType>( ( energyOn ) ? cmd1BitsEnum16Bit::COn : 0 );

	if( ( errCode = serialWriteSingleRegister( cmd1Reg, cOnBit ) ) != 0 ) {
		throw MMErrorCodeException( errCode, "Failure in GetHardWareEnergizedImpl" );
	}

}

/* Virtual Implementation - Gets the current energized state as a result of the inheriting controller logic and hardware communication
*	Note:  This will be hard-coded for every new type of controller
*	Returns - boolean value if successful  
*   throws - SerialHardwareCommunication Failure - used to determine if the value should be stored in isHardwareEnergized_
*/
bool OrientalCRK525MAKD::GetRestingHardwareEnergizedImpl( ) {

	int errCode; 

	if( (errCode = ReadRegisters( &DriverStatusReg, DriverStatusReg.getRegisterByteSize() ) ) != 0 ) {
		throw MMErrorCodeException( errCode, "Failure in GetHardWareEnergizedImpl" );
	}

	return ((DriverStatusReg.getVal() & driverStatusBitsEnum32Bit::Enable) > 0);

}

/*Virtual Implementation - Returns a code corresponding to Whether or not the Motor is Currently Running
*  Note:  Should Implement A ThreadGuard Due to the Accessing of Any Information In This Function
*         The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
*         It is the burden of the Programmer to ThreadLock AnyOther Functions Pertaining to Values Used in This Implementation
*  Returns - Code for Non-Busy, Busy, or errCode Otherwise
*/
int OrientalCRK525MAKD::IsMotorBusy( void ) {

	MMThreadGuard guard(busyLock_);

	int errCode = 0;

	std::ostringstream os;

	if( (errCode = ReadRegisters( &status1Reg, 1 ) ) != 0 )
	{
		os << "The errCode Was " << (int) errCode;
		AbstractControllerInterfaceFactory::LogMessage( os.str() );
		return errCode;
	}

	decltype( status1Reg.getVal() )	val = status1Reg.getVal();

	os << "The value is " << (int) val;
	os << "\n the Boolean Move " << (int) (status1BitsEnum16Bit::Move & val);
	os << "\n the Boolean Ready " << (int) (status1BitsEnum16Bit::Ready & val);
	AbstractControllerInterfaceFactory::LogMessage( os.str() );

	//Compare val to MOVE and READY 
	//After Testing, The Ready Bit Does Not Seem To Indicate AnyThing (Is always 0)
	if( (status1BitsEnum16Bit::Move & val) != 0 )
	{
		AbstractControllerInterfaceFactory::LogMessage( "Something Was Busy" );
		return 1;
	}
	
	//May Need to Check S-BSY, however, it should be processing and therefore would be redundant

	return 0;

}

/* Virtual Implementation - Permit the Controller To Do Another Step Process
*	 Hardware Interaction - Resets Start Bit to 0 For Toggling Of Position
*
*    Note:  This function is necessary for enabling a write process after the motor has already spun
*    Note2:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
*    Note3: Should Implement A ThreadGuard Due to the Accessing of Any Information In This Function
*			The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
*			It is the burden of the Programmer to ThreadLock AnyOther Functions Pertaining to Values Used in This Implemtation
*	 Returns - 0 on success or errCode otherwise
*/
int OrientalCRK525MAKD::PermitPosWriteImpl( void ) {

	int errCode;

	//Determine Whether or Not To Keep Energized at the end
	cmd1BitsEnum16Bit::EnumBaseType cOnBit = static_cast< cmd1BitsEnum16Bit::EnumBaseType >( ( GetRestingHardwareEnergized( false ) ) ? cmd1BitsEnum16Bit::COn : 0 );

	if( ( errCode = serialWriteSingleRegister( cmd1Reg, cmd1BitsEnum16Bit::M0 | cOnBit ) ) != 0 )
	{
		return errCode;
	}

	return 0;
}

/* Virtual Implementation - Perform Necessary Child-level and Serial Commands to Permit the Controller To Do Any Other Busy-Dependent Processes
*    Note:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
*    Note2: Should Implement A ThreadGuard Due to the Accessing of Any Member Variables In This Function
* 	   The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
*	   It is the burden of the Programmer to ThreadLock AnyOther Functions Pertaining to Members Used in This Implementation
*	 
*    Returns - 0 on success or errCode otherwise
*/
int OrientalCRK525MAKD::PermitBusyDependentProcessesImpl( void ) {

	int errCode;

	MMThreadGuard guard( busyLock_ );

	isInternalProc_ = false;

	return 0;
}

/*
* ParseData - Verifies Each Recieved Packet and Directs it to a Parsing Action
*  @param txMsgBuffer[] = byte message that was transmitted 
*  @param txMsgLen = byte length of defined Trasmit Message, used to detect undefined Index
*  @param rxMsgBufer[] = byte message that was recived (Header + Data)
*  @rxMsgLen = byte length of recieved message
*/
//Note: A lot of overhead Currently, but this makes it easier to implement other controllers
int OrientalCRK525MAKD::parseData( const unsigned char txMsgBuffer[], const int txMsgLen, const unsigned char rxBuffer[], const int rxBufLen )
{

	unsigned int compCRC = crcCompute( rxBuffer, rxBufLen - 2 );
	unsigned int msgCRC = (rxBuffer[ rxBufLen - 1 ] << 8) | rxBuffer[ rxBufLen - 2 ];
	int errCode;
	std::ostringstream os;
	os << "Comp: " << compCRC << "\nMsg: " << msgCRC;
	AbstractControllerInterfaceFactory::LogMessage(  os.str() );
	if( msgCRC != compCRC )
	{
		//Return Error Code, Data Corruption
		errCode = 1;
		return errCode;
	}

	if( (rxBuffer[1] & g_exceptionBase) != 0 )
	{
		//Return the Error Code
		//Need additional Logic to produce ERRs for MM
		AbstractControllerInterfaceFactory::LogMessage(  "Actual Error" );
		errCode = rxBuffer[2];
		return errCode;
	}
	else
	{
		switch( rxBuffer[1] )
		{
			case functionCodes::diagnose:
				//Partly unnecessary due to CRC check and even return
				//but allows for any CRC hash collisions to be eliminated
				if( rxBuffer[2] == 0 || rxBuffer[3] == 0 || rxBuffer[4] == txMsgBuffer[4] || rxBuffer[5]== txMsgBuffer[5] )
				{
					errCode = DEVICE_OK;
				}
				else
				{
					//errCode is Data Corruption in Line
					errCode = 1;
				}
				break;
			case functionCodes::multipleRegisterWrite:
				//For readabilty use other function
				AbstractControllerInterfaceFactory::LogMessage(  "Multiple Write Response" );
				errCode = onMultipleRegisterWriteResponse( txMsgBuffer, txMsgLen, rxBuffer, rxBufLen );
				break;
			case functionCodes::registerWrite:
				AbstractControllerInterfaceFactory::LogMessage(  "Single Write Response" );
					os << "The large byte is " << (int) rxBuffer[2] << " and small is " << (int) rxBuffer[3] << "\n";
					AbstractControllerInterfaceFactory::LogMessage(  os.str() );
				errCode = onSingleRegisterWriteResponse( txMsgBuffer, txMsgLen, rxBuffer, rxBufLen );				
				break;
			case functionCodes::registerRead:
				errCode = onRegisterRead( txMsgBuffer, txMsgLen, rxBuffer, rxBufLen );
				break;
			default:
				errCode = 12;
				break;
		}
	}

	return errCode;
}

/*
*
*  Private Controller Specific Data Parsing Functions
*
*/

/* Write Multiple Consecutive Registers From A PreBuilt Value Array
*   @param &startReg - Reference to the register with the first address
*   @param numRegs - number of registers to write in total
*   @param valueArray[] - Byte array of already parsed Values (most likely using ReadWrite< decltype( register), isBigEndian::read()
*   @param valueArraySize - Size of the Defined Bytes in the Array to be passed along
*   Returns - 0 on completion or errorCodes otherwise
*/
int OrientalCRK525MAKD::serialWriteMultiRegister( AbstractRegisterBase &startReg, unsigned int numRegs, unsigned char valueArray[], int valueArraySize ) {

			unsigned char packet[maxWritePacketbytes_];
			int currentByte = 0;
			int valueIndex;
			int numBytesWritten;
			int errCode = 0;
			baseAddressType addr;

			std::ostringstream os2;
			os2 << " The Value Packet is ";
			for( int i = 0; i < valueArraySize; i++ )
			{
				os2 << (int) valueArray[i] << "\n";
			}
			AbstractControllerInterfaceFactory::LogMessage( os2.str() );


			AbstractControllerInterfaceFactory::LogMessage( "Value Size Check" );
			//Verify valueArray Accounts for Number of Buffer Values
			if( valueArraySize != numRegs*2 )
			{
				//Register Incomplete ValueArray for Number of Registers
				throw 1;
			}

			AbstractControllerInterfaceFactory::LogMessage( "Slave Address" );
			//Controller Slave Address
			numBytesWritten = getAddressBuffer( packet, 1 );
			if( numBytesWritten == -1 )  
			{
				//The Buffer must have been too small
				return -1;
			}
			currentByte +=numBytesWritten;

			//Function Code
			packet[currentByte] =  functionCodes::multipleRegisterWrite;
			currentByte += sizeof( functionCodes::multipleRegisterWrite );

			AbstractControllerInterfaceFactory::LogMessage( "Register Address" );
			//Register Address
			numBytesWritten = startReg.readAddress( &packet[currentByte],  maxWritePacketbytes_ - currentByte );
			if( numBytesWritten == -1 ) 
			{
				//The Buffer Must have Been Too small
				return -1;
			}
			//Incomplete Type Workaround
			if( startReg.isBigEndianCheck() )
			{
				ReadWrite< baseAddressType, true >::write( &packet[currentByte], numBytesWritten, addr );
			}
			else
			{
				ReadWrite< baseAddressType, false >::write( &packet[currentByte], numBytesWritten, addr );
			}
			currentByte += numBytesWritten;
			
			//Append numRegs (BigEndian Two Bytes)
			packet[currentByte] = static_cast<unsigned char>(numRegs >> 8);
			packet[currentByte + 1] = static_cast<unsigned char>( numRegs );
			//Append numData Bytes ( = 2 * numRegs)
			packet[currentByte + 2] = static_cast< unsigned char >( numRegs*2 );
			currentByte += 3;

			AbstractRegisterBase* reg = &startReg;
			
			AbstractControllerInterfaceFactory::LogMessage( "Value Array" );
			//Check Value Array Against Corresponding Registers and place in packet
			for( int i = 0, valueIdx = 0; i < numRegs; )
			{
				std::ostringstream os;
				os << "Address: " << addr << "\n";
				os << "This is the index " << i;
				AbstractControllerInterfaceFactory::LogMessage( os.str() );
				//Select non-start Registers
				if( i != 0 ) 
				{
					reg = GetRegisterByAddress( addr + i );
					if( reg == nullptr )
					{
						//Error, We are accessing an unregistered Register
						return -3;
					}
				}

				numBytesWritten = reg->getRegisterByteSize();
				//Check to see if value is inside of Acceptable Range
				if( ( errCode = reg->testSerialDataConformance( &valueArray[valueIdx], numBytesWritten ) ) != 1 )
				{
					if( errCode == -1 )
					{
						//The buffer was too small
						return -1;
					}
					if( errCode == 0 )
					{
						//Value outside of acceptable Range
						return -2;
					}
				}
				//Append Value Checked ValueArray Values
				for( int a = 0; a < numBytesWritten; a++ )
				{
					packet[currentByte + a] = valueArray[valueIdx + a];
				}

				currentByte += numBytesWritten;
				valueIdx += numBytesWritten;
				//Increment Addresses for multiAddress Values
				i += numBytesWritten / baseRegisterByteSize_;
				
			}

			//Add CRCCheckValue
			numBytesWritten = appendCRCCheckValue( packet, sizeof(packet), currentByte );
			if( numBytesWritten == -1 )
			{
				//The Buffer Must have been too small
				return -1;
			}
			currentByte += numBytesWritten;

			//Communicate With Controller
			if( ( errCode = ( retrieveSerialCommHubPtr()->*retrieveSerialCommFuncPtr() )( packet, currentByte, this, false ) ) != DEVICE_OK )
			{
				//Register Was not Written
				return errCode;
			}

			return DEVICE_OK;

		}


/* Compute CRC Value For Given Packet of Number of Bytes
*   @param bytes[] - Array of Bytes to be Used in the CRC Check
*   @param numBytes - Number of Defined Bytes in the Array
*   Returns - CRC Value (No Error Checks)
*/
uint32_t OrientalCRK525MAKD::crcCompute( const unsigned char bytes[] , const int numBytes )
{
	uint32_t value = 0xFFFF;
	unsigned char fcnCode = bytes[1];
	
	for( int i = 0; i < numBytes; i++)
	{
        //cout << "I is " << i << endl;
    	int shiftCount = 0;

    	//Step 1. Bitwise Xor
	    value = value ^ bytes[i];
	
	    while( shiftCount < 8 )  //Step 4.  Do Step 2 and 3 until shifted 8 times
	    {
		    //Step 2. Shift Bits right until first 1 is discarded
    		bool overflowBitHigh = false;
	    	while( overflowBitHigh != true && shiftCount < 8 )
		    {
		        //cout << "Value is " << hex << value << endl;
			    if( (value & 0x01) == 1 )
    			{
	    			overflowBitHigh = true;
		    	}
			    //Shift current Checked Bit into overflow
    			value = (value >> 1);
	    		shiftCount++;
		    }
    		//We have reached the Shift Count Number But have no Overflowbit to XOR
	    	if( shiftCount >= 8 && overflowBitHigh == false )
		    {
			    //Exit while loop
    			break;
	    	}

		    //Calculate XOR of shifted Value with Overflowed 1 
    		value = value ^ 0xA001;
	    }


	}

	return value;

}

/* Generates and Appends the CRC Check Value to a byteBuffer to Be Transmitted
*   @param byteBuffer[] - Buffer of Defined Bytes To Be Transmitted
*   @param bufferSize - Size of the Buffer Containing Bytes
*   @param numDefinedBytes - Number of Defined Bytes that are in the byteBuffer
*   Returns - -1 if bufferSize Mismatch or Size (in Bytes) of CRC Value
*/
int OrientalCRK525MAKD::appendCRCCheckValue( unsigned char byteBuffer[], int bufferSize, int numDefinedBytes )
{
	const int CRCByteSize = 2;

	if( bufferSize < numDefinedBytes + CRCByteSize )
	{
	  //Error, Passed Small Enough Buffer
	   return -1;
	}

	uint32_t errCheck = crcCompute( byteBuffer, numDefinedBytes );
	byteBuffer[ numDefinedBytes ] = errCheck;
	byteBuffer[ numDefinedBytes + 1 ] = (errCheck >> 8);

	return CRCByteSize;
}

int OrientalCRK525MAKD::onMultipleRegisterWriteResponse( const unsigned char txMsgBuffer[], const unsigned int txMsgLen, const unsigned char rxBuffer[], const unsigned int rxBufLen )
{

	int errCode;

	std::ostringstream os2;
	os2 << "The RX Packet is: \n";
	for( int i = 0; i < rxBufLen; i++)
	{
		os2 << (int) rxBuffer[i] << "\n";
	}
	AbstractControllerInterfaceFactory::LogMessage(  os2.str() );
	//Verify Register Address Number Written is the same
	if( txMsgBuffer[2] != rxBuffer[2] || txMsgBuffer[3] != rxBuffer[3] || txMsgBuffer[4] != rxBuffer[4] || txMsgBuffer[5] != rxBuffer[5] )
	{
		AbstractControllerInterfaceFactory::LogMessage(  "Bad Data Response" );
		//error data corruption
		return 1;
	}
	//Composite Address and number written
	uint16_t startRegAddress;
	ReadWrite< uint16_t, true >::write( &rxBuffer[2], sizeof( uint16_t ), startRegAddress);
	uint16_t numRegsWritten;
	ReadWrite< uint16_t, true >::write( &rxBuffer[4], sizeof( uint16_t ), numRegsWritten);

	AbstractRegisterBase* reg;
	int valueSize;

	for( uint16_t i = 7; i < numRegsWritten + 7; )
	{
			std::ostringstream os;
			os << "current Register is " << startRegAddress + i - 7;
			AbstractControllerInterfaceFactory::LogMessage(  os.str() );
		reg = GetRegisterByAddress( startRegAddress + i - 7 );
		if( reg == nullptr )
		{
			AbstractControllerInterfaceFactory::LogMessage(  "Nullptr Returned" );
			//Log Fatal Error: Unregistered Register Written
			return 1;
		}
		
		valueSize = reg->getRegisterByteSize();
		if( reg->write( &txMsgBuffer[i], valueSize ) != 0 )
		{
			//Error Reporting
		}

		//Increment i for Registers more than the base expected
		i += valueSize/baseRegisterByteSize_;

	}

	return 0;
}

int OrientalCRK525MAKD::onSingleRegisterWriteResponse( const unsigned char txMsgBuffer[], const unsigned int txMsgLen, const unsigned char rxBuffer[], const unsigned int rxBufLen )
{

	//Verify Register Address and Value Written is the Same
	if( txMsgBuffer[2] != rxBuffer[2] || txMsgBuffer[3] != rxBuffer[3] || txMsgBuffer[4] != rxBuffer[4] || txMsgBuffer[5] != rxBuffer[5] )
	{
		//error data corruption
		return 1;
	}

	//Composite Address and number written
	uint16_t startRegAddress = (rxBuffer[2] << 8) | rxBuffer[3];
	std::ostringstream os;
	os << "The large byte is " << (int) rxBuffer[2] << " and small is " << rxBuffer[3] << "\n";
	os << " The Register Address is supposed to be " << startRegAddress << "\n";
	AbstractControllerInterfaceFactory::LogMessage(  os.str() );
	AbstractRegisterBase* reg = GetRegisterByAddress( startRegAddress );
	assert( reg != nullptr);

	//Possible Error, Tried to Write MultiRegister...  Hmmm It would Still Be written...
	if( reg->write( &rxBuffer[4], baseRegisterByteSize_ ) == 1 )
	{
		//ReWrite With the Current Register Value... This is More Sane
		//If someone is Trying to optimize, It could be top and Bottom Writing...  No... Bad Standard...
		//Determine Rewrite Function
	}

	return 0;
}

int OrientalCRK525MAKD::onRegisterRead( const unsigned char txMsgBuffer[], const unsigned int txMsgLen, const unsigned char rxBuffer[], const unsigned int rxBufLen )
{
	static const int dataStartByte = 3; //Number of Bytes At Which Data Starts

	int numRegistersRead = ( txMsgBuffer[4] << 8 ) | txMsgBuffer[5];
	int startRegisterAddress = ( txMsgBuffer[2] << 8 ) | txMsgBuffer[3];
	int numDataBytes = rxBuffer[2];

	//Check to Make Sure Number of Data Bytes is Same as Number Read * baseRegisterValue_
	if( numDataBytes != numRegistersRead * baseRegisterByteSize_ )
	{
		//Log Error
		return 1;
	}

	std::ostringstream os;
	os << "RXMessage is:\n ";
	for( int i = 0; i< rxBufLen; i++)
	{
		os << "[" << i << "] = " << (unsigned int) rxBuffer[i] << "\n";
	}
	AbstractControllerInterfaceFactory::LogMessage( os.str() );

	AbstractRegisterBase* reg = GetRegisterByAddress( startRegisterAddress );
	int registerSize;
	
	for( int i = 0; i < numDataBytes; )
	{
		//Check to See if the Register Being Written is A combination
		registerSize = reg->getRegisterByteSize();
		int multiplier = registerSize / baseRegisterByteSize_;
		if( registerSize > baseRegisterByteSize_)
		{
			if( (registerSize % baseRegisterByteSize_) != 0 )
			{
				//Error, Register Being Read To is nonMultiple of Controller Type;
				return 2;
			}
			else if ( multiplier + i >= numDataBytes )  
			{
				//Error, Somehow we read half a RegisterCombination (Should be illegal)
				return 3;
			}
		}

		std::ostringstream os2;
		os2 << " The value is projected to be " << (((unsigned int) rxBuffer[i+1]) | ( rxBuffer[i] << 8 ));
		AbstractControllerInterfaceFactory::LogMessage( os2.str() );
		reg->write(	&rxBuffer[dataStartByte + i], baseRegisterByteSize_ * multiplier );
		i += baseRegisterByteSize_ * multiplier;
		reg = GetRegisterByAddress( startRegisterAddress + i );
	}

	return 0;
	
}
