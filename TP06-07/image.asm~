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

        push		ecx
        push        eax
        push        edx


        ;récupération des arguments dans les différents registres
        mov     ecx, [ebp + 8]
        imul    ecx, [ebp + 12]

        mov     esi, [ebp + 16]
        mov     edi, [ebp + 20]

        xor                 edx,edx
        ;début de la boucle
boucle: dec      ecx
        ;on récupére le pixel de l'image source à traiter
        mov                 ebx,[esi+ecx*4]

        ;-------------------
        ;calcul de B * 0,114
        ;-------------------

        mov             eax,ebx
        ;on récupere la composante bleu dans eax
        and      eax,000000FFh
        ;on effectue le calcul
        imul         eax,1Dh ;B * 0,114
        ;on stock la somme (I) dans edx
        mov      edx,eax

        ;-------------------
        ;calcul de V *,0,587
        ;-------------------

        ;on récupere la composante verte dans eax
        mov             eax,ebx
        and      eax,0000FF00h
        shr                 eax,8
        ;on effectue le calcul
        imul         eax,96h ;V *,0,587
        add      edx,eax ;I=I+V *,0,587

        ;-------------------
        ;calcul de R*0,299
        ;-------------------
        ;on récupere la composante rouge dans eax
        mov             eax,ebx
        and      eax,00FF0000h
        shr                 eax,16
        ;on effectue le calcul
        imul         eax,4Ch
        add      edx,eax ;I=I+R*0,299

        ;on divise par 256
        shr                 edx,8
        ;on stock I dans la composante bleu de l'image destination
        mov      [edi+ecx*4],edx


        cmp    ecx, 0
        ja     boucle


		;tp6
		mov ecx,[ebp+12]
		sub ecx,2
		shl ecx,16

		mov esi,edi
		mov     edi, [ebp + 24]
		mov     ebp, [ebp + 8] ; on récupère la largeur
		mov		eax,ebp
		shl		eax,2		; on multiplie la largeur par 4
		add		edi,eax ; on ajoute au premier pixel de l'image tmp2 4*largeur
		add		edi, 4
		mov		cx,bp
		sub ecx, 00010000h

	
	boucle3:
		add		ecx,ebp
		sub		ecx,2
	
	boucle2:
		;push ebx
		;mov	ebx, 00FF0000h

		;mov [edi],ebx
		;pop	ebx

		xor		ebx, ebx
		mov		ebx, esi
		imul	ebx,-1
		add		ebx,[esi+8]
		mov		eax,[esi+ebp*4]
		imul	eax,-2
		add		ebx,eax
		mov		eax,[esi+ebp*4+8]
		imul	eax,2
		add		ebx,eax
		mov		eax,[esi+ebp*8]
		imul	eax,-1
		add		ebx,eax
		add		ebx,[esi+ebp*8+8]

		xor		edx, edx
		mov		edx, esi
		mov		eax,[esi+4]
		imul	eax,2
		add		edx, eax
		mov		eax,[esi+8]
		add		edx, eax
		mov		eax,[esi+ebp*8]
		imul	eax,-1
		add		edx,eax
		mov		eax,[esi+ebp*4+4]
		imul	eax,-2
		add		edx,eax
		mov		eax,[esi+ebp*8+8]
		imul	eax,-1
		add		ebx,eax
		add		edx,[esi+ebp*8+8]

		test	ebx,0
		jg		gx_positif
		imul ebx,-1
	
	gx_positif :
		test	edx, 0
		jg		gy_positif
		imul	edx, -1

	gy_positif :
		add ebx, edx
		xor eax, eax
		mov	eax, 255
		sub eax, ebx
		test	eax, 0
		jg		g_positif
		xor eax,eax
		g_positif:

		mov	[edi], eax
		

		add esi, 4
		add edi, 4
		dec ecx
		cmp cx, 0
		jne	   boucle2


		add esi, 8
		add edi, 8
		sub ecx, 00010000h
		jnz    boucle3

fin:
		
		pop     edx
        pop     eax
        pop     ecx
		pop     ebp
       
        ret                         ; Retour à la fonction MainWndProc

process_image_asm   ENDP
END
