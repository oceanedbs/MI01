; IMAGE.ASM
;
; MI01 - TP Assembleur 2 à 5
;
; Réalise le traitement d'une image 32 bits.

.686
.MODEL FLAT, C

.DATA

.CODE

; **********************************************************************
; Sous-programme _process_image_asm
;
; Réalise le traitement d'une image 32 bits.
;
; Entrées sur la pile : Largeur de l'image (entier 32 bits)
;           Hauteur de l'image (entier 32 bits)
;           Pointeur sur l'image source (dépl. 32 bits)
;           Pointeur sur l'image tampon 1 (dépl. 32 bits)
;           Pointeur sur l'image tampon 2 (dépl. 32 bits)
;           Pointeur sur l'image finale (dépl. 32 bits)
; **********************************************************************
PUBLIC      process_image_asm
process_image_asm   PROC NEAR       ; Point d'entrée du sous programme

  push    ebp
  mov     ebp, esp

  push    ebx
  push	  eax
  push   	edx
  push    esi
  push    edi

  mov     ecx, [ebp + 8]
  imul    ecx, [ebp + 12]

  mov     esi, [ebp + 16]
  mov     edi, [ebp + 20]

;*****************************************************************
;*****************************************************************
; Ajoutez votre code ici
;*****************************************************************
;*****************************************************************
        xor		 edx,edx
boucle: dec    ecx
		mov		 ebx,[esi+ecx*4]

        mov	     eax,ebx
        and      eax,000000FFh
        imul	 eax,1Dh
        mov      edx,eax

        mov	     eax,ebx
        and      eax,0000FF00h
		shr		 eax,8
        imul	 eax,96h
		add      edx,eax

        mov	     eax,ebx
        and      eax,00FF0000h
		shr		 eax,16
        imul	 eax,4Ch
		add      edx,eax

        shr		 edx,8
        mov		 [edi+ecx*4],edx


        cmp    ecx, 0
        ja     boucle

fin:
pop     edi
pop     esi
pop		  edx
pop		  eax
pop     ebx
pop     ebp
ret                         ; Retour à la fonction MainWndProc

process_image_asm   ENDP
END