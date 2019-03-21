import sys
import spotipy
import spotipy.util as util

class MusicCard():
	#rfid cards and their songs
	
	def __init__(self, str, player, splitter=', '):
		#init of rfidCode, songTitle, spotifyID
		
		input = str.strip().split(splitter)
		try:
			self.rfidCode = input[0]
			self.songTitle = input[1]
			self.spotifyID = input[2]
			self.sp = player
		except IndexError:
			print('Incorrect String Given')
			
					
	def getCode(self):
		return self.rfidCode
		
	def getTitle(self):
		return self.title
		
	def getID(self):
		return self.spotifyID
		
	def store(self,fileName):
		with open(fileName, 'a') as writer:
			writer.write(self.rfidCode + ', ' + 
				self.title + ', ' + 
				self.spotifyID + '\n')
			
	def play(self):
		if self.spotifyID.find('track') != -1:
			self.sp.start_playback(uris = [self.spotifyID])
		elif self.spotifyID.find('playlist') != -1:
			self.sp.shuffle(state=True)
			self.sp.start_playback(context_uri = self.spotifyID)
		elif self.spotifyID.find('artist') != -1:
			self.sp.shuffle(state=True)
			self.sp.start_playback(context_uri = self.spotifyID)
