import sys
import socket

def get(var_name):
	print("GET::"+var_name)
	client_socket.send ("g<c>"+var_name)
	data = client_socket.recv(512)
	print "RECIEVED:" , data
	#connection.num,
	#connection.max_speed_limit,
	#network.num,
	#network.active_connections_count,
	#distfile.size,
	#distfile.dld_segments_count,
	#distfile.segments_count, 
	#distfile.active_connections_count,
	#segment.num,
	#segment.try_num,
	#segment.size,
	if ((var_name=="connection.url") or (var_name=="distfile.name") or (var_name=="segment.range")):
		return data
	else:
		return int(data)

def set(var_name,var_value):
	var_value_str=str(var_value);
	print("SET::"+var_name+"="+var_value_str)
	client_socket.send ("s<c>"+var_name+"<n>"+var_value_str)
	data = client_socket.recv(512)
	print "RECIEVED:" , data
	if (data=="o<r>"):
		return 0
	else:
		return 1

def accept_segment():
	print "Accepting segment"
	client_socket.send ("a<c>")
	client_socket.close()
	sys.exit(0)

def reject_segment():
	print "Rejecting segment"
	client_socket.send ("r<c>")
	client_socket.close()
	sys.exit(0)
# in case users forget to use quotes
class Tconnection:
	num="connection.num"
	max_speed_limit="connection.max_speed_limit"
	url="connection.url"

class Tnetwork:
	num="network.num"
	mode="network.mode"
	active_connections_count="network.active_connections_count"

class Tdistfile:
	name="distfile.name"
	size="distfile.size"
	dld_segments_count="distfile.dld_segments_count"
	segments_count="distfile.segments_count"
	active_connections_count="distfile.active_connections_count"

class Tsegment:
	num="segment.num"
	try_num="segment.try_num"
	size="segment.size"
	range="segment.range"

connection=Tconnection
network=Tnetwork
distfile=Tdistfile
segment=Tsegment

client_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
client_socket.connect("/tmp/segget_script_socket")