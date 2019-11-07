/**
 * Created by Nick van Riet on 7/11/19.
 * Copyright (c) 2019 Nick van Riet.
 * 
 * Released under the [License name] license: [Link to license]
 * 
 * [Information]
 */

#ifndef ESP32_ARROWHEADCORE_H
#define ESP32_ARROWHEADCORE_H

#define DEBUG_ARROWHEAD

#include <Arduino.h>

typedef struct
{
    String definition;
    String interface;
    double version;
} AHF_Service_Info_t;

typedef struct
{
    String name;
    String ip;
    int port;
} AHF_Consumer_System_Info_t;

typedef struct
{
    String ip;
    String uri;
    String name;
    int port;

} AHF_Provider_System_Info_t;

typedef struct
{
    AHF_Service_Info_t service;
    AHF_Provider_System_Info_t provider;
} AHF_Registration_Info_t;

typedef struct
{
    AHF_Service_Info_t service;
    AHF_Consumer_System_Info_t consumer;
} AHF_Orchestration_Info_t;

typedef struct
{
    int orchestration = 8440;
    int regisrty = 8442;
    int authorisation = 8444;
    int gatekeeperInternal = 8446;
    int gatekeeperExternal = 8448;
    int eventhandler = 8454;
    int choreographer = 8456;
} AHF_Core_Service_Ports_t;

typedef struct
{
    //overrideStore: If true, the Consumer wants dynamic orchestration, and not Orchestration
    // Store-based.
    String overrideStore = "false";
    //metadataSearch: This flag is set if the Consumer wants filtering of Providers based on the
    // submitted set of metadata key-value pairs. This happens in the Service Registry.
    String metadataSearch = "false";
    //enableInterCloud: The Consumer signals with this flag, that it accepts Provider systems from
    // other clouds as well. The Orchestrator first tries to find a Provider inside the Local Cloud
    // though.
    String enableIntercloud = "false";
    //pingProviders: This is set if the Consumer wants the Service Registry to check whether the
    // suitable Providers are online or their offer is invalid (forgot to revoke their entry).
    String pingProviders = "false";

} AHF_Orchestration_Flags_t;

class ArrowheadCore
{
public:
    ArrowheadCore( const String kAddress );
    ArrowheadCore( const String kAddress, AHF_Core_Service_Ports_t ports );
    virtual ~ArrowheadCore();

    bool orchestrationRequest( AHF_Orchestration_Info_t orchestrationInfo, String* endpoint );
    bool orchestrationRequest( AHF_Orchestration_Info_t orchestrationInfo,
                               AHF_Orchestration_Flags_t flags, String* endpoint );

    bool registryAdd( AHF_Registration_Info_t registrationInfo );
    bool registryAdd( AHF_Registration_Info_t registrationInfo, int minutesAlive );
    bool registryRemove( AHF_Registration_Info_t registrationInfo );

private:
    const String _kAddress;
    AHF_Core_Service_Ports_t _ports;

    String _createEntryFrom( AHF_Registration_Info_t registrationInfo,
                             unsigned int minutesAlive = 0 );
    String _createEntryFrom( AHF_Orchestration_Info_t orchestrationInfo,
                             AHF_Orchestration_Flags_t flags );

    bool _orchestrationSend( String orchRequest, String* providerEndpoint );

    bool _serviceRegister( String serviceEntry );
    bool _serviceUnregister( String serviceEntry );

    String _createEndpoint(String ip, int port, String url);

};

#endif //ESP32_ARROWHEADCORE_H
