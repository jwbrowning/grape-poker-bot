#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

#define TRIALS 100

typedef struct HandInformation {

	int heroHand[2][2];
	int position,count,smallBlind,bigBlind,stage,heroStack,villainStack,potSize,heroBet,villainBet,actions,prevHeroBet,prevVillainBet;
	bool villainHasRaised;
	int heroRange[1326][2][2],villainRange[1326][2][2];
	float heroRangeEquities[1326],villainRangeEquities[1326],heroRangeWinRate,villainRangeWinRate,originalWinRate;
	int board[5][2],deck[52][2];
	string myLastAction;

	void resetHand() {

		smallBlind = 50;
		bigBlind = 100;
		heroStack = 20000;
		villainStack = 20000;
		potSize = 0;
		heroBet = 0;
		villainBet = 0;
		heroRangeWinRate = .5;
		villainRangeWinRate = .5;
		prevHeroBet = 0;
		prevVillainBet = 0;
		stage = 0;
		actions = 0;
		villainHasRaised = false;
		heroHand[0][0]=0;
		heroHand[0][1]=0;
		heroHand[1][0]=0;
		heroHand[1][1]=0;
		for(int i=0;i<1326;i++) {
			heroRangeEquities[i]=0;
			villainRangeEquities[i]=0;
		}
		for(int i=0;i<5;i++) {
			board[i][0]=0;
			board[i][1]=0;
		}
		myLastAction="";

		int c = 0;
		for(int i=1;i<5;i++) {
			for(int j=2;j<15;j++) {
				deck[c][0] = j;
				deck[c][1] = i;
				c++;
			}
		}

		c = 0;
		for(int i=0;i<52;i++) {
			for(int j=i;j<52;j++) {
				if(i!=j) {
					for(int k=0;k<2;k++) {
						heroRange[c][k/2][k%2] = deck[i][k%2];
						villainRange[c][k/2][k%2] = deck[i][k%2];
					}
					for(int k=2;k<4;k++) {
						heroRange[c][k/2][k%2] = deck[j][k%2];
						villainRange[c][k/2][k%2] = deck[j][k%2];
					}
					// heroRange[c][0][0] = deck[i][0];
					// heroRange[c][0][1] = deck[i][1];
					// heroRange[c][1][0] = deck[j][0];
					// heroRange[c][1][1] = deck[j][1];
					// villainRange[c][0][0] = deck[i][0];
					// villainRange[c][0][1] = deck[i][1];
					// villainRange[c][1][0] = deck[j][0];
					// villainRange[c][1][1] = deck[j][1];
					c++;
				}
			}
		}
		// cout<<c<<endl;
		// for(int i=0;i<1326;i++) {
		// 	cout<<"|"<<heroRange[i][0][0]<<"."<<heroRange[i][0][1]<<"|"<<heroRange[i][1][0]<<"."<<heroRange[i][1][1]<<"|\n";
		// }
	}

	HandInformation() {
		resetHand();
	}

} HandInfo;

void waitForTurn(int p, float interval);
void readInfo(HandInfo &handInfo);
int sizeOfRange(int range[1326][2][2]);
bool handIsInRange(int hand[2][2], int range[1326][2][2]);
bool favorableForHero(HandInfo &handInfo, float originalWinRate);
void sortRangeByEquity(HandInfo &handInfo, int range[1326][2][2], float rangeEquities[1326]);
void setRangeEquities(HandInfo &handInfo, int n, int range1[1326][2][2], int range2[1326][2][2], float range1Equities[1326]);
float min(float a, float b);
float max(float a, float b);
void action(HandInfo &handInfo);
void fold(HandInfo &handInfo);
void check(HandInfo &handInfo);
void call(HandInfo &handInfo);
void raise(HandInfo &handInfo, int total);
void villainCheck(HandInfo &handInfo);
void villainCall(HandInfo &handInfo);
void villainRaise(HandInfo &handInfo, int total);
void villainBet(HandInfo &handInfo, int total);
double strengthOfBestHand(int cards[7][2], int data[5][2], int n, int r, int s, int d);
double strengthOfHand(int cards[5][2]);
float winRateOfRangeVersusRange(HandInfo &handInfo, int n, int range1[1326][2][2], int range2[1326][2][2]);
float winRateOfHandVersusRange(HandInfo &handInfo, int n, int hand[2][2], int range[1326][2][2]);
int* balanceRangeBetCheck(HandInfo &handInfo, int range1[1326][2][2], int range2[1326][2][2], int n, float *betSize);
int* balanceRangeRaiseCallFold(HandInfo &handInfo, int range1[1326][2][2], int range2[1326][2][2], int n, float *betSize, int callSize);
void updateRangeBetCheck(int range[1326][2][2], int balance[2], char action);
void updateRangeRaiseCallFold(int range[1326][2][2], int balance[3], char action);
void updateVillainRangeBet(HandInfo &handInfo, int n, float betSize);
void updateVillainRangeRaise(HandInfo &handInfo, int n, float betSize);
float mostProfitableBetSize(HandInfo &handInfo, int *numValueBets, int *numChecks, int *numBluffs);
float mostProfitableRaiseSize(HandInfo &handInfo, int *numValueRaises, int *numCalls, int *numBluffs);

void printRange(int range[1326][2][2]) {
	for(int i=0;i<1326;i++) {
		if(range[i][0][0]!=0) {
			cout<<range[i][0][0]<<"."<<range[i][0][1]<<"|"<<range[i][1][0]<<"."<<range[i][1][1];
		} else {
			cout<<"------";
		}
		cout<<endl;
	}
}

int main() {

	cout<<"=============="<<endl;
	cout<<"  GRAPE v0.6  "<<endl;
	cout<<"=============="<<endl;

	cout<<"Which player am I?"<<endl;
	int p;
	cin>>p;

	srand(time(0));

	HandInfo handInfo = HandInformation();

	while(true) {

		waitForTurn(p,.1);
		cout<<"My turn.\n";
		readInfo(handInfo);
		cout<<"Read info.\n";
		action(handInfo);
		cout<<"Done.\n";

	}
}

void waitForTurn(int p,float t) {
	if(p==1) {
		bool exists = false;
		while(!exists) {
			ifstream ctrl("ctrl1");
			if(ctrl.good()) {
				exists=true;
				remove("ctrl1");
			}
			time_t start = time(0);
			time_t end = time(0);

			while(difftime(end,start)<t) {
				end = time(0);
			}
		}
	} else {
		bool exists = false;
		while(!exists) {
			ifstream ctrl("ctrl2");
			if(ctrl.good()) {
				exists=true;
				remove("ctrl2");
			}
			time_t start = time(0);
			time_t end = time(0);

			while(difftime(end,start)<t) {
				end = time(0);
			}
		}
	}
}

void readInfo(HandInfo &hand) {
	
	ifstream info("info.txt");

	int count = hand.count;
	info>>hand.count;
	cout<<"count: "<<hand.count<<endl;

	string prevAction;
	info>>prevAction;
	cout<<"prevaction: "<<prevAction<<endl;
	if(count!=hand.count) {
		cout<<"resetting hand...\n";
		hand.resetHand();
		if(prevAction=="none") {
			hand.position = 0;
			hand.heroBet = 50;
			hand.villainBet = 100;
			hand.prevHeroBet = 50;
			hand.prevVillainBet = 50;
		} else {
			hand.position = 1;
			hand.heroBet = 100;
			hand.villainBet = 50;
			hand.prevHeroBet = 50;
			hand.prevVillainBet = 50;
		}
	}
	
	info>>hand.heroHand[0][0]>>hand.heroHand[0][1]>>hand.heroHand[1][0]>>hand.heroHand[1][1];
	cout<<"herohand: "<<hand.heroHand[0][0]<<"."<<hand.heroHand[0][1]<<"|"<<hand.heroHand[1][0]<<hand.heroHand[1][1]<<endl;
	int myStack,opStack,pot,myBet,opBet;
	info>>myStack>>opStack;
	cout<<"stacks: "<<myStack<<" "<<opStack<<endl;
	info>>pot>>myBet>>opBet;
	cout<<"pot: "<<pot<<" "<<myBet<<" "<<opBet<<endl;

	int stage=0;
	if(!info.eof()) {
		for(int i=0;i<6;i++) {
			info>>hand.board[i/2][i%2];
		}
		stage=1;
	}
	if(!info.eof()) {
		info>>hand.board[3][0]>>hand.board[3][1];
		stage=2;
	}
	if(!info.eof()) {
		info>>hand.board[4][0]>>hand.board[3][1];
		stage=3;
	}

	if(hand.position==0 && stage!=hand.stage && count==hand.count) {
		if(hand.myLastAction=="call" && stage==1) {
			villainCheck(hand);
		} else if(hand.myLastAction=="raise") {
			villainCall(hand);
		}
	}

	if(stage!=hand.stage) {
		cout<<"reset previous bets\n";
		hand.originalWinRate = hand.heroRangeWinRate;
		hand.prevHeroBet = 0;
		hand.prevVillainBet = 0;
		hand.actions = 0;
		hand.villainHasRaised = false;
	}
	hand.stage = stage;

	setRangeEquities(hand, TRIALS, hand.heroRange, hand.villainRange, hand.heroRangeEquities);
	setRangeEquities(hand, TRIALS, hand.villainRange, hand.heroRange, hand.villainRangeEquities);
	sortRangeByEquity(hand, hand.heroRange, hand.heroRangeEquities);
	sortRangeByEquity(hand, hand.villainRange, hand.villainRangeEquities);
	hand.heroRangeWinRate = winRateOfRangeVersusRange(hand, 10*TRIALS, hand.heroRange, hand.villainRange);

	hand.heroStack = myStack;
	hand.villainStack = opStack;
	hand.potSize = pot;

	if(prevAction=="check") {
		hand.prevVillainBet = 0;
		hand.heroBet = myBet;
		hand.villainBet = opBet;
		villainCheck(hand);
	} else if(prevAction=="call") {
		hand.prevVillainBet = hand.villainBet;
		hand.heroBet = myBet;
		hand.villainBet = opBet;
		villainCall(hand);
	} else if(prevAction=="raise") {
		hand.prevVillainBet = hand.villainBet;
		hand.heroBet = myBet;
		hand.villainBet = opBet;
		if(hand.heroBet==0) {
			villainBet(hand,hand.villainBet);
		} else {
			hand.villainHasRaised = true;
			villainRaise(hand,hand.villainBet);
		}
	} else {
		hand.heroBet = myBet;
		hand.villainBet = opBet;
	}
	info.close();
}

int sizeOfRange(int range[1326][2][2]) {
	int size = 0;
	for(int i=0;i<1326;i++) {
		if(range[i][0][0]!=0) {
			size++;
		}
	}
	return size;
}

bool handIsInRange(int hand[2][2], int range[1326][2][2]) {
	for(int i=0;i<1326;i++) {
		if((hand[0][0]==range[i][0][0] && hand[0][1]==range[i][0][1]) && (hand[1][0]==range[i][1][0] && hand[1][1]==range[i][1][1])) {
			return true;
		} else if((hand[1][0]==range[i][0][0] && hand[1][1]==range[i][0][1]) && (hand[0][0]==range[i][1][0] && hand[0][1]==range[i][1][1])) {
			return true;
		}
	}
	return false;
}

bool handsAreEqual(int hand1[2][2], int hand2[2][2]) {
	if((hand1[0][0]==hand2[0][0] && hand1[0][1]==hand2[0][1]) && (hand1[1][0]==hand2[1][0] && hand1[1][1]==hand2[1][1])) {
		return true;
	} else if((hand1[1][0]==hand2[0][0] && hand1[1][1]==hand2[0][1]) && (hand1[0][0]==hand2[1][0] && hand1[0][1]==hand2[1][1])) {
		return true;
	}
	return false;
}

bool favorableForHero(HandInfo &handInfo, float originalWinRate) {
	return originalWinRate >= handInfo.heroRangeWinRate;
}

void sortRangeByEquity(HandInfo &handInfo, int range[1326][2][2], float rangeEquities[1326]) {
	float key1;
	int key2[2][2],j;
	for(int i=1;i<1326;i++) {
		key1=rangeEquities[i];
		for(int k=0;k<4;k++) {
			key2[k/2][k%2]=range[i][k/2][k%2];
		}
		j=i-1;
		while(j>=0 && rangeEquities[j]<key1) {
			rangeEquities[j+1] = rangeEquities[j];
			for(int k=0;k<4;k++) {
				range[j+1][k/2][k%2] = range[j][k/2][k%2];
			}
			j--;
		}
		rangeEquities[j+1] = key1;
		for(int k=0;k<4;k++) {
			range[j+1][k/2][k%2] = key2[k/2][k%2];
		}
	}
}

void setRangeEquities(HandInfo &handInfo, int n, int range1[1326][2][2], int range2[1326][2][2], float range1Equities[1326]) {
	for(int i=0;i<1326;i++) {
		float equity;
		if(range1[i][0][0]==0) {
			equity = 0;
		} else {
			equity = winRateOfHandVersusRange(handInfo,n,range1[i],range2);
		}
		range1Equities[i] = equity;
	}
}

float max(float a,float b) {
	if(a>b) {
		return a;
	} else {
		return b;
	}
}

float min(float a,float b) {
	if(a<b) {
		return a;
	} else {
		return b;
	}
}

void action(HandInfo &handInfo) {

	setRangeEquities(handInfo, TRIALS, handInfo.heroRange, handInfo.villainRange, handInfo.heroRangeEquities);
	setRangeEquities(handInfo, TRIALS, handInfo.villainRange, handInfo.heroRange, handInfo.villainRangeEquities);
	sortRangeByEquity(handInfo, handInfo.heroRange, handInfo.heroRangeEquities);
	sortRangeByEquity(handInfo, handInfo.villainRange, handInfo.villainRangeEquities);

	handInfo.heroRangeWinRate = winRateOfRangeVersusRange(handInfo, 10*TRIALS, handInfo.heroRange, handInfo.villainRange);
	
	if(handInfo.villainBet - handInfo.heroBet == 0) {
		if(favorableForHero(handInfo,handInfo.originalWinRate) || (handInfo.stage==3 && handInfo.position==0)) {
			float betSize = 0;
			int *balance = balanceRangeBetCheck(handInfo, handInfo.heroRange, handInfo.villainRange, TRIALS, &betSize);
			bool located = false;
			char heroAction = 'c';
			int totalPot = handInfo.potSize + handInfo.heroBet + handInfo.villainBet;

			int handIndex = -1;
			int heroRangeSize = sizeOfRange(handInfo.heroRange);
			for(int i=0;i<balance[0] && !located;i++) {
				if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
					heroAction = 'b';
					located = true;
					handIndex = i;
				}
			}
			for(int i=balance[0];i<balance[1] && !located;i++) {
				if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
					heroAction = 'c';
					located = true;
					handIndex = i;
				}
			}
			for(int i=balance[1];i<heroRangeSize && !located;i++) {
				if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
					heroAction = 'b';
					located = true;
					handIndex = i;
				}
			}
			cout<<"Found hand at index "<<handIndex<<", ";
			cout<<handIsInRange(handInfo.heroHand,handInfo.heroRange)<<endl;
			cout<<"Hero: "<<handInfo.heroHand[0][0]<<"."<<handInfo.heroHand[0][1]<<"|"<<handInfo.heroHand[1][0]<<"."<<handInfo.heroHand[1][1]<<endl;
			cout<<"Range: "<<endl;
			if(!handIsInRange(handInfo.heroHand,handInfo.heroRange)) {
				printRange(handInfo.heroRange);
			}
			cout<<endl;
			cout<<betSize<<" * "<<totalPot<<" = "<<betSize*totalPot<<endl;
			if(heroAction=='b') {
				raise(handInfo, betSize*totalPot);
			} else if(heroAction=='c') {
				check(handInfo);
			}
			updateRangeBetCheck(handInfo.heroRange,balance,heroAction);
		} else {
			check(handInfo);
		}
	} else {
		cout<<"EHY "<<sizeOfRange(handInfo.heroRange)<<" "<<sizeOfRange(handInfo.villainRange)<<endl;
		float betSize = 0;
		int *balance = balanceRangeRaiseCallFold(handInfo, handInfo.heroRange, handInfo.villainRange, TRIALS, &betSize, handInfo.villainBet-handInfo.heroBet);
		bool located = false;
		char heroAction = 'f';
		int totalPot = handInfo.potSize + handInfo.heroBet + handInfo.villainBet;

		int handIndex = -1;
		int heroRangeSize = sizeOfRange(handInfo.heroRange);
		float mdf = 1 - ((handInfo.villainBet-handInfo.prevVillainBet)/totalPot);
		for(int i=0;i<balance[0] && !located;i++) {
			if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
				heroAction = 'r';
				located = true;
				handIndex = i;
			}
		}
		for(int i=balance[0];i<balance[1] && !located;i++) {
			if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
				heroAction = 'c';
				located = true;
				handIndex = i;
			}
		}
		for(int i=balance[1];i<balance[2] && !located;i++) {
			if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
				heroAction = 'r';
				located = true;
				handIndex = i;
			}
		}
		for(int i=balance[2];i<heroRangeSize && !located;i++) {
			if(handsAreEqual(handInfo.heroHand, handInfo.heroRange[i])) {
				heroAction = 'f';
				located = true;
				handIndex = i;
			}
		}
		cout<<"Found hand at index "<<handIndex<<", ";
		cout<<handIsInRange(handInfo.heroHand,handInfo.heroRange)<<endl;
		cout<<"Hero: "<<handInfo.heroHand[0][0]<<"."<<handInfo.heroHand[0][1]<<"|"<<handInfo.heroHand[1][0]<<"."<<handInfo.heroHand[1][1]<<endl;
		cout<<"Range: "<<endl;
		if(!handIsInRange(handInfo.heroHand,handInfo.heroRange)) {
			printRange(handInfo.heroRange);
		}
		cout<<endl;
		cout<<handInfo.heroHand[0][0]<<"."<<handInfo.heroHand[0][1]<<"|"<<handInfo.heroHand[1][0]<<"."<<handInfo.heroHand[1][1]<<endl;
		cout<<betSize<<" * "<<totalPot<<" = "<<betSize*totalPot<<" total of "<<betSize*totalPot+handInfo.villainBet<<endl;
		if(heroAction=='r') {
			raise(handInfo, betSize*totalPot + handInfo.villainBet);
			updateRangeRaiseCallFold(handInfo.heroRange,balance,heroAction);
		} else if(heroAction=='c') {
			call(handInfo);
			updateRangeRaiseCallFold(handInfo.heroRange,balance,heroAction);
		} else if(heroAction=='f') {
			fold(handInfo);
		}
	}
}

void fold(HandInfo &handInfo) {
	cout<<"Fold.\n";
	ofstream info;
	info.open("info.txt");
	info<<"fold";
	info.close();

	ofstream ctrl;
	ctrl.open("ctrl");
	ctrl.close();
}

void check(HandInfo &handInfo) {
	handInfo.prevHeroBet = 0;
	cout<<"Check.\n";
	ofstream info;
	info.open("info.txt");
	info<<"check";
	info.close();
	handInfo.actions++;

	ofstream ctrl;
	ctrl.open("ctrl");
	ctrl.close();
}

void call(HandInfo &handInfo) {
	handInfo.prevHeroBet = handInfo.heroBet;
	cout<<"Call.\n";
	ofstream info;
	info.open("info.txt");
	info<<"call";
	info.close();

	ofstream ctrl;
	ctrl.open("ctrl");
	ctrl.close();
}

void raise(HandInfo &handInfo, int total) {
	if(total==handInfo.villainBet) {
		if(handInfo.villainBet==handInfo.heroBet) {
			check(handInfo);
		} else {
			call(handInfo);
		}
		return;
	}
	handInfo.prevHeroBet = handInfo.heroBet;
	cout<<"Raise to "<<total<<endl;
	ofstream info;
	info.open("info.txt");
	info<<"raise "<<total;
	info.close();
	handInfo.actions++;

	ofstream ctrl;
	ctrl.open("ctrl");
	ctrl.close();
}

void villainCheck(HandInfo &handInfo) {
	cout<<"Villain checked\n";
	handInfo.actions++;
}

void villainCall(HandInfo &handInfo) {
	cout<<"Villain called\n";
	float mdf = 1.0 - (1.0*handInfo.heroBet-handInfo.prevVillainBet)/(1.0*handInfo.potSize+handInfo.heroBet+handInfo.prevVillainBet);

	int villainRangeSize = sizeOfRange(handInfo.villainRange);
	// float equityNeeded = (1.0*handInfo.heroBet-handInfo.prevVillainBet) / (1.0*(handInfo.potSize+handInfo.heroBet+handInfo.villainBet) + (1.0*handInfo.heroBet-handInfo.prevVillainBet));
	int lastCallIndex = 0;
	// for(int i=0;i<villainRangeSize;i++) {
	// 	if(handInfo.villainRangeEquities[i] >= equityNeeded) {
	// 		lastCallIndex = i;
	// 	} else {
	// 		i = villainRangeSize;
	// 	}
	// }
	int maxIndex = max(ceil(mdf*villainRangeSize), lastCallIndex);

	cout<<maxIndex*1.0 / (1.0*villainRangeSize)<<endl;
	int balance[3];
	balance[0] = 0;
	balance[1] = maxIndex;
	balance[2] = balance[1];
	updateRangeRaiseCallFold(handInfo.villainRange,balance,'c');
}

void villainBet(HandInfo &handInfo, int total) {
	cout<<"Villain bet ";
	cout<<total<<" "<<(total*1.0)/handInfo.potSize<<"previously "<<handInfo.prevVillainBet<<endl;
	updateVillainRangeBet(handInfo,TRIALS,(total*1.0)/handInfo.potSize);
	cout<<"actions pre: "<<handInfo.actions;
	handInfo.actions++;
	cout<<" actions post: "<<handInfo.actions<<endl;
}

void villainRaise(HandInfo &handInfo, int total) {
	cout<<"Villain raised\n";
	updateVillainRangeRaise(handInfo,TRIALS,(total*1.0-handInfo.heroBet)/(handInfo.potSize+handInfo.heroBet+handInfo.prevVillainBet));
	cout<<"actions pre: "<<handInfo.actions;
	handInfo.actions++;
	cout<<" actions post: "<<handInfo.actions<<endl;
}

float winRateOfRangeVersusRange(HandInfo &handInfo, int n, int range1[1326][2][2], int range2[1326][2][2]) {
	int wins=0,ties=0;

	int deck[52][2];
	for(int i=0;i<52;i++) {
		deck[i][0]=handInfo.deck[i][0];
		deck[i][1]=handInfo.deck[i][1];
	}

	int range1Size = sizeOfRange(range1);
	int range2Size = sizeOfRange(range2);
	for(int i=0;i<n;i++) {

		int rand1,rand2;
		bool invalid;
		do {
			invalid=false;
			rand1=rand()%range1Size;
			rand2=rand()%range2Size;
			if(range1[rand1][0][0]==0 || range2[rand2][0][0]==0) {
				invalid=true;
				// cout<<"0";
			} else if(range1[rand1][0][0]==range2[rand2][0][0] && range1[rand1][0][1]==range2[rand2][0][1]) {
				invalid=true;
			} else if(range1[rand1][0][0]==range2[rand2][1][0] && range1[rand1][0][1]==range2[rand2][1][1]) {
				invalid=true;
			} else if(range1[rand1][1][0]==range2[rand2][0][0] && range1[rand1][1][1]==range2[rand2][0][1]) {
				invalid=true;
			} else if(range1[rand1][1][0]==range2[rand2][1][0] && range1[rand1][1][1]==range2[rand2][1][1]) {
				invalid=true;
			}
			// cout<<"r.";
		} while(invalid);

		int board[5][2];
		for(int j=0;j<5;j++) {
			bool dupe;
			do {
				int rand3 = rand()%50;
				board[j][0] = deck[rand3][0];
				board[j][1] = deck[rand3][1];

				dupe = false;
				for(int k=0;k<j;k++) {
					if(board[j][0]==board[k][0] && board[j][1]==board[k][1]) {
						dupe=true;
						k=j;
					}
				}
				if(board[j][0]==range1[rand1][0][0] && board[j][1]==range1[rand1][0][1]) {
					dupe=true;
				} else if(board[j][0]==range1[rand1][1][0] && board[j][1]==range1[rand1][1][1]) {
					dupe=true;
				} else if(board[j][0]==range2[rand2][0][0] && board[j][1]==range2[rand2][0][1]) {
					dupe=true;
				} else if(board[j][0]==range2[rand2][1][0] && board[j][1]==range2[rand2][1][1]) {
					dupe=true;
				}
				// cout<<"r!";
			} while(dupe);
		}

		if(handInfo.stage>=1) {
			for(int j=0;j<6;j++) {
				board[j/2][j%2] = handInfo.board[j/2][j%2];
			}
		}
		if(handInfo.stage>=2) {
			board[3][0] = handInfo.board[3][0];
			board[3][1] = handInfo.board[3][1];
		}
		if(handInfo.stage>=3) {
			board[4][0] = handInfo.board[4][0];
			board[4][1] = handInfo.board[4][1];
		}

		int cards1[7][2];
		for(int j=0;j<4;j++) {
			cards1[j/2][j%2]=range1[rand1][j/2][j%2];
		}
		int cards2[7][2];
		for(int j=0;j<4;j++) {
			cards2[j/2][j%2]=range2[rand2][j/2][j%2];
		}
		for(int j=0;j<5;j++) {
			cards1[j+2][0]=board[j][0];
			cards1[j+2][1]=board[j][1];
			cards2[j+2][0]=board[j][0];
			cards2[j+2][1]=board[j][1];
		}

		int data1[5][2]={0};
		double str1 = strengthOfBestHand(cards1,data1,7,5,0,0);
		int data2[5][2]={0};
		double str2 = strengthOfBestHand(cards2,data2,7,5,0,0);

		if(str1>str2) {
			wins++;
		} else if (str1==str2) {
			ties++;
		}

	}

	float winRate = (wins + ties/2.0) / (1.0*n);
	return winRate;
}

float winRateOfHandVersusRange(HandInfo &handInfo, int n, int simHand[2][2], int simRange[1326][2][2]) {
	
	int wins=0,ties=0;

	int deck[50][2];
	int i2=0;
	for(int i=0;i<52;i++) {
		if(!(handInfo.deck[i][0]==simHand[0][0] && handInfo.deck[i][1]==simHand[0][1]) && !(handInfo.deck[i][0]==simHand[1][0] && handInfo.deck[i][1]==simHand[1][1])) {
			deck[i2][0]=handInfo.deck[i][0];
			deck[i2][1]=handInfo.deck[i][1];
			i2++;
		}
	}

	int rangeSize = sizeOfRange(simRange);
	for(int i=0;i<n;i++) {
		int rand1;
		do {
			rand1 = rand()%rangeSize;
		} while(simRange[rand1][0]==0);

		int board[5][2];
		int ct = 0;
		for(int j=0;j<5;j++) {
			bool dupe;
			do {
				int rand2 = rand()%50;
				board[j][0] = deck[rand2][0];
				board[j][1] = deck[rand2][1];

				dupe = false;
				for(int k=0;k<j;k++) {
					if(board[j][0]==board[k][0] && board[j][1]==board[k][1]) {
						dupe=true;
						k=j;
					}
				}
				if((board[j][0]==simRange[rand1][0][0] && board[j][1]==simRange[rand1][0][1]) || (board[j][0]==simRange[rand1][1][0] && board[j][1]==simRange[rand1][1][1])) {
					dupe=true;
				}
				ct++;
				if(ct%100==0) {
					// cout<<"h.";
				}
			} while(dupe);
		}

		if(handInfo.stage>=1) {
			for(int j=0;j<6;j++) {
				board[j/2][j%2] = handInfo.board[j/2][j%2];
			}
		}
		if(handInfo.stage>=2) {
			board[3][0] = handInfo.board[3][0];
			board[3][1] = handInfo.board[3][1];
		}
		if(handInfo.stage>=3) {
			board[4][0] = handInfo.board[4][0];
			board[4][1] = handInfo.board[4][1];
		}

		int cards1[7][2];
		for(int j=0;j<4;j++) {
			cards1[j/2][j%2]=simHand[j/2][j%2];
		}
		int cards2[7][2];
		for(int j=0;j<4;j++) {
			cards2[j/2][j%2]=simRange[rand1][j/2][j%2];
		}
		for(int j=0;j<5;j++) {
			cards1[j+2][0]=board[j][0];
			cards1[j+2][1]=board[j][1];
			cards2[j+2][0]=board[j][0];
			cards2[j+2][1]=board[j][1];
		}

		int data1[5][2]={0};
		double str1 = strengthOfBestHand(cards1,data1,7,5,0,0);
		int data2[5][2]={0};
		double str2 = strengthOfBestHand(cards2,data2,7,5,0,0);

		if(str1>str2) {
			wins++;
		} else if (str1==str2) {
			ties++;
		}

	}

	float winRate = (wins + ties/2.0) / (1.0*n);
	return winRate;
}

int* balanceRangeBetCheck(HandInfo &handInfo, int range1[1326][2][2], int range2[1326][2][2], int n, float *betSize) {
	static int balance[2];

	int range1Size = sizeOfRange(range1);
	int numValueBets = 0,numChecks = range1Size,numBluffs = 0;

	cout<<"Finding best bet size...\n";
	*betSize = mostProfitableBetSize(handInfo,&numValueBets,&numChecks,&numBluffs);
	cout<<*betSize<<", ";
	cout<<numValueBets<<"v, "<<numChecks<<"c, "<<numBluffs<<"b\n";
	if(*betSize<0) {
		*betSize=0;
	}

	balance[0] = numValueBets;
	balance[1] = numValueBets + numChecks;

	return balance;
}

int* balanceRangeRaiseCallFold(HandInfo &handInfo, int range1[1326][2][2], int range2[1326][2][2], int n, float *betSize, int callSize) {
	static int balance[3];

	int range1Size = sizeOfRange(range1);
	int numValueRaises = 0,numCalls = 0,numBluffRaises = 0,numFolds = range1Size;

	*betSize = mostProfitableRaiseSize(handInfo,&numValueRaises,&numCalls,&numBluffRaises);
	numFolds = range1Size - numValueRaises - numCalls - numBluffRaises;
	cout<<numValueRaises<<"v, "<<numCalls<<"c, "<<numBluffRaises<<"b, "<<numFolds<<"f\n";
	if(*betSize<0) {
		*betSize=0;
	}

	cout<<"Villain has raised? -> "<<handInfo.villainHasRaised<<endl;
	if(handInfo.villainHasRaised) {
		*betSize = 0;
		balance[0] = 0;
		balance[1] = numValueRaises + numCalls + numBluffRaises;
		balance[2] = balance[1];
	} else if(handInfo.stage==0 && handInfo.position==0 && handInfo.heroBet==handInfo.smallBlind && handInfo.villainBet==handInfo.bigBlind) {
		balance[0] = numValueRaises + numCalls + numBluffRaises;
		balance[1] = balance[0];
		balance[2] = balance[1];
	} else {
		balance[0] = numValueRaises;
		balance[1] = numValueRaises + numCalls;
		balance[2] = numValueRaises + numCalls + numBluffRaises;
	}

	return balance;
}

float mostProfitableBetSize(HandInfo &handInfo, int *numValueBets, int *numChecks, int *numBluffs) {
	float betSizes[8] = { 0,.25,.5,.75,1,1.5,2,3 };
	float values[8] = {0};
	float bestBetSize = 0;
	float bestValue = 0;
	int totalPot = handInfo.potSize + handInfo.heroBet + handInfo.villainBet;

	int heroRangeSize = sizeOfRange(handInfo.heroRange);

	cout<<"Calculating profit of checking...\n";
	for(int i=0;i<heroRangeSize;i++) {
		float equity = winRateOfHandVersusRange(handInfo,TRIALS,handInfo.heroRange[i],handInfo.villainRange);
		float profit = equity * totalPot - (1-equity) * totalPot;
		values[0] += profit;
	}
	values[0]/=heroRangeSize;
	cout<<"Checking -> "<<values[0]<<endl;
	cout<<"Ok now for betting...\n";
	bestValue = values[0];

	for(int ii=1;ii<8;ii++) {

		/*
		 * create villain's continuing range given betsize (using mdf)
		 *
		 * numvaluebets = number of hands that have >50% equity against
		 * villain's continuing range
		 *
		 * use value bet to bluff ratio given betsize to find number
		 * of bluffs needed to balance betting range
		 *
		 * if numvaluebets + numbluffs > size of range then
		 * decrement numvaluebets until calculated
		 * numbluffs + numvaluebets <= size of range
		 * 
		 * calculate expected value of betting this range
		 *
		 */

		if(betSizes[ii]*totalPot > handInfo.heroStack) {
			betSizes[ii] = (handInfo.heroStack*1.0) / (totalPot*1.0);
		}
		// if(betSizes[ii]*totalPot > handInfo.villainStack) {
		// 	betSizes[ii] = (handInfo.villainStack*1.0) / (totalPot*1.0);
		// }

		int villainContinuingRange[1326][2][2] = {0};
		for(int i=0;i<1326;i++) {
			for(int j=0;j<4;j++) {
				villainContinuingRange[i][j/2][j%2] = handInfo.villainRange[i][j/2][j%2];
			}
		}
		int villainRangeSize = sizeOfRange(villainContinuingRange);
		float mdf1 = 1 - ((1.0*betSizes[ii]*totalPot)/(1.0*totalPot+betSizes[ii]*totalPot));
		// float equityNeeded = betSizes[ii] / (1.0 + 2.0 * betSizes[ii]);
		int lastCallIndex = 0;
		// for(int i=0;i<villainRangeSize;i++) {
		// 	if(handInfo.villainRangeEquities[i] >= equityNeeded) {
		// 		lastCallIndex = i;
		// 	} else {
		// 		i = villainRangeSize;
		// 	}
		// }
		int maxIndex = max(ceil(mdf1*villainRangeSize), lastCallIndex);
		cout<<mdf1<<"mdf, "<<(lastCallIndex*1.0)/(villainRangeSize*1.0)<<"lci"<<endl;
		float mdf = (1.0*maxIndex)/(1.0*villainRangeSize);
		for(int i=maxIndex+1;i<villainRangeSize;i++) {
			villainContinuingRange[i][0][0] = 0;
		}

		float VtoBRatio = (betSizes[ii] + 1) / (betSizes[ii]);

		float equitiesVContRange[1326]={0};
		int nValue = 0;
		for(int i=0;i<heroRangeSize;i++) {
			equitiesVContRange[i] = winRateOfHandVersusRange(handInfo,TRIALS,handInfo.heroRange[i],villainContinuingRange);
			if(equitiesVContRange[i]>.5) {
				nValue++;
			}
		}

		int nBluffs = ceil((1.0 / VtoBRatio) * nValue);
		while(nValue+nBluffs > heroRangeSize) {
			nValue--;
			nBluffs = ceil((1.0 / VtoBRatio) * nValue);
		}

		int nChecks = heroRangeSize - nValue - nBluffs;

		for(int i=0;i<nValue;i++) {
			float profit = mdf * (equitiesVContRange[i] * (totalPot+betSizes[ii]*totalPot) - (1-equitiesVContRange[i]) * (betSizes[ii]*totalPot));
			profit += (1-mdf) * totalPot;
			values[ii] += profit;
		}
		for(int i=nValue;i<nValue+nChecks;i++) {
			float equity = winRateOfHandVersusRange(handInfo,TRIALS,handInfo.heroRange[i],handInfo.villainRange);
			float profit = equity * totalPot - (1-equity) * totalPot;
			values[ii] += profit;
		}
		for(int i=nValue+nChecks;i<heroRangeSize;i++) {
			float profit = mdf * (equitiesVContRange[i] * (totalPot+betSizes[ii]*totalPot) - (1-equitiesVContRange[i]) * (betSizes[ii]*totalPot));
			profit += (1-mdf) * totalPot;
			values[ii] += profit;
		}
		values[ii]/=heroRangeSize;

		if(values[ii] > bestValue) {
			bestBetSize = betSizes[ii];
			bestValue = values[ii];
			*numValueBets = nValue;
			*numChecks = nChecks;
			*numBluffs = nBluffs;
			cout<<"Better, ";
		}
		cout<<"Finished with bet size "<<betSizes[ii]<<" -> "<<values[ii]<<endl;

	}

	return bestBetSize;
}

float mostProfitableRaiseSize(HandInfo &handInfo, int *numValueRaises, int *numCalls, int *numBluffRaises) {
	float betSizes[9] = { -1,0,.25,.5,.75,1,1.5,2,3 };
	float values[9] = {0};
	float bestBetSize = -1;
	float bestValue = 0;
	int totalPot = handInfo.potSize + handInfo.heroBet + handInfo.villainBet;

	int heroRangeSize = sizeOfRange(handInfo.heroRange);
	float mdfc = 1 - ((1.0*handInfo.villainBet-1.0*handInfo.prevVillainBet)/(1.0*totalPot));
	float equityNeededc = (1.0*handInfo.villainBet-1.0*handInfo.prevVillainBet) / ((totalPot) + (1.0*handInfo.villainBet-1.0*handInfo.prevVillainBet));
	int lastCallIndexc = 0;
	// for(int i=0;i<heroRangeSize;i++) {
	// 	if(handInfo.heroRangeEquities[i] >= equityNeededc) {
	// 		lastCallIndexc = i;
	// 	} else {
	// 		i = heroRangeSize;
	// 	}
	// }
	int maxIndexc = max(ceil(mdfc*heroRangeSize), lastCallIndexc);
	cout<<"TEST ";
	cout<<handInfo.potSize<<" "<<handInfo.heroBet<<" "<<handInfo.villainBet<<" "<<totalPot<<" \n";
	float spr = (handInfo.heroStack*1.0) / (totalPot*1.0);
	int numC = 0;
	if(spr < 1.0) {
		for(int i=0;i<heroRangeSize;i++) {
			float equity = winRateOfHandVersusRange(handInfo,TRIALS,handInfo.heroRange[i],handInfo.villainRange);
			float profit = 0;
			if(equity > equityNeededc) {
				profit = equity * totalPot - (1-equity) * (handInfo.villainBet-handInfo.heroBet);
				numC++;
			}
			values[1] += profit;
		}
	}
	if(numC==0) {
		for(int i=0;i<maxIndexc+1;i++) {
			float equity = winRateOfHandVersusRange(handInfo,TRIALS,handInfo.heroRange[i],handInfo.villainRange);
			float profit = equity * totalPot - (1-equity) * (handInfo.villainBet-handInfo.heroBet);
			values[1] += profit;
			numC++;
		}
	}
	values[1]/=heroRangeSize;
	bestBetSize = 0;
	bestValue = values[1];
	*numCalls = numC;

	if(handInfo.stage==0 && handInfo.position==0 && handInfo.heroBet==handInfo.smallBlind && handInfo.villainBet==handInfo.bigBlind) {
		bestBetSize = 1;
	}

	cout<<"Folding -> "<<values[0]<<endl;
	cout<<"Calling -> "<<values[1]<<endl;

	for(int ii=2;ii<9;ii++) {

		if(betSizes[ii]*totalPot+handInfo.villainBet-handInfo.heroBet > handInfo.heroStack) {
			betSizes[ii] = (1.0*handInfo.heroStack+handInfo.heroBet-handInfo.villainBet) / (1.0*totalPot);
		}

		int heroContinuingRange[1326][2][2] = {0};
		for(int i=0;i<1326;i++) {
			for(int j=0;j<4;j++) {
				heroContinuingRange[i][j/2][j%2] = handInfo.heroRange[i][j/2][j%2];
			}
		}
		float mdfh = 1 - ((1.0*handInfo.villainBet-1.0*handInfo.prevVillainBet)/(1.0*totalPot));

		float equityNeededh = (1.0*handInfo.villainBet-1.0*handInfo.heroBet)/(1.0*totalPot+(1.0*handInfo.villainBet-1.0*handInfo.heroBet));
		int lastCallIndexh = 0;
		// for(int i=0;i<heroRangeSize;i++) {
		// 	if(handInfo.heroRangeEquities[i] >= equityNeededh) {
		// 		lastCallIndexh = i;
		// 	} else {
		// 		i = heroRangeSize;
		// 	}
		// }
		int maxIndexh = max(ceil(mdfh*heroRangeSize), lastCallIndexh);

		for(int i=maxIndexh+1;i<heroRangeSize;i++) {
			heroContinuingRange[i][0][0] = 0;
		}
		int heroContRangeSize = sizeOfRange(heroContinuingRange);

		int villainContinuingRange[1326][2][2] = {0};
		for(int i=0;i<1326;i++) {
			for(int j=0;j<4;j++) {
				villainContinuingRange[i][j/2][j%2] = handInfo.villainRange[i][j/2][j%2];
			}
		}
		int villainRangeSize = sizeOfRange(villainContinuingRange);
		//double check this math:
		float mdf1 = 1 - ((1.0*betSizes[ii]*totalPot+handInfo.villainBet-1.0*handInfo.heroBet)/(1.0*totalPot+betSizes[ii]*totalPot+handInfo.villainBet-handInfo.heroBet));

		float equityNeeded = (1.0*betSizes[ii]*totalPot) / (totalPot + 2.0 * betSizes[ii]*totalPot);
		int lastCallIndex = 0;
		for(int i=0;i<villainRangeSize;i++) {
			if(handInfo.villainRangeEquities[i] >= equityNeeded) {
				lastCallIndex = i;
			} else {
				i = villainRangeSize;
			}
		}
		int maxIndex = max(ceil(mdf1*villainRangeSize), lastCallIndex);
		cout<<mdf1<<"mdf, "<<(lastCallIndex*1.0)/(villainRangeSize*1.0)<<"lci"<<endl;
		float mdf = (1.0*maxIndex)/(1.0*villainRangeSize);

		for(int i=maxIndex+1;i<villainRangeSize;i++) {
			villainContinuingRange[i][0][0] = 0;
		}

		float VtoBRatio = (betSizes[ii] + 1) / (betSizes[ii]);

		float equitiesVContRange[1326]={0};
		int nValue = 0;
		for(int i=0;i<heroContRangeSize;i++) {
			equitiesVContRange[i] = winRateOfHandVersusRange(handInfo,TRIALS,heroContinuingRange[i],villainContinuingRange);
			if(equitiesVContRange[i]>.5) {
				nValue++;
			}
		}

		int nBluffs = ceil((1.0 / VtoBRatio) * nValue);
		while(nValue+nBluffs > heroContRangeSize) {
			nValue--;
			nBluffs = ceil((1.0 / VtoBRatio) * nValue);
		}

		int nCalls = heroContRangeSize - nValue - nBluffs;

		for(int i=0;i<nValue;i++) {
			float profit = mdf * (equitiesVContRange[i] * (totalPot+betSizes[ii]*totalPot+handInfo.villainBet-handInfo.prevHeroBet) - (1-equitiesVContRange[i]) * (betSizes[ii]*totalPot+handInfo.villainBet-handInfo.prevHeroBet));
			profit += (1-mdf) * totalPot;
			values[ii] += profit;
		}
		for(int i=nValue;i<nValue+nCalls;i++) {
			float equity = winRateOfHandVersusRange(handInfo,TRIALS,heroContinuingRange[i],handInfo.villainRange);
			float profit = equity * totalPot - (1-equity) * handInfo.villainBet-handInfo.heroBet;
			values[ii] += profit;
		}
		for(int i=nValue+nCalls;i<heroContRangeSize;i++) {
			float profit = mdf * (equitiesVContRange[i] * (totalPot+betSizes[ii]*totalPot+handInfo.villainBet-handInfo.prevHeroBet) - (1-equitiesVContRange[i]) * (betSizes[ii]*totalPot+handInfo.villainBet-handInfo.prevHeroBet));
			profit += (1-mdf) * totalPot;
			values[ii] += profit;
		}
		values[ii]/=heroRangeSize;

		if(values[ii] > bestValue) {
			bestBetSize = betSizes[ii];
			bestValue = values[ii];
			*numValueRaises = nValue;
			*numCalls = nCalls;
			*numBluffRaises = nBluffs;
			cout<<"Better, ";
		}
		cout<<"Finished with raise size "<<betSizes[ii]<<" -> "<<values[ii]<<endl;

	}

	return bestBetSize;
}

void updateRangeBetCheck(int range[1326][2][2], int balance[2], char action) {
	int rangeCopy[1326][2][2];
	for(int i=0;i<1326;i++) {
		for(int j=0;j<4;j++) {
			rangeCopy[i][j/2][j%2] = range[i][j/2][j%2];
		}
	}
	int rangeSize = sizeOfRange(rangeCopy);
	if(action=='b') {
		for(int i=balance[0];i<balance[1];i++) {
			rangeCopy[i][0][0] = 0;
		}
	} else if(action=='c') {
		for(int i=0;i<balance[0];i++) {
			rangeCopy[i][0][0] = 0;
		}
		for(int i=balance[1];i<rangeSize;i++) {
			rangeCopy[i][0][0] = 0;
		}
	}
	if(sizeOfRange(rangeCopy) > 0) {
		for(int i=0;i<1326;i++) {
			for(int j=0;j<4;j++) {
				range[i][j/2][j%2] = rangeCopy[i][j/2][j%2];
			}
		}
	}
}

void updateRangeRaiseCallFold(int range[1326][2][2], int balance[3], char action) {
	int rangeCopy[1326][2][2];
	for(int i=0;i<1326;i++) {
		for(int j=0;j<4;j++) {
			rangeCopy[i][j/2][j%2] = range[i][j/2][j%2];
		}
	}
	int rangeSize = sizeOfRange(rangeCopy);
	if(action=='r') {
		for(int i=balance[0];i<balance[1];i++) {
			rangeCopy[i][0][0] = 0;
		}
		for(int i=balance[2];i<rangeSize;i++) {
			rangeCopy[i][0][0] = 0;
		}
	} else if(action=='c') {
		for(int i=0;i<balance[0];i++) {
			rangeCopy[i][0][0] = 0;
		}
		for(int i=balance[1];i<rangeSize;i++) {
			rangeCopy[i][0][0] = 0;
		}
	}
	if(sizeOfRange(rangeCopy) > 0) {
		for(int i=0;i<1326;i++) {
			for(int j=0;j<4;j++) {
				range[i][j/2][j%2] = rangeCopy[i][j/2][j%2];
			}
		}
	}
}

void updateVillainRangeBet(HandInfo &handInfo, int n, float betSize) {

	int totalPot = handInfo.potSize + handInfo.heroBet + handInfo.villainBet;
	int villainRangeSize = sizeOfRange(handInfo.villainRange);

	int heroContinuingRange[1326][2][2] = {0};
	for(int i=0;i<1326;i++) {
		for(int j=0;j<4;j++) {
			heroContinuingRange[i][j/2][j%2] = handInfo.heroRange[i][j/2][j%2];
		}
	}
	int heroRangeSize = sizeOfRange(heroContinuingRange);
	float mdf1 = 1 - ((1.0*betSize*handInfo.potSize)/(1.0*handInfo.potSize+betSize*handInfo.potSize));

	// float equityNeeded = betSize / (1.0 + 2.0 * betSize);
	int lastCallIndex = 0;
	// for(int i=0;i<heroRangeSize;i++) {
	// 	if(handInfo.heroRangeEquities[i] >= equityNeeded) {
	// 		lastCallIndex = i;
	// 	} else {
	// 		i = heroRangeSize;
	// 	}
	// }
	int maxIndex = max(ceil(mdf1*heroRangeSize), lastCallIndex);
	cout<<mdf1<<"mdf, "<<(lastCallIndex*1.0)/(heroRangeSize*1.0)<<"lci"<<endl;
	float mdf = (1.0*maxIndex)/(1.0*heroRangeSize);

	cout<<"oink "<<mdf<<endl;
	for(int i=maxIndex+1;i<heroRangeSize;i++) {
		heroContinuingRange[i][0][0] = 0;
	}

	float VtoBRatio = (betSize + 1) / (betSize);

	int nValue = 0;
	for(int i=0;i<heroRangeSize;i++) {
		float equity = winRateOfHandVersusRange(handInfo,TRIALS,handInfo.villainRange[i],heroContinuingRange);
		if(equity>.5) {
			nValue++;
		}
	}
	cout<<"Value before balance: "<<nValue<<endl;

	int nBluffs = ceil((1.0 / VtoBRatio) * nValue);
	while(nValue+nBluffs > heroRangeSize) {
		nValue--;
		nBluffs = ceil((1.0 / VtoBRatio) * nValue);
	}

	int nChecks = heroRangeSize - nValue - nBluffs;

	cout<<"Value bets: "<<nValue<<endl;
	cout<<"Checks: "<<nChecks<<endl;
	cout<<"Bluffs: "<<nBluffs<<endl;

	int balance[2];
	balance[0] = nValue;
	balance[1] = nValue + nChecks;

	updateRangeBetCheck(handInfo.villainRange,balance,'b');
}

void updateVillainRangeRaise(HandInfo &handInfo, int n, float betSize) {
	int totalPot = handInfo.potSize + handInfo.heroBet + handInfo.villainBet;

	int villainRangeSize = sizeOfRange(handInfo.villainRange);

	int villainContinuingRange[1326][2][2] = {0};
	for(int i=0;i<1326;i++) {
		for(int j=0;j<4;j++) {
			villainContinuingRange[i][j/2][j%2] = handInfo.villainRange[i][j/2][j%2];
		}
	}
	float mdfh = 1 - ((1.0*handInfo.heroBet-1.0*handInfo.prevHeroBet)/(1.0*totalPot));

	// float equityNeededh = (1.0*handInfo.heroBet-handInfo.prevVillainBet) / (1.0*totalPot + (1.0*handInfo.heroBet-handInfo.prevVillainBet));
	int lastCallIndexh = 0;
	// for(int i=0;i<villainRangeSize;i++) {
	// 	if(handInfo.villainRangeEquities[i] >= equityNeededh) {
	// 		lastCallIndexh = i;
	// 	} else {
	// 		i = villainRangeSize;
	// 	}
	// }
	int maxIndexh = max(ceil(mdfh*villainRangeSize), lastCallIndexh);

	for(int i=maxIndexh+1;i<villainRangeSize;i++) {
		villainContinuingRange[i][0][0] = 0;
	}
	int villainContRangeSize = sizeOfRange(villainContinuingRange);

	int heroContinuingRange[1326][2][2] = {0};
	for(int i=0;i<1326;i++) {
		for(int j=0;j<4;j++) {
			heroContinuingRange[i][j/2][j%2] = handInfo.heroRange[i][j/2][j%2];
		}
	}
	int heroRangeSize = sizeOfRange(heroContinuingRange);
	//float mdf = 1 - ((betSize*totalPot+handInfo.heroBet-handInfo.villainBet)/(totalPot+betSize*totalPot+handInfo.heroBet-handInfo.villainBet));
	//double check this math:
	float mdf1 = 1 - (betSize*(handInfo.potSize+handInfo.heroBet+handInfo.prevVillainBet)+handInfo.heroBet-handInfo.villainBet)/((betSize*(handInfo.potSize+handInfo.heroBet+handInfo.prevVillainBet)+handInfo.heroBet-handInfo.villainBet)+handInfo.potSize+handInfo.heroBet+handInfo.prevVillainBet);

	float equityNeeded = betSize / (1.0 + 2.0 * betSize);
	int lastCallIndex = 0;
	for(int i=0;i<heroRangeSize;i++) {
		if(handInfo.heroRangeEquities[i] >= equityNeeded) {
			lastCallIndex = i;
		} else {
			i = heroRangeSize;
		}
	}
	int maxIndex = max(ceil(mdf1*heroRangeSize), lastCallIndex);
	cout<<mdf1<<"mdf, "<<(lastCallIndex*1.0)/(heroRangeSize*1.0)<<"lci"<<endl;
	float mdf = (1.0*maxIndex)/(1.0*heroRangeSize);

	for(int i=maxIndex+1;i<heroRangeSize;i++) {
		heroContinuingRange[i][0][0] = 0;
	}

	float VtoBRatio = (betSize + 1) / (betSize);

	float equitiesVContRange[1326]={0};
	int nValue = 0;
	for(int i=0;i<villainContRangeSize;i++) {
		equitiesVContRange[i] = winRateOfHandVersusRange(handInfo,TRIALS,villainContinuingRange[i],heroContinuingRange);
		if(equitiesVContRange[i]>.5) {
			nValue++;
		}
	}

	int nBluffs = ceil((1.0 / VtoBRatio) * nValue);
	while(nValue+nBluffs > villainContRangeSize) {
		nValue--;
		nBluffs = ceil((1.0 / VtoBRatio) * nValue);
	}

	int nCalls = villainContRangeSize - nValue - nBluffs;

	int balance[3];
	balance[0] = nValue;
	balance[1] = nValue + nCalls;
	balance[2] = nValue + nCalls + nBluffs;

	updateRangeRaiseCallFold(handInfo.villainRange,balance,'r');
}

double strengthOfBestHand(int cards[7][2],int data[5][2],int n,int r,int s,int d) {
	double max = 0;

	if(r==0) {
		max = strengthOfHand(data);
	} else {
		for(int i=s;i<s+n-r+1;i++) {
			data[d][0] = cards[i][0];
			data[d][1] = cards[i][1];
			double str = strengthOfBestHand(cards,data,n-(i-s)-1,r-1,i+1,d+1);
			if(str>max) {
				max=str;
			}
		}
	}

	return max;
}

double strengthOfHand(int cards1[5][2]) {

	int cards[5][2];
	for(int i=0;i<5;i++) {
		cards[i][0]=cards1[i][0];
		cards[i][1]=cards1[i][1];
	}

	//sort
	int key1,key2,j;
	for(int i=1;i<5;i++) {
		key1=cards[i][0];
		key2=cards[i][1];
		j=i-1;
		while(j>=0 && cards[j][0]<key1) {
			cards[j+1][0]=cards[j][0];
			cards[j+1][1]=cards[j][1];
			j--;
		}
		cards[j+1][0]=key1;
		cards[j+1][1]=key2;
	}

	//Four of a Kind
	if (cards[0][0]==cards[1][0] && cards[0][0]==cards[2][0] && cards[0][0]==cards[3][0]) {
		return 8000 + cards[4][0];
	} else if (cards[4][0]==cards[1][0] && cards[4][0]==cards[2][0] && cards[4][0]==cards[3][0]) {
		return 8000 + cards[0][0];
	}

	//Full House
	if (cards[0][0]==cards[1][0] && cards[0][0]==cards[2][0] && cards[3][0]==cards[4][0]) {
		return 7000 + cards[0][0] * 10 + cards[4][0] * .1;
	} else if (cards[0][0]==cards[1][0] && cards[2][0]==cards[3][0] && cards[2][0]==cards[4][0]) {
		return 7000 + cards[4][0] * 10 + cards[0][0] * .1;
	}

	bool flush=false;
	//Flush
	if (cards[0][1]==cards[1][1] && cards[0][1]==cards[2][1] && cards[0][1]==cards[3][1] && cards[0][1]==cards[4][1]) {
		flush=true;
	}

	//Straight and Straight Flush
	if (cards[0][0]-cards[4][0]==4 && cards[0][0]!=cards[1][0] && cards[1][0]!=cards[2][0] && cards[2][0]!=cards[3][0] && cards[3][0]!=cards[4][0]) {
		if(flush) {
			return 9000 + cards[0][0];
		} else {
			return 5000 + cards[0][0];
		}
	} else if (cards[0][0]==14 && cards[1][0]==5 && cards[2][0]==3 && cards[3][0]==3 && cards[4][0]==2) {
		if(flush) {
			return 9000 + cards[1][0];
		} else {
			return 5000 + cards[1][0];
		}
	} else if (flush) {
		return 6000 + cards[0][0] * 10 + cards[1][0] * .1 + cards[2][0] * .001 + cards[3][0] * .00001 + cards[4][0] * .0000001;
	}

	//Three of a Kind
	if (cards[0][0]==cards[1][0] && cards[0][0]==cards[2][0]) {
		return 4000 + cards[0][0]*10 + cards[3][0]*.1 + cards[4][0]*.001;
	} else if (cards[1][0]==cards[2][0] && cards[1][0]==cards[3][0]) {
		return 4000 + cards[1][0]*10 + cards[0][0]*.1 + cards[4][0]*.001;
	} else if (cards[2][0]==cards[3][0] && cards[2][0]==cards[4][0]) {
		return 4000 + cards[2][0]*10 + cards[0][0]*.1 + cards[1][0]*.001;
	}

	//Two Pair
	if (cards[0][0]==cards[1][0] && cards[2][0]==cards[3][0]) {
		return 3000 + cards[0][0]*10 + cards[2][0]*.1 + cards[4][0]*.001;
	} else if (cards[0][0]==cards[1][0] && cards[3][0]==cards[4][0]) {
		return 3000 + cards[0][0]*10 + cards[3][0]*.1 + cards[2][0]*.001;
	} else if (cards[1][0]==cards[2][0] && cards[3][0]==cards[4][0]) {
		return 3000 + cards[1][0]*10 + cards[3][0]*.1 + cards[0][0]*.001;
	}

	//Pair
	if (cards[0][0]==cards[1][0]) {
		return 2000 + cards[0][0]*10 + cards[2][0]*.1 + cards[3][0]*.001 + cards[4][0]*.00001;
	} else if (cards[1][0]==cards[2][0]) {
		return 2000 + cards[1][0]*10 + cards[0][0]*.1 + cards[3][0]*.001 + cards[4][0]*.00001;
	} else if (cards[2][0]==cards[3][0]) {
		return 2000 + cards[2][0]*10 + cards[0][0]*.1 + cards[1][0]*.001 + cards[4][0]*.00001;
	} else if (cards[3][0]==cards[4][0]) {
		return 2000 + cards[3][0]*10 + cards[0][0]*.1 + cards[1][0]*.001 + cards[2][0]*.00001;
	}

	//High Card
	return 1000 + cards[0][0]*10 + cards[1][0]*.1 + cards[2][0]*.001 + cards[3][0]*.00001 + cards[4][0]*.0000001;
}



