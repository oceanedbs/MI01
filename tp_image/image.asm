; IMAGE.ASM
;
; MI01 - TP Assembleur 2 � 5
;
; R�alise le traitement d'une image 32 bits.

.686
.MODEL FLAT, C

.DATA

.CODE

; **********************************************************************
; Sous-programme _process_image_asm
;
; R�alise le traitement d'une image 32 bits.
;
; Entr�es sur la pile : Largeur de l'image (entier 32 bits)
;           Hauteur de l'image (entier 32 bits)
;           Pointeur sur l'image source (d�pl. 32 bits)
;           Pointeur sur l'image tampon 1 (d�pl. 32 bits)
;           Pointeur sur l'image tampon 2 (d�pl. 32 bits)
;           Pointeur sur l'image finale (d�pl. 32 bits)
; **********************************************************************
PUBLIC      process_image_asm
process_image_asm   PROC NEAR       ; Point d'entrée du sous programme

        push    ebp
        mov     ebp, esp

        push    ecx
        push	eax
        push   	edx


        ;récupération des arguments dans les différents registres
        mov     ecx, [ebp + 8]
        imul    ecx, [ebp + 12]

        mov     esi, [ebp + 16]
        mov     edi, [ebp + 20]

        xor		 edx,edx
        ;début de la boucle
boucle: dec      ecx
        ;on récupére le pixel de l'image source à traiter
        mov		 ebx,[esi+ecx*4]

        ;-------------------
        ;calcul de B * 0,114
        ;-------------------

        mov	     eax,ebx
        ;on récupere la composante bleu dans eax
        and      eax,000000FFh
        ;on effectue le calcul
        imul	 eax,1Dh ;B * 0,114
        ;on stock la somme (I) dans edx
        mov      edx,eax

        ;-------------------
        ;calcul de V *,0,587
        ;-------------------

        ;on récupere la composante verte dans eax
        mov	     eax,ebx
        and      eax,0000FF00h
        shr		 eax,8
        ;on effectue le calcul
        imul	 eax,96h ;V *,0,587
        add      edx,eax ;I=I+V *,0,587

        ;-------------------
        ;calcul de R*0,299
        ;-------------------
        ;on récupere la composante rouge dans eax
        mov	     eax,ebx
        and      eax,00FF0000h
        shr		 eax,16
        ;on effectue le calcul
        imul	 eax,4Ch
        add      edx,eax ;I=I+R*0,299

        ;on divise par 256
        shr		 edx,8
        ;on stock I dans la composante bleu de l'image destination
        mov		 [edi+ecx*4],edx


        cmp    ecx, 0
        ja     boucle

fin:
        pop     edx
        pop     eax
        pop     ecx
        pop     ebp
        ret                         ; Retour à la fonction MainWndProc

process_image_asm   ENDP
END
