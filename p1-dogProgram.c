#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <termio.h>
#include <sys/ioctl.h>


#define SIZETABLE 15803
#define STDINFD  0
#define SIZEDOGTYPE sizeof(dogType)

// structs

typedef struct dogType dogType;

struct dogType {
    int nextPosition;

    char name[32];
    char type[32];
    int age;
    char race[16];
    int height;
    float weight;
    char sex;
    char clinicHistory[100]; 
};

//global variables

char nameDataBase[] = "dataBase.dat";
char nameBinFile[] = "dataDogs.dat";
char nameBinTable[] = "hashTable.dat";
char nameAuxBinFile[] = "auxDataDogs.dat";
char nameAuxBinTable[] = "auxHashTable.dat";
char nameClinicHistory[] = "clinicHistory.txt";

//functions

int menu();
void inkey();
void goMenu();
long int getBytesFile(char nameFile[]);
void checkFiles();
long int getCodeName(char name[]);
void* createAnimal();
int functionHash(long codeName);
void addInFile(void* newDog, int headList, int signal);
void addInTable(void* newDog);
void clinicHistory(void* animal, int number);
void consultAnimal(int numAnimal);
void modifyStruct(int posInFile);
void deleteBinTable(int posInTable, int newHeadList, int posDelete);
void findAnimalToDelete(int position);
int searchAnimal(char petName[]);
int consultAnimalSearched(char petName[], int numberToConsult);
void generateRandomAnimal(int numAnimals);


int menu()
{
    char option[20];

    printf(" 1. Ingresar Registro\n 2. Ver Registro\n"
        " 3. Borrar registro\n 4. Buscar Registro \n 5. generar animales aleatorios.\n 6. Salir\n");

    scanf("%s", option);
    system("clear");
    if((strcmp(option,"1")==0) || (strcmp(option,"2")==0) || (strcmp(option,"3")==0) || (strcmp(option,"4")==0) || (strcmp(option,"5")==0) || (strcmp(option,"6")==0)){
       return atoi(option); 
    }else{
        return 0;
    }
}

void inkey()
{
  char c[20];
  int d;
  struct termio param_ant, params;

  int q = ioctl(STDINFD,TCGETA,&param_ant);
  params = param_ant;
  params.c_lflag &= ~(ICANON|ECHO);
  params.c_cc[20] = 20;

  ioctl(STDINFD,TCSETA,&params);

  fflush(stdin); fflush(stderr); fflush(stdout);
  read(STDINFD,c,20);

  ioctl(STDINFD,TCSETA,&param_ant);
}

void goMenu()
{
    printf("Presione cualquier tecla para dirigirse al menu.\n");
    inkey();
    system("clear");
}

long int getBytesFile(char nameFile[])
{
    FILE* fl;
    long int bytes;

    fl = fopen(nameFile, "rb");

    if (fl == NULL)
    {
        perror("Error fopen");
        exit(-1);
    }

    fseek(fl, 0, SEEK_END);
    bytes = ftell(fl);
    fclose(fl);

    return bytes;
}

void checkFiles()
{
    FILE* fl;

    int number     = 0,
        size     = 0,
        q       = 0,
        r       = 0,
        i       = 0;

    fl = fopen(nameBinFile, "rb");

    system("clear");

    if (fl != NULL)
    {
           fclose(fl);
        size = getBytesFile(nameBinFile) / SIZEDOGTYPE;
        if (size > 0)
        {
            printf("Hay %i animales registrados, ¿desea restaurarlos?\n 1. Si. \n 2. No.\n", size);
            scanf(" %i", &number);
            if (number == 2)
            {
                fl = fopen(nameBinTable, "wb"); 
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }

                r = -1;
                for (i = 0; i < SIZETABLE; ++i)
                {
                    q = fwrite(&r, sizeof(r), 1, fl);
                    if (q < 0)
                    {
                        perror("Error fwrite in function checkFiles.");
                        exit(-1);
                    }
                }

                fclose(fl);

                fl = fopen(nameBinFile, "wb"); 
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }
                fclose(fl);

                fl = fopen(nameClinicHistory, "wb");
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }
                fclose(fl);
            }
        }
    }else
    {
        fl = fopen(nameBinTable, "wb"); 
        if (fl == NULL)
        {
            perror("Error fopen");
            exit(-1);
        }

        r = -1;
        for (i = 0; i < SIZETABLE; ++i)
        {
            q = fwrite(&r, sizeof(r), 1, fl);
            if (q < 0)
            {
                perror("Error fwrite in function checkFiles.");
                exit(-1);
            }
        }

        fclose(fl);

        fl = fopen(nameBinFile, "wb"); 
        if (fl == NULL)
        {
            perror("Error fopen");
            exit(-1);
        }
        fclose(fl);
    }
    system("clear");
}

long int getCodeName(char name[])
{
    long d      = 0,
         ascii  = 0;
    int j       = 0;

    for (j = 0; j < strlen(name); j++) {

        ascii = name[j];
        d = d * 21 + ascii * 5;     
    }
    return ((d >= 0) ? d : -d);
}

void* createAnimal() 
{
    dogType *aux;

    long codeName = 0;
    char TempName[32];
    char tempClinicH[100] = "";

    aux = malloc(sizeof (dogType));

    if (aux == NULL) {
        perror("Error en malloc.");
        exit(-1);
    }

    printf("Digite el nombre: \n");
     scanf(" %31[^\n]", TempName);

    for (int i = 0; TempName[i] != '\0'; ++i) {
        TempName[i] = tolower(TempName[i]);
    }
    strcpy(aux->name, TempName);

    printf("Digite el tipo: \n");
    scanf(" %31[^\n]", aux -> type);

    printf("Digite la edad: \n");
    scanf(" %i", &aux -> age);

    printf("Digite la raza: \n");
    scanf(" %31[^\n]", aux -> race);

    printf("Digite la estatura: \n");
    scanf(" %i", &aux -> height);

    printf("Digite el peso: \n");
    scanf(" %f", &aux -> weight);

    printf("Digite el genero: \n");
    scanf(" %c", &aux -> sex);

    strcpy(aux -> clinicHistory, tempClinicH);

    aux -> nextPosition = -1;

    return aux;
}

int functionHash(long codeName)
{
    return codeName % SIZETABLE;
}

void addInFile(void* newDog, int headList, int signal)
{
    FILE* fl;
    int numAnimals  = 0,
        nextNode    = 0,
        endList     = 0,
        q           = 0;

    dogType *tempDog;
    dogType *auxDog;

    tempDog = (dogType*)newDog;    
    numAnimals = getBytesFile(nameBinFile) / SIZEDOGTYPE;
    endList = -1;

    if (signal == 0)
    {
        auxDog = malloc(SIZEDOGTYPE);
        if (auxDog == NULL)
        {
            perror("malloc in function addInFile");
            exit(-1);
        }

        fl = fopen(nameBinFile, "rb+");
        if (fl == NULL) {
            perror("Error fopen 2 in function addInFile");
            exit(-1);
        }

        nextNode = headList;
        do
        {
            q = fseek(fl, SIZEDOGTYPE * nextNode, SEEK_SET);
            if (q < 0)
            {
                perror("Error fseek 1 in function addInFile");
                exit(-1);
            }

            q = fread(auxDog, SIZEDOGTYPE, 1, fl);
            if (q < 0)
            {
                perror("Error fread 2 in function addInFile");
                exit(-1);
            }
            endList = auxDog->nextPosition;

            if (endList < 0)
            {
                auxDog->nextPosition = numAnimals;

                q = fseek(fl, SIZEDOGTYPE * nextNode, SEEK_SET);
                if (q < 0)
                {
                    perror("Error fseek 4 in function addInFile");
                    exit(-1);
                }

                q = fwrite(auxDog, SIZEDOGTYPE, 1, fl);
                if (q < 0)
                {
                    perror("Error fread 3 in function addInFile");
                    exit(-1);
                }
            }else
            {
                nextNode = endList;
            }
        }while(endList > 0);

        fclose(fl);
        free(auxDog);
    }

    fl = fopen(nameBinFile, "ab");
    if (fl == NULL) {
        perror("Error fopen 3 in function addInFile");
        exit(-1);
    }

    q = fwrite(tempDog, SIZEDOGTYPE, 1, fl); 
    if (q == 0) {
        perror("Error fwrite 2 in function addInFile");
        exit(-1);
    }

    fclose(fl);
    free(tempDog);
}

void addInTable(void* newDog)
{
    FILE* fl;

    int posInTable  = 0,
        signal      = 0, 
        headList    = 0,
        nextNode    = 0,
        endList     = 0,
        numAnimals  = 0,
        q           = 0;
    long codeName   = 0;
    char tempName[32] = "";

    dogType *tempDog;

    tempDog = (dogType*) newDog;

    strcpy(tempName, tempDog->name);
    codeName = getCodeName(tempName);
    posInTable = functionHash(codeName);

    numAnimals = getBytesFile(nameBinFile) / SIZEDOGTYPE;

    fl = fopen(nameBinTable, "rb+");
    if (fl == NULL) {
        perror("Error fopen 1 in function addInFile");
        exit(-1);
    }

    q = fseek(fl, sizeof(int) * posInTable, SEEK_SET);
    if (q < 0)
    {
        perror("Error fseek 1 in function addInTable");
        exit(-1);
    }

    q = fread(&headList, sizeof(headList), 1, fl);
    if (q < 0)
    {
        perror("Error fread 1 in function addInTable");
        exit(-1);
    }

    if (headList < 0)
    {
        headList = numAnimals;

        q = fseek(fl, sizeof(headList) * posInTable, SEEK_SET);
        if (q < 0)
        {
            perror("Error fseek 2 in function addInTable");
            exit(-1);
        }

        q = fwrite(&headList, sizeof(headList), 1, fl);
        if (q < 0)
        {
            perror("Error fwrite 1 in function addInTable");
            exit(-1);
        }
        signal = 1; //there is no head
    }

    fclose(fl);
    addInFile(tempDog, headList, signal);
}

void clinicHistory(void* animal, int number)
{    
    FILE* fl;
    dogType* auxAnimal;

    auxAnimal = (dogType*) animal;

    fl = fopen(nameClinicHistory, "w+");
    if (fl == NULL) {
        perror("Error fopen in function clinicHistory");
        exit(-1);
    }

    fprintf(fl, "Historia clinica del animal cuyo registro es el: %d\n", (number  + 1));
    fprintf(fl, "Nombre: %s\n", auxAnimal->name);
    fprintf(fl, "Tipo: %s\n", auxAnimal->type);
    fprintf(fl, "Edad: %i\n", auxAnimal->age);
    fprintf(fl, "Raza: %s\n", auxAnimal->race);
    fprintf(fl, "Estatura: %i\n", auxAnimal->height);
    fprintf(fl, "Peso: %.2f\n", auxAnimal->weight);
    fprintf(fl, "Genero: %c\n", auxAnimal->sex);
    fprintf(fl, "Historia Clinica: %s\n", auxAnimal->clinicHistory);
    fclose(fl);
    free( auxAnimal);
}

void consultAnimal(int numAnimal)
{
    FILE* fl;

    int b = 0;

    char command[40] = "xdg-open clinicHistory.txt";

    dogType* tempDog;
 
    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc in function consultAnimal");
        exit(-1);
    }

    fl = fopen(nameBinFile, "rb");
    if (fl == NULL)
    {
        perror("Error fopen in function consultAnimal");
        exit(-1);
    }

    b = fseek(fl, numAnimal * SIZEDOGTYPE, SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek in function consultAnimal");
        exit(-1);
    }

    b = fread(tempDog, SIZEDOGTYPE, 1, fl);
    if (b == 0)
    {
        perror("Error fread in function consultAnimal");
        exit(-1);
    }

       clinicHistory(tempDog, numAnimal);

    b = system(command);
    if (b < 0)
    {
        perror("Error system in function consultAnimal");
        exit(-1);
    }
    free(tempDog);
}

void modifyStruct(int posInFile)
{
    FILE* fl;

    int tempNext    = 0,
        signal      = 0,
        i           = 0,
        r           = 0;
    char line[100]      = "";
    char input[100]     = "";
    char option[100]    = "";
    char separator[2]   = ":";
    char tempCH[100]    = "";
    char tempName[32]   = "";
    char tempRace[32]   = "";
    char tempType[16]   = "";
    char tempSex;
    char* ptr;
    char* ptr2;

    dogType* auxAnimal;
    dogType* tempDog;

    auxAnimal = malloc(SIZEDOGTYPE);
    if (auxAnimal == NULL)
    {
        perror("Error malloc 1 in function modifyStruct");
        exit(-1);
    }
    
    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc 2 in function modifyStruct");
        exit(-1);
    }

    fl = fopen(nameBinFile,"rb");
    if (fl == NULL)
    {
        perror("Error fopen 2 in function modifyStruct");
        exit(-1);
    }

    r = fseek(fl,SIZEDOGTYPE * posInFile,SEEK_SET);
    if (r < 0)
    {
        perror("Error fseek 1 in funcion modifyStruct");
    }

    r = fread(tempDog, SIZEDOGTYPE, 1, fl);
    if (r == 0)
    {
        perror("Error fread ModifyStruct");
        exit(-1);
    }

    strcpy(tempName, tempDog->name);
    strcpy(tempType, tempDog->type);
    strcpy(tempRace, tempDog->race);
    strcpy(&tempSex, &tempDog->sex);
    tempNext = tempDog->nextPosition;
    fclose(fl);
    free(tempDog);

    fl = fopen(nameClinicHistory,"r+");
    if (fl == NULL)
    {
        perror("Error fopen in function modifyStruct");
        exit(-1);
    }

    signal == 0;
    while(!feof(fl))
    {
        ptr=NULL;
        strcpy(line, "");
        strcpy(option,"");
        strcpy(input, "");
        ptr2 = fgets(line, sizeof(line), fl);

        if (signal != 1)
        {
            signal = 0;
            ptr= strtok(line, separator);
            strcpy(option, ptr);
            ptr= strtok(NULL, separator);
            strcpy(input, ptr);
            i = strlen(input) - 1;
            input[i] = '\0';

            strcpy(line, "");
            for (i = 0; i < strlen(input); ++i)
            {   
                line[i] = input[i+1];
            }

            if (strcmp("Nombre", option) == 0)
            {   
                strcpy(auxAnimal->name, line);          
            }else if (strcmp("Tipo", option) == 0)
            {
                strcpy(auxAnimal->type, line);
            }else if (strcmp("Edad", option) == 0)
            {
                auxAnimal->age = atoi(line);         
            }else if (strcmp("Raza", option) == 0)
            {
                strcpy(auxAnimal->race, line);
            }else if (strcmp("Estatura", option) == 0)
            {
                auxAnimal->height = atoi(line);               
            }else if (strcmp("Peso", option) == 0)
            {
                auxAnimal->weight = atof(line);
            }else if (strcmp("Genero", option) == 0)
            {
                strcpy(&auxAnimal->sex, line);
            }else if (strcmp("Historia Clinica", option) == 0)
            {
                strcpy(auxAnimal->clinicHistory, line);
                strcat(auxAnimal->clinicHistory, "\n");
                signal = 1;
            }   
        }else if((ptr2 != NULL) && (signal == 1)){
            strcat(auxAnimal->clinicHistory, line);
        }
    }
    strcpy(tempCH, auxAnimal->clinicHistory);
    strcpy(auxAnimal->clinicHistory, "");
    fclose(fl);

    strcpy(auxAnimal->name, tempName);
    strcpy(auxAnimal->type, tempType);
    strcpy(auxAnimal->race, tempRace);
    strcpy(&auxAnimal->sex, &tempSex);
    auxAnimal->nextPosition = tempNext;

    tempCH[strlen(tempCH) - 1] = '\0';
    strcpy(auxAnimal->clinicHistory, tempCH);
    
    fl = fopen(nameBinFile,"rb+");
    if (fl == NULL)
    {
        perror("Error fopen 2 in function modifyStruct");
        exit(-1);
    }

    r = fseek(fl, SIZEDOGTYPE * posInFile, SEEK_SET);
    if (r < 0)
    {
        perror("Error fseek 2 in funcion modifyStruct");
    }

    r = fwrite(auxAnimal, SIZEDOGTYPE, 1, fl);
    if (r < 0)
    {
        perror("Error fwrite in funcion modifyStruct");
    }

    fclose(fl);
    free(auxAnimal);
}

void deleteBinTable(int posInTable, int newHeadList, int posDelete)
{
    FILE* fl;   
    int tempHeadList    = 0,
        auxHeadList     = 0,
        i               = 0,
        b               = 0;  
    
    fl = fopen(nameBinTable, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function deleteBinTable");
        exit(-1);
    }

    b = fseek(fl, posInTable * sizeof(int), SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function deleteBinTable");
        exit(-1);
    }

    b = fread(&tempHeadList, sizeof(int), 1, fl);
    if (b < 0)
    {
        perror("Error fread 1 in function deleteBinTable");
        exit(-1);
    }

    fclose(fl);

    if (tempHeadList == posDelete)
    {
        fl = fopen(nameBinTable, "rb+");
        if (fl == NULL)
        {
            perror("Error fopen 1 in function deleteBinTable");
            exit(-1);
        }

        for (i = 0; i < SIZETABLE; ++i)
        {
            if (i == posInTable)
            {                
                b = fwrite(&newHeadList, sizeof(int), 1, fl);
                if (b <0)
                {
                    perror("Error write 1 in function deleteBinTable");
                    exit(-1);
                }
            }else
            {
                auxHeadList = 0;
                b = fread(&auxHeadList, sizeof(int), 1, fl);
                if (b < 0)
                {
                    perror("Error fread 2 in function deleteBinTable");
                    exit(-1);
                }

                if (auxHeadList > posDelete)
                {
                    b = fseek(fl, i * sizeof(int), SEEK_SET);
                    if (b < 0)
                    {
                        perror("Error fseek 2 in function deleteBinTable");
                        exit(-1);
                    }
                    auxHeadList --;

                    b = fwrite(&auxHeadList, sizeof(int), 1, fl);
                    if (b <0)
                    {
                        perror("Error write 2 in function deleteBinTable");
                        exit(-1);
                    }

                }
            }
        }
    }

    fclose(fl);
}

void findAnimalToDelete(int positionDelete)
{
    FILE* fl;
    FILE* auxfl;

    int tempNextPosition    = 0,
        newNextPosition     = 0,
        numberAnimals       = 0,
        newHeadList         = 0,
        posInTable          = 0,
        count               = 0,
        b                   = 0;
    long codeName           = 0;
    char tempName[32] = "";
    char command[40] = "rm -r dataDogs.dat";
    dogType* animalTemp;



    numberAnimals = getBytesFile(nameBinFile) / SIZEDOGTYPE;

    fl = fopen(nameBinFile, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function findAnimalToDelete");
        exit(-1);
    }    

    auxfl = fopen(nameAuxBinFile,  "wb+");
    if (auxfl == NULL)
    {
        perror("Error fopen 2 in function findAnimalToDelete");
        exit(-1);
    }

    animalTemp = malloc(SIZEDOGTYPE);
    if(animalTemp == NULL){
        perror("Error malloc in function findAnimalToDelete");
        exit(-1);
    }

    b = fseek(fl, positionDelete * SIZEDOGTYPE, SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function findAnimalToDelete");
        exit(-1);
    }

    b = fread(animalTemp, SIZEDOGTYPE, 1, fl);
    if (b == 0)
    {
        perror("Error fread 1 in function findAnimalToDelete");
        exit(-1);
    }

    strcpy(tempName, animalTemp->name);
    codeName = getCodeName(tempName);
    posInTable = functionHash(codeName);

    newHeadList = animalTemp->nextPosition;

    if (newHeadList > 0)
    {
        newHeadList --;
    }

    deleteBinTable(posInTable, newHeadList, positionDelete);

    b = fseek(fl, 0, SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 2 in function findAnimalToDelete");
        exit(-1);
    }

    while(count < numberAnimals)
    {
        if (count == positionDelete)
        {
            b = fread(animalTemp, SIZEDOGTYPE, 1, fl);
            if (b == 0)
            {
                perror("Error fread 2 in function findAnimalToDelete");
                exit(-1);
            }
            count ++;
        }

        if (count < numberAnimals)
        {
            b = fread(animalTemp, SIZEDOGTYPE, 1, fl);
            if (b == 0)
            {
                perror("Error fread 3 in function findAnimalToDelete");
                exit(-1);
            }

            tempNextPosition = animalTemp->nextPosition;
            newNextPosition = tempNextPosition;

            if (tempNextPosition >= positionDelete)
            {
                if (tempNextPosition == positionDelete)
                {
                    newNextPosition = newHeadList;
                }else
                {
                    newNextPosition = tempNextPosition - 1; 
                }
            }
            
            animalTemp->nextPosition = newNextPosition;

            b = fwrite(animalTemp, SIZEDOGTYPE, 1, auxfl); 
            if (b == 0)
            {
                perror("Error fwrite in function findAnimalToDelete");
                exit(-1);
            }   
            count ++;
        }       
    }

    b = system(command);
    if (b < 0){
        perror("Error system in function findAnimalToDelete");
        exit(-1);
    }
    
    if (rename(nameAuxBinFile, nameBinFile) != 0)
    {
        perror("Error rename in function findAnimalToDelete");
        exit(-1);
    }
    fclose(fl);
    fclose(auxfl);
    free(animalTemp);
}

int searchAnimal(char petName[])
{
    FILE* fl;
    long codeName   = 0;
    int posInTable  = 0,
        numAnimals  = 0,
        posInFile   = 0,
        nextPos     = 0,
        b           = 0;
    dogType* tempDog;

    codeName = getCodeName(petName);
    posInTable = functionHash(codeName);

    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc in function searchAnimal");
        exit(-1);
    }

    fl = fopen(nameBinTable, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function searchAnimal");
        exit(-1);
    }

    b = fseek(fl, posInTable * sizeof(int), SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function searchAnimal");
        exit(-1);
    }

    b = fread(&posInFile, sizeof(int), 1, fl);
    if (b < 0)
    {
        perror("Error fread 1 in function searchAnimal");
        exit(-1);
    }
    fclose(fl);

    if (posInFile >= 0)
    {
        fl = fopen(nameBinFile, "rb");
        if (fl == NULL)
        {
            perror("Error fopen 2 in function searchAnimal");
            exit(-1);
        } 

        nextPos = posInFile;

        while(nextPos >= 0)
        {
            b = fseek(fl, nextPos * SIZEDOGTYPE, SEEK_SET);
            if (b < 0)
            {
                perror("Error fseek 1 in function searchAnimal");
                exit(-1);
            }

            b = fread(tempDog, SIZEDOGTYPE, 1, fl);
            if (b < 0)
            {
                perror("Error fread in function searchAnimal");
                exit(-1);
            }

            if (strcmp(tempDog->name, petName) == 0)
            {
                numAnimals ++;
            }
            nextPos = tempDog->nextPosition;
        }
        fclose(fl);
    }

    free(tempDog);
    return numAnimals;
}

int consultAnimalSearched(char petName[], int numberToConsult)
{
    FILE* fl;

    long codeName   = 0;
    int posInFile   = 0,
        posInTable  = 0,
        nextPos     = 0,
        count         = 0,
        b           = 0;

    dogType* tempDog;

    codeName = getCodeName(petName);
    posInTable = functionHash(codeName);

    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc in function consultAnimalSearched");
        exit(-1);
    }

    fl = fopen(nameBinTable, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function consultAnimalSearched");
        exit(-1);
    }

    b = fseek(fl, posInTable * sizeof(int), SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function consultAnimalSearched");
        exit(-1);
    }

    b = fread(&posInFile, sizeof(int), 1, fl);
    if (b < 0)
    {
        perror("Error fread 1 in function consultAnimalSearched");
        exit(-1);
    }
    fclose(fl);

    fl = fopen(nameBinFile, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 2 in function consultAnimalSearched");
        exit(-1);
    } 

    nextPos = posInFile;

    while(count < numberToConsult)
    {
        b = fseek(fl, nextPos * SIZEDOGTYPE, SEEK_SET);
        if (b < 0)
        {
            perror("Error fseek 2 in function consultAnimalSearched");
            exit(-1);
        }

        b = fread(tempDog, SIZEDOGTYPE, 1, fl);
        if (b < 0)
        {
            perror("Error fread 2 in function consultAnimalSearched");
            exit(-1);
        }

        if (strcmp(tempDog->name, petName) == 0)
        {
            count ++;
        }

        posInFile = nextPos;
        nextPos = tempDog->nextPosition;
    }
    fclose(fl);

    free(tempDog);
    consultAnimal(posInFile);
    return posInFile;
}

void generateRandomAnimal(int numAnimals)
{
    FILE* fl;

    int count           = 0,
        rdmName         = 0,
        rdmType         = 0,
        rdmAge          = 0,
        rdmRace         = 0,
        rdmHeight       = 0,
        rdmSex          = 0,
        i               = 0,    
        r               = 0;
    float rdmWeight     = 0;
    char buffer[32];
    char tempClinicH[100] = "";
    char character = 'M';

    dogType* tempDog;

    srand(time(NULL));
    srand48(time(NULL));


    for (i = 0; i < numAnimals; ++i)
    {
        tempDog = malloc(SIZEDOGTYPE);
        if (tempDog == NULL)
        {
            perror("Error malloc in function generateRandomAnimal");
            exit(-1);
        }

        strcpy(tempClinicH, "");

        fl = fopen(nameDataBase, "rb");
        if (fl == NULL)
        {
            perror("Error fopen gnerateRandomAnimal");
            exit(-1);
        }

        rdmName = rand()%(21021 + 1);
        rdmType = rand()%(21230 - 21022 + 1) + 21022;
        rdmAge = rand()%(20 - 1 + 1) + 1;
        rdmRace = rand()%(21230 - 21022 + 1) + 21022;
        rdmHeight = rand()%(100 -10 + 1) + 10;
        rdmWeight = drand48()*(30.0 + 1.0 - 1.0) + 1.0 ;
        rdmSex = rand()%(1 + 1);

        r = fseek(fl, rdmName*sizeof(buffer), SEEK_SET);
        if (r < 0)
        {
            perror("Error fseek 1 in function generateRandomAnimal");
            exit(-1);
        }

        r = fread(buffer, sizeof(char), sizeof(buffer), fl);
        if (r == 0)
        {
            perror("Error fread generateRandom rdmName");
            exit(-1);
        }
        for (int i = 0; buffer[i] != '\0'; ++i)
        {
            buffer[i] = tolower(buffer[i]);
        }
        strcpy(tempDog -> name, buffer);

        r = fseek(fl, rdmType*sizeof(buffer), SEEK_SET);
        if (r < 0)
        {
            perror("Error fseek 1 in function generateRandomAnimal");
            exit(-1);
        }

        r = fread(buffer, sizeof(char), sizeof(buffer), fl);
        if (r == 0)
        {
            perror("Error fread generateRandom rdmType");
            exit(-1);
        }
        strcpy(tempDog -> type, buffer);

        r = fseek(fl, rdmRace*sizeof(buffer), SEEK_SET);
        if (r < 0)
        {
            perror("Error fseek 1 in function generateRandomAnimal");
            exit(-1);
        }

        r = fread(buffer, sizeof(char), 16, fl);
        if (r == 0)
        {
            perror("Error fread generateRandom rdmRace");
            exit(-1);
        }
        strcpy(tempDog -> race, buffer);

        if (rdmSex == 0)
        {
            character = 'H';
        }

        strcpy(tempDog -> clinicHistory, tempClinicH);
        tempDog -> nextPosition = -1;

        tempDog -> sex = character;    
        tempDog -> age = rdmAge;
        tempDog -> height = rdmHeight;
        tempDog -> weight = rdmWeight;

        fclose(fl); 
        addInTable(tempDog);  
        //borrar
        printf("%i\n", i+1);
    }    
    printf("\n");
}

int main()
{
    checkFiles();

    char nameDog[32];
    int option          = 0,
        positionInFile  = 0,
        sizeFileDogs    = 0,
        sizeDogType     = 0,
        select             = 0,
        numAnimal       = 0;

    dogType *newDog;
    
    do{
        option = menu();
        sizeFileDogs = getBytesFile(nameBinFile)/ SIZEDOGTYPE;
        
        switch(option)
        {   
            case 0:
            
                break;
            
            case 1:
                
                newDog = (dogType *) createAnimal();
                addInTable(newDog);

                printf("El animal ha sido registrado correctamente.\n");

                goMenu();               
                break;
                
            case 2:
                
                if (sizeFileDogs == 0)
                {
                    printf("El sistema no contiene animales registrados.\n");  
                    goMenu(); 
                }else
                {
                    if (sizeFileDogs == 1)
                    {
                        printf("En el sistema hay actualmente un animal registrado.\n");
                    }else
                    {    
                        printf("En el sistema actualmente hay %i animales registrados.\n", sizeFileDogs);
                    }
                    
                    printf("Por favor ingrese el número del animal que desea consutar:\n");
                    scanf(" %i", &numAnimal);

                    if (numAnimal > sizeFileDogs || numAnimal < 0)
                    {
                        printf("El numero no es vaido.\n");
                        goMenu();
                    }else
                    {
                        numAnimal --;
                        consultAnimal(numAnimal);
                        sleep(4);
                        goMenu();
                           modifyStruct(numAnimal);
                    }
                }
                            
                break;
            
            case 3:
                
                if (sizeFileDogs == 0)
                {
                    printf("El sistema no contiene animales registrados.\n");
                }else
                {
                    if (sizeFileDogs == 1)
                    {
                        printf("En el sistema hay actualmente un animal registrado.\n");
                    }else
                    {    
                        printf("En el sistema actualmente hay %i animales registrados.\n", sizeFileDogs);
                    }
                    
                    printf("Por favor ingrese el número del animal que desea borrar:\n");
                    scanf(" %i", &numAnimal);

                    if (numAnimal > sizeFileDogs || numAnimal < 0)
                    {
                        printf("El numero no es vaido.\n");
                    }else
                    {
                        numAnimal --;
                        findAnimalToDelete(numAnimal);               
                    }
                    printf("Se ha eliminado correctamente\n");
                }

                goMenu();
                
                break;
            
            case 4:
                
                if (sizeFileDogs == 0)
                {
                    printf("El sistema no contiene animales registrados.\n");
                }else
                {
                    printf("Ingrese el nombre del animal que desea consultar:\n");
                    scanf(" %31[^\n]", nameDog);

                    for (int i = 0; nameDog[i] != '\0'; ++i) {
                        nameDog[i] = tolower(nameDog[i]);
                    }

                    select = 0;
                    positionInFile = 0;
                    numAnimal = searchAnimal(nameDog);

                    switch(numAnimal)
                    {
                        case 0:
                            printf("No hay animales registrados con ese nombre.\n");
                            goMenu();
                            break;

                        case 1:
                            printf("Hay un animal registrado con ese nombre\n");
                            printf("¿Desea consultar la historia clinica de %s?\n 1. si\n 2. no\n", nameDog);

                            scanf(" %d", &select);
                            if (select == 1)
                            {
                                positionInFile = consultAnimalSearched(nameDog, numAnimal);
                                sleep(4);
                                goMenu();
                                modifyStruct(positionInFile);  
                            }else
                            {
                                goMenu();
                            }
                                               
                            break;

                        default:
                            printf("Hay %i animales registrados con el nombre de %s\n", numAnimal, nameDog);
                            printf("¿Desea consultar la historia clinica de alguno de ellos?\n 1. si\n 2. no\n");

                            scanf(" %d", &select);
                            if (select == 1)
                            {
                                printf("Por favor ingrese el numero de registro a revisar conforme al nombre ingresado\n");
                                scanf(" %d", &select);
                                
                                if ((select < 1) || (select > numAnimal))
                                {
                                    printf("El numero ingresado no es valido.\n");
                                }else
                                {
                                    positionInFile = consultAnimalSearched(nameDog, select);
                                    sleep(4);
                                    goMenu();
                                    modifyStruct(positionInFile);                                    
                                }
                            }else
                            {
                                goMenu();
                            }
                            
                            break;
                    }
                }                            
                break;
                
            case 5:
                printf("ingrese la cantidad de animales que desea generar:\n");
                
                scanf(" %i", &numAnimal);
                if (numAnimal < 0 || numAnimal > 10000000)
                {
                    printf("el numero no es valido\n");
                }else
                {
                    generateRandomAnimal(numAnimal);

                    printf("Los animales han sido registrados correctamente\n");
                }
                goMenu();
                    
                break;
        }
        
    }while(option != 6);   

    return 0;
}