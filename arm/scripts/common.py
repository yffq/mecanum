import os, sys, inspect
import subprocess
import fileinput
from xml.etree import ElementTree

# http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder/6098238#6098238
# Returns the directory of common.py, not the script
def getScriptDir():
	path = os.path.split(inspect.getfile(inspect.currentframe()))[0]
	cmd_folder = os.path.realpath(os.path.abspath(path))
	return cmd_folder

def getSetting(setting):
	global username, name, password, fqdn, mmc, macaddress, packages
	if setting == 'username':
		return username
	elif setting == 'name':
		return name
	elif setting == 'password':
		return password
	elif setting == 'fqdn':
		return fqdn
	elif setting == 'mmc':
		return mmc
	elif setting == 'macaddress':
		return macaddress
	elif setting == 'packages':
		return packages

def loadSettings():
	''' Load configuration parameters from settings.xml into the following
	    global variables:
	    username, name, password, fqdn, mmc, macaddress, packages
	'''
	global username, name, password, fqdn, mmc, macaddress, packages
	settings = ElementTree.parse(os.path.join('..', 'settings.xml')).getroot()
	username = settings.findtext('username')
	name = settings.findtext('name')
	password = settings.findtext('password')
	fqdn = settings.findtext('fqdn') # Fully-qualified domain name
	mmc = settings.findtext('mmc')
	macaddress = settings.findtext('macaddress')
	packages = []
	for pkglist in settings.findall('packages'):
		if pkglist.text:
			for pkg in str(pkglist.text).split(' '):
				packages.append(pkg)
	print('Using the following settings:')
	print('Username: ' + username)
	print('Name: ' + name)
	print('Password: ' + password)
	print('FQDN: ' + fqdn)
	print('MMC: ' + mmc)
	print('MAC address: ' + macaddress)
	print('Packages: ' + ' '.join(packages))
	print('')

# Utility function
# http://stackoverflow.com/questions/39086/search-and-replace-a-line-in-a-file-in-python
def replaceAll(file, searchExp, replaceExp):
	for line in fileinput.input(file, inplace=1):
		if searchExp in line:
			line = line.replace(searchExp, replaceExp)
		sys.stdout.write(line)

# For now, just call sudo apt-get install.
# In the future, grep dpkg --get-selections (-a) for installed packages and
# only do sudo if necessary.
def ensureDependencies(packages):
	cmd = ['sudo', 'apt-get', '-y', 'install']
	cmd.extend(packages)
	subprocess.call(cmd)

def gitCloneAndEnter(repo, commit):
	if not repo.endswith('.git'):
		raise Exception('Not a git repository')
	name = repo[:-4].rpartition('/')[2]
	if not os.path.isdir(name):
		os.makedirs(name) # If name is invalid, this will raise an exception
	if not os.path.exists(os.path.join(name, '.git', 'config')):
		subprocess.call(['git', 'clone', repo, name])
		os.chdir(name)
		subprocess.call(['git', 'checkout', commit, '-b', 'mecanum'])
	else:
		os.chdir(name)
		gitCleanup()
	return name

def gitCleanup():
	subprocess.call(['git', 'am', '--abort'])
	subprocess.call(['git', 'add', '.'])
	subprocess.call(['git', 'commit', '--allow-empty', '-a', '-m', 'empty cleanup commit'])
	subprocess.call(['git', 'checkout', 'origin/master', '-b', 'tmp-master'])
	subprocess.call(['git', 'branch', '-D', 'master']) # &>/dev/null || true
	subprocess.call(['git', 'checkout', 'origin/master', '-b', 'master'])
	subprocess.call(['git', 'branch', '-D', 'tmp-master']) # &>/dev/null || true
	subprocess.call(['git', 'pull'])

