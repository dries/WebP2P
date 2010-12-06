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
 * Filename:    ConnectionPeer.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the class that implements the javascript
 *              ConnectionPeer object.
 */

/* Boost includes */
#include <boost/make_shared.hpp>

/* Firebreath includes */
#include "variant_list.h"

/* WebP2P includes */
#include "ConnectionPeer.hpp"

ConnectionPeer::ConnectionPeer(const std::string& serverConfiguration) :
    iceClient(serverConfiguration) {
    this->serverConfiguration = std::string(serverConfiguration);
    
    registerMethod("sendText",
    	FB::make_method(this, &ConnectionPeer::sendText));
    registerEvent("ontext");
    registerMethod("sendBitmap",
    	FB::make_method(this, &ConnectionPeer::sendBitmap));
    registerEvent("onbitmap");
    registerMethod("sendFile",
    	FB::make_method(this, &ConnectionPeer::sendFile));
    registerEvent("onfile");
    registerMethod("addStream",
		FB::make_method(this, &ConnectionPeer::addStream));
    registerMethod("removeStream",
    	FB::make_method(this, &ConnectionPeer::removeStream));
    registerEvent("onstream");

    registerProperty("localStreams",
    	FB::make_property(this, &ConnectionPeer::getLocalStreams));
    registerProperty("remoteStreams",
    	FB::make_property(this, &ConnectionPeer::getRemoteStreams));

    registerMethod("getLocalConfiguration",
    	FB::make_method(this, &ConnectionPeer::getLocalConfiguration));
    registerMethod("addRemoteConfiguration",
    	FB::make_method(this, &ConnectionPeer::addRemoteConfiguration));
    registerMethod("close",
    	FB::make_method(this, &ConnectionPeer::close));
                   
    registerEvent("onconnect");
    registerEvent("onerror");
    registerEvent("ondisconnect");

    iceClient.setLocalCandidateCallback(this);
}
ConnectionPeer::~ConnectionPeer() {
}

// if second arg is true, then use unreliable low-latency transport (UDP-like),
// otherwise guarantee delivery (TCP-like)
void ConnectionPeer::sendText(
	const std::string& text,
	const /*optional*/ bool unimportant) {
}
void ConnectionPeer::sendBitmap(
	const FB::JSAPIPtr& /*HTMLImageElement*/ image) {
}
void ConnectionPeer::sendFile(
	const FB::JSAPIPtr& /*File*/ file) {
}
void ConnectionPeer::addStream(
	const FB::JSAPIPtr& /*Stream*/ stream) {
}
void ConnectionPeer::removeStream(
	const FB::JSAPIPtr& /*Stream*/ stream) {
}

FB::JSAPIPtr /*Stream[]*/ ConnectionPeer::getLocalStreams() {
    return FB::JSAPIPtr(static_cast<JSAPI*>(NULL));
}
FB::JSAPIPtr /*Stream[]*/ ConnectionPeer::getRemoteStreams() {
    return FB::JSAPIPtr(static_cast<JSAPI*>(NULL));
}

void ConnectionPeer::setLocalCandidates(
	const std::string& localConfiguration) {
    this->localConfiguration = localConfiguration;
    localConfigurationCallback->Invoke(
    	"", FB::variant_list_of(localConfiguration));
}

void ConnectionPeer::getLocalConfiguration(
	const FB::JSObjectPtr& callback) {
    localConfigurationCallback = callback;
    if(!localConfiguration.empty()) {
        localConfigurationCallback->Invoke(
    	"", FB::variant_list_of(localConfiguration));
    }
}
// remote origin is assumed to be same-origin if not specified.
// If specified, has to match remote origin (checked in handshake).
// Should support leading "*." to mean "any subdomain of".
void ConnectionPeer::addRemoteConfiguration(
	const std::string& configuration/*,
	const optional std::string& remoteOrigin*/) {
    iceClient.addRemoteCandidates(configuration);
}
// disconnects and stops listening
void ConnectionPeer::close(){
    FireEvent("ondisconnect", FB::variant_list_of(true));
}