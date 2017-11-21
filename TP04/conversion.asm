; conversion.asm
;
; MI01 - TP Assembleur 1
;
; Affiche un nombre de 32 bits sous forme lisible

title conversion.asm

.686
.model 		flat, c

extern      putchar:near
extern      getchar:near

.data

nombre      dd      95c8ah          ; Nombre à convertir
chaine      db      10 dup(?)       ; Remplacer xx par la longueur maximale n de la chaîne

.code

; Sous-programme main, automatiquement appelé par le code de
; démarrage 'C'
public      main
main        proc

			push    ebx             ; Sauvegarde pour le code 'C'

			xor		ebx, ebx
			xor		eax, eax
			mov		ecx, 16
			mov		eax, [nombre]
suivant :	div		dword ptr ecx
			push	edx

			mov		[chaine+ebx], dl
			add     esp, 4      ; Nettoyage de la pile après appel
            inc		ebx
			cmp		edx, 0
			jne		suivant

			xor ebx, ebx

affichage : movzx   eax, byte ptr[ebx + chaine]

            ; Appel à la fonction de bibliothèque ‘C’ putchar(int c) 
            ; pour afficher un caractère. La taille du type C 'int' 
            ; est de 32 bits sur IA-32. Le caractère doit être fourni
            ; sur la pile.
            push    eax         ; Caractère à afficher
            call    putchar     ; Appel de putchar
            add     esp, 4      ; Nettoyage de la pile après appel
            ; Fin de l'appel à putchar

            inc     ebx             ; Caractère suivant
            cmp     eax, 0 ; Toute la longueur ?
            jne     affichage         ; si non, passer au suivant

            call    getchar         ; Attente de l'appui sur "Entrée"

			pop     ebx

            ret                     ; Retour au code de démarrage 'C'

main       endp

            end