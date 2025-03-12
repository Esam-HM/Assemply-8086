#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;

void Dilation(int n, int filter_size, short* resimadres_org);
void Erosion(int n, int filter_size, short* resimadres_org);

int main(void) {
	int M, N, Q, i, j, filter_size;
	bool type;
	int efile;
	char org_resim[100], dil_resim[] = "dilated.pgm", ero_resim[] = "eroded.pgm";
	do {
		printf("Orijinal resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &org_resim);
		system("CLS");
		efile = readImageHeader(org_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_org = resimOku(org_resim);

	printf("Orjinal Resim Yolu: \t\t\t%s\n", org_resim);

	short *resimdizi_org = (short*) malloc(N*M * sizeof(short));

	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			resimdizi_org[i*N + j] = (short)resim_org[i][j];



	int menu;
	printf("Yapmak istediginiz islemi giriniz...\n");
	printf("1-) Dilation\n");
	printf("2-) Erosion\n");
	printf("3-) Cikis\n> ");
	scanf("%d", &menu);
	printf("Filtre boyutunu giriniz: ");
	scanf("%d", &filter_size);
	switch (menu){
		case 1:
			Dilation(N*M, filter_size, resimdizi_org);
			resimYaz(dil_resim, resimdizi_org, N, M, Q);
			break;
		case 2:
			Erosion(N*M, filter_size, resimdizi_org);
			resimYaz(ero_resim, resimdizi_org, N, M, Q);
			break;
		case 3:
			system("EXIT");
			break;
		default:
			system("EXIT");
			break;
	}

	system("PAUSE");
	return 0;
}



void Dilation(int n, int filter_size, short* resim_org) {

	__asm {
		//KODUNUZU BURAYA YAZINIZ, ASAGIDAKI 5 ORNEK ERISIM ICIN VERILMISTIR SIZ YAZMAYA BASLAMADAN SILINIZ
			XOR EBX, EBX
			XOR EDX, EDX
			MOV EAX, filter_size
			MOV BX, 2
			IDIV BX
			MOV filter_size, EAX

			MOV EBX, n				//MATRİSİN SATİR SAYİ BULMA 
			MOV ECX, 1
			J1:
			INC ECX
			MOV EAX, ECX
			MUL ECX
			CMP EAX, EBX
			JNE J1
			MOV EDX, ECX			//N*N >>> N=EDX


			MOV ESI, resim_org
			MOV ECX, n
			XOR EBX, EBX
			AT :					        //RESİM MATRİSİN ELEMANLARI STACKA ATMA
			PUSH WORD PTR[ESI]
			ADD ESI, 2
			LOOP AT
			MOV ECX, n
			ADD ECX, ECX
			PUSH ECX

			XOR ESI, ESI
			XOR EAX, EAX
			XOR ECX, ECX
			XOR EBX, EBX
			XOR EDI, EDI
			MOV ESI, resim_org
			MOV ECX, 0				//LOOP SAYMA İCİN KOSUL
			MOV EAX, 1				//BULUNDUGUMZ SATİR 

		SATIR_LP:					//<<<<ANA  SATİR LOOP
			PUSH EAX				//SATIR LOOP İCİN SATIR SAYİSİ KORUMA   <<<PUSH
			MOV EAX, 1				//BULUNDUMUZ SUTUN
		SUTUN_LP :					//	<<<<ANA SUTUN LOOP
			PUSH EAX				//SUTUN	LOOP İCİN SUTUN SAYİSİ KORUMA	<<<PUSH
			XOR EBX, EBX
			MOV EDI, ESI			// HANGİ ELEMANDAYİZ (SE NEYİN UZERİNDE)
			MOV BX, [ESI]
			INC ECX
			PUSH ECX				//SUTUN LOOPU SARTİ	PUSH ETTİM			<<<PUSH
			MOV ECX, 1				//SE SUTUN SAYMA
			MOV EAX, [ESP + 4]		//HANGİ SUTUNDEYİZ
		CN1:						// SE BULUNDUGU SATIRI SAGİ KARSİLASTİRMA
			ADD EDI, 2
			INC EAX					//SUTUN  ARTTIRMA
			CMP ECX, filter_size
			JA CIKIS1
			CMP EAX, EDX
			JA CIKIS1
			CMP BX, [EDI]
			JAE DEVAM1
			MOV BX, [EDI]			//MAX SAYİ BULMA
			DEVAM1:
			INC ECX
			JMP CN1
			CIKIS1 :
			MOV EDI, ESI
			MOV EAX, [ESP + 4]		//HANGİ SUTUNDEYİZ
			MOV ECX, 1
		CN2 :						//SE SOLUNDAKI ELEMANLARI KARSILASTIRMA
			SUB EDI, 2
			DEC EAX					// ARKADAKİ SUTUNE GİTME
			CMP ECX, filter_size
			JA CIKIS2
			CMP EAX, 0
			JE CIKIS2
			CMP BX, [EDI]			//MAX BULMA
			JAE	DEVAM2
			MOV BX, [EDI]
			DEVAM2:
			INC ECX
			JMP CN2
			CIKIS2 :
			MOV EDI, ESI
			MOV EAX, [ESP + 8]		//HANGİ SATİRDAYİZ
			MOV ECX, 1				//CN3 LOOPUN SARTİ
			PUSH EDX				//İMAGE MATRISIN SATIR SAYISI KORUMA <<<<PUSH
			ADD EDX, EDX			//EDX=2N >> YUKARİ SATİRA GİTMEK İCİN
		CN3 :						//SE YUKARI SATIRLARLA KARSILASTIRMA 
			DEC EAX
			SUB EDI, EDX
			PUSH EDI				//CİKTİGİMİZ ELEMANİN ADRESİ		<<<<PUSH
			PUSH ECX				//KORUMAK İCİN                      <<<<PUSH
			CMP ECX, filter_size
			JA CIKIS3
			CMP EAX, 0
			JE CIKIS3
			CMP BX, [EDI]
			JAE DEVAM3
			MOV BX, [EDI]
			DEVAM3:
			PUSH EAX					//SIMDIKI SATIR						<<<<PUSH
			MOV  EAX, [ESP + 20]		//SIMDIKI SUTUN
			MOV ECX, 1
		CN3LP1 :					    //CN3_LP 1. LP  >>YUKARİ SATİRLAR SAG TARAF İCİN
			ADD EDI, 2
			INC EAX
			CMP	ECX, filter_size
			JA CIKIS4
			CMP EAX, [ESP + 12]			//SUTUN SAYİSİ <<< N
			JA CIKIS4
			CMP BX, [EDI]
			JAE DEVAM4
			MOV BX, [EDI]
			DEVAM4:
			INC ECX
			JMP CN3LP1
			CIKIS4 :
			MOV EDI, [ESP + 8]			//SIMDIKI SE
			MOV  EAX, [ESP + 20]		//SIMDIKI SUTUN
			MOV ECX, 1
		CN3LP2 :						//YUKARIDAKI SE SOL TARAFI
			DEC EAX
			SUB EDI, 2
			CMP ECX, filter_size
			JA CIKIS5
			CMP EAX, 0
			JE CIKIS5
			CMP BX, [EDI]
			JAE DEVAM6
			MOV BX, [EDI]
			DEVAM6:
			INC ECX
			JMP CN3LP2
			CIKIS5 :
			POP EAX					//YUKARIDA GEZDGIMIZ SIMDIKI SATIRI POPLAMA <<<<POP
			POP ECX					//											<<<<POP
			INC ECX
			POP EDI					//YUKARIDA BULUNDUGUMUZ SE                  <<<<POP
			JMP CN3
			CIKIS3 :
			POP ECX
			POP EDI
			MOV EDI, ESI			//BULUNDUMUZ ANA SE 
			MOV EAX, [ESP + 12]		//ANA SE BULUNDUGU SATIR
			MOV ECX, 1
		CN4 :						//ALTTAKI SATIRLARI GEZECEGIZ        >>>>EBP>EDX UZERINDE
			INC EAX
			ADD EDI, EDX
			PUSH EDI                //SIMDIKI SE KORUMA					<<<<PUSH
			PUSH EAX				//									<<<<PUSH
			CMP ECX, filter_size
			JA CIK9
			CMP EAX, [ESP + 8]
			JA CIK9
			CMP BX, [EDI]
			JAE DEV1
			MOV BX, [EDI]
			DEV1:
			PUSH ECX               //									<<<<PUSH
			MOV ECX, 1
			MOV EAX, [ESP + 20]     //ALTTA BULUNDUMUZ SUTUN
		CN4LP1 :
			INC EAX
			ADD EDI, 2
			CMP ECX, filter_size
			JA CIK11
			CMP EAX, [ESP + 12]
			JA CIK11
			CMP BX, [EDI]
			JAE DEV11
			MOV BX, [EDI]
			DEV11:
			INC ECX
			JMP CN4LP1
			CIK11 :
			MOV EDI, [ESP + 8]
			MOV EAX, [ESP + 20]
			MOV ECX, 1
		CN4LP2 :
			DEC EAX
			SUB EDI, 2
			CMP ECX, filter_size
			JA CIK15
			CMP EAX, 0
			JE CIK15
			CMP BX, [EDI]
			JAE DEV15
			MOV BX, [EDI]
			DEV15:
			INC ECX
			JMP CN4LP2
			CIK15 :
			POP ECX						//						<<<<POP
			INC ECX
			POP EAX						//						<<<<POP
			POP EDI						//						<<<<POP
			JMP CN4
			CIK9 :
			POP EAX
			POP EDI

			POP EDX						//						<<<<POP
			POP ECX						//						<<<<POP

			MOV EDI, [ESP + 8]
			MOV WORD PTR[ESP + EDI + 8], BX
			SUB EDI, 2
			MOV[ESP + 8], EDI


			ADD ESI, 2
			POP EAX						//						<<<<POP
			INC EAX
			CMP EAX, EDX
			JNA SUTUN_LP				//<<<<<
			POP EAX						//						<<<<POP
			INC EAX
			CMP EAX, EDX
			JNA SATIR_LP				//<<<<

			MOV ESI, resim_org
			POP ECX
			MOV ECX, n
			MOV EBX, n
			DEC EBX
			ADD EBX, EBX
			yerles :			//OUTPUT RESİM MATRİSİN ELEMANLARI STACKTAN CIKARMA
			POP WORD PTR[ESI + EBX]
			SUB EBX, 2
			LOOP yerles
	}
	printf("\nDilation islemi sonucunda resim \"dilated.pgm\" ismiyle olusturuldu...\n");
}

void Erosion(int n, int filter_size, short* resim_org) {
	__asm {
		//KODUNUZU BURAYA YAZINIZ, ASAGIDAKI 5 ORNEK ERISIM ICIN VERILMISTIR SIZ YAZMAYA BASLAMADAN SILINIZ
			XOR EBX, EBX
			XOR EDX, EDX
			MOV EAX, filter_size
			MOV BX, 2
			IDIV BX
			MOV filter_size, EAX

			MOV EBX, n				//MATRİSİN SATİR SAYİ BULMA 
			MOV ECX, 1
			J1:
			INC ECX
			MOV EAX, ECX
			MUL ECX
			CMP EAX, EBX
			JNE J1
			MOV EDX, ECX			//N*N >>> N=EDX


			MOV ESI , resim_org
			MOV ECX , n
			XOR EBX , EBX
			AT:
			PUSH WORD PTR [ESI]
			ADD ESI , 2
			LOOP AT
			MOV ECX , n
			ADD ECX , ECX
			PUSH ECX

			XOR ESI, ESI
			XOR EAX, EAX
			XOR ECX, ECX
			XOR EBX, EBX
			XOR EDI, EDI
			MOV ESI , resim_org
			MOV ECX , 0				//LOOP SAYMA İCİN KOSUL
			MOV EAX , 1				//BULUNDUGUMZ SATİR 

		SATIR_LP:					//<<<<ANA  SATİR LOOP
			PUSH EAX				//SATIR LOOP İCİN SATIR SAYİSİ KORUMA   <<<PUSH
			MOV EAX, 1				//BULUNDUMUZ SUTUN
		SUTUN_LP :					//	<<<<ANA SUTUN LOOP
			PUSH EAX				//SUTUN	LOOP İCİN SUTUN SAYİSİ KORUMA	<<<PUSH
			XOR EBX, EBX
			MOV EDI, ESI			// HANGİ ELEMANDAYİZ (SE NEYİN UZERİNDE)
			MOV BX, [ESI]
			INC ECX
			PUSH ECX				//SUTUN LOOPU SARTİ	PUSH ETTİM			<<<PUSH
			MOV ECX, 1				//SE SUTUN SAYMA
			MOV EAX, [ESP + 4]		//HANGİ SUTUNDEYİZ
		CN1:						// SE BULUNDUGU SATIRI SAGİ KARSİLASTİRMA
			ADD EDI, 2
			INC EAX					//SUTUN  ARTTIRMA
			CMP ECX, filter_size
			JA CIKIS1
			CMP EAX, EDX
			JA CIKIS1
			CMP BX, [EDI]
			JBE DEVAM1
			MOV BX, [EDI]			//MİN SAYİ BULMA
			DEVAM1:
			INC ECX
			JMP CN1
			CIKIS1 :
			MOV EDI, ESI
			MOV EAX, [ESP + 4]		//HANGİ SUTUNDEYİZ
			MOV ECX, 1
		CN2 :						//SE SOLUNDAKI ELEMANLARI KARSILASTIRMA
			SUB EDI, 2
			DEC EAX					// ARKADAKİ SUTUNE GİTME
			CMP ECX, filter_size
			JA CIKIS2
			CMP EAX, 0
			JE CIKIS2
			CMP BX, [EDI]			//MİN BULMA
			JBE	DEVAM2
			MOV BX, [EDI]
			DEVAM2:
			INC ECX
			JMP CN2
			CIKIS2 :
			MOV EDI, ESI
			MOV EAX, [ESP + 8]		//HANGİ SATİRDAYİZ
			MOV ECX, 1				//CN3 LOOPUN SARTİ
			PUSH EDX				//İMAGE MATRISIN SATIR SAYISI KORUMA <<<<PUSH
			ADD EDX, EDX			//EDX=2N >> YUKARİ SATİRA GİTMEK İCİN
		CN3 :						//SE YUKARI SATIRLARLA KARSILASTIRMA 
			DEC EAX
			SUB EDI, EDX
			PUSH EDI				//CİKTİGİMİZ ELEMANİN ADRESİ		<<<<PUSH
			PUSH ECX				//KORUMAK İCİN                      <<<<PUSH
			CMP ECX, filter_size
			JA CIKIS3
			CMP EAX, 0
			JE CIKIS3
			CMP BX, [EDI]
			JBE DEVAM3
			MOV BX, [EDI]
			DEVAM3:
			PUSH EAX				//SIMDIKI SATIR						<<<<PUSH
			MOV  EAX, [ESP + 20]		//SIMDIKI SUTUN
			MOV ECX, 1
		CN3LP1 :					    //CN3_LP 1. LP  >>YUKARİ SATİRLAR SAG TARAF İCİN
			ADD EDI, 2
			INC EAX
			CMP	ECX, filter_size
			JA CIKIS4
			CMP EAX, [ESP + 12]		//SUTUN SAYİSİ <<< N
			JA CIKIS4
			CMP BX, [EDI]
			JBE DEVAM4
			MOV BX, [EDI]
			DEVAM4:
			INC ECX
			JMP CN3LP1
			CIKIS4 :
			MOV EDI, [ESP + 8]			//SIMDIKI SE
			MOV  EAX, [ESP + 20]		//SIMDIKI SUTUN
			MOV ECX, 1
		CN3LP2 :							//YUKARIDAKI SE SOL TARAFI
			DEC EAX
			SUB EDI, 2
			CMP ECX, filter_size
			JA CIKIS5
			CMP EAX, 0
			JE CIKIS5
			CMP BX, [EDI]
			JBE DEVAM6
			MOV BX, [EDI]
			DEVAM6:
			INC ECX
			JMP CN3LP2
			CIKIS5 :
			POP EAX					//YUKARIDA GEZDGIMIZ SIMDIKI SATIRI POPLAMA <<<<POP
			POP ECX					//											<<<<POP
			INC ECX
			POP EDI					//YUKARIDA BULUNDUGUMUZ SE                  <<<<POP
			JMP CN3
			CIKIS3 :
			POP ECX
			POP EDI
			MOV EDI, ESI			//BULUNDUMUZ ANA SE 
			MOV EAX, [ESP + 12]		//ANA SE BULUNDUGU SATIR
			MOV ECX, 1
		CN4 :						//ALTTAKI SATIRLARI GEZECEGIZ        >>>>EBP>EDX UZERINDE
			INC EAX
			ADD EDI, EDX
			PUSH EDI                //SIMDIKI SE KORUMA					<<<<PUSH
			PUSH EAX				//									<<<<PUSH
			CMP ECX, filter_size
			JA CIK9
			CMP EAX, [ESP + 8]
			JA CIK9
			CMP BX, [EDI]
			JBE DEV1
			MOV BX, [EDI]
			DEV1:
			PUSH ECX               //									<<<<PUSH
			MOV ECX, 1
			MOV EAX, [ESP + 20]     //ALTTA BULUNDUMUZ SUTUN
		CN4LP1 :
			INC EAX
			ADD EDI, 2
			CMP ECX, filter_size
			JA CIK11
			CMP EAX, [ESP + 12]
			JA CIK11
			CMP BX, [EDI]
			JBE DEV11
			MOV BX, [EDI]
			DEV11:
			INC ECX
			JMP CN4LP1
			CIK11 :
			MOV EDI, [ESP + 8]
			MOV EAX, [ESP + 20]
			MOV ECX, 1
		CN4LP2 :
			DEC EAX
			SUB EDI, 2
			CMP ECX, filter_size
			JA CIK15
			CMP EAX, 0
			JE CIK15
			CMP BX, [EDI]
			JBE DEV15
			MOV BX, [EDI]
			DEV15:
			INC ECX
			JMP CN4LP2
			CIK15 :
			POP ECX						//						<<<<POP
			INC ECX
			POP EAX						//						<<<<POP
			POP EDI						//						<<<<POP
			JMP CN4
			CIK9 :
			POP EAX
			POP EDI

			POP EDX						//						<<<<POP
			POP ECX						//						<<<<POP

			MOV EDI , [ESP+8]
			MOV WORD PTR [ESP+EDI+8] , BX
			SUB EDI , 2
			MOV [ESP+8] , EDI


			ADD ESI, 2
			POP EAX						//						<<<<POP
			INC EAX
			CMP EAX, EDX
			JNA SUTUN_LP				//<<<<<
			POP EAX						//						<<<<POP
			INC EAX
			CMP EAX, EDX
			JNA SATIR_LP				//<<<<

			MOV ESI , resim_org
			POP ECX
			MOV ECX , n
			MOV EBX , n
			DEC EBX
			ADD EBX , EBX
			yerles:
			XOR EAX , EAX
			POP WORD PTR [ESI+EBX]
			SUB EBX , 2
			LOOP yerles
	}
	printf("\nErosion islemi sonucunda resim \"eroded.pgm\" ismiyle olusturuldu...\n");
}
