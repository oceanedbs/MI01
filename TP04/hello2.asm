; hello2.asm
;
; MI01 - TP Assembleur 1
;
; Affiche une chaîne de caractères à l'écran

title hello2.asm

.686
.model      flat, c

extern      putchar:near
extern      getchar:near

.data

msg db "bonjour tout le monde", 0
; Ajoutez les variables msg et longueur ici

.code

; Sous-programme main, automatiquement appelé par le code de
; démarrage 'C'
public      main
main        proc

            push    ebx             ; Sauvegarde pour le code 'C'

            mov     ebx, 0

            ; On suppose que la longueur de la chaîne est non nulle
            ; => pas de test de la condition d'arrêt au départ
suivant:    movzx   eax, byte ptr[ebx + msg]

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
            jne     suivant         ; si non, passer au suivant

            call    getchar         ; Attente de l'appui sur "Entrée"
            pop     ebx

            ret                     ; Retour au code de démarrage 'C'

main        endp

            end