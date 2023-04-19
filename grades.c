#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define ASIZE 52
#define BSIZE 53
#define NHW 6
#define NLAB 11
#define NPRJ 4
#define NTST 2
#define NUMGRADES NHW+NLAB+NPRJ+NTST+1
#define NUM10GRADES NHW+NLAB
#define MAXSECT 100

char * gets(char *str);
void getGrades(int grds[NUMGRADES][MAXSECT],int n);
void getAvgs(int grds[NUMGRADES][MAXSECT],int n);
char * getUserLine();
void printGrades(int grds[NUMGRADES][MAXSECT],int n,char *names[]);

int aGrades[NUMGRADES][MAXSECT];
int bGrades[NUMGRADES][MAXSECT];
char retBufferArea[1024+4096];
char *retBuffer;

char *colNames[NUMGRADES] = {
	"HW00","HW01","HW02","HW03","HW04","HW05",
	"Lab01","Lab02","Lab03","Lab04","Lab05","Lab06","Lab07","Lab08","Lab09","Lab10","Lab11",
	"Prj01","Prj02","Prj03","Prj04",
	"Tst01","Tst02",
	"AVG" };
char *aNames[ASIZE] = {
"Abbey_Adam",
"Argenson_Maxime",
"Atagun_Melis",
"Atakur_Dogukan",
"Balta_Korkut",
"Battle_Clayton",
"Bingham_Christopher",
"Bivins_Joshua",
"Buckler_Samuel",
"Chen_Eric",
"Choe_Jacqueline",
"DiGrigoli_Luca",
"Dileepan_Varun",
"Dortkardes_Elif",
"Edmiston_Brittney",
"Enders_Charles",
"Eng_Emily",
"Eychner_Griffin",
"Fang_Andy",
"Fehrer_Michael",
"Holt_Hamilton",
"Hopkins_Colin",
"Jain_Aryan",
"James_Jimson",
"Kagan_Matthew",
"Lese_Michael",
"Li_Hans",
"Liu_Brian",
"Meserve_Brendan",
"Ng_Ronald",
"Ozden_Enis",
"Ozguven_Abdullah",
"Park_Jiyun",
"Perez_Bryan",
"Perrone_Katherine",
"Potthast_Christopher",
"Pouerie_Jonel",
"Ramaiah_Shiva",
"Reyes_Nicholas",
"Rizzuto_Danielle",
"Saha_Sharan",
"Selim_Albayrak",
"Solar_Nicholas",
"Stefman_Ryan",
"Thomas_Spencer",
"Vural_Mert Can",
"Walsh_Alexander",
"Yang_Jason",
"Yeung_Dylan",
"Young_Stella",
"Zheng_Kavey",
"Zheng_Michael"
};
char *bNames[BSIZE] ={
"Abreu_Jeffrey",
"Ampadu_Paul",
"Bhagat_Ram",
"Bitra_Yashwant",
"Chen_Xinyu",
"Cheng_Thomas",
"Cohen_Isaac",
"Demir_Emre",
"Eminoglu_Ozgur",
"Eppakayala_Saha",
"Gaur_Deepanshi",
"Gorgun_Ali",
"Hamdi_Adel",
"Hoffmann_Jacob",
"Huang_Wilson",
"Jenkins_Jewel",
"Joseph_Jerin",
"Julien_Najah",
"Kaman_Teoman",
"Kaur_Manjot",
"Kronsberg_Carter",
"Kukreti_Satvik",
"Li_Axin",
"Lin_Chao",
"Luna_Andy",
"Mahmud_Rafsan",
"Mattern_Nicholas",
"Mehta_Daksh",
"Mines_Spencer",
"Nelson_Erika",
"Rally_Luke",
"Rauschenbach_Wylie",
"Roberts_Anthony",
"Rudin-Aulenbach_Mare",
"Scotti_Madeline",
"Shaikh_Abdulaziz",
"Singh_Gagandeep",
"Smithers_Nolan",
"Strier_Aubrey",
"Suarez_Jan_Ruzle",
"Sudheer_Kannu_Fahad",
"Tasan_Emir",
"Taylor_Alexis",
"Taylor_John",
"Tici_Omer",
"Tosuner_Zeynep",
"Ucar_Taha",
"Veljkovic_Klara",
"Villar_Giancarlo",
"Ye_Gene",
"Yildirim_Selen",
"Zhang_Jiahui",
"Zhang_Joey"
};

FILE *txtFile;

int main(int argc, char ** argv) {
	char * secID;
	time_t t;

	long pagesize = sysconf(_SC_PAGESIZE);;
	setbuf(stdout,0);
	retBuffer=(char *)(((long)retBufferArea + pagesize-1) & ~(pagesize-1));
	if(mprotect(retBuffer, 1024, PROT_READ|PROT_EXEC|PROT_WRITE)) {
      printf("mprotect failed\n");
      return(1);
   }
   if (argc==1) srand((unsigned) time(&t));
   else {
   	unsigned int seed=atoi(argv[1]);
   	srand(seed);
	}
	getGrades(aGrades,ASIZE);
	getGrades(bGrades,BSIZE);
	getAvgs(aGrades,ASIZE);
	getAvgs(bGrades,BSIZE);

	printf("Choose section A or B: ");
	secID=getUserLine();
	printf("\n");

	if (secID[0]=='A') {
		printGrades(aGrades,ASIZE,aNames);
	} else if (secID[0]=='B') {
		printGrades(bGrades,BSIZE,bNames);
	} else {
		printf("Unable to determine which section you wanted.\n");
	}
	exit(0);
}

void getGrades(int grds[NUMGRADES][MAXSECT],int n) {
	int s,c;
	for(s=0; s<n; s++) {
		for(c=0;c<NHW;c++) 	grds[c][s] = 5+rand()%2;
		for(c=NHW; c<(NHW+NLAB); c++) grds[c][s] = 5+rand()%3;
		for(c=NHW+NLAB; c<(NHW+NLAB+NPRJ); c++) grds[c][s] = 53+rand()%7;
		for(c=(NHW+NLAB+NPRJ);c<(NHW+NLAB+NPRJ+NTST);c++) grds[c][s] = 54+rand()%8;
	}
}

void getAvgs(int grds[NUMGRADES][MAXSECT],int n) {
	int s,c;
	int sumHW,sumLab,sumProj,sumTest;
	int minProj;

	for(s=0; s<n; s++) {
		sumHW=sumLab=sumProj=sumTest=0;
		minProj=101;
		for(c=0;c<NHW;c++) 	sumHW+=grds[c][s];
		for(c=NHW; c<(NHW+NLAB); c++) sumLab+=grds[c][s];
		for(c=NHW+NLAB; c<(NHW+NLAB+NPRJ); c++) {
			sumProj+=grds[c][s];
			if (grds[c][s]<minProj) minProj=grds[c][s];
		}
		sumProj-=minProj; // Remove the lowest project grade
		for(c=(NHW+NLAB+NPRJ);c<(NHW+NLAB+NPRJ+NTST);c++)
			sumTest+=grds[c][s];
		float avg = 0.15 * (10.0 * (float)sumHW/NHW) +
		                 0.15 * (10.0 * (float)sumLab/NLAB) +
		                 0.20 * ((float)sumProj/NPRJ) +
		                 0.20 * ((float)sumTest/NTST);
		 // So far, this is the average without participation and final exam
		 avg=(100.0/70.0)*avg; // What is the average without participation and final
		grds[NUMGRADES-1][s] = avg; // Truncate the decimal
	}
}



char * getUserLine() {
	char buffer[256];

	if (gets(buffer)) {
		memcpy(retBuffer,buffer,sizeof(buffer));
		return retBuffer;
	}
	return NULL;
}

void printGrades(int grds[NUMGRADES][MAXSECT],int n,char * names[]) {
	int s;
	printf(" %20s |","Student");
	for(int i=0;i<NUMGRADES-1;i++) printf("%5s|",colNames[i]);
	printf("| %3s |\n","AVG");
	printf("----------------------|");
	for(int i=0;i<NUMGRADES-1;i++) printf("-----|");
	printf("|-----|\n");
	for(s=0; s<n; s++) {
		printf(" %20s |",names[s]);
		for(int i=0;i<NUMGRADES-1;i++) printf(" %3d |",grds[i][s]);
		printf("| %3d |\n",grds[NUMGRADES-1][s]);
	}
	printf("----------------------|");
	for(int i=0;i<NUMGRADES-1;i++) printf("-----|");
	printf("|-----|\n");
}
