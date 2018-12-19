/*-------------include declaration-------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
//#define PORT "51135" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#include <openssl/md5.h>
int main(){


	// creat a socket
	int network_socket;
	network_socket= socket(AF_INET,SOCK_STREAM,0);


	//define the server address
	struct sockaddr_in server_address;
	server_address.sin_family =AF_INET;
	server_address.sin_port=htons(5135);

	server_address.sin_addr.s_addr=INADDR_ANY; //the actual address resolve to any ip address of the machine


	 // bind the socket to the sepicified ip address and port
	 bind(network_socket,(struct sockaddr*)&server_address,sizeof (server_address));

	 // listen for connection
	 listen(network_socket,5);

	 //accept connection
	 int client_socket;
	 client_socket=accept(network_socket,NULL,NULL);

	 //send message
	 //recv(client_socket,server_message,sizeof (server_message),0);

	 //--------------------calculate server Md5----------------------------------------------//

	   unsigned char c[16];  //16 byte
	   MD5_CTX mdContext;
	   char *filemd5 = (char*) malloc(33 *sizeof(char));
	   int n;

	 //--------------------------- open this file for reading-------------------
	  FILE * fp;

	fp= fopen("EssentialC.pdf","rb"); //


	//------------------read data from file and send it to client-----------------------------//
	    /* ------intiat md5------*/



	int j;
	 char buff[201*1024];

		//read file in chunks
		 //make pointer to identify the location of the data we got back from the socket
            //read binary data from file
		MD5_Init (&mdContext);

		int read_input;
		int total = 0;
		int server_md5;
		unsigned char server_data[16];
		  int len;
		int server_seek;


       //fclose(fp);
		// 1. Seek to beginning
		// 2. Read entire file and calculate md5
		// 3. Seek to beginning
		// 4. Read entire file and send to client-----------------------------//

		/*seek;
		while (file_not_completely_read) {
			read_chunk
			add_to_md5
		}

		seek;*/


		do {
		server_seek=  fseek(fp, 0, SEEK_END);
		server_md5= fread(buff, 1, 10 * 1024, fp);
		  len = ftell(fp);
		  fseek(fp, 0, SEEK_SET);
		  MD5_Update(&mdContext, buff, len);
		  printf("Total size of the transmitted file = %d Kbytes\n", len);
		}while(server_md5 > 0);

		MD5_Final(c, &mdContext);
		for(n = 0; n < 16; n++) {
			sprintf(&filemd5[n*2],"%02x",(unsigned int) c[n]);
        }
	     int written = send (client_socket,filemd5,33,0);//write(client_socket, filemd5, 16);
		printf("Send MD5: %s\n", filemd5);


		do {
			read_input = fread(buff, 1, 10 * 1024, fp);
			total += read_input;

			if (read_input > 0) {

				int written = write(client_socket, buff, read_input);	//write data out of a buffer and put it into the socket
				printf("Send: %i/%i byte, total: %i bytes\n", read_input, written, total);
			}
		} while (read_input > 0);


		/*
		int written = write(client_socket, c, 16);
		printf("Wrote MD5: %i\n", written);
		written = write(client_socket, c, 16);
		printf("Wrote MD5: %i\n", written);
		*/
		fsync(client_socket);

		printf("The file has successfylly sent\n");
		printf("calculated md5:%s ", filemd5);

	close(client_socket);
	return 0;
}
