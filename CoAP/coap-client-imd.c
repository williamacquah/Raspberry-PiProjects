#include <coap3/coap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/* FIXME 2.1 Instantiate a new Client, provide your Pi's address */
char * serverip="127.0.0.1";

static coap_response_t client_response_handler(coap_session_t *session, const coap_pdu_t *sent,
                                               const coap_pdu_t *received, const coap_mid_t mid) {
  size_t size;
  uint8_t *data;

  if (coap_get_data(received, &size, &data)) {
 
    
    char temperature_str[16];
    sscanf((char *)data, "Temperature: %15s", temperature_str); // Extract the temperature value
    
		/* Get the float value from the response payload */
    float temperature = strtof(temperature_str, NULL);
    
    /* FIXME 6.1 Add some behavior. */
    /* Determine the new state to be sent to the server */
    char payload[16] = "high";
    if (temperature < 21) {
      strcpy(payload, "off");
    } else if (temperature < 25) {
      strcpy(payload, "low");
    } else if (temperature < 28) {
      strcpy(payload, "medium");
    }

    printf("Temperature = %f\n",temperature);

    /* FIXME 6.2 Create a PUT request to "/ACControl" */
    coap_pdu_t *request = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_PUT, coap_new_message_id(session), coap_session_max_pdu_size(session));
    coap_add_option(request, COAP_OPTION_URI_PATH, 9, (const uint8_t *)"ACControl");
    coap_add_data(request, strlen(payload), (const uint8_t *)payload);
    
    
    /* FIXME 6.3 Send the request */
    coap_send(session, request);
  }

  return COAP_RESPONSE_OK;
}

int main(int argc, char **argv) {
  coap_context_t *ctx;
  coap_session_t *session;
  coap_address_t dst_addr;
  coap_uri_t uri;

  // Build Server URI
  char server_uri[200];
  sprintf(server_uri,"coap://%s/temperature", serverip);


  coap_startup();

  if (coap_split_uri((const uint8_t *)server_uri, strlen(server_uri), &uri) == -1) {
    coap_log_emerg("Invalid CoAP URI\n");
    return EXIT_FAILURE;
  }
    printf("Debug:\nUri Host: %s\nUri Path %s\n",uri.host.s,uri.path.s );

  if (uri.scheme != COAP_URI_SCHEME_COAP && uri.scheme != COAP_URI_SCHEME_COAPS) {
    coap_log_emerg("URI scheme not supported\n");
    return EXIT_FAILURE;
  }

  ctx = coap_new_context(NULL);
  if (!ctx) {
    coap_log_emerg("cannot initialize context\n");
    return EXIT_FAILURE;
  }

  // Resolve the address
  int address_family = AF_UNSPEC;
  if (uri.host.length > 0) {
    address_family = strchr((const char *)uri.host.s, ':') ? AF_INET6 : AF_INET;
  }

  coap_address_init(&dst_addr);
  dst_addr.addr.sin.sin_family = address_family;
  if (address_family == AF_INET) {
    dst_addr.addr.sin.sin_port = htons(5683);
    inet_pton(AF_INET, serverip, &(dst_addr.addr.sin.sin_addr)); // convert INET address to byte format

  }

  session = coap_new_client_session(ctx, NULL, &dst_addr, COAP_PROTO_UDP);
  if (!session) {
    coap_log_emerg("cannot create client session\n");
    return EXIT_FAILURE;
  }

  coap_register_response_handler(ctx, client_response_handler);

  /* FIXME 2.2 Create a request */
  coap_pdu_t *request = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_GET, coap_new_message_id(session), coap_session_max_pdu_size(session));
  coap_add_option(request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s);

  /* FIXME 4.2 Add the observe option to the request */
  coap_add_option(request, COAP_OPTION_OBSERVE, 0, NULL);
  
  /* FIXME 2.3 Send the request */
  coap_send(session, request);

  /* FIXME 4.1 Add a while loop to prevent the client from exiting on a response.*/
  while (1) {
    coap_io_process(ctx, COAP_IO_WAIT);
  }

  coap_session_release(session);
  coap_free_context(ctx);
  coap_cleanup();

  return EXIT_SUCCESS;
}
