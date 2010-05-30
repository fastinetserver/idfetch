# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id: BlockerDepPriority.py 13690 2009-06-25 04:13:47Z zmedico $

from _emerge.DepPriority import DepPriority
class BlockerDepPriority(DepPriority):
	__slots__ = ()
	def __int__(self):
		return 0

	def __str__(self):
		return 'blocker'

BlockerDepPriority.instance = BlockerDepPriority()
