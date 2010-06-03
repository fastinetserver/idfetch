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
from portage.checksum import perform_md5, verify_all
from portage.process import spawn
import idfetch_settings
import curses
import time
import subprocess
import json
from threading import Thread
CLEAN_LINE="                                                                                                                                             "
MAX_ACTIVE_DOWNLOADS=8
TASK_SPACE=3
USE_CURSES_FLAG=1
DEBUG_ON=0

def fit_string(text):
	text=text+CLEAN_LINE
	return text[0:max_x-1]
def msg(Y,msg_text):
	global exit_flag
	if exit_flag:
		pass
	else:
		if USE_CURSES_FLAG:
			stdscr.addstr(Y,0,fit_string(msg_text))
			stdscr.refresh()
			pass
		else:
			print(fit_string(":::"+msg_text))
#			time.sleep(0.1)

def sms(msg_text):
	msg(MSG_Y_SMS,"SMS:"+msg_text)

def total_msg():
	if distfiles_for_manual_download:
		manual_download_msg_text="Need to download manually "+str(len(distfiles_for_manual_download))+" files"
	else:
		manual_download_msg_text=""
	if total_size:
		size_msg_text=" Kb = "+str(downloaded_size*100/total_size)+"%] "
	else:
		size_msg_text=""
	msg(2,"-> Pkgs: "\
		+str(pkgs_downloaded_count)\
		+"/:"+str(pkgs_count)\
		+" - Distfiles: "+str(distfiles_downloaded_count)\
		+"/"+str(distfiles_count)\
		+" [ "+str((downloaded_size+999)/1000)\
		+"/"+str((total_size+999)/1000)\
		+size_msg_text\
		+manual_download_msg_text\
		+"<-")

def debug(msg_text):
	if DEBUG_ON:
		msg(MSG_Y_DEBUG,"DEBUG:"+msg_text)

def status_msg(msg_text):
	msg(MSG_Y_STATUS,"   ["+msg_text+"]")

def time_msg():
	if USE_CURSES_FLAG:
		msg(MSG_Y_TIME,"["+time.ctime()+"]  q - Quit")

def download_msg(place_in_the_list,distfile_num,prefix,size,url):
	msg_text=prefix+str(distfile_num)+" size:"+str((size+999)/1000)+"Kb ]: "+url
	msg(TASK_SPACE*place_in_the_list,str(place_in_the_list)+")"+msg_text)

def progress_msg(place_in_the_list,msg_text1):
		msg(TASK_SPACE*place_in_the_list+1,msg_text1)

def error_msg(msg_text):
	msg(MSG_Y_ERROR,"ERROR: "+msg_text)

def open_task_list():
	debug(" function open_task_list start")
	idfetch_pkg_list_file_name=idfetch_settings.TASK_DIR+"/pkg.list"
	idfetch_pkg_list_file_lock_name=idfetch_settings.TASK_DIR+"/!.#pkg.list"

	#lock file
	try:
		idfetch_pkg_list_lock_file = open(idfetch_pkg_list_file_lock_name,"w")
		idfetch_pkg_list_lock_file.close()
	except:
		error_msg("open_task_list(): Can't open file: "+idfetch_pkg_list_file_lock_name)
	try:
		idfetch_pkg_list_file = open(idfetch_pkg_list_file_name,"r")
		idfetch_pkg_list=json.loads(idfetch_pkg_list_file.read())
		idfetch_pkg_list_file.close()
	except:
		idfetch_pkg_list=[]
		error_msg("open_task_list(): Can't open file: "+idfetch_pkg_list_file_name+"   >>> Nothing to do.")
		time.sleep(3)

	if not(DEBUG_ON):
		#delete task list file
		try:
#			os.unlink(idfetch_pkg_list_file_name)
			debug("deleted:"+idfetch_pkg_list_file_name)
		except:
			error_msg("open_task_list(): Can't delete file: "+idfetch_pkg_list_file_name)
			time.sleep(3)

	#unlock file
	os.unlink(idfetch_pkg_list_file_lock_name)

	debug(" function open_task_list end")

	return idfetch_pkg_list

class fetchit(Thread):
	def __init__ (self,parent_pkg, distfile, distfiles_count):
		Thread.__init__(self)
		debug("_init_ in fetchit")
		self.parent_pkg=parent_pkg
		self.distfile = distfile
		self.index=distfiles_count
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
	def fetch(self):
		self.url_list=list(self.distfile['url_list'])
		self.trials_log_file= open (self.trials_log_file_name,"w")
		self.tries_counter=0
		if not(self.url_list):
			error_msg("url_list is empty for distfile:"+self.distfile['name'])
		while (self.url_list) and (self.status !=0) and (not(exit_flag)) :
			self.tries_counter+=1
			self.current_url=self.url_list.pop()
#			self.msg_text="[DL "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
#			self.trials_log_file.write(self.msg_text+"\n")
			download_msg(self.place_in_the_list,self.index,"[DL ",self.distfile['size'],self.current_url)
			self.wget_subprocess = subprocess.Popen(['/usr/bin/wget',\
				"-c",\
				"--connect-timeout=1", self.current_url,\
				"--directory-prefix="+idfetch_settings.DIST_DIR,\
				"--tries="+str(idfetch_settings.WGET_TRIES),\
				"--read-timeout="+str(idfetch_settings.WGET_READ_TIMEOUT)],\
				bufsize=1024,stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
			mypids.append(self.wget_subprocess.pid)
			while 1:
				try:
					self.line=self.wget_subprocess.stdout.readline()
					if self.line:
						progress_msg(self.place_in_the_list,self.line.rstrip())
						self.igot = re.findall('saved',self.line)
						if self.igot:
							self.verified_ok, self.reason = verify_all(
								self.myfile_path, self.distfile)
							if not self.verified_ok:
								sms("ERROR: while checking hash for "+self.myfile_path+" Refetching...")
								os.unlink(self.myfile_path)
							else:
								self.status=0
							self.wget_subprocess.wait()
							break
					else:
						break
				except:
					error_msg("ERROR!!! CODE:100")
			self.wget_subprocess.stdout.close()

			if self.status ==0:
				self.msg_text="[FIN "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
				self.trials_log_file.write(self.msg_text+"\n")
				download_msg(self.place_in_the_list,self.index,"[FIN ",self.distfile['size'],self.current_url)
			elif self.url_list:
				self.msg_text="[ERROR "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
				self.trials_log_file.write(self.msg_text+"\n")
				download_msg(self.place_in_the_list,self.index,"[ERROR ",self.distfile['size'],self.current_url)
			else:
				self.msg_text="[ERROR+LIST_IS_EMPTY "+str(self.index)+" size:"+str(self.distfile['size'])+"b ]: "+self.current_url
				self.trials_log_file.write(self.msg_text+"\n")
				download_msg(self.place_in_the_list,self.index,"[ERROR+LIST_IS_EMPTY ",self.distfile['size'],self.current_url)
		self.trials_log_file.close()

	def run(self):
		sms("started "+str(self.index))
		global exit_flag
		self.myfile_path=idfetch_settings.DIST_DIR+"/"+self.distfile['name']
#		"""
#		fetched  status
#		0        nonexistent
#		1        partially downloaded
#		2        completely downloaded
#		"""
		self.fetched = 0

		if 1==1:
				try:
					self.mystat = os.stat(self.myfile_path)
				except OSError as e:
#					if e.errno not in (errno.ENOENT, errno.ESTALE):
#						raise
					debug("cant get mystat for file: "+self.myfile_path)
					self.fetch()
					del e
				else:
					# If the file is empty then it's obviously invalid. Remove
					# the empty file and try to download if possible.
					if self.mystat.st_size == 0:
							try:
								os.unlink(myfile_path)
								self.fetch()
							except EnvironmentError:
								debug("Can\'t delete the file")
					else:
						if self.mystat.st_size < self.distfile["size"]:
#							and \
#							not restrict_fetch:
							self.fetched = 1 # Try to resume this download.
							debug("Size is small, continue fetching...")
							self.fetch()
						else:
							self.verified_ok, self.reason = verify_all(
								self.myfile_path, self.distfile)
							if not self.verified_ok:
								sms("ERROR: while checking hash for "+self.myfile_path+" Refetching...")
								os.unlink(self.myfile_path)
								self.fetch()
							else:
								# Already fetched and with correct hashes
								self.status=0

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
#		['RMD160']  see fetch.py for more details
#		['SHA1']
#		['SHA256']

	fetch_distfile_thread_list = []
	global pkgs_count
	global pkgs_downloaded_count
	global pkgs_installed_count
	global distfiles_count
	global distfiles_downloaded_count
	global total_size
	global downloaded_size
	global install_pkgs_is_running
	global distfiles_for_manual_download

	pkgs_count=0
	pkgs_downloaded_count=0
	pkgs_installed_count=0
	distfiles_count=0
	distfiles_downloaded_count=0
	total_size=0
	downloaded_size=0
	install_pkgs_is_running=0

	for current_pkg in idfetch_pkg_list:
		current_pkg['installed']=0
		current_pkg['distfiles_count']=0
		current_pkg['fetched_distfiles_count']=0
		current_pkg_has_distfiles_to_download=0
		for current_distfile in current_pkg['distfile_list']:
			#some files may have duplicates, let's skip them
			if not(current_distfile['url_list']):
				if current_distfile['myuri']:
					error_msg("Download "+current_distfile['myuri']+" manually and save to "+idfetch_settings.DIST_DIR)
					if not(current_distfile['name'] in distfiles_for_manual_download):
						distfiles_for_manual_download.append(current_distfile['name'])
						total_msg()
				continue # donwload rest of the files
			no_duplicate=1
			for cur_fetch_distfile_thread in fetch_distfile_thread_list:
				if current_distfile['name']==cur_fetch_distfile_thread.distfile['name']:
					no_duplicate=0
					break
			if no_duplicate:
				#create new thread because there is no duplicate
#				pkg_contains_distfiles_for_download=1
				distfiles_count+=1
				current_pkg_has_distfiles_to_download=1
				current_pkg['distfiles_count']+=1
				debug("============================================")
				debug("pkg:  "+current_pkg['pkg_name'])
				debug("name: "+current_distfile['name'])
				debug("size: "+str(current_distfile['size']/1000))
				total_size+=current_distfile['size']
				current_fetch_distfile_thread = fetchit(current_pkg, current_distfile, distfiles_count)
				fetch_distfile_thread_list.append(current_fetch_distfile_thread)
		if current_pkg_has_distfiles_to_download:
			pkgs_count+=1

#	pkgs_downloaded_count
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

	total_msg()

	key='x'
	if USE_CURSES_FLAG:
		stdscr.nodelay(1)
	while (key != ord('q')) and (key != ord('Q')) or exit_flag:
	
		for current_fetch_distfile_thread in running_fetch_distfile_thread_list:
			if current_fetch_distfile_thread.status==0:
				distfiles_downloaded_count+=1
				current_fetch_distfile_thread.parent_pkg['fetched_distfiles_count']+=1
				downloaded_size+=current_fetch_distfile_thread.distfile['size']
				running_fetch_distfile_thread_list.remove(current_fetch_distfile_thread)
				if to_start_fetch_distfile_thread_list:
				#start next one on the same place in the list
					starting_fetch_distfile_thread=to_start_fetch_distfile_thread_list.pop()
					running_fetch_distfile_thread_list.append(starting_fetch_distfile_thread)
					starting_fetch_distfile_thread.start(current_fetch_distfile_thread.get_place_in_the_list())
					#install if package has all the files and previous pkgs are installed
					if current_fetch_distfile_thread.parent_pkg['fetched_distfiles_count']>=current_fetch_distfile_thread.parent_pkg['distfiles_count']:
						pkgs_downloaded_count+=1
				total_msg()
			time_msg()
		# Don't make it too hard for your cpu. Not much changes in 0.5 secs. Let's wait and give cpu some rest.
		time.sleep(0.5)
		if USE_CURSES_FLAG:
			key = stdscr.getch()
			if (key == ord('q')) or (key == ord('Q')):
				status_msg(">>>>>>>>>>>>>>>>> EXITING  <<<<<<<<<<<<<<<<<<<<<<<<<<")
		if (distfiles_downloaded_count == distfiles_count) and not(install_pkgs_is_running):
			msg(28,"All files have been downloaded and installed. Exiting in 3 secs ...")
			time.sleep(3)
			exit_flag=1
			break
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

if __name__ == '__main__':
	exit_flag=0
	distfiles_for_manual_download=list()
	idfetch_pkg_list=[]
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
		MSG_Y_TIME=max_y-5
		MSG_Y_SMS=max_y-4
		MSG_Y_DEBUG=max_y-3
		MSG_Y_ERROR=max_y-2
		MSG_Y_STATUS=max_y-1

		main(idfetch_pkg_list)
	finally:
		sys.exit()
