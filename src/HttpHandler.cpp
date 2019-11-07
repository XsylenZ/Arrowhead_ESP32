/**
 * Created by Nick van Riet on 7/11/19.
 * Copyright (c) 2019 Nick van Riet.
 * 
 * Released under the [License name] license: [Link to license]
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
    return 0;
}

int HttpHandler::send( const String url, const String contentType, const HTTP_Method_t method )
{
    return 0;
}

String HttpHandler::getResponse()
{
    return nullptr;
}

void HttpHandler::setConnectionTimeOut( int connectionTimeOut )
{

}
