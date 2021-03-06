/*
 * $RCSfile: DeviceSubsystem.cpp,v $
 * $Revision: 1.6 $
 * $Date: 2009/12/19 00:27:36 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class DeviceSubsystem implementation
 */


#include "DeviceSubsystem.hpp"
#include <assert.h>


namespace AIOUSB {

DeviceSubsystem::DeviceSubsystem( USBDevice &parent ) {
	assert( &parent != 0 );
	this->parent = &parent;
}	// DeviceSubsystem::DeviceSubsystem()

DeviceSubsystem::~DeviceSubsystem() {
	assert( parent != 0 );
}	// DeviceSubsystem::~DeviceSubsystem()

int DeviceSubsystem::getDeviceIndex() const {
	assert( parent != 0 );
	return parent->getDeviceIndex();
}	// DeviceSubsystem::getDeviceIndex()

}	// namespace AIOUSB

/* end of file */
