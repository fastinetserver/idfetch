# test_dodir.py -- Portage Unit Testing Functionality
# Copyright 2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id: test_dodir.py 14446 2009-09-27 18:23:32Z zmedico $

from portage.tests.bin.setup_env import BinTestCase, dodir, exists_in_D

class DoDir(BinTestCase):
	def testDoDir(self):
		dodir("usr /usr")
		exists_in_D("/usr")
		dodir("/var/lib/moocow")
		exists_in_D("/var/lib/moocow")
