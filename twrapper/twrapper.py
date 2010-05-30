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
import pickle
import curses
import time
import subprocess
import shlex
from threading import Thread
CLEAN_LINE="                                                                                                                                             "
MAX_ACTIVE_DOWNLOADS=8
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
	except:
		idfetch_pkg_list=[]
		error_msg("open_task_list(): Can't open file: "+idfetch_pkg_list_file_name+"   >>> Nothing to do.")
		time.sleep(3)

	#delete task list file
	try:
		os.unlink(idfetch_pkg_list_file_name)
		debug("deleted:"+idfetch_pkg_list_file_name)
	except:
		error_msg("open_task_list(): Can't delete file: "+idfetch_pkg_list_file_name)
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
	def fetch(self):
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

	def run(self):
		sms("started "+str(self.index))
		global exit_flag
#		self.download_file_url_list_file=open(idfetch_settings.TASK_DIR+'/urls/'+self.download_file+".urllist")
#		self.download_file_url_list=pickle.load(self.download_file_url_list_file)
#		self.download_file_url_list_file.close()
		self.myfile_path=idfetch_settings.DIST_DIR+"/"+self.distfile['name']
#	for myfile in filedict:
#		"""
#		fetched  status
#		0        nonexistent
#		1        partially downloaded
#		2        completely downloaded
#		"""
#		fetched = 0

#		orig_digests = mydigests.get(myfile, {})
#		size = orig_digests.get("size")

#		if size == 0:
			# Zero-byte distfiles are always invalid, so discard their digests.
#			del mydigests[myfile]
#			orig_digests.clear()
#			size = None
#		pruned_digests = orig_digests
#		if parallel_fetchonly:
#			pruned_digests = {}
#			if size is not None:
#				pruned_digests["size"] = size

#		myfile_path = os.path.join(mysettings["DISTDIR"], myfile)
#		has_space = True
#		has_space_superuser = True
#		file_lock = None
#		if listonly:
#			writemsg_stdout("\n", noiselevel=-1)
#		else:
#			# check if there is enough space in DISTDIR to completely store myfile
			# overestimate the filesize so we aren't bitten by FS overhead
#			if size is not None and hasattr(os, "statvfs"):
#				vfs_stat = os.statvfs(mysettings["DISTDIR"])
#				try:
#					mysize = os.stat(myfile_path).st_size
#				except OSError as e:
#					if e.errno not in (errno.ENOENT, errno.ESTALE):
#						raise
#					del e
#					mysize = 0
#				if (size - mysize + vfs_stat.f_bsize) >= \
#					(vfs_stat.f_bsize * vfs_stat.f_bavail):

#					if (size - mysize + vfs_stat.f_bsize) >= \
#						(vfs_stat.f_bsize * vfs_stat.f_bfree):
#						has_space_superuser = False

#					if not has_space_superuser:
#						has_space = False
#					elif secpass < 2:
#						has_space = False
#					elif userfetch:
#						has_space = False

#			if not has_space:
#				writemsg(_("!!! Insufficient space to store %s in %s\n") % \
#					(myfile, mysettings["DISTDIR"]), noiselevel=-1)

#				if has_space_superuser:
#					writemsg(_("!!! Insufficient privileges to use "
#						"remaining space.\n"), noiselevel=-1)
#					if userfetch:
#						writemsg(_("!!! You may set FEATURES=\"-userfetch\""
#							" in /etc/make.conf in order to fetch with\n"
#							"!!! superuser privileges.\n"), noiselevel=-1)

#			if distdir_writable and use_locks:

#				lock_kwargs = {}
#				if fetchonly:
#					lock_kwargs["flags"] = os.O_NONBLOCK

#				try:
#					file_lock = lockfile(myfile_path,
#						wantnewlockfile=1, **lock_kwargs)
#				except TryAgain:
#					writemsg(_(">>> File '%s' is already locked by "
#						"another fetcher. Continuing...\n") % myfile,
#						noiselevel=-1)
#					continue
#		try:
#			if not listonly:
#
#				eout = EOutput()
#				eout.quiet = mysettings.get("PORTAGE_QUIET") == "1"
#				match, mystat = _check_distfile(
#					myfile_path, pruned_digests, eout)
#				if match:
#					if distdir_writable:
#						try:
#							apply_secpass_permissions(myfile_path,
#								gid=portage_gid, mode=0o664, mask=0o2,
#								stat_cached=mystat)
#						except PortageException as e:
#							if not os.access(myfile_path, os.R_OK):
#								writemsg(_("!!! Failed to adjust permissions:"
#									" %s\n") % str(e), noiselevel=-1)
#							del e
#					continue

#				if distdir_writable and mystat is None:
#					# Remove broken symlinks if necessary.
#					try:
#						os.unlink(myfile_path)
#					except OSError:
#						pass

#				if mystat is not None:
#					if stat.S_ISDIR(mystat.st_mode):
#						writemsg_level(
#							_("!!! Unable to fetch file since "
#							"a directory is in the way: \n"
#							"!!!   %s\n") % myfile_path,
#							level=logging.ERROR, noiselevel=-1)
#						return 0

#					if mystat.st_size == 0:
#						if distdir_writable:
#							try:
#								os.unlink(myfile_path)
#							except OSError:
#								pass
#					elif distdir_writable:
#						if mystat.st_size < fetch_resume_size and \
#							mystat.st_size < size:
#							# If the file already exists and the size does not
#							# match the existing digests, it may be that the
#							# user is attempting to update the digest. In this
#							# case, the digestgen() function will advise the
#							# user to use `ebuild --force foo.ebuild manifest`
#							# in order to force the old digests to be replaced.
#							# Since the user may want to keep this file, rename
#							# it instead of deleting it.
#							writemsg(_(">>> Renaming distfile with size "
#								"%d (smaller than " "PORTAGE_FETCH_RESU"
#								"ME_MIN_SIZE)\n") % mystat.st_size)
#							temp_filename = \
#								_checksum_failure_temp_file(
#								mysettings["DISTDIR"], myfile)
#							writemsg_stdout(_("Refetching... "
#								"File renamed to '%s'\n\n") % \
#								temp_filename, noiselevel=-1)
#						elif mystat.st_size >= size:
#							temp_filename = \
#								_checksum_failure_temp_file(
#								mysettings["DISTDIR"], myfile)
#							writemsg_stdout(_("Refetching... "
#								"File renamed to '%s'\n\n") % \
#								temp_filename, noiselevel=-1)

#				if distdir_writable and ro_distdirs:
#					readonly_file = None
#					for x in ro_distdirs:
#						filename = os.path.join(x, myfile)
#						match, mystat = _check_distfile(
#							filename, pruned_digests, eout)
#						if match:
#							readonly_file = filename
#							break
#					if readonly_file is not None:
#						try:
#							os.unlink(myfile_path)
#						except OSError as e:
#							if e.errno not in (errno.ENOENT, errno.ESTALE):
#								raise
#							del e
#						os.symlink(readonly_file, myfile_path)
#						continue

#				if fsmirrors and not os.path.exists(myfile_path) and has_space:
#					for mydir in fsmirrors:
#						mirror_file = os.path.join(mydir, myfile)
#						try:
#							shutil.copyfile(mirror_file, myfile_path)
#							writemsg(_("Local mirror has file: %s\n") % myfile)
#							break
#						except (IOError, OSError) as e:
#							if e.errno not in (errno.ENOENT, errno.ESTALE):
#								raise
#							del e
		if 1==1:
				try:
					self.mystat = os.stat(self.myfile_path)
				except OSError as e:
#					if e.errno not in (errno.ENOENT, errno.ESTALE):
#						raise
					debug("cant get mystat for file: "+self.myfile_path)
					del e
					self.fetch()
				else:
#					try:
#						apply_secpass_permissions(
#							self.myfile_path, gid=portage_gid, mode=0o664, mask=0o2,
#							stat_cached=self.mystat)
#					except PortageException as e:
#						if not os.access(self.myfile_path, os.R_OK):
#							writemsg(_("!!! Failed to adjust permissions:"
#								" %s\n") % str(e), noiselevel=-1)
					if 1==1:
					# If the file is empty then it's obviously invalid. Remove
					# the empty file and try to download if possible.
#					if mystat.st_size == 0:
#						if distdir_writable:
#							try:
#								os.unlink(myfile_path)
#							except EnvironmentError:
#								pass
#					elif myfile not in mydigests:
						# We don't have a digest, but the file exists.  We must
						# assume that it is fully downloaded.
#						continue
#					else:
						if self.mystat.st_size < self.distfile["size"]:
#							and \
#							not restrict_fetch:
							self.fetched = 1 # Try to resume this download.
#						elif parallel_fetchonly and \
#							mystat.st_size == mydigests[myfile]["size"]:
#							eout = EOutput()
#							eout.quiet = \
#								mysettings.get("PORTAGE_QUIET") == "1"
#							eout.ebegin(
#								"%s size ;-)" % (myfile, ))
#							eout.eend(0)
#							continue
							debug("size too small, continue fetch")
							self.fetch()
						else:
							self.verified_ok, self.reason = verify_all(
								self.myfile_path, self.distfile)
							if not self.verified_ok:
								sms("ERROR: while checking hash for "+self.myfile_path+" Refetching...")
								os.unlink(self.myfile_path)
								self.fetch()
							else:
#								eout = EOutput()
#								eout.quiet = \
#									mysettings.get("PORTAGE_QUIET", None) == "1"
#								digests = mydigests.get(myfile)
#								if digests:
#									digests = list(digests)
#									digests.sort()
#									eout.ebegin(
#										"%s %s ;-)" % (myfile, " ".join(digests)))
#									eout.eend(0)
#								continue # fetch any remaining files
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

			#some files may have duplicates, let's skip them
			no_duplicate=1
			for cur_fetch_distfile_thread in fetch_distfile_thread_list:
				if current_distfile['name']==cur_fetch_distfile_thread.distfile['name']:
					no_duplicate=0
					break
			if no_duplicate:
				#create new thread because there is no duplicate
				index+=1
				debug("============================================")
				debug("pkg:  "+current_pkg['pkg_name'])
				debug("name: "+current_distfile['name'])
				debug("size: "+str(current_distfile['size']/1000))
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
	while (key != ord('q')) and (key != ord('Q')) or exit_flag:
	
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
		if finished_downloads == index:
			msg(28,"All files have been downloaded. Exiting in 3 secs ...")
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
