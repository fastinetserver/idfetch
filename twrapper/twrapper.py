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
from portage.process import spawn
import idfetch_settings
import pickle
import curses
import time
from threading import Thread
CLEAN_LINE="                                                                                                                                             "
MAX_ACTIVE_DOWNLOADS=8
TASK_SPACE=4

def time_msg():
	stdscr.addstr(0,50,"["+time.ctime()+"]  q - Quit")
	stdscr.refresh()

def fit_string(text):
	text=text+CLEAN_LINE
	return text[0:max_x-1]
def msg(index,msg_text):
	global exit_flag
#	print(msg_text)
	if exit_flag:
		pass
	else:
		stdscr.addstr(index,0,fit_string(msg_text))
		stdscr.refresh()
#	stdscr.nodelay(1)
#	key = stdscr.getch()
#	if key== ord('q'):
#		sys.exit()

def download_msg(index,msg_text):
	msg(TASK_SPACE*index,str(index)+")"+msg_text)
	
def status_msg(index,msg_text):
	msg(TASK_SPACE*index+1,"   ["+msg_text+"]")

def total_msg(msg_text):
	msg(2,msg_text)

	
def progress_msg(index,msg_text1,msg_text2):
#	global exit_flag
#	if exit_flag:
#		pass
#	else:
		stdscr.addstr(TASK_SPACE*index+1,0,fit_string("  "+msg_text1))
		stdscr.addstr(TASK_SPACE*index+2,0,fit_string("  "+msg_text2))
		stdscr.refresh()
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
#		msg(14+place_in_the_list,"lkajkldfjjlsdkjflaskjdfkalskdjallllllllfalskdjflaskdjffff"+str(self.place_in_the_list))
		Thread.start(self)
	def run(self):
#		msg(0,'DOWNLOADING: '+self.download_file)
		self.download_file_url_list_file=open(idfetch_settings.TASK_DIR+'/urls/'+self.download_file+".urllist")
		self.download_file_url_list=pickle.load(self.download_file_url_list_file)
		self.download_file_url_list_file.close()
#		print(self.download_file_url_list)
#		while downloadfile_url_list:
#			print(downloadfile_url_list.pop())
#		return
#		a=os.spawnl(os.P_NOWAIT,"/bin/touch","/usr/lib/portage/swrapper/1.txt")
		self.trials_log_file= open (self.trials_log_file_name,"w")
		#wget_output = os.popen("/usr/bin/wget "+self.download_file_url_list.pop()+" -o "+log_file,"r")
		while (self.download_file_url_list) and (self.status !=0) :
			self.current_url=self.download_file_url_list.pop()
			self.trials_log_file.write("[DL "+str(self.index)+"]: "+self.current_url+"\n")
			download_msg(self.place_in_the_list,"[DL "+str(self.index)+"]: "+self.current_url)
#			wget_run_status=os.spawnl(os.P_WAIT,"/usr/bin/wget","wget","--connect-timeout=1","--progress=bar:force",self.current_url, "-o",self.wget_log_file_name)
			wget_run_status=os.spawnl(os.P_WAIT,"/usr/bin/wget","wget","--connect-timeout=1",\
				self.current_url,"--directory-prefix="+idfetch_settings.DIST_DIR,\
				'--tries='+str(idfetch_settings.WGET_TRIES),\
				"--read-timeout="+str(idfetch_settings.WGET_READ_TIMEOUT),"-o",self.wget_log_file_name)
#			wget_run_status = os.popen("/usr/bin/wget "+self.download_file_url_list.pop(),"r")
			wget_output= open(self.wget_log_file_name,"r")
			while 1:
				line = wget_output.readline()
				if not line: break
				igot = re.findall('saved',line)
				if igot:
					self.status = 0
					break
#				int(igot[0])
#			msg(10+self.index,"exited")
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
####		a=os.spawnl(os.P_NOWAIT,"/usr/bin/wget","wget",downloadfile_url_list.pop(), "-o",log_file)
#		a=os.system("/bin/touch /usr/lib/portage/swrapper/touch.txt")
#		a=os.system("wget www.mail.ru")
#		print("boom:",a)
#		spawn(["/bin/bash", "-c", "exec \"$@\""],'','')
	def show_fetch_progress(self):
		try:
			fileHandle = open(self.wget_log_file_name,"r")
			lineList = fileHandle.readlines()
			fileHandle.close()
			if len(lineList)>1:
				progress_msg(self.place_in_the_list,lineList[-2],lineList[-1])
		except: 
			#error_msg("ERROR while reading"+self.wget_log_file_name)
			pass
	def get_place_in_the_list(self):
		return self.place_in_the_list
def do_tasks(task_list,exit_flag):
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
		key = stdscr.getch()
		for current_fetch_distfile_thread in running_fetch_distfile_thread_list:
#			status_msg(current_fetch.index,"Status: "+report[current_fetch.status])
			current_fetch_distfile_thread.show_fetch_progress()
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
	exit_flag=1


def main(task_list,exit_flag):
	try:
		task_list=open_task_list()
		do_tasks(task_list,exit_flag)
	finally:
		curses.nocbreak()
		stdscr.keypad(0)
		curses.echo()
		curses.endwin()
	print("twrapper exited")
	sys.exit()

#		if key== ord('q'):
#		for current_fetch in fetchlist:
#			current_fetch.join()
##			print("Status for",current_fetch.download_file,"is",report[current_fetch.status])

if __name__ == '__main__':
	exit_flag=0
	task_list=[]
	stdscr = curses.initscr()
	max_y,max_x=stdscr.getmaxyx()
	curses.noecho()
	curses.cbreak()
	stdscr.keypad(1)
#	win = curses.newwin(10, 30, 20, 0)
#	win.addstr(1, 1, "Current")
#	win.refresh()
#	time.sleep(5)
	main(task_list,exit_flag)
