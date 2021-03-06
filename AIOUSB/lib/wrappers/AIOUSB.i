
%module AIOUSB
%include "cpointer.i"


/* %inline { */
/*   extern unsigned long AIOUSB_GetStreamingBlockSize(unsigned long DeviceIndex, unsigned long *BlockSize ); */
/* } */
%pointer_functions( unsigned long,  ulp );
%pointer_functions( unsigned short, usp );
%pointer_functions( double , dp );


%include typemaps.i
%apply unsigned long *INOUT { unsigned long *result };
%{
/*   extern unsigned long AIOUSB_GetStreamingBlockSize(unsigned long DeviceIndex, unsigned long *BlockSize ); */
  extern unsigned long ADC_BulkPoll( unsigned long DeviceIndex, unsigned long *INOUT );
%}


%{
  #include "AIOUSB_Core.h"
  #include "aiousb.h"
  #include "libusb.h"

%}

%newobject CreateSmartBuffer;
%newobject NewBuffer;
%delobject AIOBuf::DeleteBuffer;

%include "AIOUSB_Core.h"
%include "aiousb.h"



%extend AIOBuf {
  AIOBuf(int bufsize)  {
    return (AIOBuf *)NewBuffer( bufsize );
  }
  ~AIOBuf()  {
    DeleteBuffer($self);
  }

}




