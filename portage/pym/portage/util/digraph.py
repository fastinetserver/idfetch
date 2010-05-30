# Copyright 2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id: digraph.py 15659 2010-03-02 20:59:20Z zmedico $

__all__ = ['digraph']

from portage.util import writemsg

class digraph(object):
	"""
	A directed graph object.
	"""

	def __init__(self):
		"""Create an empty digraph"""
		
		# { node : ( { child : priority } , { parent : priority } ) }
		self.nodes = {}
		self.order = []

	def add(self, node, parent, priority=0):
		"""Adds the specified node with the specified parent.
		
		If the dep is a soft-dep and the node already has a hard
		relationship to the parent, the relationship is left as hard."""
		
		if node not in self.nodes:
			self.nodes[node] = ({}, {}, node)
			self.order.append(node)
		
		if not parent:
			return
		
		if parent not in self.nodes:
			self.nodes[parent] = ({}, {}, parent)
			self.order.append(parent)

		priorities = self.nodes[node][1].get(parent)
		if priorities is None:
			priorities = []
			self.nodes[node][1][parent] = priorities
			self.nodes[parent][0][node] = priorities
		priorities.append(priority)
		priorities.sort()

	def remove(self, node):
		"""Removes the specified node from the digraph, also removing
		and ties to other nodes in the digraph. Raises KeyError if the
		node doesn't exist."""
		
		if node not in self.nodes:
			raise KeyError(node)
		
		for parent in self.nodes[node][1]:
			del self.nodes[parent][0][node]
		for child in self.nodes[node][0]:
			del self.nodes[child][1][node]
		
		del self.nodes[node]
		self.order.remove(node)

	def difference_update(self, t):
		"""
		Remove all given nodes from node_set. This is more efficient
		than multiple calls to the remove() method.
		"""
		if isinstance(t, (list, tuple)) or \
			not hasattr(t, "__contains__"):
			t = frozenset(t)
		order = []
		for node in self.order:
			if node not in t:
				order.append(node)
				continue
			for parent in self.nodes[node][1]:
				del self.nodes[parent][0][node]
			for child in self.nodes[node][0]:
				del self.nodes[child][1][node]
			del self.nodes[node]
		self.order = order

	def remove_edge(self, child, parent):
		"""
		Remove edge in the direction from child to parent. Note that it is
		possible for a remaining edge to exist in the opposite direction.
		Any endpoint vertices that become isolated will remain in the graph.
		"""

		# Nothing should be modified when a KeyError is raised.
		for k in parent, child:
			if k not in self.nodes:
				raise KeyError(k)

		# Make sure the edge exists.
		if child not in self.nodes[parent][0]:
			raise KeyError(child)
		if parent not in self.nodes[child][1]:
			raise KeyError(parent)

		# Remove the edge.
		del self.nodes[child][1][parent]
		del self.nodes[parent][0][child]

	def __iter__(self):
		return iter(self.order)

	def contains(self, node):
		"""Checks if the digraph contains mynode"""
		return node in self.nodes

	def get(self, key, default=None):
		node_data = self.nodes.get(key, self)
		if node_data is self:
			return default
		return node_data[2]

	def all_nodes(self):
		"""Return a list of all nodes in the graph"""
		return self.order[:]

	def child_nodes(self, node, ignore_priority=None):
		"""Return all children of the specified node"""
		if ignore_priority is None:
			return list(self.nodes[node][0])
		children = []
		if hasattr(ignore_priority, '__call__'):
			for child, priorities in self.nodes[node][0].items():
				for priority in priorities:
					if not ignore_priority(priority):
						children.append(child)
						break
		else:
			for child, priorities in self.nodes[node][0].items():
				if ignore_priority < priorities[-1]:
					children.append(child)
		return children

	def parent_nodes(self, node, ignore_priority=None):
		"""Return all parents of the specified node"""
		if ignore_priority is None:
			return list(self.nodes[node][1])
		parents = []
		if hasattr(ignore_priority, '__call__'):
			for parent, priorities in self.nodes[node][1].items():
				for priority in priorities:
					if not ignore_priority(priority):
						parents.append(parent)
						break
		else:
			for parent, priorities in self.nodes[node][1].items():
				if ignore_priority < priorities[-1]:
					parents.append(parent)
		return parents

	def leaf_nodes(self, ignore_priority=None):
		"""Return all nodes that have no children
		
		If ignore_soft_deps is True, soft deps are not counted as
		children in calculations."""
		
		leaf_nodes = []
		if ignore_priority is None:
			for node in self.order:
				if not self.nodes[node][0]:
					leaf_nodes.append(node)
		elif hasattr(ignore_priority, '__call__'):
			for node in self.order:
				is_leaf_node = True
				for child, priorities in self.nodes[node][0].items():
					for priority in priorities:
						if not ignore_priority(priority):
							is_leaf_node = False
							break
					if not is_leaf_node:
						break
				if is_leaf_node:
					leaf_nodes.append(node)
		else:
			for node in self.order:
				is_leaf_node = True
				for child, priorities in self.nodes[node][0].items():
					if ignore_priority < priorities[-1]:
						is_leaf_node = False
						break
				if is_leaf_node:
					leaf_nodes.append(node)
		return leaf_nodes

	def root_nodes(self, ignore_priority=None):
		"""Return all nodes that have no parents.
		
		If ignore_soft_deps is True, soft deps are not counted as
		parents in calculations."""
		
		root_nodes = []
		if ignore_priority is None:
			for node in self.order:
				if not self.nodes[node][1]:
					root_nodes.append(node)
		elif hasattr(ignore_priority, '__call__'):
			for node in self.order:
				is_root_node = True
				for parent, priorities in self.nodes[node][1].items():
					for priority in priorities:
						if not ignore_priority(priority):
							is_root_node = False
							break
					if not is_root_node:
						break
				if is_root_node:
					root_nodes.append(node)
		else:
			for node in self.order:
				is_root_node = True
				for parent, priorities in self.nodes[node][1].items():
					if ignore_priority < priorities[-1]:
						is_root_node = False
						break
				if is_root_node:
					root_nodes.append(node)
		return root_nodes

	def is_empty(self):
		"""Checks if the digraph is empty"""
		return len(self.nodes) == 0

	def clone(self):
		clone = digraph()
		clone.nodes = {}
		memo = {}
		for children, parents, node in self.nodes.values():
			children_clone = {}
			for child, priorities in children.items():
				priorities_clone = memo.get(id(priorities))
				if priorities_clone is None:
					priorities_clone = priorities[:]
					memo[id(priorities)] = priorities_clone
				children_clone[child] = priorities_clone
			parents_clone = {}
			for parent, priorities in parents.items():
				priorities_clone = memo.get(id(priorities))
				if priorities_clone is None:
					priorities_clone = priorities[:]
					memo[id(priorities)] = priorities_clone
				parents_clone[parent] = priorities_clone
			clone.nodes[node] = (children_clone, parents_clone, node)
		clone.order = self.order[:]
		return clone

	def delnode(self, node):
		try:
			self.remove(node)
		except KeyError:
			pass

	def firstzero(self):
		leaf_nodes = self.leaf_nodes()
		if leaf_nodes:
			return leaf_nodes[0]
		return None

	def hasallzeros(self, ignore_priority=None):
		return len(self.leaf_nodes(ignore_priority=ignore_priority)) == \
			len(self.order)

	def debug_print(self):
		def output(s):
			writemsg(s, noiselevel=-1)
		for node in self.nodes:
			output("%s " % (node,))
			if self.nodes[node][0]:
				output("depends on\n")
			else:
				output("(no children)\n")
			for child, priorities in self.nodes[node][0].items():
				output("  %s (%s)\n" % (child, priorities[-1],))

	# Backward compatibility
	addnode = add
	allnodes = all_nodes
	allzeros = leaf_nodes
	hasnode = contains
	__contains__ = contains
	empty = is_empty
	copy = clone
