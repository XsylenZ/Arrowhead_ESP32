/**
 * Created by Nick van Riet on 7/11/19.
 * Copyright (c) 2019 Nick van Riet.
 * 
 * Released under the [License name] license: [Link to license]
 * 
 * [Information]
 */

#ifndef ESP32_HTTPHANDLER_H
#define ESP32_HTTPHANDLER_H

#include <HTTPClient.h>

typedef enum
{
    GET,
    POST,
    PUT,
    DELETE

} HTTP_Method_t;

class HttpHandler
{
public:
    HttpHandler();
    int send( const String url, const String contentType, const HTTP_Method_t method,
              const String body );
    int send( const String url, const String contentType, const HTTP_Method_t method);
    String getResponse();
    void setConnectionTimeOut( int connectionTimeOut );
    virtual ~HttpHandler();

private:
    String _response;
    int _connectionTimeOut = 250;
};

#endif //ESP32_HTTPHANDLER_H
