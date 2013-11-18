LO41
====

Compilation :
=============
make mrproper
make all

Exécution :
===========
Attention : Le nombre de GT lançés doit être identique au nombre de pilotes lançés.

1. Exécution du programme central : ./central
2. Exécution des groupes de traitements : ./gt [N Nombre de GT à lancer] [Nom du fichier de configuration]
3. Exécution des pilotes : ./pilote [N Nombre de pilotes à lancer] [Attente minimum d'un pilote après la transmission d'un appel] [Attente maximum d'un pilote après transmission d'un appel] [Durée minimum d'un appel] [Durée maximum d'un appel]