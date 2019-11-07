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
//    httpClient.setConnectTimeout( _connectionTimeOut );
    httpClient.begin( url );
    httpClient.addHeader( "Content-Type", contentType ); //Specify content-type header

    int httpResponseCode = 0;
    switch ( method )
    {
        case POST:
            httpResponseCode = httpClient.sendRequest( "POST", body );
            break;
        case PUT:
            httpResponseCode = httpClient.sendRequest( "PUT", body );
            break;
        case DELETE:
            httpResponseCode = httpClient.sendRequest( "DELETE", body );
            break;
        case GET:
            httpResponseCode = httpClient.sendRequest( "GET" );
            break;
    }

    _response = httpClient.getString();

#ifdef DEBUG_HTTPREST
    Serial.println( "[DEBUG HttpRest] URL: " );
    Serial.println(url);
    Serial.println( "[DEBUG HttpRest] Body: " );
    Serial.println(body);
    Serial.print( "[DEBUG HttpRest] HTTP Response code is: " );
    Serial.println( httpResponseCode );
    Serial.println( "[DEBUG HttpRest] HTTP Response message is: " );
    Serial.println( _response );
#endif

    httpClient.end();
    return httpResponseCode;
}

int HttpHandler::send( const String url, const String contentType, const HTTP_Method_t method )
{
    String empty = "";
    return send( url, contentType, method, empty );
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
