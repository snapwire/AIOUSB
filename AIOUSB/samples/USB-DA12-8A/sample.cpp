/*
 * $RCSfile: sample.cpp,v $
 * $Revision: 1.1 $
 * $Date: 2010/01/29 23:00:04 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * AIOUSB library sample program
 */


// {{{ notes and build instructions
/*
 * This source code looks best with a tab width of 4.
 *
 * All the API functions that DO NOT begin "AIOUSB_" are standard API functions, largely
 * documented in http://accesio.com/MANUALS/AIOUSB_API_Reference.html. The functions
 * that DO begin with "AIOUSB_" are "extended" API functions added to the Linux
 * implementation. Source code lines in this sample program that are prefixed with the
 * comment "/ * API * /" highlight calls to the AIOUSB API.
 *
 * LIBUSB (http://www.libusb.org/) must be installed on the Linux box (the AIOUSB code
 * was developed using libusb version 1.0.3). After installing libusb, it may also be
 * necessary to set an environment variable so that the libusb and aiousb header files can
 * be located:
 *
 *     export CPATH=/usr/local/include/libusb-1.0/:/usr/local/include/aiousb/
 *
 * Once libusb is installed properly, it should be possible to compile the sample program
 * using the simple command:
 *
 *     make
 *
 * Alternatively, one can "manually" compile the sample program using the command:
 *
 *     g++ sample.cpp -laiousbcpp -lusb-1.0 -o sample
 *
 * or, to enable debug features
 *
 *     g++ -ggdb sample.cpp -laiousbcppdbg -lusb-1.0 -o sample
 */
// }}}

// {{{ includes
#include <aiousb.h>
#include <stdio.h>
using namespace AIOUSB;
// }}}

int main( int argc, char **argv ) {
	printf(
		"USB-DA12-8A sample program version 1.1, 29 January 2010\n"
		"  AIOUSB library version %s, %s\n"
		"  This program demonstrates controlling a USB-DA12-8A device on\n"
		"  the USB bus. For simplicity, it uses the first such device found\n"
		"  on the bus.\n"
/*API*/	, AIOUSB_GetVersion(), AIOUSB_GetVersionDate()
	);

	/*
	 * MUST call AIOUSB_Init() before any meaningful AIOUSB functions;
	 * AIOUSB_GetVersion() above is an exception
	 */
/*API*/	unsigned long result = AIOUSB_Init();
	if( result == AIOUSB_SUCCESS ) {
		/*
		 * call GetDevices() to obtain "list" of devices found on the bus
		 */
/*API*/	unsigned long deviceMask = GetDevices();
		if( deviceMask != 0 ) {
			/*
			 * at least one ACCES device detected, but we want one of a specific type
			 */
/*API*/		AIOUSB_ListDevices();				// print list of all devices found on the bus
			const int MAX_NAME_SIZE = 20;
			char name[ MAX_NAME_SIZE + 2 ];
			unsigned long productID, nameSize, numDIOBytes, numCounters;
			unsigned long deviceIndex = 0;
			bool deviceFound = false;
			while( deviceMask != 0 ) {
				if( ( deviceMask & 1 ) != 0 ) {
					// found a device, but is it the correct type?
					nameSize = MAX_NAME_SIZE;
/*API*/				result = QueryDeviceInfo( deviceIndex, &productID, &nameSize, name, &numDIOBytes, &numCounters );
					if( result == AIOUSB_SUCCESS ) {
						if(
							productID >= USB_DA12_8A_REV_A
							&& productID <= USB_DA12_8E
						) {
							// found a USB-DA12-8A/E family device
							deviceFound = true;
							break;				// from while()
						}	// if( productID ...
					} else
						printf( "Error '%s' querying device at index %lu\n"
/*API*/						, AIOUSB_GetResultCodeAsString( result ), deviceIndex );
				}	// if( ( deviceMask ...
				deviceIndex++;
				deviceMask >>= 1;
			}	// while( deviceMask ...
			if( deviceFound ) {
/*API*/			AIOUSB_SetCommTimeout( deviceIndex, 500 );

				__uint64_t serialNumber;
/*API*/			result = GetDeviceSerialNumber( deviceIndex, &serialNumber );
				if( result == AIOUSB_SUCCESS )
					printf( "Serial number of device at index %lu: %llx\n", deviceIndex, ( long long ) serialNumber );
				else
					printf( "Error '%s' getting serial number of device at index %lu\n"
/*API*/					, AIOUSB_GetResultCodeAsString( result ), deviceIndex );

				/*
				 * demonstrate getting enhanced device properties
				 */
				DeviceProperties properties;
/*API*/			result = AIOUSB_GetDeviceProperties( deviceIndex, &properties );
				if( result == AIOUSB_SUCCESS )
					printf( "Device properties successfully retrieved\n" );
				else {
					const int MAX_CHANNELS = 8;
					properties.DACChannels = MAX_CHANNELS;
					printf( "Error '%s' getting device properties\n"
/*API*/					, AIOUSB_GetResultCodeAsString( result ) );
				}	// if( result ...

				/*
				 * demonstrate writing to one D/A channel
				 */
				const int TEST_CHANNEL = 0;			// channels are numbered 0 to properties.DACChannels - 1
				const unsigned MAX_COUNTS = 0xfff;
				const unsigned counts = MAX_COUNTS / 2;	// half of full scale
/*API*/			result = DACDirect( deviceIndex, TEST_CHANNEL, counts );
				if( result == AIOUSB_SUCCESS )
					printf( "%u D/A counts successfully output to channel %d\n", counts, TEST_CHANNEL );
				else
					printf( "Error '%s' attempting to output %u D/A counts successfully to channel %d\n"
/*API*/					, AIOUSB_GetResultCodeAsString( result )
						, counts, TEST_CHANNEL );

				/*
				 * demonstrate writing to multiple D/A channels
				 */
				unsigned short dacData[ properties.DACChannels * 2 ];		// channel/count pairs
				for( int channel = 0; channel < ( int ) properties.DACChannels; channel++ ) {
					dacData[ channel * 2 ] = channel;
					dacData[ channel * 2 + 1 ] = ( unsigned short ) (
						( unsigned long ) ( channel + 1 )
						* ( unsigned long ) MAX_COUNTS
						/ properties.DACChannels
					);
				}	// for( int channel ...
/*API*/			result = DACMultiDirect( deviceIndex, dacData, properties.DACChannels );
				if( result == AIOUSB_SUCCESS )
					printf( "D/A counts successfully output to %u channels simultaneously\n", properties.DACChannels );
				else
					printf( "Error '%s' attempting to output D/A counts to %u channels simultaneously\n"
/*API*/					, AIOUSB_GetResultCodeAsString( result )
						, properties.DACChannels );
			} else
				printf( "Failed to find USB-DA12-8A device\n" );
		} else
			printf( "No ACCES devices found on USB bus\n" );

		/*
		 * MUST call AIOUSB_Exit() before program exits,
		 * but only if AIOUSB_Init() succeeded
		 */
/*API*/	AIOUSB_Exit();
	}	// if( result ...
	return ( int ) result;
}	// main()


/* end of file */
