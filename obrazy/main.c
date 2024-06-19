#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

typedef struct str1
{
    char* nazwa; 
    char standard[3];
    int X;
    int Y;
    int max;
    int** tab;
}str1;

void fre(str1* obrazy, int ilosc_obrazow)
{
    int e = ilosc_obrazow - 1;
    int j;
    while (e >= 0)
    {
        free(obrazy[e].nazwa);
        j = obrazy[e].Y - 1;
        for (j; j >= 0; j--)
        {
            free(obrazy[e].tab[j]);
        }
        free(obrazy[e].tab);
        e--;
    }
    printf("zwolnione poprawnie");
    printf("\n");
}

int menu()
{
    printf("Program obsluguje pliki PGM\n");
    printf("Co chcesz zrobic?\n");
    printf("1. Dodaj obraz do bazy danych\n");
    printf("2. Usun obraz z bazy danych\n");
    printf("3. Wyswietl baze obrazow\n");
    printf("4. Obrabiaj obraz\n");
    printf("5. Wyjdz z programu.\n");
    int o = 0;
    do
    {
        printf("Podaj 1, 2, 3, 4 lub 5: \n");
        o = _getch();
        o = toupper(o);
    } while (!(o == '2' || o == '1' || o == '3' || o == '4' || o == '5'));
    printf("\n");
    return o;
}

int mini_menu()
{
    printf("> MENU OBROBKI OBRAZOW <\n");
    printf("Co chcesz zrobic?\n");
    printf("1. Wyswietl obraz w formacie PGM.\n");
    printf("2. Obroc o '90*k' stopni.\n");
    printf("3. Histogram z zapisem do pliku .CSV.\n");
    printf("4. Progowanie z dowolnym progiem\n");
    printf("5. Dodawanie szumu typu pieprz i sol\n");
    printf("6. Filtr medianowy\n");
    printf("7. Zapisz obraz do pliku.\n");
    printf("8. Wyjdz z obrobki obrazow\n");
    int o = 0;
    do
    {
        printf("Wybierz (1-8): \n");
        o = _getch();
        o = toupper(o);
    } while (o<'0' || o>'8');
    printf("\n");
    int w = (int)(o - 48);
    return w;
}

void drukuj_dopliku(FILE* plik, str1* obrazy, int numer)
{
    fprintf(plik, "%s\n", obrazy[numer].standard);
    fprintf(plik, "%d ", obrazy[numer].X);
    fprintf(plik, "%d\n", obrazy[numer].Y);
    fprintf(plik, "%d\n", obrazy[numer].max);
    for (int i = 0; i < obrazy[numer].Y; i++)
    {
        for (int j = 0; j < obrazy[numer].X; j++)
        {
            fprintf(plik, "%d ", obrazy[numer].tab[i][j]);
        }
        fprintf(plik, "\n");
    }
}

void drukuj_opcje(str1* obrazy, int zmienna)
{
        printf("%d. %s\n", zmienna + 1, obrazy[zmienna].nazwa);
}

void drukuj(str1* obrazy, int numer)
{
    printf("%s\n", obrazy[numer].standard);
    printf("%d ", obrazy[numer].X);
    printf("%d\n", obrazy[numer].Y);
    printf("%d\n", obrazy[numer].max);
    for (int i = 0; i < obrazy[numer].Y; i++)
    {
        for (int j = 0; j < obrazy[numer].X; j++)
        {
            printf("%d ", obrazy[numer].tab[i][j]);
        }
        printf("\n");
    }
    printf("--------------------------\n");
}

str1 czytanie_danych(str1* obrazy, int numer, int ilosc_obrazow)
{
    char tytol[30];
    str1 pgm = { 0, 0, 0, 0 };
    errno_t a = 0;
    FILE* plik;
    int p = 0;
    f1:
    do
    {
        printf("-> Uwaga! Program obsluguje tylko pliki znajdujace sie w folderze utworzonym program, o tej samej nazwie!\n");
        printf("-> Uwaga! Folder ten znajdowac sie musi znajdowac w folderze w ktorym jest plik main tego programu!\n");
        printf("-> Uwaga! Program obsluguje tylko pliki o standardzie ASCI P2.\n");
        if (a != 0) printf("Nie znalazlem pliku o takiej nawie. Podaj inna nawe pliku: \n");
        else if (a == 0) printf("Podaj nazwe pliku ktory chcesz pobrac.\n");
        else if (a == 3) printf("Plik jest w formacie P5. Program obslufuje tylko format P2. Podaj inna nazwe:\n");
        p = 0;
        while (p == 0)
        {
            fflush(stdin);
            p = scanf_s("%s", &tytol, 30);
        }
        if (p == NULL)
        {
            printf("Blad!");
        }
        a = fopen_s(&plik, tytol, "r");
        system("cls");
    } while (a != 0);

    if (a == NULL)
    {
        printf("Udalo sie\n");
    }
    else
    {
        printf("Nie moge otworzyc pliku!\n");
        a = 3;
        goto f1;
    }
    int wielkosc = 1, dlugosc = 0;
    dlugosc = 0;
    do
    {
        wielkosc = tytol[dlugosc];
        dlugosc++;
    } while (wielkosc != 0);
    
    pgm.nazwa = malloc(dlugosc * sizeof(char));
    if (pgm.nazwa == NULL)
    {
        fre(obrazy, ilosc_obrazow);
        exit(1);
    }
    strcpy_s(pgm.nazwa, dlugosc, tytol);
    
    //allokacja obraz.tytol dla wielkosci sizeof(nazwa) nastepnie dopisanie wartosci
    do
    {
        fgets(pgm.standard, sizeof(pgm.standard), plik);
    } while (!(strcmp(pgm.standard, "P2") != 0 || strcmp(pgm.standard, "P5") != 0));

    if (strcmp(pgm.standard, "P2") != 0)
    {
        printf("Plik jest w binarnym standardzie zapisu P5, program obsluguje tylko format ASCII P2!");
        a = 3;
        goto f1;
    }
    printf("%s\n", pgm.standard);
    int wartosc_pobrana;
    for (int i = 1; i < 3; i++)
    {
        switch (i)
        {
        case 1:
        {
            do
            {
                wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.X);
                if (wartosc_pobrana == 0)
                {
                    while (fgetc(plik) != '\n');
                    wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.X);
                }
            } while (wartosc_pobrana == 0);

            do
            {
                wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.Y);
                if (wartosc_pobrana == 0)
                {
                    while (fgetc(plik) != '\n');
                    wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.Y);
                }
            } while (wartosc_pobrana == 0);

            printf("%d   %d\n", pgm.X, pgm.Y);
            break;
        }
        case 2:
        {
            do
            {
                wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.max);
            } while (wartosc_pobrana == 0);
            printf("%d\n", pgm.max);
            break;
        }
        }

    }
    //DEKLARACJA TABLICY DWUWYMIAROWEJ
    pgm.tab = malloc(pgm.Y * sizeof(int*));
    if (pgm.tab == NULL) goto f2;
    for (int i = 0; i < pgm.Y; i++)
    {
        pgm.tab[i] = malloc(pgm.X * sizeof(int*));
        if (pgm.tab[i] == NULL)
        {
            printf("lipa\n");
            for (int z = i; z >= 0; z--)
            {
                free(pgm.tab[z]);
            }
            free(pgm.tab);
            exit(1);
        }
    }
    obrazy[numer].nazwa = malloc(dlugosc * sizeof(char));
    if (pgm.nazwa == NULL)
    {
        fre(obrazy, ilosc_obrazow);
        exit(1);
    }
    obrazy[numer].tab = malloc(pgm.Y * sizeof(int*));
    if (obrazy[numer].tab == NULL) 
    {
        fre(obrazy, ilosc_obrazow);
        goto f2;
    }
    for (int i = 0; i < pgm.Y; i++)
    {
        obrazy[numer].tab[i] = malloc(pgm.X * sizeof(int*));
        if (obrazy[numer].tab == NULL)
        {
            fre(obrazy, ilosc_obrazow);
            goto f2;
        }
    }

    for (long int i = 0; i < pgm.Y; i++)
    {
        for (long int j = 0; j < pgm.X; j++)
        {
            int point = ftell(plik);
            wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.tab[i][j]);
            while(wartosc_pobrana == 0)
            { 
                int non = 0;
                do
                {
                    char polo[2] = "0";
                    fgets(polo, sizeof(polo), plik);;
                    non = 0;
                    if (strcmp(polo, "\n")) non = 1;
                } while (non == 1);
                wartosc_pobrana = fscanf_s(plik, "%zd", &pgm.tab[i][j]);
            }
        }
    }
    f2:
    if (plik) a = fclose(plik);
    system("cls");
    printf("Plik %s\n",tytol);
    printf("--------------------------\n");
    return pgm;
}

int bubblesort(int table[], int size)
{
    int i, j, temp;
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - 1 - i; j++)
        {
            if (table[j] > table[j + 1])
            {
                temp = table[j + 1];
                table[j + 1] = table[j];
                table[j] = temp;
            }
        }
    }
    return table[4];
}

void zapisywanie_danych(str1* obrazy, int numer)
{
    char nazwa[30];
    errno_t a = 0;
    FILE* plik;
    int p = 0;
        do //istnienie pliku
        {
            do //nazwa
            {
                printf("--------------------------\n");
                printf("-> Podaj nazwe nowego pliku. Nazwa nie moze przkraczac 30 znakow.\n");
                printf("-> Dodaj na koncu '.pgm'.\n");
                if (a == 1) printf("UWAGA! Nie moge utworzyc takiego pliku. Podaj inna nawe pliku: \n");
                while (p != 1)
                {
                    p = 0;
                    p = scanf_s("%s", &nazwa, 30);
                }
                printf("Czy chcesz nazwac swoj plik: %s\n1. TAK\n2. NIE\n", nazwa);
                int o = 0;
                do
                {
                    printf("Podaj 1 lub 2: \n");
                    o = _getch();
                    o = toupper(o);
                } while (!(o == '2' || o == '1'));
                if (o == '2') p = 0;
                else p = 1;
                printf("\n");
            } while (p != 1);
            a = fopen_s(&plik, nazwa, "r");
            if (a == 0)
            {
                a = 1;
                fclose(plik);
            }
            else if (a != 0)
            {
                a = fopen_s(&plik, nazwa, "w");
            }
        } while (a == 1);
        drukuj_dopliku(plik, obrazy, numer);
        if (plik) a = fclose(plik);
}

int srednia(str1* obrazy, int numer)
{
    int suma = 0;
    float srednia;
    for (int j = 0; j < obrazy[numer].Y; j++)
    {
        for (int k = 0; k < obrazy[numer].X; k++)
        {
            suma = suma + obrazy[numer].tab[j][k];
        }
    }
    srednia = suma / (obrazy[numer].Y * obrazy[numer].X);
    return srednia;
}

int main(void)
{
    str1* obrazy;
    int ilosc_obrazow = 0;
    int numer = 1;
    int o;
    obrazy = malloc(ilosc_obrazow * sizeof(str1));
    if (obrazy == NULL) exit(4);
    do
    {
        o = menu();
        int zmienna = 0;
        system("cls");
        switch (o)
        {
        case '1': //dodanie obrazu
        {
            ilosc_obrazow++;
            numer = ilosc_obrazow - 1;
            obrazy = realloc(obrazy, ilosc_obrazow * sizeof(str1));
            if (obrazy == NULL)
            {
                fre(obrazy, ilosc_obrazow);
                exit(4);
            }
            obrazy[numer] = czytanie_danych(obrazy, numer, ilosc_obrazow);
            break;
        }
        case '2':            //obsloga usuwania plikow
        {
            if (ilosc_obrazow == 0)
            {
                printf("Nie ma zadnych zaladowanych obrazow!\n");
                break;
            }
            int w;
            while (zmienna < ilosc_obrazow)
            {
                drukuj_opcje(obrazy, zmienna);
                zmienna++;
            }
            do
            {
                printf("Ktory obraz chcesz usunac: \n");
                o = _getch();
                o = toupper(o);
                w = (int)(o - 48 - 1);
            } while (w < 0 || w >= ilosc_obrazow);
            numer = w;

            int j;
            free(obrazy[numer].nazwa);
            j = obrazy[numer].Y - 1;
            for (j; j >= 0; j--)
            {
                free(obrazy[numer].tab[j]);
            }
            free(obrazy[numer].tab);
            int i = numer;
            for (i; i < ilosc_obrazow; i++)
            {
                obrazy[i] = obrazy[i + 1];
            }
            ilosc_obrazow--;
            obrazy = realloc(obrazy, ilosc_obrazow * sizeof(str1));
            break;
        }
        case '3':            //obsloga wyswietlania 
        {
            if (ilosc_obrazow == 0)
            {
                printf("Nie ma zadnych zaladowanych obrazow!\n");
                printf("--------------------------\n");
                printf("\n");
                break;
            }
            zmienna = 0;
            printf("Obrazy w bazie danych:\n");
            while (zmienna < ilosc_obrazow)
            {
                drukuj_opcje(obrazy, zmienna);
                zmienna++;
            }
            printf("--------------------------\n");
            printf("\n");
            break;
        }
        case '4':            //wybor wskaznika na obiekt
        {                           
            if (ilosc_obrazow == 0)
            {
                printf("Nie ma zadnych zaladowanych obrazow!\n");
                printf("--------------------------\n");
                printf("\n");
                break;
            }
            printf("Wybierz na jakim pliku chcesz operowac: \n");
            zmienna = 0;
            int w, q;
            while (zmienna < ilosc_obrazow)
            {
                drukuj_opcje(obrazy, zmienna);
                zmienna++;
            }
            printf("--------------------------\n");
            printf("\n");
            int p = 0;
            do
            {
                o = _getch();
                o = toupper(o);
                w = (int)(o - 48 - 1);
            } while (w < 0 || w >= ilosc_obrazow);
            numer = w;
            system("cls");
            printf("Plik %s\n", obrazy[w].nazwa);
            printf("--------------------------\n");
            printf("\n");
            do
            {
                q = mini_menu();
                switch (q)
                {
                case 1: //wyswietl 
                {
                    drukuj(obrazy, numer);
                    printf("\n");
                    break;
                }
                case 2: //obrot  
                {
                    p = 0;
                    char cos[5];
                    float m;
                    int wybor = 5;
                    do
                    {
                        system("cls");
                        printf("OBRACANIE OBRAZU: %s\n", obrazy[numer].nazwa);
                        printf("--------------------------\n");
                        if (wybor < 0 || wybor>100) printf("ZLA WARTOSC! PODAJ JESZCZE RAZ!\n");
                        printf("O ile stopni chcesz obrocic obraz w lewo?\n");
                        printf("Podaj wartosc z zakresu <0;100> program zinterpretuje ja jako x*90 stopni.\n");
                        p = scanf_s("%s", &cos, 5);
                        m = strtof(cos, NULL, strlen(cos));
                        printf("\n");
                        wybor = m;
                        printf("Czy chcesz obrocic swoj obraz %d razy w lewo?\n1. TAK\n2. NIE\n", wybor);
                        int o = 0;
                        do
                        {
                            printf("Podaj 1 lub 2: \n");
                            o = _getch();
                            o = toupper(o);
                        } while (!(o == '2' || o == '1'));
                        if (o == '2') p = 0;
                        else p = 1;
                    } while (wybor < 0 || wybor>100 || p != 1);
                    
                    if (o == 0)
                    {
                        printf("Obraz obrocony o %d stopni w lewo.", 90 * w);
                        break;
                    }
                    int n = 0;
                    w = wybor;
                    w = w % 4;
                    while (n < w)
                    {
                        int** T;
                        T = malloc(obrazy[numer].X * sizeof(int*));
                        if (T == NULL)
                        {
                            fre(obrazy, ilosc_obrazow);
                            exit(2);
                        }
                        for (int i = 0; i < obrazy[numer].X; i++)
                        {
                            T[i] = malloc(obrazy[numer].Y * sizeof(int));
                            if (T[i] == NULL)
                            {
                                for (i; i >= 0; i--)
                                {
                                    free(T[i]);
                                }
                                free(T);
                                fre(obrazy, ilosc_obrazow);
                                exit(4);
                            }
                        }
                        printf("\n");
                        int k, p = obrazy[numer].X - 1;
                        for (int i = 0; i < obrazy[numer].X; i++)
                        {
                            k = 0;
                            for (int j = 0; j < obrazy[numer].Y; j++)
                            {
                                T[i][j] = obrazy[numer].tab[k][p];
                                k++;
                            }
                            p--;
                        }
                        int bufor = obrazy[numer].Y;
                        obrazy[numer].Y = obrazy[numer].X;
                        obrazy[numer].X = bufor;

                        if (obrazy[numer].X <= obrazy[numer].Y)
                        {
                            obrazy[numer].tab = realloc(obrazy[numer].tab, obrazy[numer].Y * sizeof(int*));
                            if (obrazy[numer].tab == NULL)
                            {
                                fre(obrazy, ilosc_obrazow);
                                exit(2);
                            }
                            for (int i = 0; i < obrazy[numer].X; i++)
                            {
                                obrazy[numer].tab[i] = realloc(obrazy[numer].tab[i], obrazy[numer].X * sizeof(int));
                                if (obrazy[numer].tab[i] == NULL)
                                {
                                    for (i; i >= 0; i--)
                                    {
                                        free(obrazy[numer].tab[i]);
                                    }
                                    free(obrazy[numer].tab);
                                    fre(obrazy, ilosc_obrazow);
                                    exit(4);
                                }
                            }
                            for (int i = obrazy[numer].X; i < obrazy[numer].Y; i++)
                            {
                                obrazy[numer].tab[i] = malloc(obrazy[numer].X * sizeof(int));
                                if (obrazy[numer].tab[i] == NULL)
                                {
                                    for (i; i >= 0; i--)
                                    {
                                        free(obrazy[numer].tab[i]);
                                    }
                                    free(obrazy[numer].tab);
                                    fre(obrazy, ilosc_obrazow);
                                    exit(4);
                                }
                            }
                        }
                        if (obrazy[numer].X > obrazy[numer].Y)
                        {
                            obrazy[numer].tab = realloc(obrazy[numer].tab, obrazy[numer].Y * sizeof(int*));
                            if (obrazy[numer].tab == NULL)
                            {
                                fre(obrazy, ilosc_obrazow);
                                exit(2);
                            }
                            for (int i = 0; i < obrazy[numer].Y; i++)
                            {
                                obrazy[numer].tab[i] = realloc(obrazy[numer].tab[i], obrazy[numer].X * sizeof(int));
                                if (obrazy[numer].tab[i] == NULL)
                                {
                                    for (i; i >= 0; i--)
                                    {
                                        free(obrazy[numer].tab[i]);
                                    }
                                    free(obrazy[numer].tab);
                                    fre(obrazy, ilosc_obrazow);
                                    exit(4);
                                }
                            }
                        }
                        
                        for (int i = 0; i < obrazy[numer].Y; i++)
                        {
                            for (int j = 0; j < obrazy[numer].X; j++)
                            {
                                obrazy[numer].tab[i][j] = T[i][j];
                                //printf("%d ", obrazy[numer].tab[i][j]);
                            }
                            //printf("\n");
                        }

                        int g = obrazy[numer].Y - 1;
                        for (g; g >= 0; g--)
                        {
                            free(T[g]);
                        }
                        free(T);
                        n++;
                    } 

                    break;
                }
                case 3: //histogram
                {
                    char nazwa[30];
                    errno_t a = 0;
                    FILE* plik = 0;
                    int p = 0;
                    do //istnienie pliku
                    {
                        do //nazwa
                        {
                            system("cls");
                            printf("HISTOGRAM OBRAZU: %s\n", obrazy[numer].nazwa);
                            printf("--------------------------\n");
                            printf("-> Podaj nazwe nowego pliku CSV. Nazwa nie moze przkraczac 30 znakow.\n");
                            printf("-> Dodaj na koncu '.csv'.\n");
                            if (a == 1) printf("UWAGA! Nie moge utworzyc takiego pliku. Podaj inna nawe pliku: \n");
                            while (p != 1)
                            {
                                p = 0;
                                p = scanf_s("%s", &nazwa, 30);
                            }
                            printf("Czy chcesz nazwac swoj plik csv: %s\n1. TAK\n2. NIE\n", nazwa);
                            int o = 0;
                            do
                            {
                                printf("Podaj 1 lub 2: \n");
                                o = _getch();
                                o = toupper(o);
                            } while (!(o == '2' || o == '1'));
                            if (o == '2') p = 0;
                            else p = 1;
                            printf("\n");
                        } while (p != 1);
                        
                        a = fopen_s(&plik, nazwa, "r");
                        if (a == 0)
                        {
                            a = 1;
                            fclose(plik);
                        }
                        else if (a != 0)
                        {
                            a = fopen_s(&plik, nazwa, "w");
                        }
                    } while (a == 1);
                    int licznik = 0;
                    for (int i = 0; i <= obrazy[numer].max; i++)
                    {
                        licznik = 0;
                        for (int j = 0; j < obrazy[numer].Y; j++)
                        {
                            for (int k = 0; k < obrazy[numer].X; k++)
                            {
                                if (obrazy[numer].tab[j][k] == i) licznik++;
                            }
                        }
                        fprintf(plik, "%d ; %d\n", i, licznik);
                    }
                    a = fclose(plik);
                    break;
                }
                case 4: //progowanie
                {
                    int p, wybor = 5;
                        p = 0;
                        char cos[30];
                        float m;
                            do
                            {
                                system("cls");
                                printf("PROGOWANIE OBRAZU: %s\n", obrazy[numer].nazwa);
                                printf("--------------------------\n");
                                if (wybor < 0 || wybor>obrazy[numer].max) printf("ZLA WARTOSC! PODAJ JESZCZE RAZ!\n");
                                printf("Wartosc maksymalna szarosci piksela to %d i oznacza na obrazie kolor bialy.\nWartosc minimalna to 0 i oznacz kolor czarny\n", obrazy[numer].max);
                                printf("Podaj prog: ");
                                p = scanf_s("%s", &cos, 30);
                                m = strtof(cos, NULL, strlen(cos));
                                wybor = m;
                                printf("\n");
                                printf("Czy chcesz nazwac swoj plik csv: %d\n1. TAK\n2. NIE\n", wybor);
                                int o = 0;
                                do
                                {
                                    printf("Podaj 1 lub 2: \n");
                                    o = _getch();
                                    o = toupper(o);
                                } while (!(o == '2' || o == '1'));
                                if (o == '2') p = 0;
                                else p = 1;
                            }while (wybor < 0 || wybor>obrazy[numer].max || p!=1);

                        printf("\n");

                    for (int i = 0; i < obrazy[numer].Y; i++)
                    {
                        for (int j = 0; j < obrazy[numer].X; j++)
                        {
                            if (obrazy[numer].tab[i][j] >= wybor)
                            {
                                obrazy[numer].tab[i][j] = 1;
                            }
                            else if (obrazy[numer].tab[i][j] < wybor)
                            {
                                obrazy[numer].tab[i][j] = 0;

                            }
                        }
                    }
                    obrazy[numer].max = 1;
                    break;
                }
                case 5: //pieprz i sól
                {
                    p = 0;
                    char cos[30];
                    float m;
                    int wybor = 5;
                    do
                    {
                        system("cls");
                        printf("SZUM PIEPRZ I SOL OBRAZU: %s\n", obrazy[numer].nazwa);
                        printf("--------------------------\n");
                        if (wybor < 0 || wybor>100) printf("ZLA WARTOSC! PODAJ JESZCZE RAZ!\n");
                        printf("Jaki procent obrazu chcesz zaszumic?\n");
                        p = 0;
                        printf("Podaj procent jako liczba calkowita z zakresu 0-100 (program rozumie to jako x%): ");
                        p = scanf_s("%s", &cos, 30);
                        m = strtof(cos, NULL, strlen(cos));
                        printf("\n");
                        wybor = m;
                        printf("Czy chcesz zaszumic swoj obraz w %d%?\n1. TAK\n2. NIE\n", wybor);
                        int o = 0;
                        do
                        {
                            printf("Podaj 1 lub 2: \n");
                            o = _getch();
                            o = toupper(o);
                        } while (!(o == '2' || o == '1'));
                        if (o == '2') p = 0;
                        else p = 1;
                    } while (wybor < 0 || wybor>100 || p != 1);

                    float ile = obrazy[numer].Y * obrazy[numer].X * wybor / 100;
                    for (int i = 0; i < ile; i++)
                    {
                        int A = (rand() % obrazy[numer].Y);
                        int B = (rand() % obrazy[numer].X);
                        int C = (rand() % 2);
                        obrazy[numer].tab[A][B] = C;
                    }

                    break;
                }
                case 6: //filtr
                {
                    int sr = srednia(obrazy, numer);
                    int** T;
                    T = malloc((obrazy[numer].Y+2) * sizeof(int*));
                    if (T == NULL)
                    {
                        fre(obrazy, ilosc_obrazow);
                        exit(2);
                    }
                    for (int i = 0; i < obrazy[numer].Y+2; i++)
                    {
                        T[i] = malloc((obrazy[numer].X+2) * sizeof(int*));
                        if (T[i] == NULL)
                        {
                            for (i; i >= 0; i--)
                            {
                                free(T[i]);
                            }
                            free(T);
                            fre(obrazy, ilosc_obrazow);
                            exit(4);
                        }
                    }
                    for (int i = 0; i < obrazy[numer].X + 2; i++) T[0][i] = sr;
                    for (int i = 0; i < obrazy[numer].X + 2; i++) T[obrazy[numer].Y+1][i] = sr;
                    for (int i = 1; i < obrazy[numer].Y+1; i++) T[i][0] = sr;
                    for (int i = 1; i < obrazy[numer].Y+1; i++) T[i][obrazy[numer].X + 1] = sr;
                    for (int i = 1; i < obrazy[numer].Y+1; i++)
                    {
                        for (int j = 1; j < obrazy[numer].X+1; j++)
                        {
                            T[i][j] = obrazy[numer].tab[i-1][j-1];  
                        }
                    }
                    int K[9];
                    int** P;
                    P = malloc(obrazy[numer].Y * sizeof(int*));
                    if (P == NULL)
                    {
                        fre(obrazy, ilosc_obrazow);
                        exit(9);
                    }
                    for (int i = 0; i < obrazy[numer].Y; i++)
                    {
                        P[i] = malloc(obrazy[numer].X * sizeof(int*));
                        if (obrazy[numer].tab[i] == NULL)
                        {
                            printf("lipa\n");
                            for (int z = i; z >= 0; z--)
                            {
                                free(obrazy[numer].tab[z]);
                            }
                            free(obrazy[numer].tab);
                            exit(1);
                        }
                    }

                    for (int i = 1; i < obrazy[numer].Y+1; i++)
                    {
                        for (int j = 1; j < obrazy[numer].X+1; j++)
                        {
                            int z = -1;
                            int f = -1;
                            int l = 0;
                            for (z; z < 2; z++)
                            {
                                f = -1;
                                for (f; f < 2; f++)
                                {
                                    K[l] = T[i + z][j + f];
                                    l++;
                                }
                            }
                            int buf = bubblesort(K, 9);
                            P[i-1][j-1] = buf;
                        }
                    }
                    for (int i = 0; i < obrazy[numer].Y; i++)
                    {
                        for (int j = 0; j < obrazy[numer].X; j++)
                        {
                            obrazy[numer].tab[i][j] = P[i][j];
                        }
                    }
                    for (int i = 0; i < obrazy[numer].Y; i++)
                    {
                        free(P[i]);
                    }
                    free(P);
                    break;
                }
                case 7: //zapisz obraz w pliku
                {
                    zapisywanie_danych(obrazy, numer);
                    break;
                } 
                case 8: //wyjdz z obrobki
                {
                    system("cls");
                    break;
                }
                }
            } while (q != 8);
        }
        case '5':
        {
            break;
        }
        }
    } while (o != '5');

    fre(obrazy, ilosc_obrazow);
    return 0;
}