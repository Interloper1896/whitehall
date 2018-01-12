#include <SDL2/SDL.h>        
#include <SDL2/SDL_image.h>        
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

volatile int synchro;
char gbuffer[256];
char gServerIpAddress[256];
int gServerPort;
char gClientIpAddress[256];
int gClientPort;
char gName[256];
int gId, idCollegue;
struct _poi
{
	int x;
	int y;
} poi[533];
int indice; // mouvement souris
int position, posCollegue, posPoliceR, posPoliceJ, posPoliceV, posPotIndice, posIndice, posZonePolice, posIndiceOuZone, posZoneJack; // clic souris
int tour = 10; // personne ne peut jouer avant que le serveur le décide
int monTour = 0; // 1 si c'est mon tour
int jackEstPasse[533];
int zoneDecouvertePolice[5];
int zoneDecouverteJack[5];
int nbIndices = 0;
int nbZonesDecouvertePolice = 0;
int nbZonesDecouverteJack = 0;
int indiceOuZone;
int phaseInit = 0; // 0 : connection, 1 : initialisation, 2 : jeu
int initJack = 0, initPol1 = 0, initPol2 = 0, initPol3 = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void initPOI()
{
	poi[1].x=74; poi[1].y=55;
	poi[2].x=220; poi[2].y=42;
	poi[3].x=299; poi[3].y=48;
	poi[4].x=418; poi[4].y=46;
	poi[5].x=477; poi[5].y=46;
	poi[6].x=561; poi[6].y=47;
	poi[7].x=688; poi[7].y=68;
	poi[8].x=42; poi[8].y=98;
	poi[9].x=94; poi[9].y=70;
	poi[10].x=106; poi[10].y=106;
	poi[11].x=127; poi[11].y=78;
	poi[12].x=152; poi[12].y=77;
	poi[13].x=179; poi[13].y=122;
	poi[14].x=202; poi[14].y=77;
	poi[15].x=226; poi[15].y=127;
	poi[16].x=237; poi[16].y=91;
	poi[17].x=269; poi[17].y=91;
	poi[18].x=296; poi[18].y=97;
	poi[19].x=347; poi[19].y=109;
	poi[20].x=423; poi[20].y=72;
	poi[21].x=434; poi[21].y=121;
	poi[22].x=469; poi[22].y=84;
	poi[23].x=514; poi[23].y=74;
	poi[24].x=559; poi[24].y=117;
	poi[25].x=619; poi[25].y=79;
	poi[26].x=702; poi[26].y=130;
	poi[27].x=750; poi[27].y=88;
	poi[28].x=59; poi[28].y=157;
	poi[29].x=99; poi[29].y=138;
	poi[30].x=104; poi[30].y=179;
	poi[31].x=141; poi[31].y=185;
	poi[32].x=170; poi[32].y=158;
	poi[33].x=206; poi[33].y=166;
	poi[34].x=245; poi[34].y=170;
	poi[35].x=285; poi[35].y=170;
	poi[36].x=309; poi[36].y=195;
	poi[37].x=334; poi[37].y=147;
	poi[38].x=372; poi[38].y=136;
	poi[39].x=374; poi[39].y=184;
	poi[40].x=408; poi[40].y=173;
	poi[41].x=438; poi[41].y=156;
	poi[42].x=484; poi[42].y=152;
	poi[43].x=530; poi[43].y=196;
	poi[44].x=542; poi[44].y=153;
	poi[45].x=616; poi[45].y=136;
	poi[46].x=620; poi[46].y=186;
	poi[47].x=742; poi[47].y=158;
	poi[48].x=66; poi[48].y=203;
	poi[49].x=83; poi[49].y=226;
	poi[50].x=84; poi[50].y=255;
	poi[51].x=141; poi[51].y=243;
	poi[52].x=193; poi[52].y=266;
	poi[53].x=206; poi[53].y=237;
	poi[54].x=223; poi[54].y=213;
	poi[55].x=247; poi[55].y=262;
	poi[56].x=333; poi[56].y=213;
	poi[57].x=374; poi[57].y=245;
	poi[58].x=429; poi[58].y=260;
	poi[59].x=456; poi[59].y=213;
	poi[60].x=524; poi[60].y=251;
	poi[61].x=570; poi[61].y=254;
	poi[62].x=578; poi[62].y=214;
	poi[63].x=626; poi[63].y=248;
	poi[64].x=688; poi[64].y=252;
	poi[65].x=705; poi[65].y=218;
	poi[66].x=729; poi[66].y=265;
	poi[67].x=741; poi[67].y=224;
	poi[68].x=74; poi[68].y=300;
	poi[69].x=118; poi[69].y=316;
	poi[70].x=163; poi[70].y=330;
	poi[71].x=216; poi[71].y=308;
	poi[72].x=280; poi[72].y=278;
	poi[73].x=340; poi[73].y=276;
	poi[74].x=377; poi[74].y=304;
	poi[75].x=416; poi[75].y=316;
	poi[76].x=472; poi[76].y=289;
	poi[77].x=486; poi[77].y=320;
	poi[78].x=541; poi[78].y=316;
	poi[79].x=602; poi[79].y=310;
	poi[80].x=645; poi[80].y=313;
	poi[81].x=52; poi[81].y=354;
	poi[82].x=123; poi[82].y=364;
	poi[83].x=191; poi[83].y=380;
	poi[84].x=205; poi[84].y=343;
	poi[85].x=227; poi[85].y=384;
	poi[86].x=280; poi[86].y=384;
	poi[87].x=283; poi[87].y=349;
	poi[88].x=312; poi[88].y=364;
	poi[89].x=368; poi[89].y=386;
	poi[90].x=402; poi[90].y=355;
	poi[91].x=429; poi[91].y=396;
	poi[92].x=434; poi[92].y=359;
	poi[93].x=501; poi[93].y=370;
	poi[94].x=561; poi[94].y=362;
	poi[95].x=608; poi[95].y=352;
	poi[96].x=693; poi[96].y=398;
	poi[97].x=701; poi[97].y=335;
	poi[98].x=755; poi[98].y=366;
	poi[99].x=89; poi[99].y=410;
	poi[100].x=143; poi[100].y=441;
	poi[101].x=147; poi[101].y=405;
	poi[102].x=201; poi[102].y=460;
	poi[103].x=218; poi[103].y=420;
	poi[104].x=265; poi[104].y=463;
	poi[105].x=310; poi[105].y=420;
	poi[106].x=372; poi[106].y=436;
	poi[107].x=411; poi[107].y=445;
	poi[108].x=491; poi[108].y=435;
	poi[109].x=493; poi[109].y=407;
	poi[110].x=552; poi[110].y=464;
	poi[111].x=578; poi[111].y=402;
	poi[112].x=602; poi[112].y=453;
	poi[113].x=663; poi[113].y=428;
	poi[114].x=670; poi[114].y=467;
	poi[115].x=704; poi[115].y=457;
	poi[116].x=756; poi[116].y=436;
	poi[117].x=71; poi[117].y=516;
	poi[118].x=86; poi[118].y=471;
	poi[119].x=140; poi[119].y=505;
	poi[120].x=202; poi[120].y=530;
	poi[121].x=211; poi[121].y=501;
	poi[122].x=284; poi[122].y=510;
	poi[123].x=298; poi[123].y=477;
	poi[124].x=331; poi[124].y=505;
	poi[125].x=360; poi[125].y=530;
	poi[126].x=398; poi[126].y=522;
	poi[127].x=403; poi[127].y=477;
	poi[128].x=463; poi[128].y=477;
	poi[129].x=485; poi[129].y=505;
	poi[130].x=512; poi[130].y=480;
	poi[131].x=623; poi[131].y=507;
	poi[132].x=687; poi[132].y=497;
	poi[133].x=719; poi[133].y=490;
	poi[134].x=106; poi[134].y=566;
	poi[135].x=152; poi[135].y=587;
	poi[136].x=187; poi[136].y=600;
	poi[137].x=227; poi[137].y=568;
	poi[138].x=257; poi[138].y=579;
	poi[139].x=283; poi[139].y=550;
	poi[140].x=335; poi[140].y=559;
	poi[141].x=405; poi[141].y=577;
	poi[142].x=497; poi[142].y=567;
	poi[143].x=533; poi[143].y=538;
	poi[144].x=609; poi[144].y=584;
	poi[145].x=636; poi[145].y=562;
	poi[146].x=657; poi[146].y=593;
	poi[147].x=666; poi[147].y=546;
	poi[148].x=690; poi[148].y=577;
	poi[149].x=703; poi[149].y=535;
	poi[150].x=716; poi[150].y=563;
	poi[151].x=744; poi[151].y=549;
	poi[152].x=47; poi[152].y=653;
	poi[153].x=79; poi[153].y=612;
	poi[154].x=77; poi[154].y=660;
	poi[155].x=108; poi[155].y=618;
	poi[156].x=137; poi[156].y=622;
	poi[157].x=150; poi[157].y=646;
	poi[158].x=180; poi[158].y=638;
	poi[159].x=203; poi[159].y=666;
	poi[160].x=291; poi[160].y=620;
	poi[161].x=295; poi[161].y=662;
	poi[162].x=363; poi[162].y=605;
	poi[163].x=364; poi[163].y=662;
	poi[164].x=413; poi[164].y=639;
	poi[165].x=487; poi[165].y=644;
	poi[166].x=523; poi[166].y=609;
	poi[167].x=602; poi[167].y=658;
	poi[168].x=633; poi[168].y=627;
	poi[169].x=666; poi[169].y=649;
	poi[170].x=691; poi[170].y=635;
	poi[171].x=707; poi[171].y=664;
	poi[172].x=737; poi[172].y=608;
	poi[173].x=52; poi[173].y=734;
	poi[174].x=79; poi[174].y=698;
	poi[175].x=154; poi[175].y=710;
	poi[176].x=216; poi[176].y=725;
	poi[177].x=261; poi[177].y=691;
	poi[178].x=316; poi[178].y=689;
	poi[179].x=361; poi[179].y=721;
	poi[180].x=412; poi[180].y=696;
	poi[181].x=447; poi[181].y=684;
	poi[182].x=448; poi[182].y=727;
	poi[183].x=486; poi[183].y=706;
	poi[184].x=520; poi[184].y=683;
	poi[185].x=551; poi[185].y=728;
	poi[186].x=623; poi[186].y=695;
	poi[187].x=652; poi[187].y=701;
	poi[188].x=682; poi[188].y=724;
	poi[189].x=104; poi[189].y=745;
	poi[200].x=66; poi[200].y=83;
	poi[201].x=114; poi[201].y=56;
	poi[202].x=144; poi[202].y=53;
	poi[203].x=195; poi[203].y=59;
	poi[204].x=240; poi[204].y=31;
	poi[205].x=228; poi[205].y=67;
	poi[206].x=258; poi[206].y=74;
	poi[207].x=314; poi[207].y=82;
	poi[208].x=377; poi[208].y=63;
	poi[209].x=50; poi[209].y=131;
	poi[210].x=81; poi[210].y=108;
	poi[211].x=139; poi[211].y=102;
	poi[212].x=166; poi[212].y=106;
	poi[213].x=213; poi[213].y=104;
	poi[214].x=276; poi[214].y=111;
	poi[215].x=324; poi[215].y=109;
	poi[216].x=369; poi[216].y=117;
	poi[217].x=87; poi[217].y=192;
	poi[218].x=121; poi[218].y=161;
	poi[219].x=157; poi[219].y=134;
	poi[220].x=190; poi[220].y=147;
	poi[221].x=232; poi[221].y=147;
	poi[222].x=285; poi[222].y=148;
	poi[223].x=373; poi[223].y=162;
	poi[224].x=57; poi[224].y=227;
	poi[225].x=109; poi[225].y=227;
	poi[226].x=161; poi[226].y=209;
	poi[227].x=184; poi[227].y=184;
	poi[228].x=219; poi[228].y=191;
	poi[229].x=265; poi[229].y=184;
	poi[230].x=283; poi[230].y=209;
	poi[231].x=332; poi[231].y=184;
	poi[232].x=374; poi[232].y=208;
	poi[233].x=54; poi[233].y=255;
	poi[234].x=131; poi[234].y=270;
	poi[235].x=177; poi[235].y=241;
	poi[236].x=230; poi[236].y=232;
	poi[237].x=301; poi[237].y=277;
	poi[238].x=329; poi[238].y=250;
	poi[239].x=377; poi[239].y=283;
	poi[240].x=48; poi[240].y=313;
	poi[241].x=99; poi[241].y=285;
	poi[242].x=145; poi[242].y=298;
	poi[243].x=201; poi[243].y=289;
	poi[244].x=259; poi[244].y=289;
	poi[245].x=76; poi[245].y=337;
	poi[246].x=181; poi[246].y=360;
	poi[247].x=228; poi[247].y=331;
	poi[248].x=269; poi[248].y=318;
	poi[249].x=308; poi[249].y=336;
	poi[250].x=378; poi[250].y=323;
	poi[251].x=98; poi[251].y=378;
	poi[252].x=133; poi[252].y=384;
	poi[253].x=202; poi[253].y=398;
	poi[254].x=249; poi[254].y=371;
	poi[255].x=316; poi[255].y=391;
	poi[256].x=391; poi[256].y=368;
	poi[300].x=462; poi[300].y=33;
	poi[301].x=489; poi[301].y=62;
	poi[302].x=531; poi[302].y=83;
	poi[303].x=595; poi[303].y=64;
	poi[304].x=638; poi[304].y=44;
	poi[305].x=402; poi[305].y=90;
	poi[306].x=450; poi[306].y=102;
	poi[307].x=482; poi[307].y=112;
	poi[308].x=571; poi[308].y=102;
	poi[309].x=641; poi[309].y=93;
	poi[310].x=694; poi[310].y=100;
	poi[311].x=726; poi[311].y=120;
	poi[312].x=417; poi[312].y=137;
	poi[313].x=502; poi[313].y=138;
	poi[314].x=532; poi[314].y=138;
	poi[315].x=578; poi[315].y=148;
	poi[316].x=641; poi[316].y=168;
	poi[317].x=415; poi[317].y=193;
	poi[318].x=440; poi[318].y=187;
	poi[319].x=502; poi[319].y=181;
	poi[320].x=553; poi[320].y=170;
	poi[321].x=602; poi[321].y=196;
	poi[322].x=757; poi[322].y=198;
	poi[323].x=415; poi[323].y=285;
	poi[324].x=468; poi[324].y=239;
	poi[325].x=494; poi[325].y=272;
	poi[326].x=554; poi[326].y=232;
	poi[327].x=592; poi[327].y=277;
	poi[328].x=652; poi[328].y=245;
	poi[329].x=663; poi[329].y=266;
	poi[330].x=713; poi[330].y=239;
	poi[331].x=624; poi[331].y=286;
	poi[332].x=581; poi[332].y=332;
	poi[333].x=547; poi[333].y=384;
	poi[334].x=505; poi[334].y=349;
	poi[335].x=467; poi[335].y=339;
	poi[336].x=524; poi[336].y=290;
	poi[337].x=417; poi[337].y=342;
	poi[338].x=455; poi[338].y=378;
	poi[339].x=738; poi[339].y=397;
	poi[340].x=443; poi[340].y=313;
	poi[400].x=60; poi[400].y=433;
	poi[401].x=102; poi[401].y=433;
	poi[402].x=156; poi[402].y=428;
	poi[403].x=229; poi[403].y=440;
	poi[404].x=288; poi[404].y=446;
	poi[405].x=369; poi[405].y=410;
	poi[406].x=376; poi[406].y=459;
	poi[407].x=379; poi[407].y=483;
	poi[408].x=396; poi[408].y=546;
	poi[409].x=314; poi[409].y=717;
	poi[410].x=318; poi[410].y=659;
	poi[411].x=313; poi[411].y=601;
	poi[412].x=362; poi[412].y=556;
	poi[413].x=351; poi[413].y=504;
	poi[414].x=305; poi[414].y=506;
	poi[415].x=304; poi[415].y=537;
	poi[416].x=308; poi[416].y=560;
	poi[417].x=240; poi[417].y=483;
	poi[418].x=242; poi[418].y=508;
	poi[419].x=252; poi[419].y=559;
	poi[420].x=293; poi[420].y=596;
	poi[421].x=283; poi[421].y=644;
	poi[422].x=273; poi[422].y=713;
	poi[423].x=245; poi[423].y=662;
	poi[424].x=205; poi[424].y=579;
	poi[425].x=172; poi[425].y=481;
	poi[426].x=58; poi[426].y=489;
	poi[427].x=113; poi[427].y=454;
	poi[428].x=154; poi[428].y=541;
	poi[429].x=160; poi[429].y=558;
	poi[430].x=85; poi[430].y=542;
	poi[431].x=105; poi[431].y=591;
	poi[432].x=47; poi[432].y=629;
	poi[433].x=108; poi[433].y=643;
	poi[434].x=170; poi[434].y=614;
	poi[435].x=161; poi[435].y=671;
	poi[436].x=212; poi[436].y=703;
	poi[437].x=158; poi[437].y=733;
	poi[438].x=118; poi[438].y=677;
	poi[439].x=39; poi[439].y=677;
	poi[440].x=42; poi[440].y=718;
	poi[441].x=48; poi[441].y=754;
	poi[442].x=103; poi[442].y=725;
	poi[500].x=434; poi[500].y=417;
	poi[501].x=420; poi[501].y=465;
	poi[502].x=462; poi[502].y=450;
	poi[503].x=460; poi[503].y=504;
	poi[504].x=414; poi[504].y=612;
	poi[505].x=410; poi[505].y=668;
	poi[506].x=410; poi[506].y=724;
	poi[507].x=484; poi[507].y=688;
	poi[508].x=482; poi[508].y=724;
	poi[509].x=491; poi[509].y=600;
	poi[510].x=500; poi[510].y=533;
	poi[511].x=502; poi[511].y=512;
	poi[512].x=520; poi[512].y=449;
	poi[513].x=532; poi[513].y=420;
	poi[514].x=680; poi[514].y=446;
	poi[515].x=711; poi[515].y=418;
	poi[516].x=728; poi[516].y=470;
	poi[517].x=710; poi[517].y=512;
	poi[518].x=661; poi[518].y=486;
	poi[519].x=648; poi[519].y=513;
	poi[520].x=639; poi[520].y=539;
	poi[521].x=697; poi[521].y=558;
	poi[522].x=735; poi[522].y=571;
	poi[523].x=751; poi[523].y=528;
	poi[524].x=685; poi[524].y=598;
	poi[525].x=634; poi[525].y=589;
	poi[526].x=628; poi[526].y=662;
	poi[527].x=611; poi[527].y=728;
	poi[528].x=646; poi[528].y=727;
	poi[529].x=657; poi[529].y=671;
	poi[530].x=675; poi[530].y=625;
	poi[531].x=715; poi[531].y=643;
	poi[532].x=694; poi[532].y=687;
}

int findPOI(int mx,int my)
{
	int i;

	for (i=1; i<533; i++)
	{
		if ((mx>=(poi[i].x-8)) && (mx<=(poi[i].x+8)) && (my>=(poi[i].y-8)) && (my<=(poi[i].y+8)))
			return i;
	}
	return -1;
}

void sendMessageToServer(char *ipAddress, int portno, char *mess)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char sendbuffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(ipAddress);

    if (server == NULL) 
	{
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
        printf("ERROR connecting\n");
		exit(1);
	}

    sprintf(sendbuffer,"%s\n",mess);
    n = write(sockfd,sendbuffer,strlen(sendbuffer));

    close(sockfd);
}

void *server_function(void *arg)
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd<0)
	{
		printf("sockfd error\n");
		exit(1);
	}

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = gClientPort;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("bind error\n");
		exit(1);
	}
     
	listen(sockfd,5);
     	
	clilen = sizeof(cli_addr);

    while (1)
    {
    	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    	if (newsockfd < 0)
		{
			printf("accept error\n");
			exit(1);
		}
    	bzero(gbuffer,256);
    	n = read(newsockfd,gbuffer,255);

    	if (n < 0)
		{
			printf("read error\n");
			exit(1);
		}
    	printf("%s",gbuffer);
   		pthread_mutex_lock( &mutex );
		synchro=1;
   		pthread_mutex_unlock( &mutex );
		while (synchro);
    	close(newsockfd);
    }
}

int main(int argc, char ** argv)
{
    int quit = 0;
    SDL_Event event;
    pthread_t thread1;
	int mx,my;

	char com;
	int px,py;
	char mess[256];
 
	if (argc<6)
	{
		printf("<app> <Main server ip address> <Main server port> <Client ip address> <Client port> <player name>\n");
		exit(1);
	}

	strcpy(gServerIpAddress,argv[1]);
	gServerPort=atoi(argv[2]);
	strcpy(gClientIpAddress,argv[3]);
	gClientPort=atoi(argv[4]);
	strcpy(gName,argv[5]);


    SDL_Init(SDL_INIT_VIDEO);
 
    SDL_Window * window = SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800+128, 800, 0);
 
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface * image = IMG_Load("whitehall_800x800.png");
	SDL_Surface * connect_button = IMG_Load("connect_button.png");
    SDL_Surface * pawn = IMG_Load("pawn.png");
    SDL_SetColorKey( pawn, SDL_TRUE, SDL_MapRGB( pawn->format, 0, 0, 0 ) );
    SDL_Surface * rond = IMG_Load("rond.png");
	SDL_Surface * jack = IMG_Load("jack.png");
	SDL_Surface * policeR = IMG_Load("rouge.png");
	SDL_Surface * policeJ = IMG_Load("jaune.png");
	SDL_Surface * policeV = IMG_Load("vert.png");
    SDL_SetColorKey( rond, SDL_TRUE, SDL_MapRGB( rond->format, 0, 0, 0 ) );
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_Texture * texture_button = SDL_CreateTextureFromSurface(renderer, connect_button);
    SDL_Texture * texture_pawn = SDL_CreateTextureFromSurface(renderer, pawn);
    SDL_Texture * texture_rond = SDL_CreateTextureFromSurface(renderer, rond);
	SDL_Texture * texture_jack = SDL_CreateTextureFromSurface(renderer, jack);
	SDL_Texture * texture_policeR = SDL_CreateTextureFromSurface(renderer, policeR);
	SDL_Texture * texture_policeJ = SDL_CreateTextureFromSurface(renderer, policeJ);
	SDL_Texture * texture_policeV = SDL_CreateTextureFromSurface(renderer, policeV);
	SDL_SetTextureAlphaMod(texture_rond,128);
	SDL_SetTextureAlphaMod(texture_jack,128);
	SDL_SetTextureAlphaMod(texture_policeR,140);
	SDL_SetTextureAlphaMod(texture_policeJ,128);
	SDL_SetTextureAlphaMod(texture_policeV,128);
 
	initPOI();

    synchro=0;

    pthread_create( &thread1, NULL, server_function, NULL);

    while (!quit)
    {
		if (SDL_PollEvent(&event))
		{
        	switch (event.type)
        	{
            case SDL_QUIT:
            	quit = 1;
    			pthread_cancel(thread1);
            	break;
			case  SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState( &mx, &my );
				printf("mx=%d my=%d\n",mx,my);
				if ((mx>=800) && (my<=31))    // si c'est le bouton connection
				{
					sprintf(mess,"C %s %d %s", gClientIpAddress, gClientPort, gName);
					printf("mess vers server=%s\n",mess);
					sendMessageToServer(gServerIpAddress, gServerPort, mess);
				}
				else if (mx < 800)
				{
					if (phaseInit == 1)  // initialisation
					{
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							if ((gId == 1) && (initPol1 == 0))
							{
								position=findPOI(mx,my);
								printf("position=%d\n",position);
								if (position != -1)   //////  !!!!!!!!!!!! Ici donner les carrés jaunes valides
								{
									sprintf(mess,"I %d %d", gId, position);
									printf("mess vers server=%s\n", mess);
									sendMessageToServer(gServerIpAddress, gServerPort, mess);
								}
								initPol1++;
							}
							else if ((gId == 2) && (initPol2 == 0))
							{
								position=findPOI(mx,my);
								printf("position=%d\n",position);
								if (position != -1)   //////  !!!!!!!!!!!! Ici donner les carrés jaunes valides
								{
									sprintf(mess,"I %d %d", gId, position);
									printf("mess vers server=%s\n", mess);
									sendMessageToServer(gServerIpAddress, gServerPort, mess);
								}
								initPol2++;
							}
							else if ((gId == 3) && (initPol3 == 0))
							{
								position=findPOI(mx,my);
								printf("position=%d\n",position);
								if (position != -1)   //////  !!!!!!!!!!!! Ici donner les carrés jaunes valides
								{
									sprintf(mess,"I %d %d", gId, position);
									printf("mess vers server=%s\n", mess);
									sendMessageToServer(gServerIpAddress, gServerPort, mess);
								}
								initPol3++;
							}
							else if ((gId == 0) && (initJack <= 4))
							{
								if (initJack <= 3)
								{
									posZoneJack=findPOI(mx,my);
									printf("positionZoneJack=%d\n",posZoneJack);
									if (posZoneJack != -1)
									{
										nbZonesDecouverteJack++;
										zoneDecouverteJack[nbZonesDecouverteJack] = posZoneJack;
										sprintf(mess,"I %d %d", gId, posZoneJack);
										printf("mess vers server=%s\n", mess);
										sendMessageToServer(gServerIpAddress, gServerPort, mess);
									}
									initJack++;
								}
								else if (initJack == 4)
								{
									position=findPOI(mx,my);
									printf("position=%d\n",position);
									if (position != -1)
									{
										sprintf(mess,"I %d %d", gId, position);
										printf("mess vers server=%s\n", mess);
										sendMessageToServer(gServerIpAddress, gServerPort, mess);
									}
									initJack++;
								}
							}
						}
					}
					else if (phaseInit == 2)  // jeu
					{
						if (monTour)
						{
							if (event.button.button == SDL_BUTTON_LEFT)
							{
								position=findPOI(mx,my);
								printf("position=%d\n",position);
								if (position != -1)
								{
									sprintf(mess,"X %d %d", gId, position);
									printf("mess vers server=%s\n", mess);
									sendMessageToServer(gServerIpAddress, gServerPort, mess);
								}
							}
							else if (event.button.button == SDL_BUTTON_RIGHT)  // demande à une case si Jack est passé par là
							{
								posPotIndice=findPOI(mx,my);
								printf("Position potentielle d'un indice : %d\n", posPotIndice);
								if (posPotIndice != -1)
								{
									sprintf(mess,"A %d %d", gId, posPotIndice);
									printf("mess vers server=%s\n", mess);
									sendMessageToServer(gServerIpAddress, gServerPort, mess);
								}
							}
						}
					}
				}
				break;
			case  SDL_MOUSEMOTION:
				SDL_GetMouseState( &mx, &my );
				indice=findPOI(mx,my);
				break;
        	}
		}

		if (synchro==1)
		{
			puts("consomme");
   			pthread_mutex_lock( &mutex );

			switch (gbuffer[0])
			{
				case 'C':
					sscanf(gbuffer,"%c %d %d",&com, &px, &py);
					printf("COM=%c (%d,%d)\n",com,px,py);
					break;
				case 'I':
					sscanf(gbuffer,"%c %d",&com, &gId);
					printf("COM=%c id=%d\n",com,gId);
					sprintf(gbuffer, "%d %s", gId, gName);
					SDL_SetWindowTitle(window, gbuffer);
					break;
				case 'T':  // envoie d'un message d'information
					sscanf(gbuffer,"%c %s",&com, mess);
					printf("COM=%c mess=%s\n",com,mess);
					break;
				case 'P':  // position d'un autre joueur
					sscanf(gbuffer,"%c %d %d", &com, &posCollegue, &idCollegue);
					printf("COM=%c posCollegue=%d, idCollegue=%d\n", com, posCollegue, idCollegue);
					break;
				case 'O':  // a qui le tour
					sscanf(gbuffer,"%c %d", &com, &tour);
					printf("COM=%c tour=%d\n", com, tour);
					if (tour == gId)
						monTour = 1;
					else 
						monTour = 0; 
					break;
				case 'R':   // reponse si l'indice est juste
					sscanf(gbuffer,"%c %d %d", &com, &posIndiceOuZone, &indiceOuZone);
					printf("COM=%c posIndiceOuZone=%d indiceOuZone=%d\n",com,posIndiceOuZone,indiceOuZone);
					if (indiceOuZone == 0)
					{
						posIndice = posIndiceOuZone;      // la case demandée est un indice
						if (nbIndices < 532) nbIndices++;
						jackEstPasse[nbIndices] = posIndice;
					}
					else if (indiceOuZone == 1)
					{
						posZonePolice = posIndiceOuZone;    // la case demandée est une zone
						if (nbZonesDecouvertePolice < 4) nbZonesDecouvertePolice++;
						zoneDecouvertePolice[nbZonesDecouvertePolice] = posZonePolice;
					}
					break;
				case 'J':   // indication du statut des phases
					sscanf(gbuffer,"%c %d", &com, &phaseInit);
					printf("phase = %d\n", phaseInit);
					break;
				default:
					break;
			}
			synchro=0;
   			pthread_mutex_unlock( &mutex );
		}

    	SDL_Rect dstrect = { 0, 0, 800, 800 };
		SDL_Rect dstrect_button = { 800, 0, 128, 31 };
    	SDL_Rect dstrect_pawn = { 100, 100, 87, 131 };
    	SDL_Rect dstrect_rond = { 300, 300, 200, 200 };
		SDL_Rect dstrect_jack = { 300, 300, 200, 200 };
		SDL_Rect dstrect_policeR = { 300, 300, 200, 200 };
		SDL_Rect dstrect_policeJ = { 300, 300, 200, 200 };
		SDL_Rect dstrect_policeV = { 300, 300, 200, 200 };
    	SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		SDL_RenderCopy(renderer, texture_button, NULL, &dstrect_button);


		if (gId != 0)   // place les collegues policier si le joueur n'et pas Jack
		{
			if ((idCollegue == 1) && (idCollegue != gId))
				posPoliceR = posCollegue;
			else if ((idCollegue == 2) && (idCollegue != gId))
				posPoliceJ = posCollegue;
			else if ((idCollegue == 3) && (idCollegue != gId))
				posPoliceV = posCollegue;

			if (gId == 1)
			{
				SDL_Rect dstrect_policeJ = { poi[posPoliceJ].x-85, poi[posPoliceJ].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeJ, NULL, &dstrect_policeJ);
				SDL_Rect dstrect_policeV = { poi[posPoliceV].x-85, poi[posPoliceV].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeV, NULL, &dstrect_policeV);
			}
			else if (gId == 2)
			{
				SDL_Rect dstrect_policeR = { poi[posPoliceR].x-85, poi[posPoliceR].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeR, NULL, &dstrect_policeR);
				SDL_Rect dstrect_policeV = { poi[posPoliceV].x-85, poi[posPoliceV].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeV, NULL, &dstrect_policeV);
			}
			else if (gId == 3)
			{
				SDL_Rect dstrect_policeR = { poi[posPoliceR].x-85, poi[posPoliceR].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeR, NULL, &dstrect_policeR);
				SDL_Rect dstrect_policeJ = { poi[posPoliceJ].x-85, poi[posPoliceJ].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeJ, NULL, &dstrect_policeJ);
			}
		}

		if (indice != -1)   // propose les positions au passage de la souris
		{
			if (gId == 0)
			{
				SDL_Rect dstrect_jack = { poi[indice].x-16, poi[indice].y-16, 32, 32 };
    	    	SDL_RenderCopy(renderer, texture_jack, NULL, &dstrect_jack);		
			}
			else if (gId == 1)
			{
				SDL_Rect dstrect_policeR = { poi[indice].x-85, poi[indice].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeR, NULL, &dstrect_policeR);
			}
			else if (gId == 2)
			{
				SDL_Rect dstrect_policeJ = { poi[indice].x-85, poi[indice].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeJ, NULL, &dstrect_policeJ);
			}
			else if (gId == 3)
			{
				SDL_Rect dstrect_policeV = { poi[indice].x-85, poi[indice].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeV, NULL, &dstrect_policeV);
			}
		}

		if (position != -1)  // place le joueur au clic de souris
		{
			if (gId == 0)
			{
				SDL_Rect dstrect_jack = { poi[position].x-16, poi[position].y-16, 32, 32 };
    	    	SDL_RenderCopy(renderer, texture_jack, NULL, &dstrect_jack);		
			}
			else if (gId == 1)
			{
				SDL_Rect dstrect_policeR = { poi[position].x-85, poi[position].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeR, NULL, &dstrect_policeR);
			}
			else if (gId == 2)
			{
				SDL_Rect dstrect_policeJ = { poi[position].x-85, poi[position].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeJ, NULL, &dstrect_policeJ);
			}
			else if (gId == 3)
			{
				SDL_Rect dstrect_policeV = { poi[position].x-85, poi[position].y-25, 160, 42 };
    	    	SDL_RenderCopy(renderer, texture_policeV, NULL, &dstrect_policeV);
			}
		}


		// Affichage des ronds
		if (gId != 0)
		{
			for (int i = 1; i <= nbIndices; i++)   // affiche les indices
			{
				SDL_Rect dstrect_rond = { poi[jackEstPasse[i]].x-16, poi[jackEstPasse[i]].y-16, 32, 32 };
    		    SDL_RenderCopy(renderer, texture_rond, NULL, &dstrect_rond);
			}
			for (int i = 1; i <= nbZonesDecouvertePolice; i++)   // affiche les zones decouvertes par les policiers
			{
				SDL_Rect dstrect_rond = { poi[zoneDecouvertePolice[i]].x-16, poi[zoneDecouvertePolice[i]].y-16, 32, 32 };
    		    SDL_RenderCopy(renderer, texture_rond, NULL, &dstrect_rond);
			}
		}
		else if (gId == 0)
		{
			for (int i = 1; i <= nbZonesDecouverteJack; i++)   // affiche les zones de decouvertes choisies par Jack
			{
				SDL_Rect dstrect_rond = { poi[zoneDecouverteJack[i]].x-16, poi[zoneDecouverteJack[i]].y-16, 32, 32 };
    		    SDL_RenderCopy(renderer, texture_rond, NULL, &dstrect_rond);
			}
		}

    	SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
	SDL_DestroyTexture(texture_button);
	SDL_DestroyTexture(texture_pawn);
	SDL_DestroyTexture(texture_rond);
	SDL_DestroyTexture(texture_jack);
	SDL_DestroyTexture(texture_policeR);
	SDL_DestroyTexture(texture_policeJ);
	SDL_DestroyTexture(texture_policeV);
    SDL_FreeSurface(image);
	SDL_FreeSurface(connect_button);
	SDL_FreeSurface(pawn);
	SDL_FreeSurface(rond);
	SDL_FreeSurface(jack);
	SDL_FreeSurface(policeR);
	SDL_FreeSurface(policeJ);
	SDL_FreeSurface(policeV);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
 
    SDL_Quit();
 
    return 0;
}
