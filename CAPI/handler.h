//#define _GNU_SOURCE
#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <string.h>
#include <pthread.h>

BIO * bio;
SSL_CTX * ctx;

char * body_retriever(char* r) {
	char *rest;
        char *token;
        char *last;

//	printf("Response:\n");

/*	token = strtok_r(r, "\x0D\x0A\x0D\x0A", &rest);
	while(token) {
                printf("Token: %s\n", token);
		if(strstr(token, "HTTP/1.1") != NULL) {
                        if(sscanf(token, "HTTP/1.1 %d", &response_code)!=1){
                                printf("Failed to get appropiate response code\n");
                                exit(1);
                        }
                }
		last = token;
                token = strtok_r(NULL, "\r\n\r\n", &rest);
        }
	return last;*/
	char *pch = strstr(r, "\x0D\x0A\x0D\x0A");
    if (pch != NULL)
        printf("We found it! It is: %s\n", pch);
}

int polling(char **body, int update_id) {

	/* Costruzione richiesta HTTP */

	int t;
	char *ptr, *messaggio, *request;
	char *re = malloc (sizeof (char) * (4096+633+1));
	int size = asprintf(&messaggio, "%s%d%s", "{\"limit\":1,\"offset\":", update_id, "}");
	size = asprintf(&request, "%s%d%s%s", "GET /bot199805787:AAHugpIHv3kuYEuP35ugcqvmam7C6utuevg/getUpdates HTTP/1.1\x0D\x0AHost: api.telegram.org\x0D\x0A\x43ontent-Type: application/json\x0D\x0A\x43ontent-Length: ", strlen(messaggio), "\x0D\x0A\x43onnection: keep-alive\x0D\x0A\x0D\x0A", messaggio);
	free(messaggio);

	/* Write request */

        BIO_write(bio, request, strlen(request));
	free(request);

        /* Read in the response */

	for(;;) {
                t = BIO_read(bio, re, 4096+633);
                if(t <= 0) break;	//Errore nella ricezione
                re[t] = 0;
		if(strstr(re, "HTTP/1.1 200 OK")==NULL) {
			free(re);
			return 0;
		}			
		/* Lunghezza del corpo */
		const char *p1 = strstr(re, "Content-Length: ")+16;
		const char *p2 = strstr(p1, "\x0D\x0A");
		size_t len = p2-p1;
		char *res = (char*)malloc(sizeof(char)*(len+1));
		strncpy(res, p1, len);
		res[len] = '\0';
		*body = strstr(re, "\x0D\x0A\x0D\x0A")+4;	//Corpo della risposta
		free(re);
		/* controlla se hai letto tutto il corpo */
		if(strlen(*body)==strtol(res, &ptr, 10)) {
			free(res);
			break;
		}
        }
        return t;

}

int writer(char *method, int chat_id, char *text, char **body) {

	/* Costruzione richiesta HTTP */

	int p;
	char *ptr, *messaggio, *request;
	char *re = malloc (sizeof (char) * (4096+633+1));
	int size = asprintf(&messaggio, "%s%d%s%s%s", "{\"chat_id\":", chat_id, ",\"text\":\"", text, "\"}");
	size = asprintf(&request, "%s%s%s%d%s%s", "GET /bot199805787:AAHugpIHv3kuYEuP35ugcqvmam7C6utuevg/", method, " HTTP/1.1\x0D\x0AHost: api.telegram.org\x0D\x0A\x43ontent-Type: application/json\x0D\x0A\x43ontent-Length: ", strlen(messaggio)*sizeof(char), "\x0D\x0A\x43onnection: keep-alive\x0D\x0A\x0D\x0A", messaggio);
	free(messaggio);

	/* Write request */

	BIO_write(bio, request, strlen(request));
	free(request);

        /* Read in the response */

	/*for(;;) {
                p = BIO_read(bio, re, 4096+633);
                if(p <= 0) break;
                r[p] = 0;
		printf("strlen risposta: %d\n", strlen(r));
		const char *p1 = strstr(r, "Content-Length: ")+16;
		const char *p2 = strstr(p1, "\x0D\x0A");
		size_t len = p2-p1;
		char *res = (char*)malloc(sizeof(char)*(len+1));
		strncpy(res, p1, len);
		res[len] = '\0';
		body = strstr(r, "\x0D\x0A\x0D\x0A")+4;
		if(strlen(body)==strtol(res, &ptr, 10)) {
			free(res);
			break;
		}
        }*/

	for(;;) {
                p = BIO_read(bio, re, 4096+633);
                if(p <= 0) break;	//Errore nella ricezione
                re[p] = 0;
		if(strstr(re, "HTTP/1.1 200 OK")==NULL) {
			free(re);
			return 0;
		}			
		/* Lunghezza del corpo */
		const char *p1 = strstr(re, "Content-Length: ")+16;
		const char *p2 = strstr(p1, "\x0D\x0A");
		size_t len = p2-p1;
		char *res = (char*)malloc(sizeof(char)*(len+1));
		strncpy(res, p1, len);
		res[len] = '\0';
		*body = strstr(re, "\x0D\x0A\x0D\x0A")+4;	//Corpo della risposta
		free(re);
		/* controlla se hai letto tutto il corpo */
		if(strlen(*body)==strtol(res, &ptr, 10)) {
			free(res);
			break;
		}
        }

//        char *result = body_retriever(r);       //r is the response
//	printf("Body:\n%s", body);
        return p;
}

int connecter() {
	SSL * ssl;

	/* Set up the library */

	ERR_load_BIO_strings();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	/* Set up the SSL context */

	ctx = SSL_CTX_new(SSLv23_client_method());

	/* Load the trust store */

	if(! SSL_CTX_load_verify_locations(ctx, NULL, "/etc/ssl/certs")) {
		fprintf(stderr, "Error loading trust store\n");
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ctx);
		return 0;
	}

	/* Setup the connection */

	bio = BIO_new_ssl_connect(ctx);

	/* Set the SSL_MODE_AUTO_RETRY flag */

	BIO_get_ssl(bio, & ssl);
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

	/* Create and setup the connection */

	BIO_set_conn_hostname(bio, "api.telegram.org:https");

	if(BIO_do_connect(bio) <= 0) {
		fprintf(stderr, "Error attempting to connect\n");
		ERR_print_errors_fp(stderr);
		BIO_free_all(bio);
		SSL_CTX_free(ctx);
		return 0;
	}

	/* Check the certificate */

	if(SSL_get_verify_result(ssl) != X509_V_OK) {
		fprintf(stderr, "Certificate verification error: %i\n", SSL_get_verify_result(ssl));
		BIO_free_all(bio);
		SSL_CTX_free(ctx);
		return 0;
	}

	/* Send the request */

//	BIO_write(bio, request, strlen(request));

	/* Read in the response */

/*	for(;;) {
		p = BIO_read(bio, r, 1023);
		if(p <= 0) break;
		r[p] = 0;
	}

	char *result = body_retriever(r);	//r is the response

	strcpy(body, result);
	return response_code;*/
	return 0;
}

int disconnecter() {
	/* Close the connection and free the context */

	BIO_free_all(bio);
	SSL_CTX_free(ctx);
	printf("\nConnection closed\n");
	return 0;
}
