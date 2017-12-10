/*  Atelier Photo - Travaux Pratiques UV MI01
 Copyright (C) 2005, 2017 S. Bonnet, Université de Technologie de Compiègne

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdarg.h>
#include "appli_r.h"

/*
 * Constantes symboliques
 */

/* Identifiants des menus */
#define IDM_FILE        0
#define IDM_FILEOPEN    10
#define IDM_APPEXIT     11
#define IDM_SHOW        1
#define IDM_SHOWSOURCE  21
#define IDM_SHOWTEMP_1  22
#define IDM_SHOWTEMP_2  23
#define IDM_SHOWDEST    24
#define IDM_PROCESS     2
#define IDM_LAUNCH_ASM  30
#define IDM_LAUNCH_C    31
#define IDM_LAUNCH_SIMD 32
#define IDM_SET_REPEAT  33

/*
 * Structures de données / définition de types
 */
/* ProcessedBitmap : bitmap 32 bits (0 R G B) ou 8 bits utilisé pour le traitement */
struct ProcessedBitmap {
  UINT biWidth;     /* Largeur */
  UINT biHeight;    /* Hauteur */
  BYTE* pBits;      /* Pixels  */
  HBITMAP hBitmap;  /* Handle pour l'affichage */
} ;

/* Structure d'association Menu<->Message d'aide */
struct MenuDescription {
  UINT uId;
  PSTR lpszText;
} MenuDescription;

/* Implémentations des fonctions de calcul */
enum ProcessingImplementationType { PROCESS_IMAGE_NONE, PROCESS_IMAGE_C, PROCESS_IMAGE_SIMD, PROCESS_IMAGE_ASM };

/*
 * Variables globales
 */

/* Messages d'aide des menus */
struct MenuDescription MenuDescriptions[] = {
  {IDM_FILEOPEN, TEXT("Charger l'image bitmap source")},
  {IDM_APPEXIT, TEXT("Quitter l'application")},
  {IDM_SHOWSOURCE, TEXT("Afficher l'image source")},
  {IDM_SHOWTEMP_1, TEXT("Afficher l'image intermédiaire 1")},
  {IDM_SHOWTEMP_2, TEXT("Afficher l'image intermédiaire 2")},
  {IDM_SHOWDEST, TEXT("Afficher l'image finale")},
  {IDM_LAUNCH_ASM, TEXT("Lancer l'implémentation assembleur du traitement")},
  {IDM_LAUNCH_C, TEXT("Lancer l'implémentation C du traitement")},
  {IDM_LAUNCH_SIMD, TEXT("Lancer l'implémentation SIMD du traitement")},
  {IDM_SET_REPEAT, TEXT("Programmer le nombre de répétitions du traitement")},
  {0, ""}};

/* Titre de l'application */
TCHAR szAppName[] = TEXT("Atelier photo");

/* Noms des images */
PSTR szBitmapNames[] = {
  TEXT("Image source"),
  TEXT("Image temporaire 1"),
  TEXT("Image temporaire 2"),
  TEXT("Image finale"),
};

/* Handle des fenêtres */
HWND hwndMain;
HWND hwndStatus;

/* Fréquence d'incrémentation des compteurs de performances (hz) */
LARGE_INTEGER PerformanceFrequency;

/* Nombre de répétitions du traitement */
UINT nRepeats = 1;

/* Bitmaps de travail */
struct ProcessedBitmap* Bitmaps[4] = {NULL, NULL, NULL, NULL};

/* Numéro du bitmap affiché */
UINT iVisibleBitmap = 0;

/*
 * Fonctions
 */

/* Déclarations de fonctions externes (définies dans d'autres fichiers source) */
extern UINT __cdecl process_image_asm(UINT biWidth, UINT biHeight,
  BYTE *img_src, BYTE *img_temp1, BYTE *img_temp2, BYTE *img_dest);

extern UINT __cdecl process_image_simd(UINT biWidth, UINT biHeight,
  BYTE *img_src, BYTE *img_temp1, BYTE *img_temp2, BYTE *img_dest);

LRESULT CALLBACK MainWndProc (HWND, UINT, WPARAM, LPARAM);

/* process_image_C
 *
 * Fonction de traitement réalisant le même traitement que le sous-programme
 * assembleur utilisée à titre de comparaison.
 *
 */
UINT __cdecl process_image_C(UINT biWidth, UINT biHeight, 
  BYTE *img_src, BYTE *img_temp1, BYTE *img_temp2, BYTE *img_dest)
{
  unsigned int c, n;
  unsigned int x, y;
  int sum;

  n = biWidth * biHeight * 4;

  for (c = 0; c < n; c = c + 4) {
    img_temp1[c] = (BYTE)((((int)img_src[c + 2]) * 0x4c
      + ((int)img_src[c + 1] * 0x96)
      + ((int)img_src[c] * 0x1d)) >> 8);
  }
  return 0;
  /* Détecteur de contours de Sobel */
  n = biWidth * 4;

  for (y = 0; y < biHeight - 2; y++) {
    for (x = 0; x < (biWidth - 2) * 4; x = x + 4) {
      sum = 255
        - (abs(-(int)((*(img_temp1 + (x + y * n))))
          + (int)((*(img_temp1 + (x + 8 + y * n))))
          - 2 * ((int)((*(img_temp1 + (x + (y + 1)* n)))))
          + 2 * ((int)((*(img_temp1 + (x + 8 + (y + 1) * n)))))
          - ((int)((*(img_temp1 + (x + (y + 2) * n)))))
          + ((int)((*(img_temp1 + (x + 8 + (y + 2) * n))))))
        + abs((int)((*(img_temp1 + (x + y * n))))
          + 2 * (int)((*(img_temp1 + (x + 4 + y * n))))
          + (int)((*(img_temp1 + (x + 8 + y *n))))
          - (int)((*(img_temp1 + (x + (y + 2) * n))))
          - 2 * (int)((*(img_temp1 + (x + 4 + (y + 2) * n))))
          - (int)((*(img_temp1 + (x + 8 + (y + 2) * n))))));

      if (sum < 0) {
        sum = 0;
      }

      *((int *)(img_temp2 + (x + 4 + (y + 1) * n))) = sum | (sum << 8) | (sum << 16);
    }
  }

  n = biWidth * biHeight * 4;

  for (c = 0; c < n; c = c + 4) {
    if (img_temp2[c] > 150) {
      *((int *)(img_dest + c)) = 0xffffff;
    }
    else {
      *((int *)(img_dest + c)) = 0;
    }
  }

  return 0;
}

/* CreateMenuBar
 *
 * Crée la barre de menu et retourne son handle.
 *
 */
HMENU CreateMenuBar(void) {
  HMENU hMenu;
  HMENU hSubMenu;

  hMenu = CreateMenu();

  hSubMenu = CreateMenu();
  AppendMenu(hSubMenu, MF_STRING, IDM_FILEOPEN, 
             "&Ouvrir l'image source...");
  AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
  AppendMenu(hSubMenu, MF_STRING, IDM_APPEXIT, 
             "&Quitter");

  AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hSubMenu, 
             "&Fichier");

  hSubMenu = CreateMenu();
  AppendMenu(hSubMenu, MF_STRING, IDM_SHOWSOURCE, 
             "Image &source");
  AppendMenu(hSubMenu, MF_STRING, IDM_SHOWTEMP_1, 
             "Image intermédiaire &1");
  AppendMenu(hSubMenu, MF_STRING, IDM_SHOWTEMP_2, 
             "Image intermédiaire &2");
  AppendMenu(hSubMenu, MF_STRING, IDM_SHOWDEST, 
             "Image &finale");

  AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hSubMenu, 
             "&Affichage");

  hSubMenu = CreateMenu();
  AppendMenu(hSubMenu, MF_STRING, IDM_LAUNCH_ASM,
             "Lancer l'implémentation &assembleur");
  AppendMenu(hSubMenu, MF_STRING, IDM_LAUNCH_SIMD,
             "Lancer l'implémentation &SIMD");
  AppendMenu(hSubMenu, MF_STRING, IDM_LAUNCH_C, 
             "Lancer l'implémentation &C");
  AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
  AppendMenu(hSubMenu, MF_STRING, IDM_SET_REPEAT, 
             "Nombre de &répétitions...");

  AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hSubMenu, 
             "&Traitement");

  return(hMenu);
}

/* EnableProcessMenu
 *
 * Active/Désactive le menu Traitements
 *
 */
void EnableProcessMenu(BOOL enabled)
{
  HMENU hMenu;

  hMenu = GetMenu(hwndMain);

  if (enabled) {
    EnableMenuItem(hMenu, IDM_PROCESS, MF_BYPOSITION | MF_ENABLED);
  } else {
    EnableMenuItem(hMenu, IDM_PROCESS, MF_BYPOSITION | MF_GRAYED);
  }
  DrawMenuBar(hwndMain);
}

/* CreateStatus
 *
 * Crée la barre d'état et retourne son handle.
 *
 */
HWND CreateStatus(HINSTANCE hInstance, HWND hwndParent)
{
  return CreateWindowEx(0, STATUSCLASSNAME, (LPCTSTR) NULL,
    SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
     hwndParent, (HMENU) 1, hInstance, NULL);
}

/* UpdateStatus
 *
 * Met à jour le texte de la barre d'état
 *
 */
void UpdateStatus(PSTR szText)
{
  SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) szText);
}

/* CreateProcessedBitmap
 *
 * Crée un bitmap de taille biHeight x biWidth. Le nombre de bits par pixel est
 * précisé par wBitCount :
 *  - 8 crée un bitmap 8 bits par pixel, couleurs indexées dans une table de 256
 *    niveaux de gris (0 noir, 255 blanc).
 *    Chaque pixel occupe un octet en mémoire, le début de chaque ligne est
 *    aligné sur une frontière de double mot.
 *  - 32 crée un bitmap 32 bits par pixel. La couleur de chaque pixel est
 *    donnée directement par la valeur du pixel sur 4 octets selon le format
 *    0 R V B. Chaque composante Rouge, Verte et Bleue occupe un octet.
 *    La composante Bleue occupe l'octet de poids faible.
 *
 * Retourne un pointeur sur le nouveau bitmap, NULL si échec.
 */
struct ProcessedBitmap *CreateProcessedBitmap(UINT lWidth, UINT lHeight, WORD wBitCount) {
  BITMAPINFO *pbmi;
  BITMAPINFOHEADER *pbmih;
  RGBQUAD *pColorTable;
  BYTE *pBits;
  HBITMAP hBitmap;
  int i;

  if (wBitCount == 8) {
    /* Image 8 bits en niveaux de gris */
    if ((pbmi = (BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD))) == NULL) return NULL;
    pbmih = &pbmi->bmiHeader;
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth = lWidth;
    pbmih->biHeight = lHeight;
    pbmih->biPlanes = 1;
    pbmih->biBitCount = 8;
    pbmih->biCompression = BI_RGB;
    pbmih->biSizeImage = 0;
    pbmih->biXPelsPerMeter = 0;
    pbmih->biYPelsPerMeter = 0;
    pbmih->biClrUsed = 256;
    pbmih->biClrImportant = 256;

    pColorTable = (RGBQUAD *)((BYTE *)pbmi + pbmi->bmiHeader.biSize);
    /* Générer la table de couleurs */
    for (i = 0; i < 256; ++i) {
      pColorTable[i].rgbBlue = pColorTable[i].rgbGreen = pColorTable[i].rgbRed = i;
      pColorTable[i].rgbReserved = 0;
    }
  } else if (wBitCount == 32) {
    if ((pbmi = (BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER))) == NULL) return NULL;
    pbmih = &pbmi->bmiHeader;
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth = lWidth;
    pbmih->biHeight = lHeight;
    pbmih->biPlanes = 1;
    pbmih->biBitCount = 32;
    pbmih->biCompression = BI_RGB;
    pbmih->biSizeImage = 0;
    pbmih->biXPelsPerMeter = 0;
    pbmih->biYPelsPerMeter = 0;
    pbmih->biClrUsed = 0;
    pbmih->biClrImportant = 0;
  } else {
    return NULL;
  }

  hBitmap = CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, (void **) &pBits, NULL, 0);
  free(pbmi);

  if (hBitmap != NULL) {
    struct ProcessedBitmap *bitmap = (struct ProcessedBitmap *) malloc(sizeof(struct ProcessedBitmap));
    bitmap->biHeight = lHeight;
    bitmap->biWidth = lWidth;
    bitmap->hBitmap = hBitmap;
    bitmap->pBits = pBits;
    return bitmap;
  } else {
    return NULL;
  }
}

/* DeleteProcessedBitmap
 *
 * Libère les ressources associées à un bitmap créé par CreateProcessedBitmap
 *
 */
void DeleteProcessedBitmap(struct ProcessedBitmap *bitmap)
{
  if (bitmap->hBitmap != NULL) {
    DeleteObject(bitmap->hBitmap);
  }
  free(bitmap);
}

/* GetBitmapFileName
 *
 * Affiche une boîte de dialogue de sélection de fichiers image .BMP.
 *
 * Retourne le chemin d'accès complet au fichier si sélection réussie, NULL sinon.
 *
 */
PSTR GetBitmapFileName(HWND hwndParent)
{
  static int first = 1;

  static OPENFILENAME ofn;
  static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
  static TCHAR szFilter[] = TEXT("Fichiers bitmap (*.BMP)\0*.BMP\0\0");

  if (first) {
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = szTitleName;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = TEXT("bmp");
    ofn.lCustData = 0;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    first = !first;
  }

  ofn.hwndOwner = hwndParent;

  if (GetOpenFileName(&ofn)) {
    return ofn.lpstrFile;
  }
  return NULL;
}

/* LoadProcessedBitmap
 *
 * Charge un fichier bitmap .BMP 24 ou 32 bits et le convertit dans le format
 * de traitement (32 bits).
 *
 * Retourne un pointeur vers le nouveau bitmap si réussite, NULL si échec.
 *
 */
struct ProcessedBitmap *LoadImageFile(PSTR szFileName)
{
  struct ProcessedBitmap *bitmap = NULL;
  DWORD dwHighSize;
  DWORD dwFileSize;
  BITMAPFILEHEADER *pbmfh;
  BITMAPCOREHEADER *pbmch;
  HANDLE hFile;
  DWORD dwBytesRead;
  BOOL bSuccess;
  BYTE *pBits;
  UINT biWidth, biHeight, biBitCount;
  UINT i, j;
  UINT nPadding, iSrcPel, iDestPel;

  /* Ouvrir le fichier en lecture */
  hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    return NULL;
  }

  /* Récupérer la taille du fichier */
  dwFileSize = GetFileSize(hFile, &dwHighSize);

  if (dwHighSize) {
    CloseHandle(hFile);
    return NULL;
  }

  /* Allouer un buffer pour le fichier */
  pbmfh = (BITMAPFILEHEADER *) malloc(dwFileSize);
  if (!pbmfh) {
    CloseHandle(hFile);
    return NULL;
  }

  /* Copier le fichier dans le buffer */

  bSuccess = ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
  CloseHandle(hFile);

  /* Vérifier que c'est bien un fichier au format BMP */
  if (bSuccess && (dwBytesRead == dwFileSize)
               && (pbmfh->bfType == *(WORD *) "BM")
               && (pbmfh->bfSize == dwFileSize)) {

    /* Convertir le bitmap en fonction de la profondeur de couleur */

    /* Récupérer la taille et la profondeur de couleurs dans l'en-tête */

    BITMAPINFOHEADER *pbmih = (BITMAPINFOHEADER *)(((BYTE *)pbmfh) + sizeof(BITMAPFILEHEADER));

    if (pbmih->biSize == sizeof(BITMAPCOREHEADER)) {
      pbmch = (BITMAPCOREHEADER *)(((BYTE *)pbmfh) + sizeof(BITMAPFILEHEADER));
      biWidth = pbmch->bcWidth;
      biHeight = pbmch->bcHeight;
      biBitCount = pbmch->bcBitCount;
    } else {
      biWidth = pbmih->biWidth;
      biHeight = pbmih->biHeight;
      biBitCount = pbmih->biBitCount;
    }

    /* Récupérer l'adresse du premier pixel */
    pBits = ((BYTE *)pbmfh) + pbmfh->bfOffBits;

    /* On ne traite que les bitmaps 24 ou 32 bits */
    if (biBitCount == 24) {
      bitmap = CreateProcessedBitmap(biWidth, biHeight, 32);
      if (bitmap) {
        /* Copier les bits de l'image, conversion 24->32 bits */
        nPadding = (4 - ((biWidth * 3) & 3)) & 3;
        iSrcPel = 0;
        iDestPel = 0;
        memset(bitmap->pBits, 0, biWidth * biHeight * 4);

        for (i = 0; i < biHeight; i++) {
          for (j = 0; j < biWidth; j++) {
            memcpy(bitmap->pBits + iDestPel, pBits + iSrcPel, 3);
            iSrcPel = iSrcPel + 3;
            iDestPel = iDestPel + 4;
          }
        iSrcPel += nPadding;
        }
      }
    } else if (biBitCount == 32) {
      bitmap = CreateProcessedBitmap(biWidth, biHeight, 32);
      if (bitmap) {
        /* Copier les bits de l'image directement */
        memcpy(bitmap->pBits, pBits, (pbmfh->bfSize - pbmfh->bfOffBits));
      }
    }
  }

  /* Libérer le buffer de fichier */
  free(pbmfh);

  return bitmap;
}

/* LaunchProcessing 
 * 
 * Réalise le lancement de la fonction de calcul demandée par le paramètre pit.
 *
 */
void LaunchProcessing(HWND hwnd, enum ProcessingImplementationType pit)
{
  unsigned int i;
  LARGE_INTEGER beginProcess;
  LARGE_INTEGER endProcess;
  LARGE_INTEGER processTime;
  TCHAR buffer[128];

  /* Vérifier qu'une image a été chargée */
  if (Bitmaps[0] == NULL) {
    return;
  }

  /* Supprimer les bitmaps existants */
  for (i = 1; i <= 3; ++i) {
    if (Bitmaps[i] != NULL) {
      DeleteProcessedBitmap(Bitmaps[i]);
      Bitmaps[i] = NULL;
    }
  }

  /* Créer les bitmaps avec le bon format */
  if (pit == PROCESS_IMAGE_SIMD) {
    Bitmaps[1] = CreateProcessedBitmap(Bitmaps[0]->biWidth, Bitmaps[0]->biHeight, 32);
    Bitmaps[2] = CreateProcessedBitmap(Bitmaps[0]->biWidth, Bitmaps[0]->biHeight, 8);
    Bitmaps[3] = CreateProcessedBitmap(Bitmaps[0]->biWidth, Bitmaps[0]->biHeight, 8);
  }
  else {
    Bitmaps[1] = CreateProcessedBitmap(Bitmaps[0]->biWidth, Bitmaps[0]->biHeight, 32);
    Bitmaps[2] = CreateProcessedBitmap(Bitmaps[0]->biWidth, Bitmaps[0]->biHeight, 32);
    Bitmaps[3] = CreateProcessedBitmap(Bitmaps[0]->biWidth, Bitmaps[0]->biHeight, 32);
  }



  /* Enregistrer la date initiale */
  QueryPerformanceCounter(&beginProcess);

  for (i = 0; i < nRepeats; ++i) {
    switch (pit) {
    case PROCESS_IMAGE_ASM:
      process_image_asm(Bitmaps[0]->biWidth,
        Bitmaps[0]->biHeight,
        Bitmaps[0]->pBits,
        Bitmaps[1]->pBits,
        Bitmaps[2]->pBits,
        Bitmaps[3]->pBits);
      break;

    case PROCESS_IMAGE_SIMD:
      process_image_simd(Bitmaps[0]->biWidth,
        Bitmaps[0]->biHeight,
        Bitmaps[0]->pBits,
        Bitmaps[1]->pBits,
        Bitmaps[2]->pBits,
        Bitmaps[3]->pBits);
      break;

    case PROCESS_IMAGE_C:
      process_image_C(Bitmaps[0]->biWidth,
        Bitmaps[0]->biHeight,
        Bitmaps[0]->pBits,
        Bitmaps[1]->pBits,
        Bitmaps[2]->pBits,
        Bitmaps[3]->pBits);
      break;

    default:
      break;
    }
  }

  /* Enregsitrer la date finale */
  QueryPerformanceCounter(&endProcess);

  /* Calcul du temps écoulé */
  processTime.QuadPart = endProcess.QuadPart - beginProcess.QuadPart;

  if (PerformanceFrequency.QuadPart != 0) {
    sprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), "Temps de calcul pour %d itération(s) : \n"
      "Total : %f ms \n Par itération : %f ms",
      nRepeats,
      (((double)processTime.QuadPart) * 1000.0)
      / ((double)PerformanceFrequency.QuadPart),
      (((double)processTime.QuadPart) * 1000.0)
      / ((double)PerformanceFrequency.QuadPart)
      / ((double)nRepeats));
  }
  else {
    sprintf_s(buffer, sizeof(buffer)  / sizeof(TCHAR), "Pas de support matériel pour l'évaluation du"
      " temps de calcul.");
  }

  /* Afficher l'image et le rapport du temps écoulé */
  InvalidateRect(hwnd, NULL, TRUE);
  MessageBox(hwnd, buffer, "Temps de calcul",
    MB_ICONINFORMATION | MB_OK);
}

/* PaintWindow 
 *
 * Rafraîchit la fenêtre principale.
 *
 */
void PaintWindow(HWND hwnd)
{
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);
  struct ProcessedBitmap *mybm = Bitmaps[iVisibleBitmap];
  RECT rcBitmap, rect;
  HDC hdcMem;

  if (mybm) {
    rcBitmap.left = 0;
    rcBitmap.top = 0;
    rcBitmap.right = mybm->biWidth;
    rcBitmap.bottom = mybm->biHeight;

    /* Ne rafraîchir que la partie découverte */
    if (IntersectRect(&rect, &rcBitmap, &ps.rcPaint)) {
      hdcMem = CreateCompatibleDC(hdc);
      SelectObject(hdcMem, mybm->hBitmap);
      BitBlt(hdc, rect.left, rect.top, rect.right, rect.bottom,
        hdcMem, rect.left, rect.top, SRCCOPY);
      DeleteDC(hdcMem);
    }
  }
  else {
    GetClientRect(hwnd, &rect); 
    DrawText(hdc, TEXT("Pas d'image à afficher."), -1, &rect,
      DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  }
  EndPaint(hwnd, &ps);
}

/* ShowBitmap
 *
 * Affiche le bitmap iBitmap dans la fenêtre principale.
 *
 */
void ShowBitmap(HWND hwnd, UINT iBitmap)
{
  TCHAR buffer[128];

  sprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), "%s - %s", &szAppName, szBitmapNames[iBitmap]);
  SetWindowText(hwnd, buffer);
  iVisibleBitmap = iBitmap;
  InvalidateRect(hwnd, NULL, TRUE);
}

/* OpenBitmapFile
 *
 * Ouvre un nouveau bitmap source. Cette fonction demande à l'utilisateur de sélectionner un fichier,
 * le charge et réinitialise toutes les images de l'application.
 *
 */
void OpenBitmapFile(HWND hwnd)
{
  PSTR szFileName;
  unsigned int i;
  RECT rect, rcWindow, rcStatus;
  struct ProcessedBitmap *mybm;

  if ((szFileName = GetBitmapFileName(hwnd)) != NULL) {
     mybm = LoadImageFile(szFileName);
    if (mybm == NULL) {
      MessageBox(hwnd,
        TEXT("Impossible d'ouvrir le fichier image.\n"),
        TEXT("Erreur"), MB_ICONERROR | MB_OK);
    }
    else {
      iVisibleBitmap = 0;

      for (i = 0; i < 4; i++) {
        if (Bitmaps[i] != NULL) {
          DeleteProcessedBitmap(Bitmaps[i]);
          Bitmaps[i] = NULL;
        }
      }
      Bitmaps[iVisibleBitmap] = mybm;
      EnableProcessMenu(TRUE);
      rect.left = 0;
      rect.top = 0;
      rect.right = mybm->biWidth;
      rect.bottom = mybm->biHeight;
      AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, 0);

      GetWindowRect(hwnd, &rcWindow);
      GetWindowRect(hwndStatus, &rcStatus);
      MoveWindow(hwnd, rcWindow.left, rcWindow.top, rect.right - rect.left, 
        rect.bottom - rect.top + rcStatus.bottom - rcStatus.top, TRUE);
      ShowBitmap(hwnd, iVisibleBitmap);
    }
  }
}

/* RepeatDlgProc
 *
 * Procédure de fenêtre de la boîte de saisie du nombre de répétitions.
 *
 * Traite les messages envoyés par Windows à la boîte de dialogue
 *
 */
BOOL CALLBACK RepeatDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UINT repeats;
  BOOL bParsedOK;

  switch (message) {
  case WM_INITDIALOG:
    SetDlgItemInt(hDlg, EDT_NREPEATS, nRepeats, FALSE);
    return TRUE;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      repeats = GetDlgItemInt(hDlg, EDT_NREPEATS, &bParsedOK, FALSE);
      if (bParsedOK) {
        nRepeats = repeats;
        EndDialog(hDlg, 0);
      }
      return TRUE;
    case IDCANCEL:
      EndDialog(hDlg, 0);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

/* MainWndProc
 *
 * Procédure de fenêtre de la fenêtre principale
 *
 * Traite les messages envoyés par Windows.
 *
 */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HINSTANCE hInstance;

  switch (message) {
    case WM_CREATE:
      hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
      return 0;

    case WM_PAINT:
      PaintWindow(hwnd);
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    case WM_MENUSELECT:
      if (HIWORD(wParam) != 0xffff) {
        UINT i = 0;
        while((MenuDescriptions[i].uId != 0)
               && (MenuDescriptions[i].uId != (UINT) LOWORD(wParam))) {
          i++;
        }
        UpdateStatus(MenuDescriptions[i].lpszText);
      } else {
        UpdateStatus("");
      }
      break;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDM_FILEOPEN:
          OpenBitmapFile(hwnd);
          break;

        case IDM_APPEXIT:
          SendMessage(hwnd, WM_CLOSE, 0, 0);
          break;

        case IDM_SHOWSOURCE:
        case IDM_SHOWTEMP_1:
        case IDM_SHOWTEMP_2:
        case IDM_SHOWDEST:
          ShowBitmap(hwnd, LOWORD(wParam) - IDM_SHOWSOURCE);
          break;

        case IDM_LAUNCH_C:
          LaunchProcessing(hwnd, PROCESS_IMAGE_C);
          break;
        case IDM_LAUNCH_SIMD:
          LaunchProcessing(hwnd, PROCESS_IMAGE_SIMD);
          break;
        case IDM_LAUNCH_ASM:
          LaunchProcessing(hwnd, PROCESS_IMAGE_ASM);
          break;

        case IDM_SET_REPEAT:
          DialogBox(hInstance, TEXT("DLG_REPEAT"), hwnd, (DLGPROC) RepeatDlgProc);
          break ;
      }
      return 0;

    case WM_SIZE:
      SendMessage(hwndStatus, WM_SIZE, wParam, lParam);
      if (Bitmaps[iVisibleBitmap] == NULL) {
        InvalidateRect(hwnd, NULL, TRUE);
      }
      return 0;
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

/* WinMain
 *
 * Fonction de démarrage du programme
 *
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
  static MSG  msg;
  WNDCLASS wndclass;

  InitCommonControls();

  wndclass.style = 0; //CS_HREDRAW | CS_VREDRAW ;
  wndclass.lpfnWndProc = MainWndProc ;
  wndclass.cbClsExtra = 0 ;
  wndclass.cbWndExtra = 0 ;
  wndclass.hInstance = hInstance ;
  wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION) ;
  wndclass.hCursor = LoadCursor (NULL, IDC_ARROW) ;
  wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
  wndclass.lpszMenuName = NULL ;
  wndclass.lpszClassName = TEXT("TPMI01MAIN");

  if (!RegisterClass(&wndclass)) {
    MessageBox(NULL, TEXT("Ce programme requiert Windows 2000 ou supérieur"),
      szAppName, MB_ICONERROR);
    return 0;
  }

  hwndMain = CreateWindow(TEXT("TPMI01MAIN"), szAppName, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  CW_USEDEFAULT, NULL,
    CreateMenuBar(), hInstance, NULL);

  hwndStatus = CreateStatus(hInstance, hwndMain);
  EnableProcessMenu(FALSE);
  ShowWindow(hwndMain, iCmdShow);
  UpdateWindow(hwndMain);

  PerformanceFrequency.QuadPart = 0;
  if (!QueryPerformanceFrequency(&PerformanceFrequency)) {
    MessageBox(hwndMain, "Ce système ne dispose pas de support pour la mesure"
     " de performance.\nLes mesures de performances ne seront pas activées.",
     szAppName, MB_ICONWARNING | MB_OK);
  }

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return((int) msg.wParam);
}