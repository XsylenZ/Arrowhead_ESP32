/**
 * Created by Nick van Riet on 7/11/19.
 * Copyright (c) 2019 Nick van Riet.
 * 
 * Released under the MIT license: license.md
 * 
 * [Information]
 */

//json parser library: https://arduinojson.org
//NOTE: using version 5.13.1, since 6.x is in beta (but the Arduino IDE updates it!)
//Make sure you are using 5.13.x within the Package Manager!
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "HttpHandler.h"
#include "ArrowheadCore.h"

const int kSixtySeconds = 60;

ArrowheadCore::ArrowheadCore( const String address )
: _kAddress( address )
{
    AHF_Core_Service_Ports_t ports = {};
    _ports = ports;
}

ArrowheadCore::ArrowheadCore( const String address, AHF_Core_Service_Ports_t ports )
: _kAddress( address ), _ports( ports )
{

}

ArrowheadCore::~ArrowheadCore()
{

}

bool ArrowheadCore::orchestrationRequest( AHF_Orchestration_Info_t orchestrationInfo,
                                          String* endpoint )
{

    AHF_Orchestration_Flags_t flags = {};
    return orchestrationRequest( orchestrationInfo, flags, endpoint );
}

bool ArrowheadCore::orchestrationRequest( AHF_Orchestration_Info_t orchestrationInfo,
                                          AHF_Orchestration_Flags_t flags, String* endpoint )
{
    String serviceEndpoint = "http://";

    String jsonRequest = _createEntryFrom( orchestrationInfo, flags );
    if ( _orchestrationSend( jsonRequest, &serviceEndpoint ) )
    {
        *endpoint = serviceEndpoint;
        return true;
    }
    return false;
}

bool ArrowheadCore::registryAdd( AHF_Registration_Info_t registrationInfo )
{
    return registryAdd( registrationInfo, 0 );
}

bool ArrowheadCore::registryAdd( AHF_Registration_Info_t registrationInfo, int minutesAlive )
{
    String jsonString = _createEntryFrom( registrationInfo, minutesAlive );
    return _serviceRegister( jsonString );
}

bool ArrowheadCore::registryRemove( AHF_Registration_Info_t registrationInfo )
{
    String jsonString = _createEntryFrom( registrationInfo );
    return _serviceUnregister( jsonString );
}

String ArrowheadCore::_createEntryFrom( AHF_Registration_Info_t registrationInfo,
                                        unsigned int minutesAlive )
{
    struct tm timeinfo;
    getLocalTime( &timeinfo );
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& providedService = root.createNestedObject( "providedService" );
    providedService["serviceDefinition"] = registrationInfo.service.definition;

    //TODO: Keep metadata for lib purpose or remove it for this project
//    JsonObject& metadata = providedService.createNestedObject( "serviceMetadata" );

    JsonArray& interfaces = providedService.createNestedArray( "interfaces" );
    interfaces.add( registrationInfo.service.interface );

    //TODO: add service metadata if you need!
//    metadata["unit"] = "celsius";

    root.createNestedObject( "provider" );
    String name = registrationInfo.provider.name;
    name.replace( " ", "_" );
    root["provider"]["systemName"] = name;
    root["provider"]["address"] = registrationInfo.provider.ip;
    root["provider"]["port"] = registrationInfo.provider.port;
    root["serviceURI"] = registrationInfo.provider.uri;

    //When time is given the Framework will not use this provider when exceeded given time!
    if ( minutesAlive != 0 )
    {
        time_t now = time( NULL );
        now += ( minutesAlive * kSixtySeconds );

        tm endtime = *localtime( &now );
        char endOfValidityString[32];
        strftime( endOfValidityString, sizeof endOfValidityString, "%FT%T", &endtime );
        root["endOfValidity"] = endOfValidityString;
    }

    root["version"] = registrationInfo.service.version;

    String serviceEntry;

    root.prettyPrintTo( serviceEntry );

#ifdef DEBUG_ARROWHEAD
    Serial.println( "Sending register request with the following payload:" );
    Serial.println( serviceEntry );
#endif

    return serviceEntry;
}

String ArrowheadCore::_createEntryFrom( AHF_Orchestration_Info_t orchestrationInfo,
                                        AHF_Orchestration_Flags_t flags )
{
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    JsonObject& requesterSystem = root.createNestedObject( "requesterSystem" );
    String name = orchestrationInfo.consumer.name;
    name.replace( " ", "_" );
    requesterSystem["systemName"] = name;
    requesterSystem["address"] = orchestrationInfo.consumer.ip;
    requesterSystem["port"] = orchestrationInfo.consumer.port;

    JsonObject& requestedService = root.createNestedObject( "requestedService" );
    requestedService["serviceDefinition"] = orchestrationInfo.service.definition;
    JsonArray& interfaces = requestedService.createNestedArray( "interfaces" );
    interfaces.add( orchestrationInfo.service.interface );

    //TODO: Keep metadata for lib purpose or remove it for this project
//    JsonObject& metadata = requestedService.createNestedObject( "serviceMetadata" );

    JsonObject& orchestrationFlags = root.createNestedObject( "orchestrationFlags" );

    //TODO: add service metadata if you need!
//    metadata["unit"] = "celsius";
    orchestrationFlags["overrideStore"] = flags.overrideStore;
    orchestrationFlags["metadataSearch"] = flags.metadataSearch;
    orchestrationFlags["enableInterCloud"] = flags.enableIntercloud;
    orchestrationFlags["pingProviders"] = flags.pingProviders;

    //Note: Not all flags need to be in the Service Request Form, only the ones that are set to
    // true, since all non-present flags will default to false value.

    String orchRequest = "";
    root.prettyPrintTo( orchRequest );

#ifdef DEBUG_ARROWHEAD
    Serial.println( "Sending register request with the following payload:" );
    Serial.println( orchRequest );
#endif

    return orchRequest;
}

bool ArrowheadCore::_orchestrationSend( String orchRequest, String* providerEndpoint )
{
    String endpoint = _createEndpoint( _kAddress,
                                       _ports.orchestration,
                                       "/orchestrator/orchestration" );
    String httpResponseMessage = "";
    HttpHandler httpHandler;

    int httpResponseCode = httpHandler.send( endpoint, "application/json", POST, orchRequest );
    httpResponseMessage = httpHandler.getResponse();

    Serial.print( "Orchestration completed, with status code: " );
    Serial.println( httpResponseCode );

    if ( httpResponseCode < 0 )
    {
        Serial.println( "Orchestrator not available!" );
        Serial.println( httpResponseMessage );
        return false;
    }

    //parsing orchestration response
    StaticJsonBuffer<2000> jsonBuffer; //Memory pool
    JsonObject& root = jsonBuffer.parseObject( httpResponseMessage );

    if ( httpResponseCode == HTTP_CODE_OK )
    {
        const char* address = root["response"][0]["provider"]["address"];
        const char* port = root["response"][0]["provider"]["port"];
        const char* uri = root["response"][0]["serviceURI"];
        *providerEndpoint = "http://" + String( address ) + ":" + String( port ) + String( uri );
        Serial.println( "Received endpoint: " + *providerEndpoint );
        return true;
    }
    Serial.print( "Orchestration failed with response:" );
    Serial.println( httpResponseMessage );
    return false;
}

bool ArrowheadCore::_serviceRegister( String serviceEntry )
{
    if ( serviceEntry == "" )
    {
        return false;
    }

    bool result = true;

    String endpoint = _createEndpoint( _kAddress, _ports.regisrty, "/serviceregistry/register" );
    String httpResponseMessage = "";
    HttpHandler httpHandler;
    int httpResponseCode = httpHandler.send( endpoint, "application/json", POST, serviceEntry );
    httpResponseMessage = httpHandler.getResponse();

    Serial.print( "Registered to Service Registry with status code:" );
    Serial.println( httpResponseCode );

    if ( httpResponseCode < 0 )
    {
        Serial.println( "ServiceRegistry not available!" );
        return false;
    }

    if ( httpResponseCode != HTTP_CODE_CREATED )
    {
        //SR responded properly, check if registration was successful
        Serial.println( "Service registration failed with response:" );
        Serial.println( httpResponseMessage );

        //need to remove our previous entry and then re-register
        if ( !_serviceUnregister( serviceEntry ) )
        {
            return false;
        }
        httpResponseCode = httpHandler.send( endpoint, "application/json", POST, serviceEntry );
        httpResponseMessage = httpHandler.getResponse();
        Serial.print( "Re-registered with status code:" );
        Serial.println( httpResponseCode );

#ifdef DEBUG_SERVICE_REGISTRY
        Serial.print( "Remove response:" );
        Serial.println( httpResponseMessage );
#endif
    }

    return result;
}

bool ArrowheadCore::_serviceUnregister( String serviceEntry )
{

    if ( serviceEntry == "" )
    {
        return false;
    }

    bool result = true;

    String endpoint = _createEndpoint( _kAddress, _ports.regisrty, "/serviceregistry/remove" );
    String httpResponseMessage = "";
    HttpHandler httpHandler;
    int httpResponseCode = httpHandler.send( endpoint, "application/json", PUT, serviceEntry );
    httpResponseMessage = httpHandler.getResponse();
    Serial.print( "Removed previous entry with status code:" );
    Serial.println( httpResponseCode );

#ifdef DEBUG_ARROWHEAD
    Serial.print( "Remove response:" );
    Serial.println( httpResponseMessage );
#endif

    if ( httpResponseCode != HTTP_CODE_OK )
    {
        result = false;
    }

    return result;
}

String ArrowheadCore::_createEndpoint( String ip, int port, String url )
{
    return String( ip + ":" + String( ip ) + url );
}

