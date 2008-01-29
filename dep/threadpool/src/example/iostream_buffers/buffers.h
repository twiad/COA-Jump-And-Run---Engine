/*! \file
 * \brief iostreams buffer classes.
 *
 * This file contains some buffer implementations.
 *
 * Copyright (c) 2006 Philipp Henkel
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * http://threadpool.sourceforge.net
 *
 */


#include <boost/iostreams/concepts.hpp>
#include <boost/array.hpp>

namespace threadpool
{

template <std::streamsize PacketSize = 1024>
class packet_filter {
	
public:
    typedef char char_type;
    
    struct category 
    : boost::iostreams::output_filter_tag
    , boost::iostreams::multichar_tag
    , boost::iostreams::flushable_tag
    , boost::iostreams::closable_tag
    { };

	const static std::streamsize packet_size = PacketSize;

    explicit packet_filter()
    : m_usage(0)
    { }
    
    
    template<typename Sink>
    std::streamsize write(Sink& dest, const char* buffer, std::streamsize buffer_size)
    {
		if(!buffer || 0 == buffer_size)
		{
		  return 0;
		}

		std::streamsize remain = buffer_size;

		while(remain > 0)
		{
			if(0 != m_usage || remain < packet_size)
			{ // Fill packet buffer
			  std::streamsize copy = remain < packet_size-m_usage ? remain : packet_size-m_usage;
              std::memcpy(m_packet.begin() + m_usage, buffer + buffer_size - remain, copy);
     		  m_usage += copy;
				                                       
              if(packet_size == m_usage)
              { // Write complete packet
                std::streamsize out = boost::iostreams::write(dest, m_packet.begin(), packet_size);
                
                while(out < packet_size)
                { // Write error occurred
                  std::streamsize res = boost::iostreams::write(dest, m_packet.begin() + out, packet_size - out);
                  if(0 == res)
                  { // Write error occurred
                    m_usage = 0;
	                return buffer_size - remain + out;
                  }
                  out += res;
                }
                m_usage = 0;
              }
                  
              remain -= copy;
			  }
			  else
			  { // Write complete packet without buffering 
			    // m_usage is 0 and remaining >= packet_size
                std::streamsize out = boost::iostreams::write(dest, buffer + buffer_size - remain, packet_size);
                if(0 == out)
                { // Write error occurred
	              return buffer_size - remain;
                }
                remain -= out;
			  }
			}
	    
		
		return buffer_size;
    }


    template<typename Sink>
    void close(Sink& dest) 
    { 
       flush(dest);
    }
    
    
    template<typename Sink>
    bool flush(Sink& dest)
	{
		if(m_usage > 0)
		{
			std::streamsize out = boost::iostreams::write(dest, m_packet.begin(), m_usage);
            
            while(out < m_usage)
            { // Write error occurred
              std::streamsize res = boost::iostreams::write(dest, m_packet.begin() + out, m_usage - out);
              if(0 == res)
              { // Write error occurred
                m_usage = 0;
	            return false;
              }
              out += res;
            }
                      
            m_usage = 0;
		}
		return true;
	}

private:	
	boost::array<char, packet_size>			m_packet;	///< Packet buffer.
	std::streamsize							m_usage;	///< Fill level of the packet buffer.
};



} // namespace threadpool
