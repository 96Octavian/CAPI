#include <unistd.h>
#include "handler.h"
#include "jsmn/jsmn.h"

int update_id = 0;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
			return 0;
		}
	return -1;
}

void *incomer() {
	char *body;
	int err_poll =0;
	while(err_poll<=5) {
		sleep(1);
//		int poll = polling(&body, update_id);
		if(polling(&body, update_id)==0) {
			err_poll++;
			continue;
		}
			
//		printf("Polled %d bytes\n", poll);

//		body = "{\"ok\":\"true\",\"result\":\"vero\"}";
//		printf("New body: %s\n", body);
		
		int i;
		int r;
		char *ptr;
		jsmn_parser p;
		jsmntok_t t[34];
		jsmn_init(&p);
		r = jsmn_parse(&p, body, strlen(body), t, sizeof(t)/sizeof(t[0]));
		if (r < 0) {
			printf("Failed to parse JSON: %d\n", r);
			return (void *)1;
		}

		/* Assume the top-level element is an object */
		if (r < 1 || t[0].type != JSMN_OBJECT) {
			printf("Object expected\n");
			return (void *)1;
		}

//		printf("Body: %s\n\n", strstr(body, "text"));
		for (i = 1; i < r; i++) {
			if (jsoneq(body, &t[i], "update_id") == 0) {
				update_id = strtol(body + t[i+1].start, &ptr, 10)+1;
				/* We may want to do strtol() here to get numeric value */
				printf("\r        - id %.*s: ", t[i+1].end-t[i+1].start, body + t[i+1].start);
				i++;
			} else if (jsoneq(body, &t[i], "text") == 0) {
				/* We may use strndup() to fetch string value */
				printf("%.*s\n", t[i+1].end-t[i+1].start, body + t[i+1].start);
				i++;
			} else {
//				printf("Unexpected key: %.*s\n", t[i].end-t[i].start, body + t[i].start);
				continue;
			}
		}
	}
	printf("Too much polling errors. Shutting down...\n");
	return (void *) 5;
}	

void *outgoer() {
	char *body;
	char text[4096];
	int err_send = 0;
	while(err_send<=5) {
		printf("> ");
		fgets(text, sizeof text, stdin);
		fflush(stdin);
		if(writer("sendMessage", 66441008, text, &body)==0) {
			err_send++;
		}
	}
	return NULL;
}

int main() {

	/* Opening connection */

	printf("Starting connection...");
	if(connecter()!=0) {
		printf("Error opening connection\n");
		exit(1);
	}
	printf(" Connected\n");

	pthread_t incoming;
	pthread_create(&incoming,NULL,incomer,NULL);

	pthread_t outgoing;
	pthread_create(&outgoing,NULL,outgoer,NULL);	

	void *incoming_code;
	pthread_join(incoming, &incoming_code);
	printf("Thread return with code %d\n", (int)incoming_code);
	disconnecter();
	return 0;

}
