/******************************************************************************
 *                         rtils - rTi's Utilities                            *
 *                     Copyright (C) 2008  Franz Köhler                       *
 *                     Copyright (C) 2008  Robert Timm                        *
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

#ifndef _RTILS_LOCKFREE_DYNAMIC_QUEUE_
#define _RTILS_LOCKFREE_DYNAMIC_QUEUE_


// #include "IdManager.h"

namespace rtils
{

template<class T>
class LockfreeDynamicQueue
{
    class Entry
    {
        Entry* m_next;
        
        T m_data;

    public:
        Entry(const T& data, Entry* next = 0) : 
            m_next(next), 
            m_data(data)
        {
            
        }
        
        virtual ~Entry()
        {
            
        }
        
        inline Entry* next()
        {
            return m_next;
        }

        inline void setNext(Entry* next)
        {
            m_next = next;
        }
        
        inline T& data()
        {
            return m_data;
        }
    };

    Entry* m_head;
    Entry* m_tail;
    Entry* m_root;
    size_t m_size;
    bool headWait;

public:
    LockfreeDynamicQueue() : m_head(0), m_tail(0), m_root(0), m_size(0)
    {
        m_root = new Entry(0);
        m_root->setNext(m_root);
        m_head = m_root;
        m_tail = m_root;
        headWait = false;
    }
    
    virtual ~LockfreeDynamicQueue()
    {
        assert(!m_size && "destructing not empty queue");
    }
    
    bool push(const T& p)
    {
    	if(m_head == m_tail) 
    	{
    		m_tail = m_root;
    		m_root->setNext(m_root);
    	}
    	
    	
    	Entry* tail = new Entry(p);
        if(empty())
        {
            tail->setNext(m_root);
            m_root->setNext(tail);
            m_tail = tail;
        }
        else
        {
            tail->setNext(m_tail->next());
            m_tail->setNext(tail);
            m_tail = tail;
        }
        
        return true;
    }
    
    bool pop(T& p)
    {
        if(headWait)
        {
        	if(m_head == m_tail) 
        	{
        		return false;
        	}
        	else
        	{
        		Entry* t = m_head;
        		m_head = t->next();
        		delete t;
        		headWait = false;
        	}
        }
        
        if(empty())
        	return false;
        
        if(m_head == m_root) 
        {
        	m_head = m_root->next();
        }
        if(m_head == m_tail)
        {
        	headWait = true;
        	p = m_head->data();
        }
        else 
        {
        	Entry* t = m_head;
        	m_head = t->next();
        
        	p = t->data();
        
        	delete t;
        }

        return true;
    }
    
    bool empty()
    {
    	if((m_root->next() == m_root))
    	{
    		return(true);
    	}
    	else return(false); 	
    }
};

}


#endif
