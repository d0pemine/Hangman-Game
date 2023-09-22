// KELOMPOK 8
// ANGGOTA YANG TIDAK BEKERJA :
// GIAN GUIDO (2502016090)
// MARCEL KURNIAWAN (2501977090)
// MARTIN CANDRA (2501962706)
// RAFAEL ROGER (2501962826)
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h> 

#define LiveAmount 10 
#define BaseScore 20 
#define DictFile "Dictionary.txt" 
#define LeadFile "Leaderboard.txt" 
#define MaxWordLen 26
#define MaxClueLen 20
#define MaxNameLen 51 

char Name[MaxNameLen];

typedef struct{
    char word[MaxWordLen];
    char clue[MaxClueLen];
    bool unlocked;
} Dictionary;

typedef struct{
    char name[MaxNameLen];
    int score;
} Leaderboard;

void ReturnToMenu();
void ToLower(char *str, int len);
bool FileError(FILE *fp);
void MergeSortD(Dictionary *data, int min, int max);
void MergeSortL(Leaderboard *data, int min, int max, bool score);
int BinerSearch(Leaderboard *data, int min, int max);

void SaveScore(int score){
    FILE *ScoreFile = fopen(LeadFile, "r");
    if(FileError(ScoreFile)){
        fclose(ScoreFile);
        ScoreFile = fopen(LeadFile, "w"); 
        fprintf(ScoreFile, "1\n");
        fprintf(ScoreFile, "%s %d\n", Name, score); 
        fclose(ScoreFile);
        return;
    }

    int DataCount; 
	fscanf(ScoreFile, "%d", &DataCount);
    Leaderboard LB[DataCount+1];
    int i;
    for(i = 0; i < DataCount; i++)
        fscanf(ScoreFile, "%s%d", LB[i].name, &LB[i].score); 
    	MergeSortL(LB, 0, DataCount-1, false);

    int FoundIndex = BinerSearch(LB, 0, DataCount-1);
    if(FoundIndex == -1){
        strcpy(LB[DataCount].name, Name);
        LB[DataCount].score = score;
        DataCount++;
    }
    else if(score > LB[FoundIndex].score) LB[FoundIndex].score = score;
    fclose(ScoreFile); 
	ScoreFile = fopen(LeadFile, "w"); 
    fprintf(ScoreFile, "%d\n", DataCount); 

    for(i = 0; i < DataCount; i++) 
        fprintf(ScoreFile, "%s %d\n", LB[i].name, LB[i].score); 
    fclose(ScoreFile);
}

char GetLetter(){
    char guess;
    bool invalid = true;
    while(invalid){
        printf("Guess a letter : "); 
		scanf(" %c", &guess);
        invalid = false;
        if(!((guess >= 'A' && guess <= 'Z') || (guess >= 'a' && guess <= 'z')) && guess != '.'){ 
			invalid = true; 
			printf("Please Input Alphabets only\n"); 
		}
    }
    return guess; 
}

void PrintHangMan(int lives){
    if(lives == LiveAmount) return; 
    int percent = lives*100/LiveAmount;
    if(percent <= 70) { printf("\t\t\t\t#######\n"); }
    if(percent <= 80) { printf("\t\t\t\t##"); } if(percent <= 60) { printf("   |"); }
    if(percent <= 80) { printf("\n\t\t\t\t##"); } if(percent <= 50) { printf("   O"); }
    if(percent <= 80) { printf("\n\t\t\t\t##"); } if(percent <= 40) { printf("  /"); } if(percent <= 30) { printf("|"); } if(percent <= 20) { printf("\\"); }
    if(percent <= 80) { printf("\n\t\t\t\t##"); } if(percent <= 10) { printf("  /"); } if(percent <= 0) { printf(" \\"); }
    if(percent <= 80) { printf("\n\t\t\t\t##"); }
    if(percent <= 90) { printf("\n\t\t\t\t#########\n\n"); }
}

int LetterRarity(bool *arr){
    int total = 0;
    int i;
    char common[] = "etainoshr";
    for(i = 0; i < 9; i++){ 
	if(arr[common[i]-'a']) total += 1; 
	}
    
    char medium[] = "dlucmfwyg";
    for(i = 0; i < 9; i++){ 
	if(arr[medium[i]-'a']) total += 2; 
	}
    
    char rare[] = "pbvkqjxz";
    for(i = 0; i < 8; i++){ 
	if(arr[rare[i]-'a']) total += 3; 
	}
    return total;
}

bool RandomizeWord(char* SecretWord, char* Clue){
    FILE *words = fopen(DictFile, "r");
	if(FileError(words)){ 
		printf("\nError : File Does Not Exist or is Empty\n"); 
		return false; 
	}

    int WordCount; 
	fscanf(words, "%d", &WordCount); 
    int i;
    Dictionary Dict[WordCount];
    for(i = 0; i < WordCount; i++){ 
		int temp; 
		fscanf(words, "%s %s %d", Dict[i].word, Dict[i].clue, &temp); 
		Dict[i].unlocked = temp; 
	}

    srand(time(0)); 
    int RandIndex = (rand() % (WordCount-1+1))+1; 
    Dict[RandIndex-1].unlocked = true;
    fclose(words); 
	words = fopen(DictFile, "w");

    fprintf(words, "%d\n", WordCount); 
    for(i = 0; i < WordCount; i++) 
        fprintf(words, "%s %s %d\n", Dict[i].word, Dict[i].clue, Dict[i].unlocked); 
    strcpy(SecretWord, Dict[RandIndex-1].word);
    strcpy(Clue, Dict[RandIndex-1].clue);
    fclose(words); 
	return true; 
}

void Game(int round, int TotalScore){
    char SecretWord[MaxWordLen]; 
    char Clue[MaxClueLen];
    if(!RandomizeWord(SecretWord,Clue)) return; 
    int len = strlen(SecretWord); 
    ToLower(SecretWord, len); 
 	
    char Revealed[len+1]; 
    int i;
    for(i = 0; i < len; i++){ 
		Revealed[i] = '-'; 
	} 
	Revealed[len] = '\0'; 
    char GuessList[27] = ""; 
    int GuessNum = 0; 
   

    int lives = LiveAmount; 
    bool wrong = false, guessed = false; 
    bool UniqueChar[27]; 
	memset(UniqueChar, 0, 27*sizeof(bool));
    for(i = 0; i < len; i++) 
	UniqueChar[SecretWord[i]-'a'] = true;
    int difficulty = LetterRarity(UniqueChar);
    while(true) {
        ClearScreen(); 
        if(guessed){ 
			printf("You already guessed that letter!\n"); 
			lives -= 2; 
			if(lives < 0) lives = 0; 
		}else if(wrong){ 
			printf("Incorrect!\n"); 
			lives--; 
			if(lives < 0) lives = 0; 
		}else if(GuessNum != 0){ 
			printf("You're Correct!\n"); 
		}

        printf("Round %d\n", round); 
        printf("Theme : %s\n",Clue);
        PrintHangMan(lives); 
        printf("\t\t\t\tGuess : %s\n", Revealed); 
        printf("%d letters\n", len); 

        printf("Used Letters : %s", GuessNum == 0 ? "none" : ""); 
        for(i = 0; i < 27; i++){
            if(GuessList[i] >= 'a' && GuessList[i] <= 'z')
                printf("%c", GuessList[i]);
        }

        printf("\nRemaining Lives : %d\n", lives);
        if(strcmp(SecretWord, Revealed) == 0 || lives == 0) break;
        char guess = GetLetter(); 
        if(guess == '.') { lives = 0; break; } 
        if(guess < 'a') guess += ('a'-'A'); 

        if(GuessList[guess-'a'] != guess){ 
			GuessList[guess-'a'] = guess; 
			guessed = false; 
			GuessNum++; 
		}else guessed = true;
        wrong = true; 
        for(i = 0; i < len; i++){
            if(SecretWord[i] == guess){ 
				Revealed[i] = guess; 
				wrong = false; 
			}
        }
    }
	
	ClearScreen();
    printf("\nYou %s The secret word was %s.\n", lives > 0 ? "win!" : "lose...", SecretWord);
    if(lives >0){
    	printf("\n\t\t H H H    	HHH  		H   H   	H   H  		HHHH   		HHH\n");
    	printf("\t\t H H H  	 H   		HH  H   	HH  H  		H      		H  H \n");
    	printf("\t\t H H H  	 H   		H H H   	H H H  		HHHH   		HHH   \n");
    	printf("\t\t H H H  	 H   		H  HH   	H  HH  		H      		H  H \n");
   		printf("\t\t  HHH       	HHH 		H   H   	H   H  		HHHH   		H   H\n\n\n");
	}else{
		printf("\t\t\t HHHHH   HHH   H     H  HHHH\n");
    	printf("\t\t\t H      H   H  HH   HH  H   \n");
   	 	printf("\t\t\t H  HH  HHHHH  H HHH H  HHHH\n");
    	printf("\t\t\t H   H  H   H  H  H  H  H   \n");
    	printf("\t\t\t HHHHH  H   H  H  H  H  HHHH\n\n\n"); 
    
    	printf("\t\t\t\t\t\t HHHH  	H   H  	HHHH 	HHH   \n");
    	printf("\t\t\t\t\t\t H  H  	H   H  	H    	H  H  \n");
    	printf("\t\t\t\t\t\t H  H  	H   H  	HHHH  	HHH   \n");
    	printf("\t\t\t\t\t\t H  H  	 H H   	H     	H  H  \n");
    	printf("\t\t\t\t\t\t HHHH  	  H    	HHHH  	H   H \n\n\n\n\n");
	}
    int score = TotalScore; 
    if(lives > 0) score += BaseScore + (difficulty * lives);
    printf("%s Score is : %d\n", lives > 0 ? "Current" : "Final", score);
    if(lives > 0){
        printf("\nPress [Enter] to continue\n"); 
        scanf("%*c%*c"); 
        Game(round+1, score); 
    }else{ 
		SaveScore(score); 
		ReturnToMenu();
	}
}

void PrintDictionary(){
    FILE *words = fopen(DictFile, "r"); 
    if(FileError(words)){ 
		printf("\nError : File Does Not Exist or is Empty\n"); 
		return; 
	}
    int WordCount; 
	fscanf(words, "%d", &WordCount); 
    int i;
    Dictionary Dict[WordCount]; 
    for(i = 0; i < WordCount; i++){ 
	int temp; 
	fscanf(words, "%s%s%d", Dict[i].word, Dict[i].clue, &temp);
	Dict[i].unlocked = temp; 
	}
    MergeSortD(Dict, 0, WordCount-1);
    ClearScreen();
    printf("Word Dictionary List :\n"); 
    int PrintedCount = 0; 
    for (i = 0; i < WordCount; i++){
        if(Dict[i].unlocked){ 
			PrintedCount++; 
			printf("%d. %s\n", PrintedCount, Dict[i].word); 
		}
    }
    if(PrintedCount == 0) printf("[none]\n"); 
    printf("\n%d more words to be discovered!\n", WordCount-PrintedCount); 
    fclose(words);
    ReturnToMenu(); 
}

void PrintLeaderboard(){
    ClearScreen();
    FILE *ScoreFile = fopen(LeadFile, "r");
    if(FileError(ScoreFile)) printf("There are no scores yet\n");
    else{
        int DataCount; 
		fscanf(ScoreFile, "%d", &DataCount); 
        int i;
        Leaderboard LB[DataCount]; 
        printf("Leaderboard:\n"); 
        for(i = 0; i < DataCount; i++) 
            fscanf(ScoreFile, "%s%d", LB[i].name, &LB[i].score);
        MergeSortL(LB, 0, DataCount-1, true);

        for(i = 0; i < DataCount; i++) 
            printf("%d. %s - %d\n", i+1, LB[i].name, LB[i].score); 
    }

    fclose(ScoreFile); 
    ReturnToMenu(); 
}

void Exit(){
    ClearScreen();
	char a[]={"Thank You For Playing!\n\nCREATED BY: \tGIAN GUIDO (2502016090)\n\t\tJOVITA AURELIA (2501959743)\n\t\tMARCEL KURNIAWAN (2501977090)\n\t\tMARIO INDRA (2501996556)\n\t\tMARTIN CANDRA (2501962706)\n\t\tRAFAEL ROGER (2501962826)"};
        int i;
        double j;
        for(i=0;i<=210;i++){
            printf("%c",a[i]);
        for(j=0;j<=9990000;j++){
            }
        }
    return 0;
}

void Howtoplay(){
	ClearScreen();
	char single[200];
    FILE *Howtoplay = fopen("Howtoplay.txt", "r"); 
    while(fgets(single, sizeof(single), Howtoplay)){
        printf("%s", single);
	} 
	printf("\n\n");
    fclose(Howtoplay); 
    ReturnToMenu(); 
}

void GetName(){
    system("cls"); 
    bool invalid = true; 

    while(invalid) {
        printf("Insert UserName : "); 
		scanf(" %[^\n]", Name);
        if(strlen(Name) >= MaxNameLen){ 
		printf("Name must be less %d characters\n", MaxNameLen); 
		continue; 
		}
        invalid = false; 
        int i;
        for(i = 0; i < strlen(Name); i++) {
            if(Name[i] != '_' && (Name[i] < '0' || (Name[i] > '9' && Name[i] < 'A') || (Name[i] > 'Z' && Name[i] < 'a') || Name[i] > 'z')){ 
			invalid = true; break; 
			}
        }
        if(invalid) 
		printf("Name can only contain letters, numbers, and underscore\n");
    }
}

void MainMenu(){
    ClearScreen(); 
    printf("Welcome to Hangman, %s!\n", Name);
    printf("-------------------------------\n");
    printf("\n\n\t H   H  	 HHH   		H    H  	 HHHH  		HH     HH  	   HHH   	H    H\n");
   	printf("\t H   H 	 	H   H  		HH   H  	H     		H  H H  H 	  H   H  	HH   H\n");
    printf("\t HHHHH 		HHHHH  		H H  H 		H HHH  		H   H   H	  HHHHH  	H H  H\n");
    printf("\t H   H 		H   H  		H  H H  	H   H  		H   H   H  	  H   H  	H  H H\n");
    printf("\t H   H 		H   H  		H   HH  	 HHH  		H   H   H	  H   H  	H   HH\n\n\n");
    
    printf("\t\t\t\t #########\n");
    printf("\t\t\t\t ##   &    \n");
    printf("\t\t\t\t ##   O   \n");
    printf("\t\t\t\t ##  /|\\ \n");
    printf("\t\t\t\t ##  / \\ \n");
    printf("\t\t\t\t ##       \n");
    printf("\t\t\t\t ##########\n");
    printf("\t\t\t\t ############\n");
    printf("\t\t\t\t ##############\n\n"); 
    printf("1. Start Game\n");
    printf("2. How to Play\n");
    printf("3. Dictionary\n");
    printf("4. Leaderboard\n");
    printf("5. Change Player\n");
    printf("0. Exit\n");

    char input[MaxNameLen] = ""; 
    bool invalid = true; 
    while(invalid){
        printf("-> "); 
		scanf(" %[^\n]", input); 
        invalid = false; 
        if(input[0] < '0' || input[0] > '5' || strlen(input) > 1){ 
			invalid = true; 
			printf("Please Input 0-5\n"); 
		}
    }

    char menu = input[0]; 
    if(menu == '0') Exit(); 
    else if(menu == '1') Game(1, 0);
    else if(menu == '2') Howtoplay();
    else if(menu == '3') PrintDictionary();
    else if(menu == '4') PrintLeaderboard();
    else if(menu == '5'){ 
		GetName(); 
		MainMenu(); 
	}
}

int main(){
    GetName();
    MainMenu();
    return 0;
}

void ClearScreen(){
        system("cls");
}

void ReturnToMenu(){
    printf("\nPress [Enter] to return\n");
    scanf("%*c%*c"); 
    MainMenu(); 
}

void ToLower(char *str, int len){
    int i;
    for (i = 0; i < len; i++){
        if(str[i] < 'a') str[i] += ('a'-'A');
    }
}

bool FileError(FILE *fp){
    if(fp == NULL){ 
		fclose(fp); 
		return true; 
	}else{
        fseek(fp, 0, SEEK_END);
        int len = ftell(fp); 
        if (len > 0){
            rewind(fp); 
            return false; 
        }else{ 
		fclose(fp); 
		return true; 
		}
    }
}

void MergeArrayD(Dictionary *data, int min, int mid, int max){
    int len1 = mid-min+1, len2 = max-mid; 
	Dictionary left[len1], right[len2];
    int i;
    for(i = 0; i < len1; i++) left[i] = data[min+i]; 
    for(i = 0; i < len2; i++) right[i] = data[mid+1+i]; 

    int x = 0, y = 0; 
    for(i = min; i <= max; i++){ 
	if(x >= len1){ 
		data[i] = right[y++]; 
		continue; 
	}else if(y >= len2){ 
		data[i] = left[x++]; 
		continue; 
	}
	if(strcmp(left[x].word, right[y].word) < 0) data[i] = left[x++];
    else data[i] = right[y++];
    }
}

void MergeSortD(Dictionary *data, int min, int max){
    if(min >= max) return;
    int mid = ((max-min)/2)+min; 
    MergeSortD(data, min, mid);
    MergeSortD(data, mid+1, max); 
    MergeArrayD(data, min, mid, max); 
}

void AddNewWords(){
    FILE *words = fopen(DictFile, "r"); 
    if(FileError(words)){ 
	printf("\nError : File Does Not Exist or is Empty\n"); 
	return; 
	}
	
    int WordCount, NewWordCount; 
	fscanf(words, "%d%d", &WordCount, &NewWordCount);
    int TotalCount = WordCount + NewWordCount; 
    Dictionary Dict[TotalCount]; 
    
    int i;
    for(i = 0; i < WordCount; i++){ 
	int temp; 
	fscanf(words, "%s%s%d", Dict[i].word, Dict[i].clue, &temp);
	Dict[i].unlocked = temp; 
	}
    for(i = WordCount; i < TotalCount; i++){ 
	fscanf(words, "%s", Dict[i].word); 
	fscanf(words, "%s", Dict[i].clue); 
	Dict[i].unlocked = false; 
	}
    fclose(words); 
	words = fopen(DictFile, "w"); 

    fprintf(words, "%d\n", TotalCount); 
    for(i = 0; i < TotalCount; i++) 
        fprintf(words, "%s %s %d\n", Dict[i].word, Dict[i].clue, Dict[i].unlocked);
    fclose(words); 
}

void MergeArrayL(Leaderboard *data, int min, int mid, int max, bool score){
    int len1 = mid-min+1, len2 = max-mid;
    Leaderboard left[len1], right[len2];
    int i;
    for(i = 0; i < len1; i++) left[i] = data[min+i];
    for(i = 0; i < len2; i++) right[i] = data[mid+1+i];

    int x = 0, y = 0; 
    for(i = min; i <= max; i++){
       if(x >= len1){ 
			data[i] = right[y++]; 
			continue; 
		}else if(y >= len2){ 
			data[i] = left[x++]; 
			continue; 
		}
        if(score){
          	if(left[x].score > right[y].score) data[i] = left[x++];
            else if(left[x].score == right[y].score && strcmp(left[x].name, right[y].name) < 0) data[i] = left[x++];
            else data[i] = right[y++];
        }else{
			if(strcmp(left[x].name, right[y].name) < 0) data[i] = left[x++];
            else data[i] = right[y++];
        }
    }
}

void MergeSortL(Leaderboard *data, int min, int max, bool score){
    if(min >= max) return; 
    int mid = ((max-min)/2)+min;
    MergeSortL(data, min, mid, score);
    MergeSortL(data, mid+1, max, score); 
	MergeArrayL(data, min, mid, max, score); 
}

int BinerSearch(Leaderboard *data, int min, int max){
    while(min <= max){
        int mid = ((max-min)/2)+min;
        if(strcmp(Name, data[mid].name) == 0) return mid;
        else if(strcmp(Name, data[mid].name) < 0) max = mid-1;
        else if(strcmp(Name, data[mid].name) > 0) min = mid+1;
    }
    return -1; 
}

