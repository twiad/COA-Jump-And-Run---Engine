/******************************************************************************
 *                         rtils - rTi's Utilities                            *
 *                     Copyright (C) 2007  Robert Timm                        *
 *                   Contact............rtti@elane2k.com                      *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/

#ifndef _RTILS_LOCKFREE_QUEUE_
#define _RTILS_LOCKFREE_QUEUE_

#include <iostream>

namespace rtils
{

/**
 * Lock free thread safe queue for one reader and one writer.
 *
 * This queue is thread safe as long as there is only one reader and one writer.
 * It has a static size which has to be provided at initialization.
 */
template<class T>
class LockfreeQueue
{
    /// the size of the ring buffer
    size_t m_size;
    
    /// read position
    size_t m_head;

    /// write position
    size_t m_tail;

    /// pointer to the actual memory location of the ring buffer
    T* m_buffer;

public:
    /**
     * ctor.
     *
     * Allocate and initialize the queue.
     *
     * @ param p_size the amount of elements the queue can hold.
     */
    LockfreeQueue(const size_t p_size)
    {
        m_size = p_size;
        
        m_head = 0;
        m_tail = 0;
        
        m_buffer = (T*)malloc(m_size * sizeof(T));
    }
    
    /**
     * dtor.
     *
     * free resources.
     */
    virtual ~LockfreeQueue()
    {
        free(m_buffer);
    }
    
    /**
     * push an element at the back of the queue.
     * 
     * @param p const reference to the object to queue.
     * @return false on queue full, true on success.
     */
    bool push(const T& p)
    {
        size_t tail = m_tail;
        
        // copy value into buffer
        m_buffer[tail++] = p;
        
        // ring buffer
        if(tail == m_size)
            tail = 0;
            
        // is the list if full, cannot push
        if(tail == m_head)
        {
            // std::cerr << "LockfreeQueue: queue full, cannot push" << std::endl;
            return false;
        }
        
        m_tail = tail;
        
        return true;
    }
    
    /**
     * pop an element from the back head of the queue.
     * 
     * @param p reference to a memory location that can hold the pop'd object.
     * @return false on queue empty, true on success.
     */
    bool pop(T& p)
    {
        size_t head = m_head;
        
        // if the list is empty, cannot pop
        if(head == m_tail)
            return false;

        // copy value from buffer
        p = m_buffer[head++];
        
        // ring buffer
        if(head == m_size)
            head = 0;

        m_head = head;
        
        return true;
    }
};

}

#endif
