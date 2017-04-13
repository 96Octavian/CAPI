#include <unistd.h>
#include "handler.h"

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
	while(1) {
		int poll = polling(&body);
		printf("Polled %d bytes, body is:\n%s\n", poll, body);
		sleep(1);
	}
	writer("sendMessage", 66441008, "Ciao");

	/* Closing connection */

	disconnecter();
	return 0;
}
