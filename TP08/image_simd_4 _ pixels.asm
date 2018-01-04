; IMAGE_SIMD.ASM
;
; MI01 - TP Assembleur 2 à 5
;
; R�alise le traitement d'une image 32 bits.

.686
.XMM
.MODEL FLAT, C

.DATA

.CODE

; **********************************************************************
; Sous-programme _process_image_simd
;
; R�alise le traitement d'une image 32 bits avec des instructions SIMD.
;
; Entrées sur la pile : Largeur de l'image (entier 32 bits)
;           Hauteur de l'image (entier 32 bits)
;           Pointeur sur l'image source (d�pl. 32 bits)
;           Pointeur sur l'image tampon 1 (d�pl. 32 bits)
;           Pointeur sur l'image tampon 2 (d�pl. 32 bits)
;           Pointeur sur l'image finale (d�pl. 32 bits)
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
		sub      ecx,4

		;mise en place des coeffs
		mov eax, 264b0Eh
		movd xmm2, eax
		psllq xmm2, 32
		movd xmm1, eax
		paddw xmm1,xmm2
		punpckldq xmm1,xmm1

boucle:

	;on récupére les 4 pixel de l'image source à traiter
    movdqa xmm0, oword ptr[esi+ecx*4]
	;on fait le calcul avec les coefficients de xmm1
	pmaddubsw xmm0,xmm1
	;on ajoute la somme des deux premire composante de chaque pixels a la 3eme
	phaddw xmm0, xmm3
	;on divise par 128 car les coeffs ont été divisé par 2
	psrlw xmm0,7
	;on met le resultat du calcul dans la composante bleu
	punpcklwd xmm0,xmm3
	movdqa [edi+ecx*4], xmm0

	sub	ecx,4

        ja     boucle

fin:
        pop     edi
        pop     esi
        pop     ebx

        pop     ebp

        ret


process_image_simd   ENDP
END
