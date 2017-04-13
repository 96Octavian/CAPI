#include <unistd.h>
#include "handler.h"
#include "jsmn/jsmn.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
		if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
				return 0;
			}
		return -1;
		}

int main() {

	/* Opening connection */

	printf("Starting connection...");
	if(connecter()!=0) {
		printf("Error opening connection\n");
		exit(1);
	}
	printf(" Connected\n");

	/* Starting polling 

	char message[4096];
char *text;
	char poll[] = "GET /bot199805787:AAHugpIHv3kuYEuP35ugcqvmam7C6utuevg/getUpdates HTTP/1.1\x0D\x0AHost: api.telegram.org\x0D\x0A\x43ontent-Type: application/json\x0D\x0A\x43ontent-Length: 25\x0D\x0A\x43onnection: keep-alive\x0D\x0A\x0D\x0A{\"offset\":-1,\"timeout\":5}";
	int failed_poll = 0;
//	printf("Starting looped polling...\n");
	while(1){
//		printf("Starting looped polling...\n");
		int code = polling(poll, message);
		if(code!=200) {
			failed_poll++;
	                printf("Error %d in polling updates. Body says: %s\n", code, message);
	        }
	        else {
			printf("\nUpdate body is %s\n", message);
			sleep(1);
		}
		if(failed_poll > 5) {
			code = writer("sendMessage", 66441008, "Server return an error too many times. Aborting...\n", text);
			printf("Server return an error too many times. Aborting...\n");
			break;
		}
//		sleep(0.5);
	}

	/* Sample message */

	char *body;
	int update_id = 0;
	while(1) {
		int poll = polling(&body, update_id);
		printf("Polled %d bytes, body is:\n%s\n", poll, body);

//		body = "{\"ok\":\"true\",\"result\":\"vero\"}";
//		printf("New body: %s\n", body);
		
		int i;
		int r;
		char *ptr;
		jsmn_parser p;
		jsmntok_t t[128];
		jsmn_init(&p);
		r = jsmn_parse(&p, body, strlen(body), t, sizeof(t)/sizeof(t[0]));
		if (r < 0) {
			printf("Failed to parse JSON: %d\n", r);
			return 1;
		}

		/* Assume the top-level element is an object */
		if (r < 1 || t[0].type != JSMN_OBJECT) {
			printf("Object expected\n");
			return 1;
		}


		for (i = 1; i < r; i++) {
		if (jsoneq(body, &t[i], "ok") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- OK: %.*s\n", t[i+1].end-t[i+1].start,
					body + t[i+1].start);
			i++;
		} else if (jsoneq(body, &t[i], "result") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- Result: %.*s\n", t[i+1].end-t[i+1].start,
					body + t[i+1].start);
			i++;
		} else if (jsoneq(body, &t[i], "update_id") == 0) {
			update_id = strtol(body + t[i+1].start, &ptr, 10)+1;
			/* We may want to do strtol() here to get numeric value */
			printf("- update_id: %.*s\n", t[i+1].end-t[i+1].start,
					body + t[i+1].start);
			i++;
		} /*else if (jsoneq(body, &t[i], "groups") == 0) {
			int j;
			printf("- Groups:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings 
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, body + g->start);
			}
			i += t[i+1].size + 1;
		}*/ else {
//			printf("Unexpected key: %.*s\n", t[i].end-t[i].start, body + t[i].start);
			continue;
		}
	}



		sleep(1);
	}
	writer("sendMessage", 66441008, "Ciao");

	/* Closing connection */

	disconnecter();
	return 0;
}
