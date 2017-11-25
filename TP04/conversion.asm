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
chaine      db      35 dup(?)       ; Remplacer xx par la longueur maximale n de la chaîne
chiffres db "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

.code

; Sous-programme main, automatiquement appelé par le code de
; démarrage 'C'
public      main
main        proc
			push 	eax				; sauvegarde des registres
			push    ebx             ; Sauvegarde pour le code 'C'

			xor	ebx, ebx
			xor	eax, eax
			mov	ecx, 35
			mov	eax, [nombre]
			cmp eax, 0
			jge suivant
			dec eax
			not eax
			push 	eax				; sauvegarde des registres
			push	ecx
			push    "-"       ; on affiche le - si le nombre est négatif
            call    putchar     ; Appel de putchar
            add     esp, 4      ; Nettoyage de la pile après appel
			pop ecx
			pop eax

suivant :		xor 	edx, edx		
			div	ecx
			
			mov		[chaine+ebx], dl
            inc		ebx
			cmp		eax, 0
			jne		suivant
			dec		ebx
		

affichage : movzx   eax, byte ptr[ebx + chaine]

            ; Appel à la fonction de bibliothèque ‘C’ putchar(int c) 
            ; pour afficher un caractère. La taille du type C 'int' 
            ; est de 32 bits sur IA-32. Le caractère doit être fourni
            ; sur la pile.
			lea		edx, [chiffres + eax]
            push    [edx]         ; Caractère à afficher
            call    putchar     ; Appel de putchar
            add     esp, 4      ; Nettoyage de la pile après appel
            ; Fin de l'appel à putchar

            dec     ebx             ; Caractère suivant
            cmp     ebx,10 ; Toute la longueur ?
            jb     affichage         ; si non, passer au suivant

            call    getchar         ; Attente de l'appui sur "Entrée"

			pop     ebx
            ret                     ; Retour au code de démarrage 'C'

main       endp

            end
