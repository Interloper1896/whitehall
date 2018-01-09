import pygame
from pygame.locals import *
 
class App:
    def __init__(self):
        self._running = True
        self._display_surf = None
        self._image_surf = None
	self._font = None 
        self._mode = 1
        self._selected = -1 
	self._drag = 0 
	self._rx = -1
	self._ry = -1
	self._dx = -1
	self._dy = -1

	self.angle=30.0

	self.xoff = 0
	self.yoff = 0
	self.lPOI = [[-1,-1,0]]

	self._count = 0

    def on_init(self):
        pygame.init()
        self._display_surf = pygame.display.set_mode((850,850), pygame.HWSURFACE)
        self._running = True
        self._image_surf = pygame.image.load("whitehall_850x850.png").convert()
        #self._carte = pygame.image.load("ronde_combat1.png").convert_alpha()
	self._font = pygame.font.SysFont('Comic Sans MS', 30)
	self._rotpos = [500,500]
 
    def on_loadfile(self):
        fichier=open("POI","r")
        lignes=fichier.readlines()
        for ligne in lignes:
                ligne=ligne.strip("\n")
                if ligne[0]=='/':
                        print ligne
                else:
                        data=ligne.split(" ")
                        data[0]=int(data[0])
                        data[1]=int(data[1])
                        data[2]=int(data[2])
                        self.lPOI.append([data[0],data[1],data[2]])

    def find_poi(self):
	selected=-1
	for poi in self.lPOI:
		px, py, n= poi
		if (self._rx>(px-8)) and (self._rx<(px+8)) and (self._ry>(py-8)) and (self._ry<(py+8)):
			selected=n
	return selected

    def on_event(self, event):
        if event.type == QUIT:
            self._running = False

        elif event.type == MOUSEBUTTONDOWN and event.button==1:
                if self._mode==1:
			self._selected = -1 
                        mx,my = pygame.mouse.get_pos()
                        self._rx = mx+self.xoff
                        self._ry = my+self.yoff
			self._drag=1
			self._selected = self.find_poi()
			if self._selected!=-1:
				print "point selectionne"
				self._dx=self._rx-self.lPOI[self._selected][0]
				self._dy=self._ry-self.lPOI[self._selected][1]
			else:
				pass 

	elif event.type == MOUSEMOTION and self._drag==1:
                mx,my = pygame.mouse.get_pos()
                self._rx = mx+self.xoff
                self._ry = my+self.yoff
		if self._mode == 1:
			if self._drag==1:
				if self._selected == -1:
					pass
				else:
					self.lPOI[self._selected][0]=self._rx-self._dx	
					self.lPOI[self._selected][1]=self._ry-self._dy	

	elif event.type == MOUSEBUTTONUP and event.button==1 and self._drag==1:
		if self._mode == 1:
			if self._selected==-1:
                        	mx,my = pygame.mouse.get_pos()
                        	self._rx = mx+self.xoff
                        	self._ry = my+self.yoff
				self.lPOI.append([self._rx, self._ry, self._count])
				self._count+=1
			self._drag=0

        elif event.type == MOUSEBUTTONDOWN and event.button==3:
                if self._mode==2 and self._selected != None:
                        print self._selected
                        mx,my = pygame.mouse.get_pos()
                        self._rx = mx+self.xoff
                        self._ry = my+self.yoff

 	elif event.type == KEYDOWN:	
		if event.key == K_LEFT:
			#print "Left"
			if self.xoff>0:
				self.xoff-=512
		if event.key == K_RIGHT:
			#print "Right"
			if self.xoff<1024:
				self.xoff+=512
		if event.key == K_UP:
			#print "Up"
			if self.yoff>0:
				self.yoff-=512
		if event.key == K_DOWN:
			#print "Down"
			if self.yoff<1024:
				self.yoff+=512
                if event.key == K_a:
                        self._mode=1
                        self._selected = None
                        print "mode 1"
                if event.key == K_q:
                        self._mode=2
                        self._selected = None
                        print "mode 2"
		if event.key == K_x:
			self.on_loadfile()
		if event.key == K_o:
			self._count+=10
		if event.key == K_p:
			self._count+=1	
		if event.key == K_l:
			self._count-=10
		if event.key == K_m:
			self._count-=1	
		if event.key == K_r:
			self.lPOI = []
		if event.key == K_s:
			for poi in self.lPOI:
				code="\tpoic[%d].x=%d; poic[%d].y=%d;"%(poi[2],poi[0],poi[2],poi[1])
			for poi in self.lPOI:
				print poi[0], poi[1], poi[2]	
		if event.key == K_b:
			self.angle+=1.0
			print self.angle
		if event.key == K_n:
			self.angle-=1.0
			print self.angle
		if event.key == K_c:
			self.angle+=10.0
			print self.angle
		if event.key == K_v:
			self.angle-=10.0
			print self.angle

    def on_loop(self):
        pass

    def on_render(self):
        self._display_surf.blit(self._image_surf,(0,0),(self.xoff,self.yoff,1024,1024))

	#loc = self._carte.get_rect().center
	#rotatedSurf=pygame.transform.rotate(self._carte, self.angle);
	#rotatedRect=rotatedSurf.get_rect()
	#rx,ry=self._rotpos
	#rx-=self.xoff
	#ry-=self.yoff
        #self._display_surf.blit(rotatedSurf,(rx-rotatedRect[2]/2,ry-rotatedRect[3]/2))

	text = self._font.render(str(self._count),False,(255,255,255))
       	self._display_surf.blit(text,(10,10))
	for poi in self.lPOI:
		x, y, c = poi
		if (x>self.xoff) and (x<(self.xoff+1024)) and (y>self.yoff) and (y<(self.yoff+1024)):
			dx=x-self.xoff
			dy=y-self.yoff	
			pygame.draw.circle(self._display_surf, (255,0,0), (dx,dy),16,5)
			#pygame.draw.rect(self._display_surf, (255,0,0), (dx-20,dy-20,40,40),5)
			text = self._font.render(str(c),False,(255,0,0))
        		self._display_surf.blit(text,(dx,dy+20))
        if self._mode==2:
                if self._selected != None:
                        x, y, c = self.lPOI[self._selected-1]
                        #print x,y,c 

	if self._mode==1 and self._selected == -1 and self._drag==1:
		#pygame.draw.rect(self._display_surf, (255,0,0), (self._rx-self.xoff-20,self._ry-self.yoff-20,40,40),5)
		pygame.draw.circle(self._display_surf, (255,0,0), (self._rx,self._ry),16,5)

			
        pygame.display.flip()
 
    def on_cleanup(self):
        pygame.quit()
 
    def on_execute(self):
        if self.on_init() == False:
            self._running = False
 
        while( self._running ):
            for event in pygame.event.get():
                self.on_event(event)
            self.on_loop()
            self.on_render()
        self.on_cleanup()
 
if __name__ == "__main__" :
    theApp = App()
    theApp.on_execute()
