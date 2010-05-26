#!/usr/bin/python
# Copyright 2010 Gentoo Foundation                                                                                                                        
# Distributed under the terms of the GNU General Public License v2                                                                                        

"""
First prototype for a simple fetcher. Simultaneous downloads of different
source files will be supported
"""

from __future__ import print_function
import os
import re
import time
import sys
import signal
from portage.process import spawn
import idfetch_settings
import pickle
import curses
import time
from threading import Thread
CLEAN_LINE="                                                                                                                                             "
MAX_ACTIVE_DOWNLOADS=8
TASK_SPACE=3

def time_msg():
	stdscr.addstr(0,50,"["+time.ctime()+"]  q - Quit")
	stdscr.refresh()

def fit_string(text):
	text=text+CLEAN_LINE
	return text[0:max_x-1]
def msg(index,msg_text):
	global exit_flag
	if exit_flag:
		pass
	else:
		stdscr.addstr(index,0,fit_string(msg_text))
		stdscr.refresh()

def download_msg(index,msg_text):
	msg(TASK_SPACE*index,str(index)+")"+msg_text)
	
def status_msg(index,msg_text):
	msg(TASK_SPACE*index+1,"   ["+msg_text+"]")

def total_msg(msg_text):
	msg(2,msg_text)

	
def progress_msg(index,msg_text1,msg_text2):
		msg(TASK_SPACE*index+1,msg_text1)

def error_msg(msg_text):
	msg(max_y-1,"WARNING: "+msg_text)

def open_task_list():
	task_file_name=idfetch_settings.TASK_DIR+"/task.list"
	try:
		task_file_name=idfetch_settings.TASK_DIR+"/task.list"
		task_file = open(task_file_name,"r")
		task_list=pickle.load(task_file)
		task_file.close()
	except:
		task_list=[]
		error_msg("Can't open file: ",task_file_name)
	return task_list


class fetchit(Thread):
	def __init__ (self,download_file,index):
		Thread.__init__(self)
		self.download_file = download_file
		self.index=index
		self.status = 2
		self.place_in_the_list = 1
		self.wget_log_file_name = idfetch_settings.TASK_DIR+'/logs/log_wget/'+self.download_file+".log"
		try:
			os.unlink(self.wget_log_file_name)
		except:
			error_msg("Can't delete file"+self.wget_log_file_name)
		self.trials_log_file_name = idfetch_settings.TASK_DIR+'/logs/log_trials/'+self.download_file+".log"
	def start(self,place_in_the_list):
		self.place_in_the_list=place_in_the_list
		Thread.start(self)
	def run(self):
#		msg(0,'DOWNLOADING: '+self.download_file)
		global exit_flag
		self.download_file_url_list_file=open(idfetch_settings.TASK_DIR+'/urls/'+self.download_file+".urllist")
		self.download_file_url_list=pickle.load(self.download_file_url_list_file)
		self.download_file_url_list_file.close()
		self.trials_log_file= open (self.trials_log_file_name,"w")
		while (self.download_file_url_list) and (self.status !=0) and (not(exit_flag)) :
			self.current_url=self.download_file_url_list.pop()
			self.trials_log_file.write("[DL "+str(self.index)+"]: "+self.current_url+"\n")
			download_msg(self.place_in_the_list,"[DL "+str(self.index)+"]: "+self.current_url)
			# preparing to fork
			#if fd_pipes is None:                                                                                                                                            
			self.fd_pipes = {                                                                                                                                            
				0:sys.stdin.fileno(),                                                                                                                           
				1:sys.stdout.fileno(),                                                                                                                          
				2:sys.stderr.fileno(),                                                                                                                          
			}                                                                                                                                                       
                                                                                                                                                                    
                                                                                                                                                                    
			if 1:                                                                                                                                                     
			# Using a log file requires that stdout and stderr                                                                                                      
			# are assigned to the process we're running.                                                                                                            
				if 1 not in self.fd_pipes or 2 not in self.fd_pipes:                                                                                                              
					raise ValueError(self.fd_pipes)                                                                                                                      
                                                                                                                                                                    
				# Create a pipe                                                                                                                                         
				(self.pr, self.pw) = os.pipe()                                                                                                                                    
                                                                                                                                                                    
				# Create a tee process, giving it our stdout and stderr                                                                                                 
				# as well as the read end of the pipe.                                                                                                                  
				#    mypids.extend(spawn(('tee', '-i', '-a', logfile),                                                                                                       
				#                  returnpid=True, fd_pipes={0:pr,                                                                                                           
				#                  1:fd_pipes[1], 2:fd_pipes[2]}))                                                                                                           
                                                                                                                                                                    
				# We don't need the read end of the pipe, so close it.                                                                                                  
				###    os.close(pr)                                                                                                                                            
                                                                                                                                                                    
				# Assign the write end of the pipe to our stdout and stderr.                                                                                            
				self.fd_pipes[1] = self.pw                                                                                                                                        
				self.fd_pipes[2] = self.pw          
				self.pid = os.fork()
				if self.pid:
					# we are the parent
#					global 
					mypids.append(self.pid)
					os.close(self.pw) # use os.close() to close a file descriptor
					self.pr = os.fdopen(self.pr) # turn r into a file object
#					print "parent: reading"
#					print "--------------->"
					#    for i in range(1,50):
					self.line=self.pr.readline()
					while self.line:
						progress_msg(self.place_in_the_list, self.line,"")
#						time.ctime(1)
						self.line=self.pr.readline()
						self.igot = re.findall('saved',self.line)
						if self.igot:
							self.status = 0
							break
###					os.waitpid(self.pid, 0) # make sure the child process gets cleaned up
					try:                                                                                                                                      
						os.waitpid(self.pid, os.WNOHANG)
#						os.waitpid(self.pid, 0)                                                                                                        
					except OSError:                                                                                                                           
					# This pid has been cleaned up outside                                                                                            
					# of spawn().                                                                                                                     
						pass
				else:
					# we are the child
					os.close(self.pr)
					self.w = os.fdopen(self.pw, 'w')
#					print "child: writing"
					self.my_fds = {}                                                                                                                                                     
					# To protect from cases where direct assignment could                                                                                                           
					# clobber needed fds ({1:2, 2:1}) we first dupe the fds                                                                                                         
					# into unused fds.                                                                                                                                              
					for self.fd in self.fd_pipes:                                                                                                                                             
						self.my_fds[self.fd] = os.dup(self.fd_pipes[self.fd])                                                                                                                       
					# Then assign them to what they should be.                                                                                                                      
					for self.fd in self.my_fds:                                                                                                                                               
						os.dup2(self.my_fds[self.fd], self.fd)                                                                                                                                 
					# Then close _all_ fds that haven't been explictly                                                                                                              
					# requested to be kept open.                                                                                                                                    
					#    for fd in get_open_fds():                                                                                                                                       
					if self.fd not in self.my_fds:                                                                                                                                    
						try:                                                                                                                                            
							os.close(self.fd)                                                                                                                            
						except OSError:                                                                                                                                 
							pass                          
					os.execv('/usr/bin/wget', ["wget","--connect-timeout=1", self.current_url,\
							"--directory-prefix="+idfetch_settings.DIST_DIR,\
							"--tries="+str(idfetch_settings.WGET_TRIES),\
							"--read-timeout="+str(idfetch_settings.WGET_READ_TIMEOUT)])
					sys.exit(0)
			if self.status ==0:
				self.trials_log_file.write("[FIN "+str(self.index)+"]: "+self.current_url+"\n")
				download_msg(self.place_in_the_list,"[FIN "+str(self.index)+"]: "+self.current_url)
			elif self.download_file_url_list:
					self.trials_log_file.write("[ERROR "+str(self.index)+"]: "+self.current_url+"\n")
					download_msg(self.place_in_the_list,"[ERROR "+str(self.index)+"]: "+self.current_url)
			else:
				self.trials_log_file.write("[ERROR+LIST_IS_EMPTY "+str(self.index)+"]: "+self.current_url+"\n")
				download_msg(self.place_in_the_list,"[ERROR+LIST_IS_EMPTY "+str(self.index)+"]: "+self.current_url)
					
		self.trials_log_file.close()
	def get_place_in_the_list(self):
		return self.place_in_the_list
def do_tasks(task_list):
	global exit_flag
	msg(1,"TASK DIR: "+idfetch_settings.TASK_DIR)
	msg(0,"DOWNLOADING with twrapper...")

	fetch_distfile_thread_list = []
	index=0
	while task_list:
		current_pkg=task_list.pop()
#		report = ("OK","ERROR","Downloading...")
#		pkg_list = open(idfetch_settings.TASK_DIR+'/pkgs/jpeg-8a')
		distfiles_list_file = open(idfetch_settings.TASK_DIR+'/pkgs/'+current_pkg)
		distfiles_list=pickle.load(distfiles_list_file)
		distfiles_list_file.close()
		for distfile_item in distfiles_list:
			index+=1
			current_fetch_distfile_thread = fetchit(distfile_item, index)
			fetch_distfile_thread_list.append(current_fetch_distfile_thread)

	finished_downloads=0
	place_in_the_list=0
	running_fetch_distfile_thread_list=[]
	to_start_fetch_distfile_thread_list=list(fetch_distfile_thread_list)
	while to_start_fetch_distfile_thread_list:
		place_in_the_list+=1
		current_fetch_distfile_thread=to_start_fetch_distfile_thread_list.pop()
		running_fetch_distfile_thread_list.append(current_fetch_distfile_thread)
		current_fetch_distfile_thread.start(place_in_the_list)
		if place_in_the_list==MAX_ACTIVE_DOWNLOADS:
			break
	key='x'
	stdscr.nodelay(1)
	while (key != ord('q')) and (key != ord('Q')):
	
		for current_fetch_distfile_thread in running_fetch_distfile_thread_list:
			if current_fetch_distfile_thread.status==0:
				if to_start_fetch_distfile_thread_list:
				#start next one on the same place in the list
					finished_downloads+=1
					total_msg("Downloaded "+str(finished_downloads)+" of "+str(index)+" distfiles")
					running_fetch_distfile_thread_list.remove(current_fetch_distfile_thread)
					starting_fetch_distfile_thread=to_start_fetch_distfile_thread_list.pop()
					running_fetch_distfile_thread_list.append(starting_fetch_distfile_thread)
					starting_fetch_distfile_thread.start(current_fetch_distfile_thread.get_place_in_the_list())
			time_msg()
		time.sleep(0.5)
		key = stdscr.getch()
		if (key == ord('q')) or (key == ord('Q')):
			msg(28,">>>>>>>>>>>>>>>>> EXITING  <<<<<<<<<<<<<<<<<<<<<<<<<<")
	exit_flag=1


def main(task_list):
	try:

		task_list=open_task_list()
		do_tasks(task_list)
		msg(30,">>>>>>>>>>>> more exited <<<<<<<<<<<<<<")

	finally:
		for pid in mypids:
			try:                                                                                                                                      
				if os.waitpid(pid, os.WNOHANG) == (0, 0):                                                                                         
					os.kill(pid, signal.SIGTERM)                                                                                              
					os.waitpid(pid, 0)                                                                                                        
			except OSError:                                                                                                                           
			# This pid has been cleaned up outside                                                                                            
			# of spawn().                                                                                                                     
				pass
		curses.nocbreak()
		stdscr.keypad(0)
		curses.echo()
		curses.endwin()
	print("twrapper exited")

def cleanup():                                                                                                                                            
    while spawned_pids:                                                                                                                               
        pid = spawned_pids.pop()                                                                                                                  
        try:                                                                                                                                      
            if os.waitpid(pid, os.WNOHANG) == (0, 0):                                                                                         
                os.kill(pid, signal.SIGTERM)                                                                                              
                os.waitpid(pid, 0)                                                                                                        
        except OSError:                                                                                                                           
            # This pid has been cleaned up outside                                                                                            
            # of spawn().                                                                                                                     
            pass         


if __name__ == '__main__':
	exit_flag=0
	task_list=[]
	# mypids will hold the pids of all processes created.                                                                                                           
	mypids = []                                                                                                                                                     
	stdscr = curses.initscr()
	max_y,max_x=stdscr.getmaxyx()
	curses.noecho()
	curses.cbreak()
	stdscr.keypad(1)
	curses.curs_set(0)
	try:
		main(task_list)
	finally:
		sys.exit()
