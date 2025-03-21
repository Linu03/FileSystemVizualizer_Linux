#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>

#define PATH_MAX 4096 
#define NUME_MAX 255 
#define MAX_TABLOU 255

void afiseaza_mesaj_unic(const char *mesaj);
int confirmare_stergere(const char *cale_fisier);
int confirmare_mutare(const char *cale_fisier, const char *destinatie);
int copie_fisier(const char *cale_sursa, const char *cale_destinatie);
void init_interface_ncurses();
void cleanup_interface_ncurses();
void interfata_antet();
void continut_director(const char *cale, int rand_evidentiat);
void navigare_fisiere();


void afiseaza_mesaj_unic(const char *mesaj) {
    clear();
    mvprintw(LINES / 2, (COLS - strlen(mesaj)) / 2, "%s", mesaj);
    refresh();
}


int confirmare_stergere(const char *cale_fisier) {
    afiseaza_mesaj_unic("Esti sigur ca vrei sa stergi fisierul? (y/n)");
    int ch = getch();
    return (ch == 'y' || ch == 'Y');
}

int confirmare_mutare(const char *cale_fisier, const char *destinatie) {
    char mesaj[PATH_MAX + NUME_MAX];
    snprintf(mesaj, sizeof(mesaj), "Esti sigur ca vrei sa muti fisierul %s in %s? (y/n)", cale_fisier, destinatie);
    afiseaza_mesaj_unic(mesaj);
    int ch = getch();
    return (ch == 'y' || ch == 'Y');
}


int copie_fisier(const char *cale_sursa, const char *cale_destinatie) {
    afiseaza_mesaj_unic("Se copiaza fisierul...");
    refresh();
    int fisier_sursa = open(cale_sursa, O_RDONLY);
    if (fisier_sursa == -1) {
        perror("Eroare la deschiderea fisierului sursa");
        return 0;
    }

    int fisier_destinatie = open(cale_destinatie, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fisier_destinatie == -1) {
        perror("Eroare la deschiderea fisierului destinatie");
        close(fisier_sursa);
        return 0;
    }

    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(fisier_sursa, buffer, sizeof(buffer))) > 0) {
        if (write(fisier_destinatie, buffer, bytes) != bytes) {
            perror("Eroare la scrierea in fisierul destinatie");
            close(fisier_sursa);
            close(fisier_destinatie);
            return 0;
        }
    }

    close(fisier_sursa);
    close(fisier_destinatie);
    return 1;
}


void init_interface_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

void cleanup_interface_ncurses() {
    endwin();
}

void interfata_antet(){
    mvprintw(1, 0, "   %-30s %-10s %10s %10s", "Nume", "Tip", "Dimensiune", "Destinatie");
    mvprintw(2, 0, "   %-30s %-10s %10s %10s", "------------------------------", "----------", "----------", "--------------------");
}

void continut_director(const char *cale, int rand_evidentiat) {
    DIR *dir = opendir(cale);
    if (!dir)
    {
        printw("Eroare la deschiderea directorului: %s\n", cale);
        return;
    }

    struct dirent *intrare;
    struct stat file_stat;
    char tablou_intrari_dir[MAX_TABLOU][NUME_MAX];
    int contor_intrari = 0;

    while ((intrare = readdir(dir)) != NULL){
        strncpy(tablou_intrari_dir[contor_intrari], intrare->d_name, NUME_MAX - 1);
        tablou_intrari_dir[contor_intrari][NUME_MAX - 1] = '\0';
        contor_intrari++;
    }
    closedir(dir);

    interfata_antet();

    for (int i = 0; i < contor_intrari; i++){
        char cale_absoluta[PATH_MAX];
        snprintf(cale_absoluta, sizeof(cale_absoluta), "%s/%s", cale, tablou_intrari_dir[i]);

        char tip[10] = "";
        int dimensiune = 0;
        char destinatie_link[PATH_MAX] = "";

        if (lstat(cale_absoluta, &file_stat) == 0){
            if (S_ISDIR(file_stat.st_mode))
            {
                strcpy(tip, "Director");
            } 
            else if (S_ISLNK(file_stat.st_mode))
            {
                strcpy(tip, "Link");
                ssize_t lungime_link = readlink(cale_absoluta, destinatie_link, sizeof(destinatie_link) - 1);
                if (lungime_link != -1) {
                    destinatie_link[lungime_link] = '\0';
                }
            } 
            else if (S_ISREG(file_stat.st_mode))
            {
                strcpy(tip, "Fisier");
            }
            dimensiune = file_stat.st_size;
        }

        if (i == rand_evidentiat)
        {
            attron(A_REVERSE);
            mvprintw(3 + i, 0, "-> %-30s %-10s %10ld %s", tablou_intrari_dir[i], tip, dimensiune, destinatie_link);
            attroff(A_REVERSE);
        } else 
        {
            mvprintw(3 + i, 0, "   %-30s %-10s %10ld %s", tablou_intrari_dir[i], tip, dimensiune, destinatie_link);
        }
    }
}

void navigare_fisiere(){
    char cale_curenta[PATH_MAX];
    
    getcwd(cale_curenta, sizeof(cale_curenta));     
    
    int rand_evidentiat = 0;

    char cale_fisier_selectat[PATH_MAX] = "";
    char cale_destinatie_selectata[PATH_MAX] = "";

    while (1)
    {
        clear();
        mvprintw(0, 0, "Director: %s", cale_curenta);

        continut_director(cale_curenta, rand_evidentiat);

        mvprintw(LINES - 2, 0 , "Comenzi disponibile:");
        mvprintw(LINES - 1, 0 , "c - copiaza, m - muta, d - stergere , s - arata continut fisier, q - iesire");


        int ch = getch();
        switch (ch)
        {
            case KEY_UP: 
                if (rand_evidentiat > 0) 
                    rand_evidentiat--;
                break;
            case KEY_DOWN: 
                rand_evidentiat++;
                break;
            case 10: 
            {  
                DIR *dir = opendir(cale_curenta);
                if (!dir) break;

                struct dirent *intrare;
                int index = 0;

                while ((intrare = readdir(dir)) != NULL)
                {
                    if (index == rand_evidentiat)
                    {
                        char cale_noua[PATH_MAX];
                        snprintf(cale_noua, sizeof(cale_noua), "%s/%s", cale_curenta, intrare->d_name);

                        struct stat file_stat;

                        if (stat(cale_noua, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
                        {
                            chdir(cale_noua);                               
                            getcwd(cale_curenta, sizeof(cale_curenta));     
                            rand_evidentiat = 0;
                        }
                        break;
                    }
                    index++;
                }
                closedir(dir);
                break;
            }
            case 'd': 
            {
                afiseaza_mesaj_unic("Stergere fisier.\n");
                DIR *dir = opendir(cale_curenta);
                if (!dir) break;

                struct dirent *intrare;
                int index = 0;
                char cale_de_sters[PATH_MAX];

                while ((intrare = readdir(dir)) != NULL)
                {
                    if (index == rand_evidentiat)
                    {
                        snprintf(cale_de_sters, sizeof(cale_de_sters), "%s/%s", cale_curenta, intrare->d_name);
                        struct stat file_stat;

                        if (stat(cale_de_sters, &file_stat) == 0)
                        {
                            if (confirmare_stergere(cale_de_sters)) { 
                                if (S_ISDIR(file_stat.st_mode))
                                {
                                    DIR *subdir = opendir(cale_de_sters);
                                    if (subdir)
                                    {
                                        struct dirent *sub_intrare;
                                        int is_empty = 1;
                                        while ((sub_intrare = readdir(subdir)) != NULL)
                                        {
                                            if (strcmp(sub_intrare->d_name, ".") != 0 && strcmp(sub_intrare->d_name, "..") != 0)
                                            {
                                                is_empty = 0;
                                                break;
                                            }
                                        }
                                        closedir(subdir);

                                        if (is_empty)
                                        {
                                            if (rmdir(cale_de_sters) == 0)
                                            {
                                                printw("Directorul a fost sters!\n");
                                            }
                                            else
                                            {
                                                printw("Eroare la stergerea directorului!\n");
                                            }
                                        }
                                        else
                                        {
                                            printw("Directorul nu este gol!\n");
                                        }
                                    }
                                }
                                else
                                {
                                    if (remove(cale_de_sters) == 0)
                                    {
                                        printw("Fisierul a fost sters!\n");
                                    }
                                    else
                                    {
                                        printw("Eroare la stergerea fisierului!\n");
                                    }
                                }
                            }
                        }
                        break;
                    }
                    index++;
                }
                closedir(dir);
                break;
            }
            case 'c': 
            {
                afiseaza_mesaj_unic("Copiere fisier.\n");
                DIR *dir = opendir(cale_curenta);
                if (!dir) break;

                struct dirent *intrare;
                int index = 0;

                while ((intrare = readdir(dir)) != NULL) {
                    if (index == rand_evidentiat) {
                        char cale_fisier[PATH_MAX];
                        snprintf(cale_fisier, sizeof(cale_fisier), "%s/%s", cale_curenta, intrare->d_name);

                        char cale_destinatie[PATH_MAX];
                        mvprintw(1, 0, "Introduceti destinatia pentru copiere: ");
                        echo();
                        curs_set(1);
                        getstr(cale_destinatie);
                        noecho();
                        curs_set(0);

                        struct stat dest_stat;
                        if (stat(cale_destinatie, &dest_stat) == 0 && S_ISDIR(dest_stat.st_mode)) {
                            char cale_destinatie_fisier[PATH_MAX];
                            snprintf(cale_destinatie_fisier, sizeof(cale_destinatie_fisier), "%s/%s", cale_destinatie, intrare->d_name);
                            
                            if (copie_fisier(cale_fisier, cale_destinatie_fisier)) {
                                printw("Fisier copiat cu succes!\n");
                            } else {
                                printw("Eroare la copierea fisierului!\n");
                            }
                        } else {
                            printw("Destinatia nu este un director valid.\n");
                        }
                        break;
                    }
                    index++;
                }
                closedir(dir);
                break;
            }
            case 's': 
            {
                DIR *dir = opendir(cale_curenta);
                if(!dir) break;

                struct dirent *intrare;
                int index = 0;
                char cale_de_vizualizat[PATH_MAX];

                while((intrare = readdir(dir)) != NULL)
                {
                    if(index == rand_evidentiat)
                    {
                        snprintf(cale_de_vizualizat, sizeof(cale_de_vizualizat), "%s/%s", cale_curenta, intrare->d_name);
                        struct stat file_stat;

                        if(stat(cale_de_vizualizat, &file_stat) == 0 && S_ISREG(file_stat.st_mode))
                        {
                            FILE *fisier = fopen(cale_de_vizualizat, "r");
                            if(fisier)
                            {
                                clear();
                                mvprintw(0, 0, "Continut fisier: %s ( 'q' pentru a iesi din vizualizare)", cale_de_vizualizat);
                                int linie= 1;
                                char buffer[1024];

                                while(fgets(buffer, sizeof(buffer), fisier))
                                {
                                    if (linie >= LINES - 1)
                                    {
                                        mvprintw(LINES - 1, 0, "--Mai mult-- (ENTER)");
                                        refresh();
                                        int tasta = getch();
                                        if(tasta == 'q') break;
                                        clear();
                                        mvprintw(0, 0, "Continutul fisierului: %s ('q' pentru a reveni)", cale_de_vizualizat);
                                        linie = 1;
                                    }
                                    mvprintw(linie++, 0,"%s", buffer);
                                }
                                fclose(fisier);

                                while (getch() != 'q');
                                clear(); 
                            }
                            else
                            {
                                afiseaza_mesaj_unic("Eroare la deschiderea fisierului.");
                                getch();
                            }
                        }
                        else
                        {
                            afiseaza_mesaj_unic("Elementul selectat nu este un fisier.");
                            getch();
                        }
                        break;
                    }
                    index++;
                }
                closedir(dir);
                clear(); 
                refresh();
                break;
            }


            case 'm': 
            {
                afiseaza_mesaj_unic("Mutare fisier.\n");
                DIR *dir = opendir(cale_curenta);
                if (!dir) break;

                struct dirent *intrare;
                int index = 0;

                while ((intrare = readdir(dir)) != NULL) {
                    if (index == rand_evidentiat) {
                        char cale_fisier[PATH_MAX];
                        snprintf(cale_fisier, sizeof(cale_fisier), "%s/%s", cale_curenta, intrare->d_name);

                        char cale_destinatie[PATH_MAX];
                        mvprintw(1, 0, "Introdu destinatia pentru mutare: ");
                        echo();
                        curs_set(1);
                        getstr(cale_destinatie);
                        noecho();
                        curs_set(0);

                        struct stat dest_stat;
                        if (stat(cale_destinatie, &dest_stat) == 0 && S_ISDIR(dest_stat.st_mode)) {
                            char cale_destinatie_fisier[PATH_MAX];
                            snprintf(cale_destinatie_fisier, sizeof(cale_destinatie_fisier), "%s/%s", cale_destinatie, intrare->d_name);
                            
                            if (copie_fisier(cale_fisier, cale_destinatie_fisier)) {
                                if (remove(cale_fisier) == 0) {
                                    printw("Fisier mutat cu succes!\n");
                                }
                                else
                                {
                                    printw("Eroare la stergerea fisierului original!\n");
                                }
                            }
                            else
                            {
                                printw("Eroare mutare fisier!\n");
                            }
                        } else {
                            printw("Destinatia nu este director.\n");
                        }
                        break;
                    }
                    index++;
                }
                closedir(dir);
                break;
            }
            case 'q': 
                return;
        }

        refresh();
    }
}


int main() {
    init_interface_ncurses();
    navigare_fisiere();
    cleanup_interface_ncurses();
    return 0;
}
