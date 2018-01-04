; IMAGE.ASM
;
; MI01 - TP Assembleur 2 ? 5
;
; R?alise le traitement d'une image 32 bits.

.686
.MODEL FLAT, C

.DATA

.CODE

; **********************************************************************
; Sous-programme _process_image_asm
;
; R?alise le traitement d'une image 32 bits.
;
; Entr?es sur la pile : Largeur de l'image (entier 32 bits)
;           Hauteur de l'image (entier 32 bits)
;           Pointeur sur l'image source (d?pl. 32 bits)
;           Pointeur sur l'image tampon 1 (d?pl. 32 bits)
;           Pointeur sur l'image tampon 2 (d?pl. 32 bits)
;           Pointeur sur l'image finale (d?pl. 32 bits)
; **********************************************************************
PUBLIC      process_image_asm
process_image_asm   PROC NEAR       ; Point d'entrée du sous programme

		push    ebp
		mov     ebp, esp

		push    ebx
		push    esi
		push    edi


        ;récupération des arguments dans les différents registres
        mov     ecx, [ebp + 8]
        imul    ecx, [ebp + 12]

        mov     esi, [ebp + 16]
        mov     edi, [ebp + 20]

        xor		 edx,edx
        ;début de la boucle
boucle: dec      ecx
        ;on récupére le pixel de l'image source à traiter
        mov		 ebx,[esi+ecx*4]

        ;-------------------
        ;calcul de B * 0,114
        ;-------------------

        mov	     eax,ebx
        ;on récupere la composante bleu dans eax
        and      eax,000000FFh
        ;on effectue le calcul
        imul	 eax,1Dh ;B * 0,114
        ;on stock la somme (I) dans edx
        mov      edx,eax

        ;-------------------
        ;calcul de V *,0,587
        ;-------------------

        ;on récupere la composante verte dans eax
        mov	     eax,ebx
        and      eax,0000FF00h
        shr		 eax,8
        ;on effectue le calcul
        imul	 eax,96h ;V *,0,587
        add      edx,eax ;I=I+V *,0,587

        ;-------------------
        ;calcul de R*0,299
        ;-------------------
        ;on récupere la composante rouge dans eax
        mov	     eax,ebx
        and      eax,00FF0000h
        shr		 eax,16
        ;on effectue le calcul
        imul	 eax,4Ch
        add      edx,eax ;I=I+R*0,299

        ;on divise par 256
        shr		 edx,8
        ;on stock I dans la composante bleu de l'image destination
        mov		 [edi+ecx*4],edx


        cmp    ecx, 0
        ja     boucle

		;tp6
		mov ecx,[ebp+12];hauteur
		sub ecx,2
		shl ecx,16

		mov     esi,edi;img tmp1
		mov     edi, [ebp + 24];img tmp2
		mov     ebp, [ebp + 8] ; largeur
		mov		eax,ebp  ;eax=largeur
		shl		eax,2		; on multiplie la largeur par 4 ;taille  d'une ligne
		add		edi,eax ; on ajoute au premier pixel de l'image tmp2 4*largeur
		add		edi, 4



	boucle3:
		add		ecx,ebp
        SUB     ecx,2

	boucle2:

	;--------------
		xor		ebx, ebx
		;Gx
		xor		ebx, ebx
		mov		ebx, [esi]
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


		;Gy
		xor		edx, edx
		mov		edx, [esi]
		mov		eax,[esi+4]
		imul	eax,2
		add		edx, eax
		mov		eax,[esi+8]
		add		edx, eax
		mov		eax,[esi+ebp*8]
		imul	eax,-1
		add		edx,eax
		mov		eax,[esi+ebp*8+4]
		imul	eax,-2
		add		edx,eax
		mov		eax,[esi+ebp*8+8]
		imul	eax,-1
		add		edx,eax

		cmp	ebx,0
		jg		gx_positif
		neg		ebx
	
	gx_positif :
		cmp	edx, 0
		jg		gy_positif
		neg		edx

	gy_positif :
	add ebx, edx

		
		xor eax, eax
		mov	eax, 255
		sub eax, ebx
		cmp	eax, 0
		jg		g_positif
		xor eax,eax
		g_positif:
			MOV EDX, EAX
			SHL EDX, 8
			ADD EAX, EDX
			SHL EDX, 8
			ADD EAX, EDX
		mov	[edi], eax
	;--------------



		add esi, 4
		add edi, 4
		dec ecx
		test ecx, 0000ffffh;
		jne	   boucle2


		add esi, 8
		add edi, 8
		sub ecx, 00010000h
		jnz    boucle3

fin:

		pop     edi
		pop     esi
		pop     ebx

		pop     ebp
        ret                         ; Retour a la fonction MainWndProc

process_image_asm   ENDP
END
