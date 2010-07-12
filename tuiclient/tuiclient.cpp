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
#include <pthread.h>
#include "str.cpp"

#define BUFFER_SIZE 2000

#define TOTAL_LINE_NUM	200
#define ERROR_LINE_NUM	201
#define LOG_LINE_NUM	202
#define DEBUG_LINE_NUM	203

using namespace std;

WINDOW * screen;
int max_x, max_y;
int max_received_screenline_num;
int last_screenline_num;


void show_lines();

const uint CONNECTION_LINES=5;
const uint MAX_LINES=200;
int top_position;
string screenlines[200];
string screen_info_lines[4];
string status_str;
void msg_line(uint y, string msg_text){
	try{
//		cout << msg_text;
		msg_text=msg_text+"                                                                                                                                                         ";
		mvaddstr(y,1,msg_text.substr(0,max_x-2).c_str());
//		mvwprintw(screen,y,1,ready_msg_text.c_str());
//		mvwprintw(screen,y,1,msg_text.substr(0,max_x-2).c_str());
//		mvaddstr();
	}catch(...){
//		error_log_no_msg("Error in tui.cpp: msg()");
	}
}
void msg_short(uint y, uint x, string msg_text){
	try{
		mvaddstr(y,x,msg_text.c_str());
	}catch(...){
//		error_log_no_msg("Error in tui.cpp: msg()");
	}
}
void set_status(string str){
 status_str=str; 
 show_lines();
}

void color_downloads(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		attron(COLOR_PAIR(1));
	}
}

void color_info(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_pair(2, COLOR_BLACK, COLOR_CYAN);
		attron(COLOR_PAIR(2));
	}
}

void color_status(){
	if(has_colors()){
		start_color();			/* Start color 			*/
		init_pair(3, COLOR_RED, COLOR_BLUE);
		attron(COLOR_PAIR(3));
	}
}


void msg_status(){
		msg_short(0,max_x/2-13,"tuiclient to segget daemon");
		msg_short(max_y-1,2,status_str);
		msg_short(max_y-1,max_x-10,"[q-Quit]");
}

void show_lines(){
	//clear();
	box(screen, ACS_VLINE, ACS_HLINE);
	getmaxyx(screen,max_y,max_x);
	last_screenline_num=max_y-6;
	color_status();
	msg_status();
	color_info();
	for (int y=max_y-6,status_line_num=0; y<max_y-2; y++,status_line_num++){
		msg_line(y+1,screen_info_lines[status_line_num]);
	}
	color_downloads();
	screenlines[26]="Max_num:"+toString(max_received_screenline_num);
	for (int y=0, line_num=top_position; y<max_y-6; y++, line_num++){
		msg_line(y+1,screenlines[line_num]);
	}
	wrefresh(screen);
	refresh();
}

void set_line(uint y, string msg_text){
  screenlines[y]=msg_text;
  if (max_received_screenline_num<y) max_received_screenline_num=y;
}
void updown(int inc){
  int tmp_top_position=top_position+inc;
  if (tmp_top_position>=0 && tmp_top_position<(max_received_screenline_num-last_screenline_num)){
   top_position=tmp_top_position; 
  }
}

void * watch_keyboard_thread_function(void * ptr){
	char * args = (char *) ptr;
	while (true){
	  char key=getch();
	  switch (key){
	    //KEY_DOWN
	    case (char)2:updown(1);break;
	    //KEY_RIGHT
	    case (char)5:updown(1);break;
	    //KEY_UP
	    case (char)3:updown(-1);break;
	    //KEY_LEFT
	    case (char)4:updown(-1);break;
	    case 'u':updown(1);break;
	    case 'd':updown(-1);break;
//	    default: screenlines[25]=toString((int)key);
	  }
	}
	return 0;
}

int main()
{
	try{
		try{
			//init curses
			screen=initscr();
			curs_set(0);
			//don't echo keyboard to the screen
			noecho();
			//don't wait for enter
			cbreak();
			//enable arrow keys
			keypad(screen,true);
			top_position=0;
			max_received_screenline_num=0;
		}catch(...)
		{
			//error while init curses
		}
		try{
			pthread_t watch_keyboard_thread;
			int iret1;
			iret1 = pthread_create( &watch_keyboard_thread, NULL, watch_keyboard_thread_function, (void*) NULL);
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
					      char recv_buffer[BUFFER_SIZE+1]="";
					      read(sockfd, recv_buffer, BUFFER_SIZE);
					      //recv_msg=recv_msg+recv_buffer;
					      recv_msg="MMM";
					      recv_msg=recv_msg+recv_buffer;
//					      cout<<recv_buffer;
					      while (recv_msg.find("<.>")!=recv_msg.npos){
						      string original_msg=recv_msg;
						      recv_msg=recv_msg.substr(recv_msg.find("<y>")+3,recv_msg.npos);
						      first_part=recv_msg.substr(0,recv_msg.find("<.>"));
						      
						      recv_msg=recv_msg.substr(recv_msg.find("<.>")+3);
						      uint line_num=atoi(first_part.substr(0,first_part.find("<s>")).c_str());
						      msg_text=first_part.substr(first_part.find("<s>")+3,first_part.npos);
						      if (line_num<200){
								set_line(line_num, msg_text);
//							      set_line(line_num, "||"+toString(line_num)+"||("+first_part.substr(0,first_part.find("<s>")+3)+")"+msg_text);
//							      set_line(line_num, "||"+toString(line_num)+"||("+original_msg);
						      }else{
								screen_info_lines[line_num-200]=msg_text;
						      }
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
