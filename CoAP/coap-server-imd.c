#include <coap3/coap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <glob.h>
#include <errno.h>

//Initialize AC State
static char ac_state[16] = "off";


float read_temperature_sensor(){
    static __thread FILE *fp = NULL;
    float rv = NAN;
    int ret;
    if(fp == NULL){
        glob_t globbuf;
        int ret = glob("/sys/bus/w1/devices/28-*/temperature",0,NULL,&globbuf);
        if(ret != 0){
            fprintf(stderr, "no temperature sensor found with error: %d\n",ret);
			globfree(&globbuf);
			return rv;
        }
        else {
            //open the first found temperature sensor readable
            fp = fopen(globbuf.gl_pathv[0], "r");
            if (fp == NULL) {
                perror("Error opening file");
                globfree(&globbuf);
                return rv;
            }
            //disable buffered reading, to always read file with current temperature
            setvbuf(fp, NULL , _IONBF, 0);
        }

        globfree(&globbuf);
    }
    
    char *line = NULL;
    size_t n;

    ret = getline(&line, &n, fp);
    //reset stream pointer to always read the temperature
    rewind(fp);
    if (ret < 0) {
        perror("error reading line");
    }
    else {
        errno = 0;
        rv = strtof(line,NULL);
        if(errno != 0){
            perror("error converting temperature to float");
        }
        rv /=1000;
    }
    free(line);
    return rv;        

}

/* Handler for Temperature Resource */
static void temperature_handler(coap_resource_t *resource, coap_session_t *session,
                                const coap_pdu_t *request, const coap_string_t *query,
                                coap_pdu_t *response) {
  float reading = read_temperature_sensor();
  char response_data[64];
  snprintf(response_data, sizeof(response_data), "Temperature: %.2fC", reading);
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  coap_add_data(response, strlen(response_data), (const uint8_t *)response_data);
}

/* FIXME 5.2 Handler for AC Control Resource (PUT) */
static void ac_control_put_handler(coap_resource_t *resource, coap_session_t *session,
                                   const coap_pdu_t *request, const coap_string_t *query,
                                   coap_pdu_t *response) {
  size_t size;
  uint8_t *data;

  if (coap_get_data(request, &size, &data)) {
    snprintf(ac_state, sizeof(ac_state), "%.*s", (int)size, data);
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
  } else {
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
  }
}


/* FIXME 5.3 Handler for AC Control Resource (GET) */
static void ac_control_get_handler(coap_resource_t *resource, coap_session_t *session,
                                   const coap_pdu_t *request, const coap_string_t *query,
                                   coap_pdu_t *response) {
  char response_data[64];
  snprintf(response_data, sizeof(response_data), "AC State: %s", ac_state);
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  coap_add_data(response, strlen(response_data), (const uint8_t *)response_data);
}


typedef struct {
  coap_resource_t *resource;
  int sensor;
  int interval;
  float threshold;
} sensor_observer_t;


/* FIXME 3.2 Implement sensor_observer_thread*/
 void *sensor_observer_thread(void *arg) {
  sensor_observer_t *observer = (sensor_observer_t *)arg;
  float last_reading = read_temperature_sensor();

  while (1) {
    float reading = read_temperature_sensor();
    if (fabs(reading - last_reading) >= observer->threshold) {
      last_reading = reading;
      coap_resource_notify_observers(observer->resource, NULL);
    }
    usleep(observer->interval * 1000); // Convert milliseconds to microseconds
  }

  return NULL;
}

int main(void) {
  coap_context_t *ctx;
  coap_address_t serv_addr;
  coap_resource_t *temperature_resource;
  coap_resource_t *ac_control_resource;
  pthread_t observer_thread;
  sensor_observer_t observer;

  /* Initialize libcoap library */
  coap_startup();

  /* Set logging level */
  coap_set_log_level(COAP_LOG_WARN);

  /* FIXME 1.1 Initialize CoAP server */
  /* Define the server address */
  coap_address_init(&serv_addr);
  serv_addr.addr.sin.sin_family = AF_INET;
  serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
  serv_addr.addr.sin.sin_port = htons(5683);

  /* Create CoAP context */
  ctx = coap_new_context(NULL);
  if (!ctx) {
    coap_log_emerg("cannot initialize context\n");
    return EXIT_FAILURE;
  }

  /* FIXME 1.2 Instantiate a resource */
  /* Create the Temperature Resource */
  temperature_resource = coap_resource_init(coap_make_str_const("temperature"), 0);
  
  /* FIXME 1.3 Register GET Handler */
  coap_register_handler(temperature_resource, COAP_REQUEST_GET, temperature_handler);
  
  /* FIXME 3.1 Make the temperature resource observable */
  coap_resource_set_get_observable(temperature_resource, 1);
  
  /* FIXME 1.4 Add the resource to the "CoAP server" */
  coap_add_resource(ctx, temperature_resource);

  /* FIXME 5.1 Create the AC Control Resource */
  ac_control_resource = coap_resource_init(coap_make_str_const("ACControl"), 0);
  coap_register_handler(ac_control_resource, COAP_REQUEST_PUT, ac_control_put_handler);
  coap_register_handler(ac_control_resource, COAP_REQUEST_GET, ac_control_get_handler);
  coap_add_resource(ctx, ac_control_resource);

  /* FIXME 3.3 Set up the sensor observer */
  observer.resource = temperature_resource;
  observer.sensor = 0;
  observer.interval = 250;
  observer.threshold = 0.25f;
  pthread_create(&observer_thread, NULL, sensor_observer_thread, &observer);

  /* Create the endpoint */
  if (!coap_new_endpoint(ctx, &serv_addr, COAP_PROTO_UDP)) {
    coap_log_emerg("cannot create endpoint\n");
    coap_free_context(ctx);
    return EXIT_FAILURE;
  }

  /* Run the server */
  while (1) {
   
   /* FIXME 1.5 Run the server */
   coap_io_process(ctx, COAP_IO_WAIT);
  }

  /* Cleanup */
  coap_free_context(ctx);
  coap_cleanup();

  return EXIT_SUCCESS;
}
