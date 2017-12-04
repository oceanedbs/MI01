; IMAGE.ASM
;
; MI01 - TP Assembleur 2 ï¿½ 5
;
; Rï¿½alise le traitement d'une image 32 bits.

.686
.MODEL FLAT, C

.DATA

.CODE

; **********************************************************************
; Sous-programme _process_image_asm
;
; Rï¿½alise le traitement d'une image 32 bits.
;
; Entrï¿½es sur la pile : Largeur de l'image (entier 32 bits)
;           Hauteur de l'image (entier 32 bits)
;           Pointeur sur l'image source (dï¿½pl. 32 bits)
;           Pointeur sur l'image tampon 1 (dï¿½pl. 32 bits)
;           Pointeur sur l'image tampon 2 (dï¿½pl. 32 bits)
;           Pointeur sur l'image finale (dï¿½pl. 32 bits)
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
boucle: mov		 ebx,[esi+ecx*4]
        movzx	 eax,bl
        mov		 dx,1Dh
        mul		 dx
        push	 eax

        shr    ebx,8
        movzx	 eax,bl
        mov		 dx,96h
        mul		 dx
        push	 eax

        shr		 ebx,8
        movzx	 eax,bl
        mov		 dx,4Ch
        mul		 dx
        pop		 ebx
        add		 eax,ebx
        pop		 ebx
        add		 eax,ebx
        shr		 eax,8
        mov		 [edi+ecx*4],eax


        dec    ecx
        cmp    ecx, 0
        ja     boucle

		mov    esi, [ebp + 16] ; 1er pixel img source
		mov		ebx, [ebp + 12]
		sub		ebx, 2   ; nb de lignes

		push	ebp ; sauvegarde de ebp
		mov    ebp, [ebp + 8] ; on récupère la largeur
		mov		eax,ebp
		shl		eax,2		; on multiplie la largeur par 4
		sub		ebp, 2	; on enlve la première et la dernière colone
		add		edi,eax ; on ajoute au premier pixel de l'image tmp2 4*largeur
		
		mov		ecx, ebx
		shl		ecx, 8 ; initialisation du compteur de lignes
		mov		cx,bp
		boucle3:
		sub	ecx, 256
		test	ecx,11111111111111110000000000000000b
		boucle2:
		sub		ecx,1
		mov [edi+cx],11111111111111110000000000000000b
		mov		esi,[esi+ebp+2]
		mov		edi,[edi+ebp]
		test	ecx,00000000000000001111111111111111b
		jnz boucle2
		jnz    boucle3

fin:
pop     edi
pop     esi
pop		  edx
pop		  eax
pop     ebx
pop     ebp
ret                         ; Retour ï¿½ la fonction MainWndProc

process_image_asm   ENDP
END
