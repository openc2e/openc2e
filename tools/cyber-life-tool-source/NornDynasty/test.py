# Known bugs that this script can detect:

# 1. Set the server and port to an unknown one.
# After a few failed disconnections, the client
# freezes.

# 2. While in a massive run, turn off the server.
# Can lock up the client.

# 3. Bytes sent/received crashes Babel.

# 4. Fire ASSERT(e != WSAEWOULDBLOCK)
# in CBabelReadWriteSocket::Write - when sending
# lots of messages.

import dstest
import thread
import time
import whrandom
import os

# Sandbox
dstest.hostport('ctlcpc179', 49152)
# dstest.hostport('heart.creatures.net', 49152)

print 'Docking Station stress tester started - in Python code\n'

# Globals
global stats, finished
stats = {} # Dictionary to store bytecount stats and so on
finished = 0

################################################################################

import md5
def file_md5sum(filename):
	f = open(filename, 'rb')
	content = f.read()
	return md5.new(content).digest()

def make_warp_dirs(dstester, username):
	if not os.path.exists(username + '/inbox'):
		os.makedirs(username + '/inbox')
	if not os.path.exists(username + '/outbox'):
		os.makedirs(username + '/outbox')
	for x in os.listdir(username + '/inbox'):
		os.remove(username + '/inbox/' + x)
	for x in os.listdir(username + '/outbox'):
		os.remove(username + '/outbox/' + x)

	dstest.warpset(dstester, username + '/inbox/', username + '/outbox/', '.warptest')

################################################################################

# Main loop for each user/thread in big simulation
def stress_loop(username, password, uin, wait, debug, messagesToSend):
	# Wait a while so the threads aren't all synchronised
	whrandom.seed(int(wait * 255))
	time.sleep(wait)

	# Make test object
	a = dstest.new(username, password)
	if debug:
		print username + ' : Online first time'
	dstest.onlineoffline(a)

	# The number is the Norns to start with
	dstest.addworld(a, 1)

	# Make warping directories if necessary, and clear them out
	make_warp_dirs(a, username)
	
 	# Main loop
	while finished == 0:
		if debug:
			print username + ' : Upload history'
		dstest.uploadhistory(a);

		randomuser = dstest.randomuser(a)
		if randomuser == '':
			randomuser = 'NONE'
			randomusernickname = 'NONE'
		else:
			randomusernickname = dstest.usernickname(a, randomuser)
		if debug:
			print username, ': Fetched random user', randomuser, 'nickname', randomusernickname
		if randomuser <> 'NONE':
			if stats[randomusernickname]['uin'] <> randomuser:
				print 'ERROR - mismatched nicknames ', stats[randomusernickname]['uin'], randomuser

		if messagesToSend > 0:
			# send a file of random data (fake Norn, effectively)
			destinationuser = whrandom.choice(stats.keys())
			destinationuin = stats[destinationuser]['uin']
			filename = dstest.warpmakeoutfilename(a, destinationuin)
			f = open(filename, 'wb')
			for x in range(0, 20):
				f.write(chr(whrandom.randint(0, 255)))
			f.truncate(whrandom.randint(50000, 70000))
			for x in range(0, 20):
				f.write(chr(whrandom.randint(0, 255)))
			f.close()
			checksum = file_md5sum(filename)
			if debug:
				print username + ' : Sending file to user ' + destinationuser + ' ' + destinationuin + ' checksum ' + checksum
			# mark it to be expected by the recipient
			markdata = (uin, checksum)
			stats[destinationuser]['awaiting'].append(markdata)

			messagesToSend = messagesToSend - 1
			stats[username]['messages_sent'] = stats[username]['messages_sent'] + 1

		if debug:
			print username + ' : Message send/receive polling'
		dstest.warpsendnow(a)
		dstest.warpspoolmessages(a)

		# check all files in inbox are as expected
		if debug:
			print username + ' : Checking inbox'
		for file in os.listdir(username + '/inbox/'):
			filename = username + '/inbox/' + file
			checksum = file_md5sum(filename)
			fromuin = dstest.warpuserwhosentfile(a, filename)
			for markdata in stats[username]['awaiting']:
				if checksum == markdata[1] and fromuin == markdata[0]:
					# Found the file, say we have and remove it
					if debug:
						print username + ' : got file from user ' + fromuin + ' checksum ' + checksum
					os.remove(filename)
					stats[username]['awaiting'].remove(markdata)

		print username + " : Awaiting ", stats[username]['awaiting']
				
		stats[username]['sent'] = dstest.bytessent(a)
		stats[username]['received'] = dstest.bytesreceived(a)

	if debug:
		print username + ' : Finished everything!'
	stats[username]['terminated'] = 1


################################################################################

# Complex test with multiple threads, and underlying
# Norn Dynasty simulation.
def do_simulation_big(debug, threadCount, historyIterations, messagesToSend):
	# Function to start a new thread
	def new_stresser(username, password, uin, debug, messagesToSend):
		global stats
		stats[username] = {'sent' : 0, 'received' : 0, 'uin' : uin, 'awaiting' : [], 'terminated' : 0, 'messages_sent' : 0}
		thread.start_new_thread(stress_loop, (username, password, uin, whrandom.random(), debug, messagesToSend))

	# Make a new thread for each user we want to be
	# (note - get the name case right!  The script above looks up the user names
	# and compares them, so we need the same case as the database has)
	if threadCount > 0:
		new_stresser('Frabcus', 'wobble', '2+1',debug, messagesToSend)
	if threadCount > 1:
		new_stresser('Wobble', 'wobble', '263+10', debug, messagesToSend)
	if threadCount > 2:
		new_stresser('Stilton', 'wobble', '30+4', debug, messagesToSend)
	if threadCount > 3:
		print "ERROR - can't have thread count as large as " + threadCount

	# Old names of other stressers:
	# snowmain, paradise, dot, kiss, smile, hairslide
	
	# Run the underlying dynasty simulation
	dynastyparams = {	
		'LifeStageLength': 10000,	# In c2e game ticks (~ 1/20th sec), varied by +-5%
		'BreedGoes': 0.05,			# Chance of breeding

		'ChanceOfSplice': 250,		# Proportion of chances of making love that are instead genetic splicings

		'DeathGoes': 0.0001,		# Chance of being killed
		'CloneGoes': 0.00005,		# Chance of being cloned
		'ExportGoes': 0.0001,		# Chance of exporting a creature
		'ImportGoes': 0.0002,		# Chance of importing a creature

		'ChanceDuplicateEvent': 10	# Chance of sending a duplicate event to test the database code copes with it, does duplicate Creatures also
	}
	dstest.setdynastyparams(dynastyparams)
	mainThreadFinished = 0

	def add(a, b): 
		result = {}
		for key in a.keys():
			result[key] = a[key] + b[key]
		return result

	while mainThreadFinished == 0:
		if historyIterations > 0:
			dstest.updatedynasty()
			historyIterations = historyIterations - 1

		# Sum statistics
		stattotals = reduce(add, stats.values())

		# See if everything is over
		if stattotals['messages_sent'] >= messagesToSend * threadCount:
			if len(stattotals['awaiting']) == 0:
				global finished
				finished = 1

		if stattotals['terminated'] == threadCount:
			mainThreadFinished = 1

		# Print debugging 
#		print 'total bytes sent/recv: ', stattotals['sent'], '/', stattotals['received']
#		print 'events: ', dstest.dynastystats()['EventSentCount'], \
#			  ' norns: ', dstest.dynastystats()['AliveNornCount'], \
#			  ' wombdeaths: ', dstest.dynastystats()['MotherDiedWhileInWombCount']
		
		global finished
		print 'messages sent:', stattotals['messages_sent'],\
			'target:',  messagesToSend * threadCount, \
			'awaiting: ', len(stattotals['awaiting']), \
			'terminated threads:', stattotals['terminated'], \
			'finished:', finished

	print "Main thread finished!"

	stattotals = reduce(add, stats.values())
	if stattotals['messages_sent'] <> messagesToSend * threadCount:
		print "ERROR - messages sent mismatch " + messagesSent + " <> " + \
				messagesToSend + " * " + threadCount

################################################################################

# Simple test, going online/offline quite fast
def do_simulation_onlineoffline(count):
	a = dstest.new('wobble', 'wobble')
	b = 0
	while b < count:
		b = b + 1
		dstest.onlineoffline(a)
		print b
		# If you remove this sleep, you get errors
		# (maybe expected to avoid Denial of Service attacks?)
		time.sleep(0.1)
	dstest.delete(a)

################################################################################

# Simple test of message sending
def do_message_test():
	u1 = dstest.new('frog', 'wobble')
	u2 = dstest.new('ugaboo', 'wobble')
	make_warp_dirs(u1, 'frog')
	make_warp_dirs(u2, 'ugaboo')
	dstest.onlineoffline(u1)
	print dstest.thisuser(u1), ' online'
	dstest.onlineoffline(u2)
	print dstest.thisuser(u2), 'online'

	# Make and send file from u1 to u2
	destuin = dstest.thisuser(u2)
	filename = dstest.warpmakeoutfilename(u1, destuin)
	print filename
	f = open(filename, 'wb')
	f.truncate(whrandom.randint(50000, 70000))
	f.close()
	dstest.warpsendnow(u1)

	# Wait for file to arrive
	b = 0
	while 0:
		dstest.warpsendnow(u1)
		b = b + 1
		print b
		time.sleep(1)
		dstest.warpspoolmessages(u2)
		for file in os.listdir('ugaboo/inbox/'):
			print file

################################################################################

# Select what we actually do:
do_simulation_big(1, 3, 1000, 50) # debug, thread_count, historyIterations, messagesToSend
# do_simulation_onlineoffline(20)
# do_message_test()
