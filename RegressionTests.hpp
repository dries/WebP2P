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
 * Filename:    RegressionTests.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Definition for the class that implements the javascript
 *              regression test interface.
**/

#pragma once

/* firebreath headers */
#include "JSAPIAuto.h"
#include "JSObject.h"

class RegressionTests : 
	public FB::JSAPIAuto {
private:
    public:
    RegressionTests();
    ~RegressionTests();
    
    void runTests(const FB::JSObjectPtr& callback);
};