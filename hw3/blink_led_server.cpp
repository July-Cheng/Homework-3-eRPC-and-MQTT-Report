/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.0 on Wed Apr 27 17:49:21 2022.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "blink_led_server.h"
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
#include <new>
#include "erpc_port.h"
#endif
#include "erpc_manually_constructed.h"

#if 10900 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#endif

ERPC_MANUALLY_CONSTRUCTED_STATIC(LEDBlinkService_service, s_LEDBlinkService_service);



// Call the correct server shim based on method unique ID.
erpc_status_t LEDBlinkService_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    erpc_status_t erpcStatus;
    switch (methodId)
    {
        case kLEDBlinkService_led_on_id:
        {
            erpcStatus = led_on_shim(codec, messageFactory, sequence);
            break;
        }

        case kLEDBlinkService_led_off_id:
        {
            erpcStatus = led_off_shim(codec, messageFactory, sequence);
            break;
        }

        default:
        {
            erpcStatus = kErpcStatus_InvalidArgument;
            break;
        }
    }

    return erpcStatus;
}

// Server shim for led_on of LEDBlinkService interface.
erpc_status_t LEDBlinkService_service::led_on_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint8_t led;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&led);

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        led_on(led);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kLEDBlinkService_service_id, kLEDBlinkService_led_on_id, sequence);

        err = codec->getStatus();
    }

    return err;
}

// Server shim for led_off of LEDBlinkService interface.
erpc_status_t LEDBlinkService_service::led_off_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint8_t led;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&led);

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        led_off(led);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kLEDBlinkService_service_id, kLEDBlinkService_led_off_id, sequence);

        err = codec->getStatus();
    }

    return err;
}

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
erpc_service_t create_LEDBlinkService_service()
{
    return new (nothrow) LEDBlinkService_service();
}

void destroy_LEDBlinkService_service(erpc_service_t *service)
{
    if (*service)
    {
        delete service;
    }
}
#elif ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
erpc_service_t create_LEDBlinkService_service()
{
    s_LEDBlinkService_service.construct();
    return s_LEDBlinkService_service.get();
}

void destroy_LEDBlinkService_service()
{
    s_LEDBlinkService_service.destroy();
}
#else
#warning "Unknown eRPC allocation policy!"
#endif