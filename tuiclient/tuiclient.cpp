//Make the necessary includes and set up the variables:
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <ncurses.h>
#include "tui.h"
#include "tui.cpp"
#include "str.cpp"

#define BUFFER_SIZE 1000

using namespace std;

int main()
{
	try{
		try{
			//init curses
			initscr();
			curs_set(0);
			refresh();
		}catch(...)
		{
			//error while init curses
		}

		try{
			while (true){
			      int sockfd;
			      int len;
			      struct sockaddr_in address;
			      string recv_msg, first_part, msg_text;

			      //Create a socket for the client:
			      int result=-1;
			      ulong attempt_num=1;
			      while (result==-1){
				sockfd = socket(AF_INET, SOCK_STREAM, 0);

				//Name the socket, as agreed with the server:
				address.sin_family = AF_INET;
				address.sin_addr.s_addr = inet_addr("127.0.0.1");
				address.sin_port = htons(9999);
				len = sizeof(address);
				//Connect your socket to the server’s socket:
				result = connect(sockfd, (struct sockaddr *)&address, len);
				if(result == -1) {
				      msg(0,0,"Error connectin to segget daemon. Attempt:"+toString(attempt_num)+" Result:"+toString(result)+" Waiting for 1 sec, before reconnect");
				      close(sockfd);
				      attempt_num++;
				      sleep(1);
				}
			      }
			      
			      msg(35,70,"Connected");
			      fd_set readfds, testfds;

			      FD_ZERO(&readfds);
			      FD_SET(sockfd, &readfds);
			      testfds = readfds;
			      bool run_flag=true;
			      while (run_flag){
				      result = select(FD_SETSIZE, &testfds, (fd_set *)0,
					      (fd_set *)0, (struct timeval *) 0);

				      int nread;
				      ioctl(sockfd, FIONREAD, &nread);
				      if(nread == 0) {
					      close(sockfd);
		    //                       FD_CLR(sockfd, &readfds);
//					      printf("removing client on fd %d\n", sockfd);
					      run_flag=false;
				      }else {
					      char recv_buffer[BUFFER_SIZE];
					      read(sockfd, recv_buffer, BUFFER_SIZE);
					      recv_msg=recv_msg+recv_buffer;
					      while (recv_msg.find("<.>")!=recv_msg.npos){
						      recv_msg=recv_msg.substr(recv_msg.find("<y>")+3,recv_msg.npos);
						      first_part=recv_msg.substr(0,recv_msg.find("<.>"));
						      
						      recv_msg=recv_msg.substr(recv_msg.find("<.>")+3,recv_msg.npos);
						      uint x=0;
						      uint y=atoi(first_part.substr(0,first_part.find("<s>")).c_str());
						      msg_text=first_part.substr(first_part.find("<s>")+3,first_part.npos);
						      msg(y, x, msg_text);
					      }
				      };
			      }
			      close(sockfd);
			}
		}catch(...){
//			error_log_no_msg("Error in segget.cpp launch_tui_theread() failed");
		}
		getch();
	}
	catch(...){
		//error during init and downloading process
	}
	try{
		endwin();
	}
	catch(...)
	{
		//error while ending curses
	}
	return 0;
}
