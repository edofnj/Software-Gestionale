// cd "C:\Users\edofn\Desktop\inf\" ; if ($?) { gcc main.c -o main -lsqlite3} ; if ($?) { .\main }
#include "sqlite/sqlite3.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Prodotto {
  int codice;
  char nome[20];
  float prezzo;
  char descrizione[100];
  int quantita;
} prodotto;

typedef struct Acquisti {
  int codice;
  char nome[20];
  float prezzo;
  int quantita;
} listaSpesa;

static int callback(void *data, int argc, char **argv, char **azColName) {
  int i;
  bool *logged_in = (bool *)data;
  *logged_in = true;
  printf("%s: ", (const char *)data);
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void signup() {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;
  bool check = true; // Inizializzato a true

  // Apertura del database
  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n",
            sqlite3_errmsg(db));
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  char username[21]; // Aumentato di 1 per il carattere terminatore '\0'
  int password;
  int password_check;

  printf("Username (max 20 caratteri): ");
  scanf("%20s", username);

  do {
    printf("\nPassword (max 20 numeri): ");
    scanf("%d", &password);
    printf("\nReinserisci password: ");
    scanf("%d", &password_check);

    if (password != password_check) {
      check = false;
      printf("\nLe password non coincidono!\n");
    } else {
      check = true;
    }
  } while (!check);

  // Creazione della tabella se non esiste già
  sql = "CREATE TABLE IF NOT EXISTS accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "password INTEGER NOT NULL);";
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nella creazione della tabella: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Tabella creata con successo\n");
  }

  // Inserimento dei dati nella tabella
  sql = sqlite3_mprintf(
      "INSERT INTO accounts (username, password) VALUES ('%q', %d);",
      username, password);
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'inserimento dei dati: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Dati inseriti con successo\n");
  }

  sqlite3_close(db);
}

void login() {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;
  char username[20];
  int password;
  bool logged_in = false;
  int contatore = 0;

  // Apertura del database
  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  do {
    // Controllo delle credenziali di accesso
    printf("Inserisci l'username: ");
    getchar();
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // Rimuove il newline finale
		
    printf("\nPassword: ");
    scanf("%d", &password);
    // Controllo delle credenziali di accesso
    sql = sqlite3_mprintf("SELECT * FROM accounts WHERE username='%q' AND password='%d';", username, password);
    rc = sqlite3_exec(db, sql, callback, &logged_in, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "Errore nella verifica delle credenziali: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      if (logged_in) {
        fprintf(stdout, "Credenziali verificate con successo\n");
        contatore = 1;
      } else {
        fprintf(stdout, "Credenziali non valide\n");
      }
    }
    sqlite3_free(sql);
  } while (contatore != 1);

  sqlite3_close(db);
}

void aggiuntaProdotto(int indLog, int indLogSpesa, prodotto inventario[30], listaSpesa spesa[50]) {
  char buy;
  char op;
  bool code;
  int i;
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;

  // Apertura database
  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n",
            sqlite3_errmsg(db));
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }
  // Creazione della tabella se non esiste già
  sql = "CREATE TABLE IF NOT EXISTS inventario ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "codice INTEGER NOT NULL,"
        "nome TEXT NOT NULL,"
        "prezzo REAL NOT NULL, "
        "descrizione TEXT NOT NULL, "
        "quantita INTEGER NOT NULL);";
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nella creazione della tabella: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Tabella creata con successo\n");
  }

  do {
    do {
      printf("Inserisci il codice identificativo del prodotto: ");
      scanf("%d", &inventario[indLog].codice);
      code = true;
      for (i = 0; i < indLog; i++) {
        if (inventario[i].codice == inventario[indLog].codice) {
          printf("Errore: il codice inserito è già stato utilizzato per un altro prodotto.\n");
          code = false;
          break;
        }
      }
    } while (!code);
    printf("Inserisci il nome del prodotto: ");
    getchar();
    scanf("%[^\n]", inventario[indLog].nome);
    printf("Inserisci il prezzo del prodotto (max 2 cifre decimali): ");
    scanf("%f", &inventario[indLog].prezzo);
    printf("Inserisci una breve descrizione: ");
    getchar();
    scanf("%[^\n]", inventario[indLog].descrizione);
    printf("Inserisci la quantita' presente in magazzino: ");
    scanf("%d", &inventario[indLog].quantita);
    indLog++;
    // Inserimento dei dati nella tabella
    sql = sqlite3_mprintf(
        "INSERT INTO inventario (codice, nome, prezzo, descrizione, quantita) "
        "VALUES ('%d', '%q', '%f', '%q', '%d');",
        inventario[indLog-1].codice, inventario[indLog-1].nome,
        inventario[indLog-1].prezzo, inventario[indLog-1].descrizione,
        inventario[indLog-1].quantita);
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "Errore nell'inserimento dei dati: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      fprintf(stdout, "Dati inseriti con successo\n");
    }
    printf("\nInserire altri prodotti (S/N)? ");
    getchar();
    scanf("%c", &op);
  } while ((op == 'S' || op == 's') && indLog < 30);
  sqlite3_close(db);
}

void stampa(int indLog, int indLogSpesa, prodotto inventario[30], listaSpesa spesa[50]) {
  int op;
  int i;
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;
  sqlite3_stmt *stmt;

  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  printf("Prodotti presenti nel magazzino:\n\n");
  sql = "SELECT codice, nome, prezzo, descrizione, quantita FROM inventario;";
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nella preparazione della query: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return;
  }

  rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
  	int codice = sqlite3_column_int(stmt, 0);
    const unsigned char *nome = sqlite3_column_text(stmt, 1);
    double prezzo = sqlite3_column_double(stmt, 2);
    const unsigned char *descrizione = sqlite3_column_text(stmt, 3);
    int quantita = sqlite3_column_int(stmt, 4);

    printf("Codice prodotto: %d\n", codice);
    printf("Nome: %s\n", nome);
    printf("Prezzo: %.2f\n", prezzo);
    printf("Descrizione: %s\n", descrizione);
    printf("Quantita': %d\n", quantita);
    printf("\n");

    rc = sqlite3_step(stmt);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}


void deleteProdotto(int indLog, prodotto inventario[30]) {
  int ind;
  int i;
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;

  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  printf("\nInserisci il codice del prodotto da eliminare: ");
  scanf("%d", &ind);

  sql = sqlite3_mprintf("DELETE FROM inventario WHERE codice = %d;", ind);
  
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'esecuzione della query: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Prodotto eliminato con successo\n");
  }

  sqlite3_close(db);
}


void ricerca(int indLog, int indLogSpesa, prodotto inventario[30], listaSpesa spesa[50]) {
  int op;
  char name[20];
  int code;
  char ricerca;
  int i;
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;
  sqlite3_stmt *stmt;

  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  do {
    printf("\n1. Nome\n2. Codice\n\nInserisci il campo di ricerca: ");
    scanf("%d", &op);

    switch (op) {
      case 1:
        printf("Inserisci il nome del prodotto: ");
        getchar();
        scanf("%[^\n]", name);
        sql = sqlite3_mprintf("SELECT codice, nome, prezzo, descrizione, quantita FROM inventario WHERE nome = '%q';", name);
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
          fprintf(stderr, "Errore nella preparazione della query: %s\n", sqlite3_errmsg(db));
          sqlite3_finalize(stmt);
          sqlite3_close(db);
          return;
        }

        rc = sqlite3_step(stmt);
        while (rc == SQLITE_ROW) {
          int codice = sqlite3_column_int(stmt, 0);
          const unsigned char *nome = sqlite3_column_text(stmt, 1);
          double prezzo = sqlite3_column_double(stmt, 2);
          const unsigned char *descrizione = sqlite3_column_text(stmt, 3);
          int quantita = sqlite3_column_int(stmt, 4);

          printf("Codice prodotto: %d\n", codice);
          printf("Nome: %s\n", nome);
          printf("Prezzo: %.2f\n", prezzo);
          printf("Descrizione: %s\n", descrizione);
          printf("Quantita': %d\n", quantita);
          printf("\n");

          rc = sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
        break;

      case 2:
        printf("Inserisci il codice del prodotto: ");
        scanf("%d", &code);
        sql = sqlite3_mprintf("SELECT codice, nome, prezzo, descrizione, quantita FROM inventario WHERE codice = %d;", code);
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
          fprintf(stderr, "Errore nella preparazione della query: %s\n", sqlite3_errmsg(db));
          sqlite3_finalize(stmt);
          sqlite3_close(db);
          return;
        }

        rc = sqlite3_step(stmt);
        while (rc == SQLITE_ROW) {
          int codice = sqlite3_column_int(stmt, 0);
          const unsigned char *nome = sqlite3_column_text(stmt, 1);
          double prezzo = sqlite3_column_double(stmt, 2);
          const unsigned char *descrizione = sqlite3_column_text(stmt, 3);
          int quantita = sqlite3_column_int(stmt, 4);

          printf("Codice prodotto: %d\n", codice);
          printf("Nome: %s\n", nome);
          printf("Prezzo: %.2f\n", prezzo);
          printf("Descrizione: %s\n", descrizione);
          printf("àQuantita': %d\n", quantita);
          printf("\n");

          rc = sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
        break;

      default:
        printf("Campo di ricerca non valido!\n");
        break;
    }

    printf("Vuoi eseguire un'altra ricerca (S/N)? ");
    getchar();
    scanf("%c", &ricerca);
  } while (ricerca == 'S' || ricerca == 's');

  sqlite3_close(db);
}

void trovaQuantita(int indLog, prodotto inventario[30]) {
  int code;
  int i;
  bool ctrl = false;
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;

  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  printf("Inserisci il codice del prodotto: ");
  scanf("%d", &code);

  sql = sqlite3_mprintf("SELECT quantita FROM inventario WHERE codice = %d;", code);
  sqlite3_stmt *stmt;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nella preparazione della query: %s\n",
            sqlite3_errmsg(db));
    return;
  }

  int code_to_retrieve = code;

  sqlite3_bind_int(stmt, 1, code_to_retrieve);

  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    int field_value = sqlite3_column_int(stmt, 0);
    printf("Quantita del prodotto: %d\n", field_value);
  } else if (rc != SQLITE_DONE) {
    fprintf(stderr, "Errore nell'esecuzione della query: %s\n", sqlite3_errmsg(db));
    return;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

void modificaQuantita(int indLog, prodotto inventario[30]) {
  int code;
  int i;
  bool ctrl = false;
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char *sql;

  rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'apertura del database: %s\n",
            sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  } else {
    fprintf(stdout, "Database aperto con successo\n");
  }

  printf("Inserisci il codice identificativo del prodotto: ");
  scanf("%d", &code);
  printf("Inserire i pezzi rimasti: ");
  scanf("%d", &i);

  if (i == 0) {
    // Se la nuova quantità è 0, elimina il prodotto
    sql = sqlite3_mprintf("DELETE FROM inventario WHERE codice = %d;", code);
  } else {
    // Altrimenti, aggiorna la quantità del prodotto
    sql = sqlite3_mprintf("UPDATE inventario SET quantita = %d WHERE codice = %d;", i, code);
  }

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Errore nell'esecuzione della query: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sqlite3_close(db);
}

int main() {
  int indLog = 0, indLogSpesa = 0, indUser = 0;
  char a;
  prodotto inventario[30];
  listaSpesa spesa[50];
  time_t current_time;
  struct tm *local_time;
  time(&current_time);
  local_time = localtime(&current_time);
  int op;
  int operazione;
  bool log = false;
  bool controllo = false;
  char password[100];

  system("cls");
  printf("Benvenuto\t%02d/%02d/%d\n\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900);

  do {
    printf("1.Log In\n2.Sign Up\n\nInserisci l'operazione da eseguire: ");
    scanf("%d", &op);
    switch (op) {
    case 1:
      login();
      log = true;
      break;
    case 2:
      signup();
      break;
		default:
			printf("Operazione non riconosciuta\n");
		break;
		}
  } while (!log);

  system("cls");

  printf("Benvenuto\t%02d/%02d/%d\n\n", local_time->tm_mday,
         local_time->tm_mon + 1, local_time->tm_year + 1900);

  do {
    printf("1. Aggiungi prodotto\n2. Elimina prodotto\n3. Stampa\n4. Ricerca\n5. Trova quantita'\n6. Modifica quantita'\n\n");
    printf("Inserisci l'operazione da eseguire: ");
    scanf("%d", &operazione);

    switch (operazione) {
    case 1:
      aggiuntaProdotto(indLog, indLogSpesa, inventario, spesa);
      break;
    case 2:
      deleteProdotto(indLog, inventario);
      break;
    case 3:
      stampa(indLog, indLogSpesa, inventario, spesa);
      break;
    case 4:
      ricerca(indLog, indLogSpesa, inventario, spesa);
      break;
    case 5:
      trovaQuantita(indLog, inventario);
      break;
    case 6:
      modificaQuantita(indLog, inventario);
      break;
    default:
      printf("Hai inserito un operazione non valida");
      break;
    }
    printf("\n\nVuoi eseguire un'altra operazione (S/N)? ");
    getchar();
    scanf("%c", &a);
    printf("\n");
    if (a == 'N' || a == 'n') {
      controllo = true;
    }
  } while (!controllo);

  return 0;
}
