from functions import *
import time;
def schedule():
    localtime = time.localtime(time.time());
    hour=localtime[3];
    # disable downloading distfiles that have size more than 5 000 000 bytes
    # from 8-00 to 22-00.
    if hour>8 and hour<21 and (get("distfile.size"))>5000000:
        print "reject because distfile is too big"
        reject_segment()
    # set speed limit 50 000 cps for distfiles larger than 1 000 000 bytes
    if get("distfile.size")>1000000:
        print "limit connection speed"
        set(connection.max_speed_limit, 50000)
        accept_segment()