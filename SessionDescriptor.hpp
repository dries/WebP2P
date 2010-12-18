/**
 * This file is part of WebP2P.
 *
 * WebP2P is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WebP2P is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WebP2P.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    SessionDescriptor.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Definition of the SessionDescriptor class which implements a
 *              generic SDP message.
 * See:         http://tools.ietf.org/html/rfc4566
**/
 
#pragma once

/* WebP2P includes */
#include "SessionDescriptorData.hpp"

class SessionDescriptor {
    SessionDescriptorData data;
public:
    SessionDescriptor();
    SessionDescriptor(const std::string& sdpString) throw(std::exception);
    ~SessionDescriptor();	
    
    operator std::string();
    SessionDescriptorData& getData();
};
