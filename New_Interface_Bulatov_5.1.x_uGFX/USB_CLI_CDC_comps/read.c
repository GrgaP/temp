/*******************
 *
 * Copyright 1998-2010 IAR Systems AB.  
 *
 * This is a template implementation of the "__read" function used by
 * the standard library.  Replace it with a system-specific
 * implementation.
 *
 * The "__read" function reads a number of bytes, at most "size" into
 * the memory area pointed to by "buffer".  It returns the number of
 * bytes read, 0 at the end of the file, or _LLIO_ERROR if failure
 * occurs.
 *
 * The template implementation below assumes that the application
 * provides the function "MyLowLevelGetchar".  It should return a
 * character value, or -1 on failure.
 *
 ********************/

#include <yfuns.h>
#include "IO_DriverInterface.h"

_STD_BEGIN

#pragma module_name = "?__read"



size_t __read(int handle, unsigned char * buffer, size_t size)
{
  /* Remove the #if #endif pair to enable the implementation */
 

  size_t readByte; 
  readByte = IO_ReadCallBack(handle, buffer, size);
  return (readByte);

}

_STD_END
