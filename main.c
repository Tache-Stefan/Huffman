#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

//https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/

#define MARIME_MALLOC 100

struct frec_cuv {

    char *cuv;
    int frecv;
};

struct baza_de_date {

    char *cuv, *cod;
};

struct Nod {
    char *cuvant;
    int frec;
    struct Nod *fiu_stanga, *fiu_dreapta;
};

struct Nod* makeNod(char *cuvant, int frec) {
    struct Nod* nod = (struct Nod*)malloc(sizeof(struct Nod));
    nod->cuvant = cuvant;
    nod->frec = frec;
    nod->fiu_stanga = nod->fiu_dreapta = NULL;
    return nod;
}

int cmp(const void* a, const void* b) {
    const struct Nod* nodA = *(const struct Nod**)a;
    const struct Nod* nodB = *(const struct Nod**)b;
    return (nodA->frec - nodB->frec);
}

struct Nod* arboreHuffman(char *cuvinte[], int frec[], int marime) {
    struct Nod *stanga, *dreapta, *suma;
    
    struct Nod** noduri = (struct Nod**)malloc(marime * sizeof(struct Nod*));
    for (int i = 0; i < marime; ++i) {
        noduri[i] = makeNod(cuvinte[i], frec[i]);
    }
    
    qsort(noduri, marime, sizeof(struct Nod*), cmp);
    
    while (marime > 1) {
        stanga = noduri[0];
        dreapta = noduri[1];
        suma = makeNod("$", stanga->frec + dreapta->frec);
        suma->fiu_stanga = stanga;
        suma->fiu_dreapta = dreapta;
        
        noduri[0] = suma;
        for (int i = 1; i < marime - 1; ++i) {
            noduri[i] = noduri[i + 1];
        }
        marime--;
        
        qsort(noduri, marime, sizeof(struct Nod*), cmp);
    }
    
    struct Nod* radacina = noduri[0];
    free(noduri);
    return radacina;
}

void printArr(int cod[], int marime, FILE *out) {

    for(int i = 0; i < marime; ++i)
        fprintf(out, "%d", cod[i]);

    fprintf(out, "\n");
}

void printareCoduri(struct Nod* radacina, int cod[], int top, FILE *out) {

    if(radacina->fiu_stanga) {

        cod[top] = 0;
        printareCoduri(radacina->fiu_stanga, cod, top + 1, out);
    }

    if(radacina->fiu_dreapta) {

        cod[top] = 1;
        printareCoduri(radacina->fiu_dreapta, cod, top + 1, out);
    }

    if(!(radacina->fiu_stanga) && !(radacina->fiu_dreapta)) {

        fprintf(out, "%s:", radacina->cuvant);
        printArr(cod, top, out);
    }
}

void freeArbore(struct Nod* radacina) {
    if (radacina == NULL) return;

    freeArbore(radacina->fiu_stanga);
    freeArbore(radacina->fiu_dreapta);

    free(radacina);
}

void generareCoduri(char *cuvinte[], int frec[], int marime, FILE *out) {
    struct Nod* radacina = arboreHuffman(cuvinte, frec, marime);

    int cod[100], top = 0;

    printareCoduri(radacina, cod, top, out);

    freeArbore(radacina);
}

int cautareCuv(struct frec_cuv* cuvinte, int marime, char cuvant[]) {

    for(int i = 0; i <= marime; ++i)
        if(!strcmp(cuvinte[i].cuv, cuvant))
            return i;
    return -1;
}

struct frec_cuv* createFrec(FILE *in, int *n) {

    int marime = 1;
    struct frec_cuv *cuvinte = (struct frec_cuv*)malloc(marime * MARIME_MALLOC * sizeof(struct frec_cuv));
    if(cuvinte == NULL) {
        fprintf(stderr, "Eroare malloc\n");
        return NULL;
    }

    *n = -1;
    char sep[] = " .,?!\"()_-;:<>\\/+=[]|*&$#@%^\n“”—‘’{}\t";
    char buffer[50], c;
    bool lastSemn = false;
    strcpy(buffer, "");
    c = fgetc(in);
    if(strchr(sep, c) == NULL)
        snprintf(buffer, 50, "%c", c);
    else
        lastSemn = true;
    while((c = fgetc(in)) != EOF) {

        if(lastSemn == true && strchr(sep, c) != NULL) {
            //suntem in sir de separatori, continuam
            lastSemn = true;
            continue;
        }

        if(lastSemn == false && strchr(sep, c) != NULL) {
            //trebuie adaugat cuvant
            buffer[strlen(buffer)] = '\0';
            if(buffer[strlen(buffer) - 1] == '\n')
                buffer[strlen(buffer) - 1] = '\0';
            int index = cautareCuv(cuvinte, *n, buffer);
            if(index != -1)
                cuvinte[index].frecv++;
            else {
                if(*n + 1 >= marime * MARIME_MALLOC) {
                    marime++;
                    struct frec_cuv* new_cuvinte = (struct frec_cuv*)realloc(cuvinte, marime * MARIME_MALLOC * sizeof(struct frec_cuv));
                    if(new_cuvinte == NULL) {
                        free(new_cuvinte);
                        free(cuvinte);
                        fprintf(stderr, "Eroare realloc\n");
                        return NULL;
                    }
                    cuvinte = new_cuvinte;
                }
                *n = *n + 1;
                cuvinte[*n].cuv = strdup(buffer);
                cuvinte[*n].frecv = 1;
            }
            strcpy(buffer, "");
            lastSemn = true;
        }

        if(lastSemn == false && strchr(sep, c) == NULL) {
            //interiorul cuvantului
            lastSemn = false;
            char *aux;
            aux = strdup(buffer);
            snprintf(buffer, 50, "%s%c", aux, c);
            free(aux);
        }

        if(lastSemn == true && strchr(sep, c) == NULL) {
            //inceputul cuvantului
            lastSemn = false;
            snprintf(buffer, 50, "%c", c);
        }
    }

    if(strcmp(buffer, "")) {

        buffer[strlen(buffer)] = '\0';
        if(buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';
        int index = cautareCuv(cuvinte, *n, buffer);
        if(index != -1)
            cuvinte[index].frecv++;
        else {
            if(*n + 1 >= marime * MARIME_MALLOC) {
                marime++;
                struct frec_cuv* new_cuvinte = (struct frec_cuv*)realloc(cuvinte, marime * MARIME_MALLOC * sizeof(struct frec_cuv));
                if(new_cuvinte == NULL) {
                    free(new_cuvinte);
                    free(cuvinte);
                    fprintf(stderr, "Eroare realloc\n");
                    return NULL;
                }
                cuvinte = new_cuvinte;
            }
            *n = *n + 1;
            cuvinte[*n].cuv = strdup(buffer);
            cuvinte[*n].frecv = 1;
        }
    }

    return cuvinte;
}

char* cautareCod(struct baza_de_date *baza, char *cuv) {

    int i = 0;
    while(strcmp(cuv, baza[i].cuv)) {
        ++i;
    }

    return baza[i].cod;   
}

void codificare(struct baza_de_date *baza, FILE *in, FILE *out) {

    char sep[] = " .,?!\"()_-;:<>\\/+=[]|*&$#@%^\n“”—‘’{}\t";
    char buffer[50], c;
    bool lastSemn = false;
    strcpy(buffer, "");
    c = fgetc(in);
    if(strchr(sep, c) == NULL)
        snprintf(buffer, 50, "%c", c);
    else
        lastSemn = true;
    while((c = fgetc(in)) != EOF) {

        if(lastSemn == true && strchr(sep, c) != NULL) {
            //suntem in sir de separatori, continuam
            lastSemn = true;
            continue;
        }

        if(lastSemn == false && strchr(sep, c) != NULL) {
            //trebuie adaugat cuvant
            buffer[strlen(buffer)] = '\0';
            char *cod = cautareCod(baza, buffer);
            if(cod == NULL) {
                fprintf(stderr, "Cuvantul %s nu a fost gasit in dictionar\n", buffer);
                return;
            }
            fprintf(out, "%s", cod);
            strcpy(buffer, "");
            lastSemn = true;
        }

        if(lastSemn == false && strchr(sep, c) == NULL) {
            //interiorul cuvantului
            lastSemn = false;
            char *aux;
            aux = strdup(buffer);
            snprintf(buffer, 50, "%s%c", aux, c);
            free(aux);
        }

        if(lastSemn == true && strchr(sep, c) == NULL) {
            //inceputul cuvantului
            lastSemn = false;
            snprintf(buffer, 50, "%c", c);
        }
    }

    if(strcmp(buffer, "")) {

        buffer[strlen(buffer)] = '\0';
        char *cod = cautareCod(baza, buffer);
            if(cod == NULL) {
                fprintf(stderr, "Cuvantul %s nu a fost gasit in dictionar\n", buffer);
                return;
            }
            fprintf(out, "%s", cod);
    }
}

unsigned char bitiToDec(const char *sir) {

    unsigned char dec = 0;
    for(int i = 0; i < 8; ++i)
        dec = dec * 2 + (sir[i] - '0');
    return dec;
}

void arhivare(FILE *in, FILE *out) {

    int nr = 0;
    char buffer[9], c;
    strcpy(buffer, "");
    while((c = fgetc(in)) != EOF) {

        if(nr < 8) {
            buffer[nr] = c;
            nr++;
        }

        if(nr % 8 == 0) {
            unsigned char dec = bitiToDec(buffer);
            fwrite(&dec, sizeof(unsigned char), 1, out);
            strcpy(buffer, "");
            nr = 0;
        }
    }
}

//Am creeat structura asta pentru a cauta codurile cuvintelor in ea,
//daca cautam direct in fisier dura prea mult
struct baza_de_date* creeareBaza(FILE *dictionar, int n) {

    struct baza_de_date *baza = (struct baza_de_date*)malloc((n + 1) * sizeof(struct baza_de_date));
    if(baza == NULL) {
        fprintf(stderr, "Eroare malloc\n");
        return NULL;
    }

    char buffer[200];
    int i = -1;

    while(fgets(buffer, 200, dictionar) != NULL) {

        char *p = strtok(buffer, ":");
        baza[++i].cuv = strdup(p);
        p = strtok(NULL, "\n");
        baza[i].cod = strdup(p);
    }

    return baza;
}

void afisare_rata(const char *text, const char *dictionar, const char *arhiva, const char nr) {

    FILE *in = fopen(text, "r");
    fseek(in, 0, SEEK_END);
    long long marime_initiala = ftell(in);
    fclose(in);
    in = fopen(dictionar, "r");
    fseek(in, 0, SEEK_END);
    long long marime_arhiva = ftell(in);
    fclose(in);
    in = fopen(arhiva, "rb");
    fseek(in, 0, SEEK_END);
    marime_arhiva += ftell(in);
    fclose(in);

    double rata_compresie;
    rata_compresie = (double)(marime_initiala - marime_arhiva) / marime_initiala * 100;
    if(nr == '0')
        printf("Rata de compresie a testului assert este de %.2f%%.\n", rata_compresie);
    else
        printf("Rata de compresie a textului %c este de %.2f%%.\n", nr, rata_compresie);
}

void proces_Huffman(const char *fisier_text, const char *fisier_dictionar, const char *fisier_temporar, const char *arhivat, const char nr) {

    clock_t start, end;
    double timp;
    start = clock();

    FILE *in = fopen(fisier_text, "r");
    FILE *out = fopen(fisier_dictionar, "w");
    int marime;
    struct frec_cuv *cuvinte = createFrec(in, &marime);

    char *cuv[marime + 1];
    int frecv[marime + 1];
    for(int i = 0; i <= marime; ++i) {
        cuv[i] = strdup(cuvinte[i].cuv);
        frecv[i] = cuvinte[i].frecv;
    }

    generareCoduri(cuv, frecv, marime + 1, out);

    fclose(in);
    fclose(out);
    FILE* dictionar = fopen(fisier_dictionar, "r");
    in = fopen(fisier_text, "r");
    out = fopen(fisier_temporar, "w");

    struct baza_de_date *baza = creeareBaza(dictionar, marime);
    codificare(baza, in, out);

    for(int i = 0; i <= marime; ++i) {
        free(baza[i].cuv);
        free(baza[i].cod);
        free(cuv[i]);
        free(cuvinte[i].cuv);
    }
    free(cuvinte);
    free(baza);
    fclose(in);
    fclose(out);
    fclose(dictionar);

    in = fopen(fisier_temporar, "r");
    out = fopen(arhivat, "wb");

    arhivare(in, out);

    fclose(in);
    fclose(out);

    if(remove(fisier_temporar) != 0)
        fprintf(stderr, "Eroare la stergerea fisierului %s\n", fisier_temporar);

    end = clock();

    timp = (double)(end - start) / CLOCKS_PER_SEC;
    if(nr == '0')
        printf("Timpul de executie pentru testul assert este de %.2f secunde.\n", timp);
    else
        printf("Timpul de executie pentru textul %c este de %.2f secunde.\n", nr, timp);
    afisare_rata(fisier_text, fisier_dictionar, arhivat, nr);
}

int main() {

    proces_Huffman("text1.txt", "dictionar1.out", "temp.txt", "arhivatext1.bin", '1');
    proces_Huffman("text2.txt", "dictionar2.out", "temp.txt", "arhivatext2.bin", '2');
    proces_Huffman("text3.txt", "dictionar3.out", "temp.txt", "arhivatext3.bin", '3');
    proces_Huffman("assert1.txt", "dictionar_assert_1.out", "temp.txt", "arhiva_assert_1.bin", '0');

    FILE *in = fopen("dictionar_assert_1.out", "r");
    char *teste1[7];
    for(int i = 0; i < 7; ++i) {
        teste1[i] = (char*)malloc(50 * sizeof(char));
        fgets(teste1[i], 50, in);
        char *temp = strdup(strchr(teste1[i], ':') + 1);
        free(teste1[i]);
        teste1[i] = temp;

        if(teste1[i][strlen(teste1[i]) - 1] == '\n')
            teste1[i][strlen(teste1[i]) - 1] = '\0';
    }
    fclose(in);

    assert(!strcmp(teste1[0], "000") && !strcmp(teste1[1], "001") && 
           !strcmp(teste1[2], "01") && !strcmp(teste1[3], "100") && 
           !strcmp(teste1[4], "1010") && !strcmp(teste1[5], "1011") && 
           !strcmp(teste1[6], "11"));

    for(int i = 0; i < 7; ++i)
        free(teste1[i]);
    
    return 0;
}
