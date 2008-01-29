/*! \file
 * \brief threadpool buffer classes.
 *
 * This file contains a buffer implementation.
 *
 * Copyright (c) 2005-2006 Philipp Henkel
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * http://threadpool.sourceforge.net
 *
 */


#if (_MSC_VER >= 1400)    // VC8+
#pragma warning(disable : 4996)  // Disable all deprecation warnings
#endif // VC8+ 

#include "buffers.h"

#include "threadpool.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <boost/iostreams/filter/test.hpp>


#include <ostream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>




using namespace std;
using namespace threadpool;
using namespace boost::iostreams;

namespace io = boost::iostreams;



//
// Helpers
boost::mutex m_io_monitor;

void print(string text)
{
  boost::mutex::scoped_lock lock(m_io_monitor);
  cout << text;
}

template<class T>
string to_string(const T& value)
{
  ostringstream ost;
  ost << value;
  ost.flush();
  return ost.str();
}


// Pseudo-random number generator (linear congruential)
// Programming languages - C, ANSI/ISO/IEC 9899-1999, pg 311
static unsigned long next = 1;
int myrand(void)  /* RAND_MAX assumed to be 32767. */
{
    next = next * 1103515245 + 12345;
    return((unsigned)(next/65536) % 32768);
}

void mysrand(unsigned seed)
{
    next = seed;
}

template<typename Sink>
bool dump_random_data(Sink& dest1, Sink& dest2, const std::streamsize total_bytes, const std::streamsize max_chunk_size)
{
  mysrand(42);
  boost::scoped_array<char> chunk(new char[max_chunk_size]);
  for(int i=0; i<max_chunk_size; i++)
  {
  	chunk[i] = 97 + myrand() % 26;
  }
	
  std::streamsize remain = total_bytes;
  
  while(remain > 0)
  {
    std::streamsize chunk_size = 1 + (1023*myrand()) % max_chunk_size;
    chunk_size = remain < chunk_size ? remain : chunk_size;
	if(io::write(dest1, chunk.get(), chunk_size) == 0)
  	{
  	  return false;
    }
	
	if(io::write(dest2, chunk.get(), chunk_size) == 0)
  	{
  	  return false;
    }
  	remain -= chunk_size;
  }
  
  io::flush(dest1);
  io::flush(dest2);
  
  return true;
}


template<typename Filter>
bool test_output_filter_with_random_data(Filter out, const std::streamsize total_bytes, const std::streamsize max_chunk_size)
{
    // Create filter chain with out filter
    boost::scoped_array<char> filtered_buffer (new char[total_bytes]);
    memset(filtered_buffer.get(), 0, total_bytes);
	io::filtering_stream<io::output> filtered; 
    filtered.push(out);
    filtered.push(io::array_sink(filtered_buffer.get(), total_bytes));   
 
 	// Create filter chain for reference data without a filter
    boost::scoped_array<char> original_buffer (new char[total_bytes]);
    memset(original_buffer.get(), 0, total_bytes);
    io::filtering_stream<io::output> original; 
    original.push(io::array_sink(original_buffer.get(), total_bytes));  
   
    // Push random data into the filter chains
    if(!dump_random_data(filtered, original, total_bytes, max_chunk_size))
	{
		return false;
	}

	// Compare the filter's output
    return 0 == std::memcmp(original_buffer.get(), filtered_buffer.get(), total_bytes);
    
    /*  if(0 == std::memcmp(original_buffer.get(), filtered_buffer.get(), total_bytes))
	{
	  print("SUCCESS random test\n");
	}
	else
	{
		print("ERROR random test\n");
		std::string str1(original_buffer.get(), total_bytes);
		std::cout << str1 <<endl;
		std::string str2(filtered_buffer.get(), total_bytes);
		std::cout << str2 <<endl;
	}  
   
   */
}




const std::string lower = 
    "in addition to providing an abstract framework the "
    "library provides a number of concrete filters, sources "
    "and sinks which serve as example applications of the "
    "library but are also useful in their own right. these "
    "include components for accessing memory-mapped files, "
    "for file access via operating system file descriptors, "
    "for code conversion, for text filtering with regular "
    "expressions, for line-ending conversion and for "
    "compression and decompression in the zlib, gzip and "
    "bzip2 formats.";

int main (int argc, char * const argv[]) {
	
	print("Filter tests\n");
    


    if(test_output_filter(packet_filter<1>(), lower, lower))
   	{
   		print("Success\n");
   	}
   	else
   	{
   		print("Error\n");
   	}	
  	
   	if(test_output_filter(packet_filter<7>(), lower, lower))
   	{
   		print("Success\n");
   	}
   	else
   	{
   		print("Error\n");
   	}	
   
   
    if(test_output_filter_with_random_data(packet_filter<5>(), 1024*1024, 23))
	{
	  print("SUCCESS random test\n");
	}
	else
	{
	  print("ERROR random test");
	}
	
   
    return 0;
}
