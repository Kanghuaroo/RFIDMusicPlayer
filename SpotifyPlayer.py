import sys
import spotipy
import spotipy.util as util
import serial
from MusicCard import MusicCard

scope = 'user-modify-playback-state'
ID='52f5fff3d7aa44d6a75522471ef094d7'
SECRET='15a676a209884a2bbaf28f1afc629e58'
URI='http://localhost/'

token = util.prompt_for_user_token(username='lyirk',scope=scope,client_id=ID,client_secret=SECRET, redirect_uri=URI)
sp = spotipy.Spotify(auth=token)

#MusicCard Hydration
songs = {}
with open('Cards.txt') as fileObj:
	for line in fileObj:
		crd = MusicCard(line, sp)
		songs[crd.getCode()] = crd
		print(crd.getID())

#Opening Serial Port
ser = serial.Serial('COM6')

asd = True
while asd:
	print('Waiting for Card')
	x = ser.readline().decode().strip()
	print(x)
	print(sp.current_playback())
	if songs.get(x) != None:
		songs.get(x).play()
	elif songs.get(x) == None:
		#fix init
		newCard= x+', '+sp.current_playback()+', '+sp.current_playback()
		crd = MusicCard(newCard, sp)
		songs[crd.getCode()] = crd
		crd.store()
	elif x==1:
		sp.next_track()
	elif x==2:	
		sp.pause_playback()
	elif x==3:
		sp.previous_track()
	
	#sp.next_track()
	asd = False
