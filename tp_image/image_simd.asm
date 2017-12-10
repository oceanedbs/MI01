; IMAGE_SIMD.ASM
;
; MI01 - TP Assembleur 2 à 5
;
; Réalise le traitement d'une image 32 bits.

.686
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

        ;*****************************************************************
        ;*****************************************************************
        ; Ajoutez votre code ici
        ;*****************************************************************
        ;*****************************************************************


fin:
        pop     edi
        pop     esi
        pop     ebx

        pop     ebp

        ret                         ; Retour à la fonction MainWndProc

process_image_simd   ENDP
END
