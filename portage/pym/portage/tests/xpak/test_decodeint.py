# xpak/test_decodeint.py
# Copright Gentoo Foundation 2006
# Portage Unit Testing Functionality
# $Id: test_decodeint.py 14317 2009-09-21 12:49:33Z arfrever $

from portage.tests import TestCase
from portage.xpak import decodeint, encodeint

class testDecodeIntTestCase(TestCase):

	def testDecodeInt(self):
		
		for n in range(1000):
			self.assertEqual(decodeint(encodeint(n)), n)

		for n in (2 ** 32 - 1,):
			self.assertEqual(decodeint(encodeint(n)), n)
