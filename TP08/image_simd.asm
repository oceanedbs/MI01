; IMAGE_SIMD.ASM
;
; MI01 - TP Assembleur 2 à 5
;
; Réalise le traitement d'une image 32 bits.

.686
.XMM
.MODEL FLAT, C

.DATA

.CODE

; **********************************************************************
; Sous-programme _process_image_simd
;
; Réalise le traitement d'une image 32 bits avec des instructions SIMD.
;
; Entrées sur la pile : Largeur de l'image (entier 32 bits)
;           Hauteur de l'image (entier 32 bits)
;           Pointeur sur l'image source (dépl. 32 bits)
;           Pointeur sur l'image tampon 1 (dépl. 32 bits)
;           Pointeur sur l'image tampon 2 (dépl. 32 bits)
;           Pointeur sur l'image finale (dépl. 32 bits)
; **********************************************************************
PUBLIC      process_image_simd
process_image_simd   PROC NEAR       ; Point d'entrée du sous programme

        push    ebp
        mov     ebp, esp

        push    ebx
        push    esi
        push    edi

        mov     ecx, [ebp + 8]
        imul    ecx, [ebp + 12]

        mov     esi, [ebp + 16]
        mov     edi, [ebp + 20]
		sub      ecx,2

boucle: 
        ;on récupére le pixel de l'image source à traiter 
        movq xmm0, qword ptr[esi+ecx*4]
		
		
		pxor xmm3, xmm3
		punpcklbw xmm0, xmm3 


			; chargement des coeff
		
		
		mov eax, 4C961Dh ;???
		movd xmm2, eax
		psllq xmm2, 1
		movd xmm1, eax
		paddw xmm2, smm1
		punpcklbw xmm2, xmm3
		;on a 2 fois les coefficients multilicateurs

		pmaddwd  xmm0, xmm2

		phaddd xmm0, xmm0
		
		movd eax, xmm0 ;on récupère le premier coefficient
		shr eax, 8 ;on divise par 256

		add ecx, 2 ;on retroune au premier pixel
		mov [edi+ecx*4], eax

		psrld xmm0, 1

		movd eax, xmm0
		shr  eax, 8
		sub ecx, 1
		mov [edi+ecx*4], eax


		sub		ecx,1
        cmp    ecx, 0
        ja     boucle

fin:
        pop     edi
        pop     esi
        pop     ebx

        pop     ebp

        ret                         ; Retour à la fonction MainWndProc

process_image_simd   ENDP
END
