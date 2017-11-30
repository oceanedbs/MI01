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

nombre      dd      95c8ah          ; Nombre a convertir
chaine      db      32 dup(?)       ; Remplacer xx par la longueur maximale n de la chaine
chiffres 		db 			"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

.code

; Sous-programme main, automatiquement appele par le code de
; demarrage 'C'
public      main
main        proc
			push  ebx             ; Sauvegarde pour le code 'C'

			xor		ebx, ebx
			xor		eax, eax
			mov		ecx, 35
			mov		eax, [nombre]
			cmp 	eax, 0
			jge 	suivant
			dec 	eax
			not 	eax
			push 	eax				    ; sauvegarde des registres
			push	ecx
			push  "-"           ; on affiche le - si le nombre est negatif
      call    putchar     ; Appel de putchar
      add     esp, 4      ; Nettoyage de la pile apres appel
			pop ecx
			pop eax

suivant :		xor 	edx, edx
			div	  ecx
			mov		[chaine+ebx], dl
      inc		ebx
			cmp		eax, 0
			jne		suivant
			dec		ebx


affichage : movzx   eax, byte ptr[ebx + chaine]

			lea			edx, [chiffres + eax]
      push   	[edx]         	; Caractere a afficher
      call    putchar     		; Appel de putchar
      add     esp, 4      		; Nettoyage de la pile apres appel
  		dec     ebx             ; Caractere suivant
	    cmp     ebx,32 					; Toute la longueur ?
      jb     	affichage       ; si non, passer au suivant
			call    getchar         ; Attente de l'appui sur "Entree"
			pop     ebx
      ret                     ; Retour au code de demarrage 'C'

main       endp

end