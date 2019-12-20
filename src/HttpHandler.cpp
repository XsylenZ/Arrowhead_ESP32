/**
 * Created by Nick van Riet on 7/11/19.
 * Copyright (c) 2019 Nick van Riet.
 * 
 * Released under the MIT license: license.md
 * 
 * [Information]
 */

#include "HttpHandler.h"

HttpHandler::HttpHandler()
{
}

HttpHandler::~HttpHandler()
{

}

int HttpHandler::send( const String url, const String contentType, const HTTP_Method_t method,
                       const String body )
{
    _response = "";

    HTTPClient httpClient;
    httpClient.setConnectTimeout( _connectionTimeOut );
    httpClient.begin( url );
    httpClient.addHeader( "Content-Type", contentType ); //Specify content-type header

    int httpResponseCode = 0;
    switch ( method )
    {
        case POST:
            httpResponseCode = httpClient.POST( body );
            break;
        case PUT:
            httpResponseCode = httpClient.PUT( body );
            break;
        case DELETE:
            httpResponseCode = httpClient.sendRequest( "DELETE", body );
            break;
        case GET:
            httpResponseCode = httpClient.GET();
            break;
    }

    if ( httpResponseCode > 0 )
    {
        _response = httpClient.getString();
    }

    if ( httpResponseCode <= 0 )
    {
        Serial.println( "[DEBUG_HttpHandler_AHF] responseCode: " + String( httpResponseCode ) );
    }


#ifdef DEBUG_HTTPHANDLER
    Serial.println( "[DEBUG HttpHandler] URL: " );
    Serial.println(url);
    Serial.println( "[DEBUG HttpRest] Body: " );
    Serial.println(body);
    Serial.print( "[DEBUG HttpHandler] HTTP Response code is: " );
    Serial.println( httpResponseCode );
    Serial.println( "[DEBUG HttpHandler] HTTP Response message is: " );
    Serial.println( _response );
#endif

    httpClient.end();
    return httpResponseCode;
}

int HttpHandler::send( const String url, const String contentType, const HTTP_Method_t method )
{
    return send( url, contentType, method, "" );
}

String HttpHandler::getResponse()
{
    String result = _response;
    _response = "";
    return result;
}

void HttpHandler::setConnectionTimeOut( int connectionTimeOut )
{
    _connectionTimeOut = connectionTimeOut;
}
