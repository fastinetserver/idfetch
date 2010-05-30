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
import subprocess
import shlex
from threading import Thread
CLEAN_LINE="                                                                                                                                             "
MAX_ACTIVE_DOWNLOADS=5
TASK_SPACE=3
USE_CURSES_FLAG=1
DEBUG_ON=0

def fit_string(text):
	text=text+CLEAN_LINE
	return text[0:max_x-1]
def msg(index,msg_text):
	global exit_flag
	if exit_flag:
		pass
	else:
		if USE_CURSES_FLAG:
			stdscr.addstr(index,0,fit_string(msg_text))
			stdscr.refresh()
			pass
		else:
			print(fit_string(":::"+msg_text))
#			time.sleep(0.1)

def sms(msg_text):
	msg(max_y-3,msg_text)

def debug(msg_text):
	if DEBUG_ON:
		msg(max_y-3,msg_text)

def time_msg():
	if USE_CURSES_FLAG:
		msg(max_y-2,"["+time.ctime()+"]  q - Quit")

def download_msg(index,msg_text):
	msg(TASK_SPACE*index,str(index)+")"+msg_text)
	
def status_msg(index,msg_text):
	msg(TASK_SPACE*index+1,"   ["+msg_text+"]")

def total_msg(finished_downloads,index,downloaded_size,total_size):
	msg(2,"Downloaded "+str(finished_downloads)+" of "+str(index)+" distfiles ("\
					+str(downloaded_size)+" of "+str(total_size)+" bytes = "\
					+str(downloaded_size*100/total_size)+"% )")

def progress_msg(index,msg_text1):
		msg(TASK_SPACE*index+1,msg_text1)

def error_msg(msg_text):
	msg(max_y-4,"WARNING: "+msg_text)

def open_task_list():
	debug(" function open_task_list start")
	idfetch_pkg_list_file_name=idfetch_settings.TASK_DIR+"/pkg_list.list"
	idfetch_pkg_list_file_lock_name=idfetch_settings.TASK_DIR+"/!.#pkg_list.list"

	#lock file
	try:
		idfetch_pkg_list_lock_file = open(idfetch_pkg_list_file_lock_name,"w")
		idfetch_pkg_list_lock_file.close()
	except:
		error_msg("open_task_list(): Can't open file: "+idfetch_pkg_list_file_lock_name)
	try:
		idfetch_pkg_list_file = open(idfetch_pkg_list_file_name,"r")
		idfetch_pkg_list=pickle.load(idfetch_pkg_list_file)
		idfetch_pkg_list_file.close()
		#delete task list file
		os.unlink(idfetch_pkg_list_file_name)
	except:
		idfetch_pkg_list=[]
		error_msg("open_task_list(): Can't open file: "+idfetch_pkg_list_file_name+"   >>> Nothing to do.")
		time.sleep(3)

	#unlock file
	os.unlink(idfetch_pkg_list_file_lock_name)

	debug(" function open_task_list end")

	return idfetch_pkg_list

class fetchit(Thread):
	def __init__ (self,distfile,index):
		Thread.__init__(self)
#		sms("_init_ in fetchit")
		self.distfile = distfile
		self.index=index
		self.status = 2
		self.place_in_the_list = 1
#		self.wget_log_file_name = idfetch_settings.TASK_DIR+'/logs/log_wget/'+self.download_file+".log"
#		try:
#			os.unlink(self.wget_log_file_name)
#		except:
#			error_msg("Can't delete file"+self.wget_log_file_name)
		self.trials_log_file_name = idfetch_settings.TASK_DIR+'/logs/log_trials/'+distfile['name']+".log"
		
	def start(self,place_in_the_list):
		self.place_in_the_list=place_in_the_list
		Thread.start(self)
	def run(self):
		global exit_flag
#		self.download_file_url_list_file=open(idfetch_settings.TASK_DIR+'/urls/'+self.download_file+".urllist")
#		self.download_file_url_list=pickle.load(self.download_file_url_list_file)
#		self.download_file_url_list_file.close()
		self.url_list=list(self.distfile['url_list'])
		self.trials_log_file= open (self.trials_log_file_name,"w")
		self.tries_counter=0
		while (self.url_list) and (self.status !=0) and (not(exit_flag)) :
			self.tries_counter+=1
			self.current_url=self.url_list.pop()
			self.msg_text="[DL "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
			self.trials_log_file.write(self.msg_text+"\n")
			download_msg(self.place_in_the_list,self.msg_text)
			self.wget_subprocess = subprocess.Popen(['/usr/bin/wget',\
				"-c",\
				"--connect-timeout=1", self.current_url,\
				"--directory-prefix="+idfetch_settings.DIST_DIR,\
				"--tries="+str(idfetch_settings.WGET_TRIES),\
				"--read-timeout="+str(idfetch_settings.WGET_READ_TIMEOUT)],\
				bufsize=1024,stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
			mypids.append(self.wget_subprocess.pid)
			self.exit_flag=0
			while 1:
				try:
					self.line=self.wget_subprocess.stdout.readline()
					if self.line:
						progress_msg(self.place_in_the_list,self.line.rstrip())
						self.igot = re.findall('saved',self.line)
						if self.igot:
							self.status = 0
							self.wget_subprocess.wait()
							break
					else:
						self.exit_flag=1
				except:
					pass
					error_msg("ERROR!!! CODE:100")
					self.wget_subprocess.stdout.close()
				if self.exit_flag:
#					msg(self.place_in_the_list,"================== wget exited =============================")
					break

			if self.status ==0:
				self.msg_text="[FIN "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
				self.trials_log_file.write(self.msg_text+"\n")
				download_msg(self.place_in_the_list,self.msg_text)
			elif self.url_list:
				self.msg_text="[ERROR "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
				self.trials_log_file.write(self.msg_text+"\n")
				download_msg(self.place_in_the_list,self.msg_text)
			else:
				self.msg_text="[ERROR+LIST_IS_EMPTY "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
				self.trials_log_file.write(self.msg_text+"\n")
				download_msg(self.place_in_the_list,self.msg_text)
		
		self.trials_log_file.close()
	def get_place_in_the_list(self):
		return self.place_in_the_list
def do_tasks(idfetch_pkg_list):
	global exit_flag
	msg(1,"TASK DIR: "+idfetch_settings.TASK_DIR)
	msg(0,"DOWNLOADING with twrapper...")


#	Tidfetch_pkg_list = list of Tidfetch_pkg;

#	Tidfetch_pkg = dict
#		['pkg_name'] of string
#		['disfile_list'] = list of Tidfetch_distfile

#	Tidfetch_distfile = dict
#		['name'] of string;
#		['url_list'] of list of strings;
#		['size'] of int;
#		['hash  see fetch.py for more details
#		['hash2
#		['hash3

	fetch_distfile_thread_list = []
	index=0
	global total_size
	total_size=0
	global downloaded_size
	downloaded_size=0
#	print(idfetch_pkg_list)
	while idfetch_pkg_list:
		current_pkg=idfetch_pkg_list.pop()
#		print(current_pkg['pkg_name'])
#		distfiles_list_file = open(idfetch_settings.TASK_DIR+'/pkgs/'+current_pkg)
#		distfiles_list=pickle.load(distfiles_list_file)
#		distfiles_list_file.close()
		for current_distfile in current_pkg['distfile_list']:
#			print("distfile",current_distfile['name'])
			index+=1
			total_size+=current_distfile['size']
			current_fetch_distfile_thread = fetchit(current_distfile, index)
			fetch_distfile_thread_list.append(current_fetch_distfile_thread)

	finished_downloads=0
	place_in_the_list=0
	running_fetch_distfile_thread_list=[]
	fetch_distfile_thread_list.reverse()
	to_start_fetch_distfile_thread_list=list(fetch_distfile_thread_list)
	while to_start_fetch_distfile_thread_list:
		place_in_the_list+=1
		current_fetch_distfile_thread=to_start_fetch_distfile_thread_list.pop()
		running_fetch_distfile_thread_list.append(current_fetch_distfile_thread)
		current_fetch_distfile_thread.start(place_in_the_list)
		if place_in_the_list>=MAX_ACTIVE_DOWNLOADS:
			break

	total_msg(finished_downloads,index,downloaded_size,total_size)

	key='x'
	if USE_CURSES_FLAG:
		stdscr.nodelay(1)
	while (key != ord('q')) and (key != ord('Q')):
	
		for current_fetch_distfile_thread in running_fetch_distfile_thread_list:
			if current_fetch_distfile_thread.status==0:
				finished_downloads+=1
				downloaded_size+=current_fetch_distfile_thread.distfile['size']
				total_msg(finished_downloads,index,downloaded_size,total_size)
				running_fetch_distfile_thread_list.remove(current_fetch_distfile_thread)
				if to_start_fetch_distfile_thread_list:
				#start next one on the same place in the list
					starting_fetch_distfile_thread=to_start_fetch_distfile_thread_list.pop()
					running_fetch_distfile_thread_list.append(starting_fetch_distfile_thread)
					starting_fetch_distfile_thread.start(current_fetch_distfile_thread.get_place_in_the_list())
			time_msg()
		# Don't make it too hard for your cpu. Not much changes in 0.5 secs. Let's wait and give cpu some rest.
		time.sleep(0.5)
		if USE_CURSES_FLAG:
			key = stdscr.getch()
			if (key == ord('q')) or (key == ord('Q')):
				msg(28,">>>>>>>>>>>>>>>>> EXITING  <<<<<<<<<<<<<<<<<<<<<<<<<<")
	exit_flag=1


def main(idfetch_pkg_list):
	try:
		idfetch_pkg_list=open_task_list()
		do_tasks(idfetch_pkg_list)
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
		if USE_CURSES_FLAG:
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
	if USE_CURSES_FLAG:
		stdscr = curses.initscr()
		max_y,max_x=stdscr.getmaxyx()
		curses.noecho()
		curses.cbreak()
		stdscr.keypad(1)
		curses.curs_set(0)
	else:
		#don't complain, use this ones
		max_y=100
		max_x=100
	try:
		main(task_list)
	finally:
		sys.exit()
