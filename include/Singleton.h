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

#ifndef _RTILS_SINGLETON_H_
#define _RTILS_SINGLETON_H_

namespace rtils
{

/**
 * Singleton class used as base class!
 * @param T Any type
 */
template<typename T>
class Singleton
{
    static T *m_singleton;

public:
    /**
     * set a preinitiated singleton object instance.
     *
     * @p       p_object pointer to the initialised object instance.
     * @return  whether this operation was successful.
     * @note    only possible before any other use of this singleton,
     *          else it will fail and return false.
     */
    static bool preinit(T* p_object)
    {
        if(m_singleton)
            return false;

        m_singleton = p_object;
        return true;
    }

    /**
     * get the singleton
     * @return  The actuall data
     */
    static T& get()
    {
        if(!m_singleton)
            m_singleton = new T;

        assert(m_singleton);
        return *m_singleton;
    }

    /**
     * get the singleton ptr
     * @return  The actual data as singleton
     */
    static T* getPtr()
    {
        if(!m_singleton)
            m_singleton = new T;

        assert(m_singleton);
        return m_singleton;
    }
};

template <typename T> T* Singleton <T>::m_singleton = NULL;

}

#endif /* _RTILS_SINGLETON_H_ */
