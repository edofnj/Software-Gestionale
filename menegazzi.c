#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct Prodotto{
  int codice;
  char nome[20];
  float prezzo;
  char descrizione[100];
  int quantita;
}prodotto;

typedef struct Acquisti{
  int codice;
  char nome[20];
  float prezzo;
  int quantita;
}listaSpesa;

struct time{
  int tm_min;
  int tm_hrs;
  int tm_mday;
  int tm_mon;
  int tm_year;
};

int codici[30];

void aggiuntaProdotto(int indLog, int indLogSpesa, prodotto inventario[30], listaSpesa spesa[50]){
  char buy;
  char op;
  bool code;
  do{
    do{
      printf("Inserisci il codice identificativo del prodotto: ");
      scanf("%d", &inventario[indLog].codice);
      code = true;
      for(int i=0; i<indLog; i++){
        if(inventario[i].codice == inventario[indLog].codice){
          printf("Errore: il codice inserito è già stato utilizzato per un altro prodotto.\n");
          code = false;
          break;
        }
      }
    } while(!code);
    printf("Inserisci il nome del prodotto: ");
    getchar();
    scanf("%[^\n]", inventario[indLog].nome);
    printf("Inserisci il prezzo del prodotto: ");
    scanf("%f", &inventario[indLog].prezzo);
    printf("Inserisci una breve descrizione: ");
    getchar();
    scanf("%[^\n]", inventario[indLog].descrizione);
    printf("Inserisci la quantita presente in magazzino: ");
    scanf("%d", &inventario[indLog].quantita);
    if(inventario[indLog].quantita < 5){
      printf("Vuoi acquistarne altri (S/N)? ");
      scanf(" %c", &buy);
      if(buy == 'S' || buy == 's'){
        printf("Inserire la quantità: ");
        scanf("%d", &spesa[indLogSpesa].quantita);
        spesa[indLogSpesa].codice = inventario[indLog].codice;
        strcpy(spesa[indLogSpesa].nome, inventario[indLog].nome);
        spesa[indLogSpesa].prezzo = inventario[indLog].prezzo;
        indLogSpesa++;
      }
    }
    indLog++;
    printf("\nInserire altri prodotti (S/N)? ");
    scanf(" %c", &op);
  }while((op == 'S' || op == 's') && indLog < 30);
}

void stampa(int indLog, int indLogSpesa, prodotto inventario[30], listaSpesa spesa[50]){
  int op;
  printf("\n1. Stampa inventario\n2. Stampa lista della spesa\n\nInserisci l'operazione: ");
  scanf("%d", &op);
  switch(op){
    case 1:
      for(int i=0; i<indLog+1; i++){
        printf("Codice: %d\nNome: %s\nPrezzo: %.2f\nDescrizione: %s\nQuantità: %d\n\n", inventario[i].codice, inventario[i].nome, inventario[i].prezzo, inventario[i].descrizione, inventario[i].quantita);
      }
      break;
    case 2:
      for(int i=0; i<indLogSpesa+1; i++){
        printf("Codice: %d\nNome: %s\nPrezzo: %.2f\nQuantità: %d\n\n", spesa[i].codice, spesa[i].nome, spesa[i].prezzo, spesa[i].quantita);
      }
      break;
    default:
      printf("Operazione non valida.\n");
      break;
  }
}

void deleteProdotto(int indLog, prodotto inventario[30]){
  int ind;
  printf("Inserisci il codice del prodotto da eliminare: ");
  scanf("%d", &ind);
  int rif = 45;
  for(int i=0; i<indLog; i++){
    if(ind == inventario[i].codice){
      rif = i;
      inventario[i].codice = inventario[i+1].codice;
      strcpy(inventario[i].nome, inventario[i+1].nome);
      inventario[i].prezzo = inventario[i+1].prezzo;
      strcpy(inventario[i].descrizione, inventario[i+1].descrizione);
      inventario[i].quantita = inventario[i+1].quantita;
    }
    if(i > rif){
      if(i == indLog){
        indLog--;
      }else{
        inventario[i].codice = inventario[i+1].codice;
        strcpy(inventario[i].nome, inventario[i+1].nome);
        inventario[i].prezzo = inventario[i+1].prezzo;
        strcpy(inventario[i].descrizione, inventario[i+1].descrizione);
        inventario[i].quantita = inventario[i+1].quantita;
      }
    }
  }
}

int main(){
  
  int indLog=0, indLogSpesa=0;
  prodotto inventario[30];
  listaSpesa spesa[50];
  time_t rawtime;
  struct time *info;
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();
  time(&rawtime);
  info = localtime(&rawtime);

  printf("Benvenuto\t\n%s\n\n", asctime(info));

  printf("1. ");

  aggiuntaProdotto(indLog, indLogSpesa, inventario, spesa);

  stampa(indLog, indLogSpesa, inventario, spesa);
  
  return 0;
}