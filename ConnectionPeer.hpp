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
 * Description: Definition for the class that implements the javascript
 *              ConnectionPeer object.
**/

#pragma once

/* firebreath headers */
#include "JSAPIAuto.h"
#include "JSObject.h"

/* web_p2p headers */
#include "ICEClient.hpp"

class ConnectionPeer : 
	public FB::JSAPIAuto,
	public ICEClient::Callbacks {
private:
    ICEClient iceClient;
    
    /* configuration properties */
    std::string serverConfiguration;
    std::string localConfiguration;
    std::vector<std::string> remoteConfigurations;
    
    FB::JSObjectPtr localConfigurationCallback;

    /* HTML5 Stream objects */
    //std::vector<FB:JSAPIPtr> localStreams;
    //std::vector<FB:JSAPIPtr> remoteStreams;
public:
    ConnectionPeer(const std::string& server_configuration);
    ~ConnectionPeer();
    
    virtual void setLocalCandidates(const std::string& localConfiguration);
    virtual void negotiationComplete();
    virtual void dataReceived(const std::string& text);
    
    // if second arg is true, then use unreliable low-latency transport 
    // (UDP-like), otherwise guarantee delivery (TCP-like)
    void sendText(const std::string& text, const /*optional*/ bool unimportant);
    void sendBitmap(const FB::JSAPIPtr& /*HTMLImageElement*/ image);
    void sendFile(const FB::JSAPIPtr& /*File*/ file);
    
    void addStream(const FB::JSAPIPtr& /*Stream*/ stream);
    void removeStream(const FB::JSAPIPtr& /*Stream*/ stream);
    
    FB::JSAPIPtr /*Stream[]*/ getLocalStreams();
    FB::JSAPIPtr /*Stream[]*/ getRemoteStreams();

    // maybe this should be in the constructor, or be an event
    void getLocalConfiguration(const FB::JSObjectPtr& callback);
    // remote origin is assumed to be same-origin if not specified.
    // If specified, has to match remote origin (checked in handshake).
    // Should support leading "*." to mean "any subdomain of".
    void addRemoteConfiguration(
    	const std::string& configuration
    	/*, const optional std::string& remoteOrigin*/);
    // disconnects and stops listening
    void close();
};

/*
[Callback=FunctionOnly, NoInterfaceObject]
interface ConnectionPeerConfigurationCallback {
    void handleEvent(in ConnectionPeer server, in DOMString configuration);
};*/