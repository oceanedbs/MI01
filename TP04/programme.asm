; programme.asm
;
; MI01 - TP Assembleur 1
;
; Affiche un caractère à l'écran

title       programme.asm

.686
.model      flat, c

extern      putchar:near
extern      getchar:near

.data

cara        db  'A'

.code


; Sous-programme main, automatiquement appelé par le code de
; démarrage 'C'
public      main
main        proc

            ; Conversion du caractère en un double mot
            movzx   eax, byte ptr[cara]

            ; Appel à la fonction de bibliothèque ‘C’ putchar(int c) 
            ; pour afficher un caractère. La taille du type C 'int' 
            ; est de 32 bits sur IA-32. Le caractère doit être fourni
            ; sur la pile.
            push    eax         ; Caractère à afficher
            call    putchar     ; Appel de putchar
            add     esp, 4      ; Nettoyage de la pile après appel
            ; Fin de l'appel à putchar

            call    getchar     ; Attente de l'appui sur "Entrée"

            ret                 ; Retour au code de démarrage 'C'

main        endp

            end