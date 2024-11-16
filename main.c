#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    int minute;
    int heure;
    int jour;
} Horaire;
typedef struct
{
    int id;
    Horaire horaireDateSouhaite;
    Horaire dureeHeureMinute;
    int typeActivite;
    int idZoneSrc;
    int idZoneDest;
    int idLigne;
    int sens;
} Activite;

typedef struct
{
    int idLigne;
    int idZoneDest;
    struct destination *suivant;
} destination;
typedef destination *pileDestinations;
typedef struct
{
    int cin;
    int idZoneHabitat;
    int idZoneTravail;
    int idZoneCourante;
    int intentionVoyage;
    int idLigneCourante;
    int sensCourant;
    int idZoneDest;
    pileDestinations destinationsIntermediaires;
    int nombreActivites;
    Activite activites[8];
} Personne;

typedef struct
{
    int zoneSrc;
    int zoneDest;
    int idLigne;
    int sens;
    int idBus;
    Horaire horaireVente;
    Horaire horaireEmbarquement;
    int distance;
    int prix;
} Ticket;
typedef struct
{
    int cinPersonne;
    Ticket tk;
    struct emplacement *suivant;
} emplacement;
typedef struct
{
    emplacement *tete;
    emplacement *queue;
} filePassagers;
typedef struct
{
    filePassagers file;
    int idLigne;
    int sens;
    struct celluleFile *suivant;
} celluleFile;
typedef celluleFile *listeFilesPassagers;
typedef struct
{
    int id;
    int row;
    int column;
    int nombrePersonnes;
    Personne personnes[100];
    listeFilesPassagers listeFilesPassagersEnAttente;
} Zone;
typedef struct
{
    int row;
    int column;
    int codeRue;
} Subdivision;
typedef struct
{
    int idZoneSrc;
    int idZoneDest;
    int distance;
    Horaire dureeApproximative;
    int nombresubdivisions;
    Subdivision parcours[80];
} Troncon;

typedef struct
{
    Horaire horaireAlerte;
    int nombrePassagersEnAttente;
    int maxDureeAttente;
    int minDureeAttente;
    int moyenneDureeAttente;
    struct alerte *suivant;
} alerte;
typedef alerte *pileAlertes;
typedef struct
{
    int id;
    int nombreZones;
    int idZones[10];
    pileAlertes pileAlertesSensAller;
    pileAlertes pileAlertesSensRetour;
    int gainNet;
} Ligne;
typedef struct
{
    int id;
    int idLigne;
    int sens;
    Horaire horaireDateDepart;
} Tache;

typedef struct
{
    int id;
    int idZoneCourante;
    int row;
    int column;
    int enRoute;
    int idLigneCourante;
    int sensCourant;
    int nombreTaches;
    Tache taches[10];
    int nombrePassagers;
    Personne passagers[20];
    int nombreTickets;
    Ticket tickets[400];
    int distanceparcourue;
    int coutsEtChargesParJour;
    int prixLitreCarburant;
    int gainNet;
} Bus;
typedef struct
{
    int rue;
    int zone;
    int bus;
} CaseCarte;

typedef struct
{
    struct liaison *precedent;
    int idLigne;
    int idZoneLiaisonLigneSuivante;
    struct liaison *suivant;
} liaison;
typedef struct
{
    liaison *premier; // pointeur sur le d�but de la liste
    liaison *dernier; // pointeur sur la fin de la liste
} listeLiaisons;

void chargerZones(Zone zones[], int *nombreZones)
{
    FILE *file = fopen("zones.txt", "r");
    if (file == NULL)
    {
        printf("Error Zone");
        exit(1);
    }

    while (!feof(file))
    {
        fscanf(file, "%d %d %d", &zones[*nombreZones].id, &zones[*nombreZones].row, &zones[*nombreZones].column);
        zones[*nombreZones].nombrePersonnes = 0;
        (*nombreZones)++;
    }
    fclose(file);
}
void chargerLignes(Ligne lignes[], int *nombreLignes)
{

    FILE *file = fopen("Lignes.txt", "r");
    if (file == NULL)
    {
        printf("Error Ligne");
        exit(1);
    }

    while (fscanf(file, "%d %d", &lignes[*nombreLignes].id, &lignes[*nombreLignes].nombreZones) == 2)
    {

        for (int i = 0; i < lignes[*nombreLignes].nombreZones; i++)
        {
            if (fscanf(file, "%d", &lignes[*nombreLignes].idZones[i]) != 1)
            {
                break;
            }
        }

        (*nombreLignes)++;
    }

    fclose(file);
}

void chargerTroncons(Troncon troncons[], int *nombreTroncons)
{
    FILE *file = fopen("Troncons.txt", "r");
    if (file == NULL)
    {
        printf("Error Troncons");
        exit(1);
    }
    int test = 0, i = 0;
    while (!feof(file))
    {
        if (test == 0)
        {
            fscanf(file, "%d %d %d %d %d %d %d", &troncons[*nombreTroncons].idZoneSrc, &troncons[*nombreTroncons].idZoneDest, &troncons[*nombreTroncons].distance, &troncons[*nombreTroncons].dureeApproximative.jour, &troncons[*nombreTroncons].dureeApproximative.heure, &troncons[*nombreTroncons].dureeApproximative.minute, &troncons[*nombreTroncons].nombresubdivisions);
            test = troncons[*nombreTroncons].nombresubdivisions;
        }
        else
        {
            if (test > 1)
            {
                fscanf(file, "%d %d %d", &troncons[*nombreTroncons].parcours[i].row, &troncons[*nombreTroncons].parcours[i].column, &troncons[*nombreTroncons].parcours[i].codeRue);
                test--;
                i++;
            }
            else
            {
                fscanf(file, "%d %d %d", &troncons[*nombreTroncons].parcours[i].row, &troncons[*nombreTroncons].parcours[i].column, &troncons[*nombreTroncons].parcours[i].codeRue);
                test--;
                i = 0;
                (*nombreTroncons)++;
            }
        }
    }
    fclose(file);
}
void chargerFlotteBus(Bus flotteBus[], int *nombreBus)
{
    FILE *file = fopen("Flottebus.txt", "r");
    if (file == NULL)
    {
        printf("Error Bus");
        exit(1);
    }
    int test = 0, i = 0;
    while (!feof(file))
    {
        if (test == 0)
        {
            fscanf(file, "%d %d %d %d %d %d %d %d", &flotteBus[*nombreBus].id, &flotteBus[*nombreBus].idZoneCourante, &flotteBus[*nombreBus].row, &flotteBus[*nombreBus].column, &flotteBus[*nombreBus].enRoute, &flotteBus[*nombreBus].idLigneCourante, &flotteBus[*nombreBus].sensCourant, &flotteBus[*nombreBus].nombreTaches);
            test = flotteBus[*nombreBus].nombreTaches;
        }
        else
        {
            if (test > 1)
            {
                fscanf(file, "%d %d %d %d %d %d", &flotteBus[*nombreBus].taches[i].id, &flotteBus[*nombreBus].taches[i].idLigne, &flotteBus[*nombreBus].taches[i].sens, &flotteBus[*nombreBus].taches[i].horaireDateDepart.jour, &flotteBus[*nombreBus].taches[i].horaireDateDepart.heure, &flotteBus[*nombreBus].taches[i].horaireDateDepart.minute);
                i++;
                test--;
            }
            else
            {
                fscanf(file, "%d %d %d %d %d %d", &flotteBus[*nombreBus].taches[i].id, &flotteBus[*nombreBus].taches[i].idLigne, &flotteBus[*nombreBus].taches[i].sens, &flotteBus[*nombreBus].taches[i].horaireDateDepart.jour, &flotteBus[*nombreBus].taches[i].horaireDateDepart.heure, &flotteBus[*nombreBus].taches[i].horaireDateDepart.minute);
                flotteBus[*nombreBus].nombrePassagers = 0;
                i = 0;
                test--;
                (*nombreBus)++;
            }
        }
    }
    fclose(file);
}
void afficherZones(Zone zones[], int nombreZones)
{
    for (int i = 0; i < nombreZones; i++)
    {
        printf("%d %d %d\n", zones[i].id, zones[i].row, zones[i].column);
    }
}
void afficherLignes(Ligne lignes[], int nombreLignes)
{
    for (int i = 0; i < nombreLignes; i++)
    {
        printf("%d %d\n", lignes[i].id, lignes[i].nombreZones);
        for (int j = 0; j < lignes[i].nombreZones; j++)
        {
            printf(" %d\n", lignes[i].idZones[j]);
        }
    }
}
void afficherTroncons(Troncon troncons[], int nombreTroncons)
{
    for (int i = 0; i < nombreTroncons; i++)
    {
        printf("%d %d %d %d %d %d %d\n", troncons[i].idZoneSrc, troncons[i].idZoneDest, troncons[i].distance, troncons[i].dureeApproximative.jour, troncons[i].dureeApproximative.heure, troncons[i].dureeApproximative.minute, troncons[i].nombresubdivisions);
        for (int j = 0; j < troncons[i].nombresubdivisions; j++)
        {
            printf("%d %d %d\n", troncons[i].parcours[j].row, troncons[i].parcours[j].column, troncons[i].parcours[j].codeRue);
        }
    }
}
void afficherFlotteBus(Bus flotteBus[], int nombreBus)
{
    for (int i = 0; i < nombreBus; i++)
    {
        printf("%d %d %d %d %d %d %d %d\n", flotteBus[i].id, flotteBus[i].idZoneCourante, flotteBus[i].row, flotteBus[i].column, flotteBus[i].enRoute, flotteBus[i].idLigneCourante, flotteBus[i].sensCourant, flotteBus[i].nombreTaches);
        for (int j = 0; j < flotteBus[i].nombreTaches; j++)
        {
            printf("%d %d %d %d %d %d\n", flotteBus[i].taches[j].id, flotteBus[i].taches[j].idLigne, flotteBus[i].taches[j].sens, flotteBus[i].taches[j].horaireDateDepart.jour, flotteBus[i].taches[j].horaireDateDepart.heure, flotteBus[i].taches[j].horaireDateDepart.minute);
        }
    }
}
void sauvegarderZones(Zone zones[], int nombreZones, char *nomFichier)
{
    FILE *file = fopen(nomFichier, "w");
    for (int i = 0; i < nombreZones - 1; i++)
    {
        fprintf(file, "%d %d %d\n", zones[i].id, zones[i].row, zones[i].column);
    }
    fprintf(file, "%d %d %d", zones[nombreZones - 1].id, zones[nombreZones - 1].row, zones[nombreZones - 1].column);
    fclose(file);
}
void sauvegarderLignes(Ligne lignes[], int nombreLignes, char *nomFichier)
{
    FILE *file = fopen(nomFichier, "w");
    for (int i = 0; i < nombreLignes - 1; i++)
    {
        fprintf(file, "%d %d\n", lignes[i].id, lignes[i].nombreZones);
        for (int j = 0; j < lignes[i].nombreZones; j++)
        {
            fprintf(file, "%d\n", lignes[i].idZones[j]);
        }
    }
    fprintf(file, "%d %d\n", lignes[nombreLignes - 1].id, lignes[nombreLignes - 1].nombreZones);
    for (int i = 0; i < (lignes[nombreLignes - 1].nombreZones) - 1; i++)
    {
        fprintf(file, "%d\n", lignes[nombreLignes - 1].idZones[i]);
    }
    fprintf(file, "%d", lignes[nombreLignes - 1].idZones[(lignes[nombreLignes - 1].nombreZones) - 1]);

    fclose(file);
}

void sauvegarderTroncons(Troncon troncons[], int nombreTroncons, char *nomFichier)
{
    FILE *file = fopen(nomFichier, "w");
    for (int i = 0; i < nombreTroncons - 1; i++)
    {
        fprintf(file, "%d %d %d %d %d %d %d\n", troncons[i].idZoneSrc, troncons[i].idZoneDest, troncons[i].distance, troncons[i].dureeApproximative.jour, troncons[i].dureeApproximative.heure, troncons[i].dureeApproximative.minute, troncons[i].nombresubdivisions);
        for (int j = 0; j < troncons[i].nombresubdivisions; j++)
        {
            fprintf(file, "%d %d %d\n", troncons[i].parcours[j].row, troncons[i].parcours[j].column, troncons[i].parcours[j].codeRue);
        }
    }
    fprintf(file, "%d %d %d %d %d %d %d\n", troncons[nombreTroncons - 1].idZoneSrc, troncons[nombreTroncons - 1].idZoneDest, troncons[nombreTroncons - 1].distance, troncons[nombreTroncons - 1].dureeApproximative.jour, troncons[nombreTroncons - 1].dureeApproximative.heure, troncons[nombreTroncons - 1].dureeApproximative.minute, troncons[nombreTroncons - 1].nombresubdivisions);
    for (int i = 0; i < (troncons[nombreTroncons - 1].nombresubdivisions) - 1; i++)
    {
        fprintf(file, "%d %d %d\n", troncons[nombreTroncons - 1].parcours[i].row, troncons[nombreTroncons - 1].parcours[i].column, troncons[nombreTroncons - 1].parcours[i].codeRue);
    }
    fprintf(file, "%d %d %d", troncons[nombreTroncons - 1].parcours[(troncons[nombreTroncons - 1].nombresubdivisions) - 1].row, troncons[nombreTroncons - 1].parcours[(troncons[nombreTroncons - 1].nombresubdivisions) - 1].column, troncons[nombreTroncons - 1].parcours[(troncons[nombreTroncons - 1].nombresubdivisions) - 1].codeRue);
    fclose(file);
}

void sauvegarderFlotteBus(Bus flotteBus[], int nombreBus, char *nomFichier)
{
    FILE *file = fopen(nomFichier, "w");
    for (int i = 0; i < nombreBus - 1; i++)
    {
        fprintf(file, "%d %d %d %d %d %d %d %d\n", flotteBus[i].id, flotteBus[i].idZoneCourante, flotteBus[i].row, flotteBus[i].column, flotteBus[i].enRoute, flotteBus[i].idLigneCourante, flotteBus[i].sensCourant, flotteBus[i].nombreTaches);
        for (int j = 0; j < flotteBus[i].nombreTaches; j++)
        {
            fprintf(file, "%d %d %d %d %d %d\n", flotteBus[i].taches[j].id, flotteBus[i].taches[j].idLigne, flotteBus[i].taches[j].sens, flotteBus[i].taches[j].horaireDateDepart.jour, flotteBus[i].taches[j].horaireDateDepart.heure, flotteBus[i].taches[j].horaireDateDepart.minute);
        }
    }
    fprintf(file, "%d %d %d %d %d %d %d %d\n", flotteBus[nombreBus - 1].id, flotteBus[nombreBus - 1].idZoneCourante, flotteBus[nombreBus - 1].row, flotteBus[nombreBus - 1].column, flotteBus[nombreBus - 1].enRoute, flotteBus[nombreBus - 1].idLigneCourante, flotteBus[nombreBus - 1].sensCourant, flotteBus[nombreBus - 1].nombreTaches);
    for (int i = 0; i < (flotteBus[nombreBus - 1].nombreTaches) - 1; i++)
    {
        fprintf(file, "%d %d %d %d %d %d\n", flotteBus[nombreBus - 1].taches[i].id, flotteBus[nombreBus - 1].taches[i].idLigne, flotteBus[nombreBus - 1].taches[i].sens, flotteBus[nombreBus - 1].taches[i].horaireDateDepart.jour, flotteBus[nombreBus - 1].taches[i].horaireDateDepart.heure, flotteBus[nombreBus - 1].taches[i].horaireDateDepart.minute);
    }
    fprintf(file, "%d %d %d %d %d %d", flotteBus[nombreBus - 1].taches[(flotteBus[nombreBus - 1].nombreTaches) - 1].id, flotteBus[nombreBus - 1].taches[(flotteBus[nombreBus - 1].nombreTaches) - 1].idLigne, flotteBus[nombreBus - 1].taches[(flotteBus[nombreBus - 1].nombreTaches) - 1].sens, flotteBus[nombreBus - 1].taches[(flotteBus[nombreBus - 1].nombreTaches) - 1].horaireDateDepart.jour, flotteBus[nombreBus - 1].taches[(flotteBus[nombreBus - 1].nombreTaches) - 1].horaireDateDepart.heure, flotteBus[nombreBus - 1].taches[(flotteBus[nombreBus - 1].nombreTaches) - 1].horaireDateDepart.minute);
    fclose(file);
}
void affecterPersonneZone(Zone zones[], Personne pers)
{
    int j = zones[pers.idZoneHabitat].nombrePersonnes;
    zones[pers.idZoneHabitat].personnes[j] = pers;
    zones[pers.idZoneHabitat].nombrePersonnes++;
}
void genererPersonne(Zone zones[], int nombreZones, Ligne lignes[], int nombreLignes)
{
    srand(time(NULL));
    Personne pers;
    int indexligne = 0, indexzoneligne = 0;
    for (int i = 0; i < 300; i++)
    {
        pers.cin = i + 1;
        pers.idZoneHabitat = rand() % 20;
        do
        {
            pers.idZoneTravail = rand() % 20;
        } while (pers.idZoneHabitat == pers.idZoneTravail);
        pers.idZoneCourante = pers.idZoneHabitat;
        pers.intentionVoyage = 0;
        while (indexligne < nombreLignes)
        {
            while (indexzoneligne < lignes[indexligne].nombreZones)
            {
                if (lignes[indexligne].idZones[indexzoneligne] == pers.idZoneHabitat)
                {
                    pers.idLigneCourante = lignes[indexligne].id;
                    indexligne = 0;
                    indexzoneligne = 0;
                    break;
                }
                indexzoneligne++;
            }
            indexligne++;
        }
        pers.sensCourant = -1;
        pers.nombreActivites = 3;
        pers.activites[0].id = 0;
        pers.activites[0].horaireDateSouhaite.jour = 0;
        pers.activites[0].horaireDateSouhaite.heure = 6 + rand() % 3;
        int nb = rand() % 3;
        int values[] = {0, 15, 30};
        pers.activites[0].horaireDateSouhaite.minute = 0 + values[nb];
        pers.activites[0].dureeHeureMinute.jour = 0;
        pers.activites[0].dureeHeureMinute.heure = 8;
        pers.activites[0].dureeHeureMinute.minute = 0;
        pers.activites[0].typeActivite = 1;
        pers.activites[0].idZoneSrc = pers.idZoneHabitat;
        pers.activites[0].idZoneDest = pers.idZoneTravail;
        int l, s;
        while (indexligne < nombreLignes)
        {
            while (indexzoneligne < lignes[indexligne].nombreZones)
            {
                if (lignes[indexligne].idZones[indexzoneligne] == pers.activites[0].idZoneDest)
                {
                    l = lignes[indexligne].id;
                    indexligne = 0;
                    indexzoneligne = 0;
                    break;
                }
                indexzoneligne++;
            }
            indexligne++;
        }
        s = 0; // rechercherLigneSens(pers.activites[0].idZoneSrc, pers.activites[0].idZoneDest, lignes, nombreLignes, l);
        pers.activites[0].idLigne = l;
        pers.activites[0].sens = s;

        pers.activites[1].id = 1;
        pers.activites[1].horaireDateSouhaite.jour = 0;
        pers.activites[1].horaireDateSouhaite.heure = 18;
        nb = rand() % 3;
        pers.activites[1].horaireDateSouhaite.minute = 0 + values[nb];
        pers.activites[1].dureeHeureMinute.jour = 0;
        pers.activites[1].dureeHeureMinute.heure = 2;
        pers.activites[1].dureeHeureMinute.minute = 0;
        pers.activites[1].typeActivite = 2;
        pers.activites[1].idZoneSrc = pers.idZoneTravail;
        do
        {
            pers.activites[1].idZoneDest = rand() % 10;

        } while (pers.activites[1].idZoneDest == pers.activites[1].idZoneSrc);

        s = 0; // rechercherLigneSens(pers.activites[1].idZoneSrc, pers.activites[1].idZoneDest, lignes, nombreLignes, l);
        while (indexligne < nombreLignes)
        {
            while (indexzoneligne < lignes[indexligne].nombreZones)
            {
                if (lignes[indexligne].idZones[indexzoneligne] == pers.activites[1].idZoneDest)
                {
                    l = lignes[indexligne].id;
                    indexligne = 0;
                    indexzoneligne = 0;
                    break;
                }
                indexzoneligne++;
            }
            indexligne++;
        }
        pers.activites[1].idLigne = l;
        pers.activites[1].sens = s;

        pers.activites[2].id = 2;
        pers.activites[2].horaireDateSouhaite.jour = 0;
        pers.activites[2].horaireDateSouhaite.heure = 20;
        nb = rand() % 3;
        pers.activites[2].horaireDateSouhaite.minute = 0 + values[nb];
        pers.activites[2].dureeHeureMinute.jour = 0;
        pers.activites[2].dureeHeureMinute.heure = 10;
        pers.activites[2].dureeHeureMinute.minute = 0;
        pers.activites[2].typeActivite = 0;
        pers.activites[2].idZoneSrc = pers.activites[1].idZoneDest;
        pers.activites[2].idZoneDest = pers.idZoneHabitat;
        while (indexligne < nombreLignes)
        {
            while (indexzoneligne < lignes[indexligne].nombreZones)
            {
                if (lignes[indexligne].idZones[indexzoneligne] == pers.activites[2].idZoneDest)
                {
                    l = lignes[indexligne].id;
                    indexligne = 0;
                    indexzoneligne = 0;
                    break;
                }
                indexzoneligne++;
            }
            indexligne++;
        }
        s = 0; // rechercherLigneSens(pers.activites[2].idZoneSrc, pers.activites[2].idZoneDest, lignes, nombreLignes, l);
        pers.activites[2].idLigne = l;
        pers.activites[2].sens = s;

        affecterPersonneZone(zones, pers);
    }
}
void afficherPersonnes(Zone zones[], int nombreZones)
{
    for (int i = 0; i < nombreZones; i++)
    {
        for (int j = 0; j < zones[i].nombrePersonnes; j++)
        {
            printf("%d %d %d %d %d %d %d %d\n", zones[i].personnes[j].cin, zones[i].personnes[j].idZoneHabitat, zones[i].personnes[j].idZoneTravail, zones[i].personnes[j].idZoneCourante, zones[i].personnes[j].intentionVoyage, zones[i].personnes[j].idLigneCourante, zones[i].personnes[j].sensCourant, zones[i].personnes[j].nombreActivites);
            for (int k = 0; k < zones[i].personnes[j].nombreActivites; k++)
            {
                printf("%d %d %d %d %d %d %d %d %d %d %d %d\n", zones[i].personnes[j].activites[k].id, zones[i].personnes[j].activites[k].horaireDateSouhaite.jour, zones[i].personnes[j].activites[k].horaireDateSouhaite.heure, zones[i].personnes[j].activites[k].horaireDateSouhaite.minute, zones[i].personnes[j].activites[k].dureeHeureMinute.jour, zones[i].personnes[j].activites[k].dureeHeureMinute.heure, zones[i].personnes[j].activites[k].dureeHeureMinute.minute, zones[i].personnes[j].activites[k].typeActivite, zones[i].personnes[j].activites[k].idZoneSrc, zones[i].personnes[j].activites[k].idZoneDest, zones[i].personnes[j].activites[k].idLigne, zones[i].personnes[j].activites[k].sens);
            }
        }
    }
}
void sauvegarderPersonnes(Zone zones[], int nombreZones, char *nomFichier)
{
    FILE *file = fopen(nomFichier, "w");
    for (int i = 0; i < nombreZones - 1; i++)
    {
        fprintf(file, "%d %d %d %d\n", zones[i].id, zones[i].row, zones[i].column, zones[i].nombrePersonnes);
        for (int j = 0; j < zones[i].nombrePersonnes; j++)
        {
            fprintf(file, "%d %d %d %d %d %d %d %d\n", zones[i].personnes[j].cin, zones[i].personnes[j].idZoneHabitat, zones[i].personnes[j].idZoneTravail, zones[i].personnes[j].idZoneCourante, zones[i].personnes[j].intentionVoyage, zones[i].personnes[j].idLigneCourante, zones[i].personnes[j].sensCourant, zones[i].personnes[j].nombreActivites);
            for (int k = 0; k < zones[i].personnes[j].nombreActivites; k++)
            {
                fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d\n", zones[i].personnes[j].activites[k].id, zones[i].personnes[j].activites[k].horaireDateSouhaite.jour, zones[i].personnes[j].activites[k].horaireDateSouhaite.heure, zones[i].personnes[j].activites[k].horaireDateSouhaite.minute, zones[i].personnes[j].activites[k].dureeHeureMinute.jour, zones[i].personnes[j].activites[k].dureeHeureMinute.heure, zones[i].personnes[j].activites[k].dureeHeureMinute.minute, zones[i].personnes[j].activites[k].typeActivite, zones[i].personnes[j].activites[k].idZoneSrc, zones[i].personnes[j].activites[k].idZoneDest, zones[i].personnes[j].activites[k].idLigne, zones[i].personnes[j].activites[k].sens);
            }
        }
    }
    fprintf(file, "%d %d %d %d\n", zones[nombreZones - 1].id, zones[nombreZones - 1].row, zones[nombreZones - 1].column, zones[nombreZones - 1].nombrePersonnes);
    for (int j = 0; j < zones[nombreZones - 1].nombrePersonnes - 1; j++)
    {
        fprintf(file, "%d %d %d %d %d %d %d %d\n", zones[nombreZones - 1].personnes[j].cin, zones[nombreZones - 1].personnes[j].idZoneHabitat, zones[nombreZones - 1].personnes[j].idZoneTravail, zones[nombreZones - 1].personnes[j].idZoneCourante, zones[nombreZones - 1].personnes[j].intentionVoyage, zones[nombreZones - 1].personnes[j].idLigneCourante, zones[nombreZones - 1].personnes[j].sensCourant, zones[nombreZones - 1].personnes[j].nombreActivites);
        for (int k = 0; k < zones[nombreZones - 1].personnes[j].nombreActivites; k++)
        {
            fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d\n", zones[nombreZones - 1].personnes[j].activites[k].id, zones[nombreZones - 1].personnes[j].activites[k].horaireDateSouhaite.jour, zones[nombreZones - 1].personnes[j].activites[k].horaireDateSouhaite.heure, zones[nombreZones - 1].personnes[j].activites[k].horaireDateSouhaite.minute, zones[nombreZones - 1].personnes[j].activites[k].dureeHeureMinute.jour, zones[nombreZones - 1].personnes[j].activites[k].dureeHeureMinute.heure, zones[nombreZones - 1].personnes[j].activites[k].dureeHeureMinute.minute, zones[nombreZones - 1].personnes[j].activites[k].typeActivite, zones[nombreZones - 1].personnes[j].activites[k].idZoneSrc, zones[nombreZones - 1].personnes[j].activites[k].idZoneDest, zones[nombreZones - 1].personnes[j].activites[k].idLigne, zones[nombreZones - 1].personnes[j].activites[k].sens);
        }
    }
    fprintf(file, "%d %d %d %d %d %d %d %d\n", zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].cin, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].idZoneHabitat, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].idZoneTravail, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].idZoneCourante, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].intentionVoyage, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].idLigneCourante, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].sensCourant, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites);
    for (int k = 0; k < (zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites) - 1; k++)
    {
        fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d\n", zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].id, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].horaireDateSouhaite.jour, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].horaireDateSouhaite.heure, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].horaireDateSouhaite.minute, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].dureeHeureMinute.jour, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].dureeHeureMinute.heure, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].dureeHeureMinute.minute, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].typeActivite, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].idZoneSrc, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].idZoneDest, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].idLigne, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[k].sens);
    }
    fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d", zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].id, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].horaireDateSouhaite.jour, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].horaireDateSouhaite.heure, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].horaireDateSouhaite.minute, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].dureeHeureMinute.jour, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].dureeHeureMinute.heure, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].dureeHeureMinute.minute, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].typeActivite, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].idZoneSrc, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].idZoneDest, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].idLigne, zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].activites[zones[nombreZones - 1].personnes[zones[nombreZones - 1].nombrePersonnes - 1].nombreActivites - 1].sens);

    fclose(file);
}
void chargerPersonnes(Zone zones[], int *nombreZones, char *nomFichier)
{
    FILE *file = fopen(nomFichier, "r");
    if (file == NULL)
    {
        printf("Error ZonePersonne");
        exit(1);
    }
    (*nombreZones) = 0;
    while (!feof(file))
    {
        fscanf(file, "%d %d %d %d", &zones[*nombreZones].id, &zones[*nombreZones].row, &zones[*nombreZones].column, &zones[*nombreZones].nombrePersonnes);
        for (int j = 0; j < zones[*nombreZones].nombrePersonnes; j++)
        {
            fscanf(file, "%d %d %d %d %d %d %d %d", &zones[*nombreZones].personnes[j].cin, &zones[*nombreZones].personnes[j].idZoneHabitat, &zones[*nombreZones].personnes[j].idZoneTravail, &zones[*nombreZones].personnes[j].idZoneCourante, &zones[*nombreZones].personnes[j].intentionVoyage, &zones[*nombreZones].personnes[j].idLigneCourante, &zones[*nombreZones].personnes[j].sensCourant, &zones[*nombreZones].personnes[j].nombreActivites);
            for (int k = 0; k < zones[*nombreZones].personnes[j].nombreActivites; k++)
            {
                fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d", &zones[*nombreZones].personnes[j].activites[k].id, &zones[*nombreZones].personnes[j].activites[k].horaireDateSouhaite.jour, &zones[*nombreZones].personnes[j].activites[k].horaireDateSouhaite.heure, &zones[*nombreZones].personnes[j].activites[k].horaireDateSouhaite.minute, &zones[*nombreZones].personnes[j].activites[k].dureeHeureMinute.jour, &zones[*nombreZones].personnes[j].activites[k].dureeHeureMinute.heure, &zones[*nombreZones].personnes[j].activites[k].dureeHeureMinute.minute, &zones[*nombreZones].personnes[j].activites[k].typeActivite, &zones[*nombreZones].personnes[j].activites[k].idZoneSrc, &zones[*nombreZones].personnes[j].activites[k].idZoneDest, &zones[*nombreZones].personnes[j].activites[k].idLigne, &zones[*nombreZones].personnes[j].activites[k].sens);
            }
        }
        (*nombreZones)++;
    }

    fclose(file);
}
int main()
{
    CaseCarte(*cartegeo)[40] = malloc(40 * sizeof(CaseCarte[40]));
    Zone *zones = malloc(25 * sizeof(Zone));
    Bus *flottebus = malloc(25 * sizeof(Bus));
    Ligne *lignes = malloc(5 * sizeof(Ligne));
    Troncon *troncons = malloc(25 * sizeof(Troncon));
    Ticket *tickets = malloc(5000 * sizeof(Ticket));

    int nombreZones = 0, nombreLignes = 0, nombreTroncons = 0, nombreBus = 0;
    chdir("D:\\Iset\\S3\\Mini Projet\\Mini projet");
    chargerZones(zones, &nombreZones);

    chargerLignes(lignes, &nombreLignes);

    chargerTroncons(troncons, &nombreTroncons);

    chargerFlotteBus(flottebus, &nombreBus);

    sauvegarderZones(zones, nombreZones, "zones_sauv.txt");
    sauvegarderLignes(lignes, nombreLignes, "lignes_sauv.txt");
    sauvegarderTroncons(troncons, nombreTroncons, "troncons_sauv.txt");
    sauvegarderFlotteBus(flottebus, nombreBus, "flotteBus_sauv.txt");
    sauvegarderPersonnes(zones, nombreZones, "personnes_sauv.txt");


    afficherZones(zones, nombreZones);
    afficherLignes(lignes, nombreLignes);
    afficherTroncons(troncons, nombreTroncons);
    afficherFlotteBus(flottebus, nombreBus);
    return 0;
}
