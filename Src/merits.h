#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include "protocol.h"

/* Merits -- Enigma/Leanna */

// Mental Universal Merits
#define MERIT_AREAOFEXPERTISE          0
#define MERIT_COMMONSENSE              1
#define MERIT_DANGERSENSE              2
#define MERIT_DIRECTIONSENSE           3
#define MERIT_EIDETICMEMORY            4
#define MERIT_ENCYCLOPEDICKNOWLEDGE    5
#define MERIT_EYEFORTHESTRANGE         6
#define MERIT_FASTREFLEXES             7
#define MERIT_GOODTIMEMANAGEMENT       8
#define MERIT_HOLISTICAWARENESS        9
#define MERIT_INDOMITABLE             10
#define MERIT_INTERDISCIPLINARY       11 // Interdisciplinary Specialty
#define MERIT_INVESTIGATIVEA          12 // Investigative Aide
#define MERIT_INVESTIGATIVEP          13 // Investigative Prodigy
#define MERIT_LANGUAGE                14
#define MERIT_LIBRARY                 15
#define MERIT_ADVANCEDLIBRARY         16
#define MERIT_MEDITATIVEMIND          17
#define MERIT_MULTILINGUAL            18
#define MERIT_PATIENT                 19
#define MERIT_TOLERANCEFORBIO         20 // Tolerance for Biology
#define MERIT_TRAINEDOBSERVER         21
#define MERIT_VICERIDDEN              22
#define MERIT_VIRTUOUS                23
// Physical Universal Merits
#define MERIT_AMBIDEXTROUS            24
#define MERIT_AUTOMATIVEGENIUS        25
#define MERIT_CRACKDRIVER             26
#define MERIT_DEMOLISHER              27
#define MERIT_DOUBLEJOINTED           28
#define MERIT_FLEETOFFOOT             29
#define MERIT_GIANT                   30
#define MERIT_HARDY                   31
#define MERIT_GREYHOUND               32
#define MERIT_IRONSTAMINA             33
#define MERIT_QUICKDRAW               34
#define MERIT_RELENTLESS              35
#define MERIT_SEIZINGTHEEDGE          36
#define MERIT_SLEIGHTOFHAND           37
#define MERIT_SMALLFRAMED             38
// Social Universal Merits
#define MERIT_ALLIES                  39
#define MERIT_ALTERNATEIDENTITY       40
#define MERIT_ANONYMITY               41
#define MERIT_BARFLY                  42
#define MERIT_CLOSEDBOOK              43
#define MERIT_CONTACTS                44
#define MERIT_FAME                    45
#define MERIT_FIXER                   46
#define MERIT_HOBBYISTCLIQUE          47
#define MERIT_INSPIRING               48
#define MERIT_IRONWILL                49
#define MERIT_MENTOR                  50
#define MERIT_PUSHER                  51
#define MERIT_RESOURCES               52
#define MERIT_RETAINER                53
#define MERIT_SAFEPLACE               54
#define MERIT_SMALLUNITTACTICS        55
#define MERIT_SPINDOCTOR              56
#define MERIT_STAFF                   57
#define MERIT_STATUS                  58
#define MERIT_STRIKINGLOOKS           59
#define MERIT_SYMPATHETIC             60
#define MERIT_TABLETURNER             61
#define MERIT_TAKESONETOKNOWONE       62
#define MERIT_TASTE                   63
#define MERIT_TRUEFRIEND              64
#define MERIT_UNTOUCHABLE             65
#define MERIT_RUNECASTER              66
// Fighting style Merits
#define MERIT_CHEAPSHOT               67
#define MERIT_CHOKEHOLD               68
#define MERIT_DEFENSIVECOMBAT         69
#define MERIT_FIGHTINGFINESSE         70
#define MERIT_IRONSKIN                71
#define MERIT_KILLERINSTINCT          72
#define MERIT_SHIV                    73
#define MERIT_SUBDUINGSTRIKES         74
// Mortal+ Merits
#define MERIT_CLEARSIGHTED            75
#define MERIT_WEAKENEDBOND            76
#define MERIT_PRODUCER                77
#define MERIT_BEASTWHISPERS           78
#define MERIT_BELOVED                 79
#define MERIT_PROTECTED               80
#define MERIT_ANIMALSPEECH            81
#define MERIT_AURAREADING             82
#define MERIT_AUTOMATICWRITING        83
#define MERIT_BIOKINESIS              84
#define MERIT_CITYWALKER              85
#define MERIT_CLAIRVOYANCE            86
#define MERIT_CURSED                  87
#define MERIT_LAYINGONHANDS           88
#define MERIT_MEDIUM                  89
#define MERIT_MINDOFAMADMAN           90
#define MERIT_OMENSENSITIVITY         91
#define MERIT_NUMBINGTOUCH            92
#define MERIT_PSYCHOKINESIS           93
#define MERIT_PSYCHOMETRY             94
#define MERIT_TELEKINESIS             95
#define MERIT_TELEPATHY               96
#define MERIT_THIEFOFFATE             97
#define MERIT_UNSEENSENSE             98
#define MERIT_MAX		              98
