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
#include "str.cpp"

#define BUFFER_SIZE 1000

using namespace std;

WINDOW * screen;
int max_x, max_y;
int mx;


void show_lines();

const uint CONNECTION_LINES=5;
const uint MAX_LINES=200;
string screenlines[MAX_LINES];
string status_str;
bool msg_idle=true;
void msg_line(uint y, string msg_text){
	if (msg_idle){
		msg_idle=false;
		try{
			msg_text=msg_text+"                                                                                                                                                         ";
			mvaddstr(y,1,msg_text.substr(0,max_x-2).c_str());
//			mvwprintw(screen,y,1,ready_msg_text.c_str());
			wrefresh(screen);
		}catch(...){
//			error_log_no_msg("Error in tui.cpp: msg()");
		}
		msg_idle=true;
	}
}
void msg_short(uint y, uint x, string msg_text){
	if (msg_idle){
		msg_idle=false;
		try{
			mvaddstr(y,x,msg_text.c_str());
			wrefresh(screen);
		}catch(...){
//			error_log_no_msg("Error in tui.cpp: msg()");
		}
		msg_idle=true;
	}
}
void set_status(string str){
 status_str=str; 
 show_lines();
}

void msg_status(){
	if(has_colors())
	{
		start_color();			/* Start color 			*/
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_RED, COLOR_BLUE);
		attron(COLOR_PAIR(2));
		msg_short(max_y-1,2,status_str);
//		attroff(COLOR_PAIR(2));
		attron(COLOR_PAIR(1));
	}else{
		msg_short(max_y-1,2,status_str);
	}
}

void show_lines(){
	clear();
	box(screen, ACS_VLINE, ACS_HLINE);
	for (int y=1; y<max_y-1; y++){
		msg_line(y,screenlines[y]);
	}
	msg_status();
}

void set_line(uint y, string msg_text){
  screenlines[y]=msg_text;
}

int main()
{
	try{
		try{
			//init curses
			screen=initscr();
			curs_set(0);
			noecho();
			refresh();
//			WINDOW * spad;
//			spad=newpad(100,100);
//			box(spad, ACS_VLINE, ACS_HLINE);
//			mvwprintw(spad,1,1,"--wwwwwwwwwwwooooooooooowowwwwwwwwww --------");
//			wrefresh(spad);

			
			
//			getch();
			
//			screen = newwin(max_y, 0, 0, 0);
			getmaxyx(screen,max_y,max_x);
			wrefresh(screen);
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
				      set_status("[Connecting... Attempt:"+toString(attempt_num)+". Waiting for 1 sec, before next reconnect.]");
				      close(sockfd);
				      attempt_num++;
				      sleep(1);
				}
			      }
			      
			      set_status("[Connected]");
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
						      uint y=atoi(first_part.substr(0,first_part.find("<s>")).c_str());
						      msg_text=first_part.substr(first_part.find("<s>")+3,first_part.npos);
						      set_line(y+1, msg_text);
						      show_lines();
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
