#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "c2d2/chien2d2.h"
#include "c2d2/chien2d2primitivas.h"
#include "c2d2/chienaudio2.h"
#include "math/Vector2D.h"


using namespace math;

	//PROTOTIPOS DAS FUNCOES DE DESENHO
    void cenario();
	void title();
	int saijogo=0;

	//VALRIAVEIS DA CONSTRUCAO DO CENARIO E INDICACAO DO MOVIMENTO DO VENTO
	int nuvem_anda_inicial = 900;
	int nuvem_anda;
	int nuvem_andab_inicial = 900;
	int nuvem_andab;
	int velocidadedovento = 0 ; // kmh
    int contaloop = 1;
	
	//DADOS DOS JOGADORES
	int	vida_jogador1;
	int	vida_jogador2;
	char str[50];

	// VARIAVEIS DOS EIXOS DOS CANHOES (POSICOES DOS CANHOES)
	int eixo_canhao_x = 100, eixo_canhao_y = 420;
	int eixo_canhaob_x = 650, eixo_canhaob_y = 420;

	//Movimento do canhão
	Vector2D carga(0.5,0.5);
	Vector2D normal;
	Vector2D direcao_canhao, direcao_canhaob;
	int desenha_canhaox[4], desenha_canhaoy[4], desenha_canhaobx[4], desenha_canhaoby[4];
	int angulo_mascara_canhaoa = 0, angulo_mascara_canhaob = 180; // angulo do sprite que server como mascara para a primitiva
	
	//Gravidade do planeta Terra
	Vector2D GRAVIDADE (0.0,9.80665f);
	
	//Calculos de movimento da bala
	Vector2D posicao_ini;
	Vector2D velocidade;
	Vector2D velocidade_ini;
	
	//Tipos de Bala
	int tipo_bala = 1;
	float massa = 0.10;
	int turno = 0;
	int qtd_bala1_j1;
	int qtd_bala2_j1;
	int qtd_bala3_j1;
	int qtd_bala1_j2;
	int qtd_bala2_j2;
	int qtd_bala3_j2;
	
	//Variáveis para o cenário destrutivo
	int posicao_tabuleiro[24][17];
	int col = 0;
	int lin = 0;
	int quebrabloco = 0;
	//--------------------------------

	//PARTE MATEMÁTICA
	//Constante PI
	#define PI 3.14159265f
	#define RAD	PI/180

	

int main(int narg, char **valarg)
{
	C2D2P_Inicia();

	// Inicia a Chien2D 2 e testa se deu tudo certo
	if(C2D2_Inicia(800, 600, C2D2_JANELA, C2D2_DESENHO_OPENGL, "VECTOR SHOT"))
		printf("Iniciou a Chien2D 2 com sucesso\n");
	else
	{
		printf("Falhou ao iniciar a Chien2D 2. Encerrando o programa.\n");
		return 0;
	}

	C2D2_Botao *teclas = C2D2_PegaTeclas();	
	C2D2_Mouse *mouse = C2D2_PegaMouse();
	// Inica primitivas

    C2D2_TrocaCorLimpezaTela(0, 0, 120);

	int xmouse, ymouse;
	xmouse = mouse->x;
	ymouse = mouse->y;


	int canhaoar = C2D2_CarregaSpriteSet("imagens/weel.png", 64, 64); // CARREGA CANHAO A
	int canhaoa = C2D2_CarregaSpriteSet("imagens/CANNON.png", 64, 64); // CARREGA CANHAO A
	int canhaobr = C2D2_CarregaSpriteSet("imagens/weel.png", 64, 64); // CARREGA CANHAO A
	int canhaob = C2D2_CarregaSpriteSet("imagens/CANNON.png", 64, 64); // CARREGA CANHAO B
	int bala = C2D2_CarregaSpriteSet("imagens/ball.png", 16, 16); // CARREGA A BALA DE CANHAO
	int mira = C2D2_CarregaSpriteSet("imagens/cursor.png", 32, 32); // CARREGA A MIRA
	int _fonte = C2D2_CarregaFonte("./imagens/fonte.png", 32); 

 

	int radianos_canhao[1] = {0};
	
	Vector2D direcao;

	Vector2D poscanhao(100,420);
	Vector2D offsetangulocanhaoa(50,0);
	Vector2D poscanhaob(650,420);
	Vector2D offsetangulocanhaob(-50,0);

	offsetangulocanhaoa.rotate(toRadians(radianos_canhao[0]));
	direcao_canhao = poscanhao + offsetangulocanhaoa;
	offsetangulocanhaob.rotate(toRadians(radianos_canhao[0]));
	direcao_canhaob = poscanhaob + offsetangulocanhaob;

	// Inicia o audio
	CA2_Inicia();
	// Carrega musica de fundo
	unsigned int musica_fundo;
	musica_fundo = CA2_CarregaMusica("");
	CA2_TocaMusica(musica_fundo, -1);

	// Carrega Arquivos de som
	unsigned int som_carrega;
	unsigned int som_tiro;
	unsigned int som_acerta;
	unsigned int som_posiciona;
	unsigned int som_erra;
	unsigned int som_erro;
	unsigned int som_tipotiro;
	int espaca_efeito = 1;

	som_carrega = CA2_CarregaEfeito("./sons/carrega.wav");
	som_posiciona = CA2_CarregaEfeito("./sons/posiciona.wav");
	som_tiro = CA2_CarregaEfeito("./sons/tiro.wav");
	som_acerta = CA2_CarregaEfeito("./sons/acerta.wav");
	som_erra = CA2_CarregaEfeito("./sons/erra.wav");
	som_erro = CA2_CarregaEfeito("./sons/erro.wav");
	som_tipotiro = CA2_CarregaEfeito("./sons/tipotiro.wav");

	//Laço principal, fica rodando até pressionar a tecla ESC pra sair

	while (!teclas[C2D2_ESC].pressionado)
	{		
		if (turno == 0){
			title();
		}
		
		C2D2_LimpaTela();
		
		cenario();

		if(saijogo==1){
			break;
		}

		C2D2_DesenhaSprite(mira, 0, mouse->x, mouse->y); // DESENHA A MIRA
//---------------------------------------------------------------------------------------
//                           J  O  G  A  D  O  R 
//---------------------------------------------------------------------------------------
		if (teclas[C2D2_ESQUERDA].ativo){
			radianos_canhao[0] = -1; //real
			CA2_TocaEfeito (som_posiciona,300);
			if (turno == 1) angulo_mascara_canhaoa++; // mascara
			if (turno == 2) angulo_mascara_canhaob++; // mascara
		}
		else if (teclas[C2D2_DIREITA].ativo){
			radianos_canhao[0] = 1; // real
			CA2_TocaEfeito (som_posiciona,300);
			if (turno == 1) angulo_mascara_canhaoa--; // mascara
			if (turno == 2) angulo_mascara_canhaob--; // mascara
		}
		else{
			radianos_canhao[0] =0;
		}

		if (turno == 1){
			offsetangulocanhaoa.rotate(toRadians(radianos_canhao[0]));
			velocidade_ini = offsetangulocanhaoa;
			direcao_canhao = poscanhao + offsetangulocanhaoa;
		}else if(turno == 2)
		{
			offsetangulocanhaob.rotate(toRadians(radianos_canhao[0]));
			velocidade_ini = offsetangulocanhaob;
			direcao_canhaob = poscanhaob + offsetangulocanhaob;
		}


		//--------------------- CANHAO 1 ------------------------------
		// calcula os pontos do canhao para rotacao do sprite (Mascara)
		desenha_canhaox[0] = eixo_canhao_x-(64.0)*cos((angulo_mascara_canhaoa+45)*RAD);
		desenha_canhaox[1] = eixo_canhao_x-(64.0)*cos((angulo_mascara_canhaoa+135)*RAD);
		desenha_canhaox[2] = eixo_canhao_x-(64.0)*cos((angulo_mascara_canhaoa+225)*RAD);
		desenha_canhaox[3] = eixo_canhao_x-(64.0)*cos((angulo_mascara_canhaoa+315)*RAD);
		desenha_canhaoy[0] = eixo_canhao_y+(64.0)*sin((angulo_mascara_canhaoa+45)*RAD);
		desenha_canhaoy[1] = eixo_canhao_y+(64.0)*sin((angulo_mascara_canhaoa+135)*RAD);
		desenha_canhaoy[2] = eixo_canhao_y+(64.0)*sin((angulo_mascara_canhaoa+225)*RAD);
		desenha_canhaoy[3] = eixo_canhao_y+(64.0)*sin((angulo_mascara_canhaoa+315)*RAD);
		// C2D2P_Linha  (eixo_canhao_x,eixo_canhao_y,direcao_canhao.x,direcao_canhao.y,255, 0, 255); // NAO APAGAR ESTA INSTRUCAO - SOMENTE COMENTAR (ANGULO REAL) - MOSTRA DE ONDE SAI A BALA (PONTO INICIAL)
		C2D2_DesenhaSpriteEfeito (canhaoa, 0, desenha_canhaox, desenha_canhaoy, 255, 0, 255, 255);
		C2D2_DesenhaSprite(canhaoar,0,eixo_canhao_x-32, eixo_canhao_y-32);
		//----------------------------------------------------------

		//--------------------- CANHAO 2 ------------------------------
		desenha_canhaobx[0] = eixo_canhaob_x-(64.0)*cos((angulo_mascara_canhaob+45)*RAD);
		desenha_canhaobx[1] = eixo_canhaob_x-(64.0)*cos((angulo_mascara_canhaob+135)*RAD);
		desenha_canhaobx[2] = eixo_canhaob_x-(64.0)*cos((angulo_mascara_canhaob+225)*RAD);
		desenha_canhaobx[3] = eixo_canhaob_x-(64.0)*cos((angulo_mascara_canhaob+315)*RAD);
		desenha_canhaoby[0] = eixo_canhaob_y+(64.0)*sin((angulo_mascara_canhaob+45)*RAD);
		desenha_canhaoby[1] = eixo_canhaob_y+(64.0)*sin((angulo_mascara_canhaob+135)*RAD);
		desenha_canhaoby[2] = eixo_canhaob_y+(64.0)*sin((angulo_mascara_canhaob+225)*RAD);
		desenha_canhaoby[3] = eixo_canhaob_y+(64.0)*sin((angulo_mascara_canhaob+315)*RAD);
		// C2D2P_Linha  (eixo_canhaob_x,eixo_canhaob_y,direcao_canhaob.x,direcao_canhaob.y,255, 0, 255);// NAO APAGAR ESTA INSTRUCAO - SOMENTE COMENTAR (ANGULO REAL) - MOSTRA DE ONDE SAI A BALA (PONTO INICIAL)
		C2D2_DesenhaSpriteEfeito (canhaob, 0, desenha_canhaobx, desenha_canhaoby, 255, 0, 255, 255);
		C2D2_DesenhaSprite(canhaobr,0,eixo_canhaob_x-32,eixo_canhaob_y-32);
		//-----------------------------------------------------------



		if (teclas[C2D2_1].pressionado){
			if (turno == 1){
				if (qtd_bala1_j1 > 0){
					tipo_bala = 1;
				 	CA2_TocaEfeito (som_tipotiro,300);
				}else{
					CA2_TocaEfeito (som_erro,300);
				}
			}else if(turno == 2){
				if (qtd_bala1_j2 > 0){
					tipo_bala = 1;
				 	CA2_TocaEfeito (som_tipotiro,300);
				}else{
					CA2_TocaEfeito (som_erro,300);
				}
			}
		}
		else if (teclas[C2D2_2].ativo){
			if (turno == 1){
				if (qtd_bala2_j1 > 0){
					tipo_bala = 2;
				 	CA2_TocaEfeito (som_tipotiro,300);
				}else{
					CA2_TocaEfeito (som_erro,300);
				}
			}else if(turno == 2){
				if (qtd_bala2_j2 > 0){
					tipo_bala = 2;
				 	CA2_TocaEfeito (som_tipotiro,300);
				}else{
					CA2_TocaEfeito (som_erro,300);
				}
			}
		}
		else if (teclas[C2D2_3].ativo){
			if (turno == 1){
				if (qtd_bala3_j1 > 0){
					tipo_bala = 3;
				 	CA2_TocaEfeito (som_tipotiro,300);
				}else{
					CA2_TocaEfeito (som_erro,300);
				}
			}else if(turno == 2){
				if (qtd_bala3_j2 > 0){
					tipo_bala = 3;
				 	CA2_TocaEfeito (som_tipotiro,300);
				}else{
					CA2_TocaEfeito (som_erro,300);
				}
			}
		}
			
		if (tipo_bala == 1){
			massa = 0.10;
		    C2D2_DesenhaTexto(_fonte, 20, 500, "TIPOS DE BALA: 1/2/3 : (1) 10 KG ", C2D2_TEXTO_ESQUERDA);
			C2D2P_CirculoPintado(600, 515, 5, 255, 255, 255);
		}
		else if (tipo_bala == 2){
			massa = 0.15;
		    C2D2_DesenhaTexto(_fonte, 20, 500, "TIPOS DE BALA: 1/2/3 : (2) 15 KG ", C2D2_TEXTO_ESQUERDA);
			C2D2P_CirculoPintado(600, 515, 8, 255, 255, 255);
		}
		else if (tipo_bala == 3){
			massa = 0.20;
		    C2D2_DesenhaTexto(_fonte, 20, 500, "TIPOS DE BALA: 1/2/3 : (3) 20 KG ", C2D2_TEXTO_ESQUERDA);
			C2D2P_CirculoPintado(600, 515, 10, 255, 255, 255);
		}

		//if (teclas[C2D2_V].pressionado){ //NO CASO DE ESCOLHA MANUAL DA VELOCIDADE DO VENTO
		//	velocidadedovento = velocidadedovento + 20;
			if (velocidadedovento > 60) {
				velocidadedovento = -60;
			}
			//CA2_TocaEfeito (som_tipotiro,300);
		//}
	
		
		if (turno == 1){
			posicao_ini = Vector2D (direcao_canhao.x, direcao_canhao.y);
		}else if(turno == 2)
		{
			posicao_ini = Vector2D (direcao_canhaob.x, direcao_canhaob.y);
		}
		Vector2D posicao_atual = posicao_ini;

		float tempo_inicial = C2D2_TempoSistema() - 1;

//---------------------------------------------------------------------------------------
//                       T I R O     J  O  G  A  D  O  R 
//--------------------------------------------------------------------------------------

		C2D2_DesenhaTexto(_fonte, 20, 550, "SEGURE A BARRA DE ESPAÇO PARA CARGA DO TIRO", C2D2_TEXTO_ESQUERDA);

		if (teclas[C2D2_ESPACO].ativo)
		{  
			if (carga.x  < 2.5){ 
				carga += Vector2D (0.01,0.01);                                                                                                                                                                                                              
			}
			velocidade.x  = velocidade_ini.x * carga.x;
			velocidade.y  = velocidade_ini.y * carga.y;
					
			espaca_efeito++;
			if (espaca_efeito == 10){
				CA2_TocaEfeito(som_carrega,100);
				espaca_efeito = 1;
			}
			

		
		}else if (teclas[C2D2_ESPACO].liberado){
			CA2_PausaEfeitos();
			CA2_ContinuaEfeitos();
			CA2_TocaEfeito(som_tiro,100);
			normal = posicao_atual;
			normal.normalize(); 

			while (posicao_atual.x < 800 && posicao_atual.y < 420  && posicao_atual.x >0)
			{
				//Força Peso :  Peso = massa* gravidade
				float peso = massa * GRAVIDADE.y; 

				float tempo_atual = C2D2_TempoSistema();

				posicao_atual = posicao_ini + velocidade * ((tempo_atual - tempo_inicial)/100) + 0.5 * GRAVIDADE * (((tempo_atual - tempo_inicial)/100) * ((tempo_atual - tempo_inicial)/100)) * peso;  

				posicao_atual.x = posicao_atual.x + (velocidadedovento/5) * ((tempo_atual - tempo_inicial)/100); // posicao = posicaoinicial + velocidade * intervalodetempo

				cenario();
			
				//--------------------- CANHAO 1 ------------------------------
				C2D2_DesenhaSpriteEfeito (canhaoa, 0, desenha_canhaox, desenha_canhaoy, 255, 0, 255, 255);
				C2D2_DesenhaSprite(canhaoar,0,eixo_canhao_x-32,eixo_canhao_y-32);
				//--------------------------------------------------------------
				//--------------------- CANHAO 2 ------------------------------
				C2D2_DesenhaSpriteEfeito (canhaob, 0, desenha_canhaobx, desenha_canhaoby, 255, 0, 255, 255);
				C2D2_DesenhaSprite(canhaobr,0,eixo_canhaob_x-32,eixo_canhaob_y-32);
				//-------------------------------------------------------------	

				if (tipo_bala == 1) C2D2P_CirculoPintado(posicao_atual.x,posicao_atual.y, 5, 0, 0, 0); //C2D2_DesenhaSprite(bala,0,posicao_atual.x,posicao_atual.y);
				if (tipo_bala == 2) C2D2P_CirculoPintado(posicao_atual.x,posicao_atual.y, 8, 0, 0, 0); //C2D2_DesenhaSprite(bala,0,posicao_atual.x,posicao_atual.y);
				if (tipo_bala == 3) C2D2P_CirculoPintado(posicao_atual.x,posicao_atual.y, 10, 0, 0, 0); //C2D2_DesenhaSprite(bala,0,posicao_atual.x,posicao_atual.y);

				if (turno == 1)	C2D2_DesenhaTexto(_fonte, 400, 20, "TURNO DO JOGADOR 1", C2D2_TEXTO_CENTRALIZADO);
				if (turno == 2) C2D2_DesenhaTexto(_fonte, 400, 20, "TURNO DO JOGADOR 2", C2D2_TEXTO_CENTRALIZADO);

				C2D2_Sincroniza(C2D2_FPS_PADRAO);

				if (posicao_atual.x > (eixo_canhao_x-30) && posicao_atual.x < (eixo_canhao_x+30)  && posicao_atual.y > (eixo_canhao_y-30)  && posicao_atual.y < (eixo_canhao_y+30)){
					if (tipo_bala == 1) vida_jogador1 = vida_jogador1 - 5;
					if (tipo_bala == 2) vida_jogador1 = vida_jogador1 - 10;
					if (tipo_bala == 3) vida_jogador1 = vida_jogador1 - 20;
					CA2_TocaEfeito (som_tipotiro,300);
					break;
				}
				if (posicao_atual.x > (eixo_canhaob_x-30) && posicao_atual.x < (eixo_canhaob_x+30)  && posicao_atual.y > (eixo_canhaob_y-30)  && posicao_atual.y < (eixo_canhaob_y+30)){
					if (tipo_bala == 1) vida_jogador2 = vida_jogador2 - 5;
					if (tipo_bala == 2) vida_jogador2 = vida_jogador2 - 10;
					if (tipo_bala == 3) vida_jogador2 = vida_jogador2 - 20;
					CA2_TocaEfeito (som_tipotiro,300);
					break;
				}

				//-------------
				col = 0; // coluna
				lin = 0; // linha
				for (col=0;col<24;col++){
					for (lin=0;lin<17;lin++){
						if (posicao_atual.x > col*32-32 && posicao_atual.x < col*32+32  && posicao_atual.y > lin*32-32 && posicao_atual.y < lin*32+32){
							if (posicao_tabuleiro[col][lin] == 1)
							{
								if (tipo_bala == 2 ||tipo_bala == 3){
									posicao_tabuleiro[col][lin] = 0;
								}
								quebrabloco = 1;
							}
						}
					}
					if (quebrabloco == 1){
						break;
					}
				}	
				if (quebrabloco == 1){
					break;
				}
				//------------

			}
			quebrabloco = 0;
			CA2_PausaEfeitos();
			CA2_ContinuaEfeitos();
			CA2_TocaEfeito(som_erra,400);
			int i = 10;
			for(i=10; i<50; i=i+10){
				C2D2P_CirculoPintado(posicao_atual.x, posicao_atual.y, i, 255, 0, 0); 
				C2D2_Sincroniza(C2D2_FPS_PADRAO);
			}
			if (turno == 1)
			{
				if (tipo_bala == 1) qtd_bala1_j1 = qtd_bala1_j1;
				if (tipo_bala == 2) qtd_bala2_j1 = qtd_bala2_j1 - 1;
				if (tipo_bala == 3) qtd_bala3_j1 = qtd_bala3_j1 - 1;
				turno = 2;
			}else if(turno == 2)
			{
				if (tipo_bala == 1) qtd_bala1_j2 = qtd_bala1_j2;
				if (tipo_bala == 2) qtd_bala2_j2 = qtd_bala2_j2 - 1;
				if (tipo_bala == 3) qtd_bala3_j2 = qtd_bala3_j2 - 1;
				turno = 1;
			}
			carga = Vector2D (0.5,0.5);
			velocidadedovento = velocidadedovento + 20;// TROCA A VELOCIDADE DO VENTO A CADA TURNO
			tipo_bala = 1;
		}	
	//---------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------

		if (turno == 1){ 
			C2D2_DesenhaTexto(_fonte, 400, 20, "TURNO DO JOGADOR 1", C2D2_TEXTO_CENTRALIZADO);
			sprintf(str, "RESISTENCIA : %d", vida_jogador1);
			C2D2_DesenhaTexto(_fonte, 400, 50, str , C2D2_TEXTO_CENTRALIZADO);
			sprintf(str, "BALAS DE 15 KG : %d", qtd_bala2_j1);
			C2D2_DesenhaTexto(_fonte, 50, 80, str , C2D2_TEXTO_ESQUERDA);
			sprintf(str, "BALAS DE 20 KG : %d", qtd_bala3_j1);
			C2D2_DesenhaTexto(_fonte, 750, 80, str , C2D2_TEXTO_DIREITA);
		}
		if (turno == 2){ 
			C2D2_DesenhaTexto(_fonte, 400, 20, "TURNO DO JOGADOR 2", C2D2_TEXTO_CENTRALIZADO);
			sprintf(str, "RESISTENCIA : %d", vida_jogador2);
			C2D2_DesenhaTexto(_fonte, 400, 50, str , C2D2_TEXTO_CENTRALIZADO);
			sprintf(str, "BALAS DE 15 KG : %d", qtd_bala2_j2);
			C2D2_DesenhaTexto(_fonte, 50, 80, str , C2D2_TEXTO_ESQUERDA);
			sprintf(str, "BALAS DE 20 KG : %d", qtd_bala3_j2);
			C2D2_DesenhaTexto(_fonte, 750, 80, str , C2D2_TEXTO_DIREITA);
		} 
		//Executa tarefas internas necessárias da chien

		if (vida_jogador1 <= 0 || vida_jogador2 <= 0)
		{
			int explosao_loop;
			int explosao_fim;
			for(explosao_loop=1; explosao_loop<10; explosao_loop++){
				for(explosao_fim=1; explosao_fim<50; explosao_fim=explosao_fim+5){
					C2D2_LimpaTela();
					cenario();
					int gameover = C2D2_CarregaSpriteSet("./imagens/gameover.png", 400, 200); // CARREGA A MENSAGEM DE GAMEOVER
					C2D2_DesenhaSpriteCentro(gameover,0,400,240,400,200);
					C2D2P_CirculoPintado(posicao_atual.x, posicao_atual.y, explosao_fim, 255, 0, 0); 
					C2D2_Sincroniza(C2D2_FPS_PADRAO);
				}
				C2D2_Pausa(100);
			}
			
			title();
		}

		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	//--------------------------------------------------------------
	//--------------------------------------------------------------
	//--------------------------------------------------------------
	//--------------------------------------------------------------
	//--------------------------------------------------------------
	//Retorna 0 por ser padrão
	C2D2_Encerra();
	//encerra a musica
	CA2_RemoveMusica(musica_fundo);
	return 0;
}


void title(void)
{
	qtd_bala1_j1 = 100;
	qtd_bala2_j1 = 2;
	qtd_bala3_j1 = 3;
	qtd_bala1_j2 = 100;
	qtd_bala2_j2 = 2;
	qtd_bala3_j2 = 3;
	vida_jogador1 = 100;
	vida_jogador2 = 100;

	int _fonte = C2D2_CarregaFonte("./imagens/fonte.png", 32); 

	int titulo = C2D2_CarregaSpriteSet("./imagens/titulo.png", 400, 200);
	C2D2_Botao *teclas = C2D2_PegaTeclas();		
	while (!teclas[C2D2_ENTER].pressionado){
		C2D2_LimpaTela();
		C2D2_DesenhaSpriteCentro(titulo,0,400,240,400,200);
		C2D2_DesenhaTexto(_fonte, 400, 10, "ENZO A. MARCHIORATO / POS JOGOS DIGITAIS / PUC-PR", C2D2_TEXTO_CENTRALIZADO);
		//Fundamentos de Matemática e Física - 1º Período (Turma U )
		C2D2_DesenhaTexto(_fonte, 400, 550, "RESSIONE ENTER PARA INICIAR", C2D2_TEXTO_CENTRALIZADO);

		if(teclas[C2D2_ESC].pressionado){
			saijogo=1;
			break;
		}

		if (nuvem_anda > -100){
			C2D2P_CirculoPintado(nuvem_anda, 350, 20, 255, 255, 255); // nuvens
			C2D2P_CirculoPintado(nuvem_anda+30, 350, 30, 255, 255, 255); // nuvens
			C2D2P_CirculoPintado(nuvem_anda+60, 350, 20, 255, 255, 255); // nuvens
			nuvem_anda--;
			}else{
			nuvem_anda = 900;
		}	
		if (nuvem_andab > -100){
			C2D2P_CirculoPintado(nuvem_andab, 300, 20, 255, 255, 255); // nuvens
			C2D2P_CirculoPintado(nuvem_andab+30, 300, 30, 255, 255, 255); // nuvens
			C2D2P_CirculoPintado(nuvem_andab+60, 300, 20, 255, 255, 255); // nuvens
			nuvem_andab = nuvem_andab - 2;
		}else{
			nuvem_andab = 900;
		}
		C2D2_Sincroniza(C2D2_FPS_PADRAO);
	}
	turno = 1;

	//--------------------------------------------------------------------------------------

	//dividida a tela em pedaços de 32pxs 800 x 600 // ISTO E PARA CRIAR O CENARIO DESTRUTIVO

	for (col=0;col<24;col++){
		for (lin=0;lin<17;lin++){
			posicao_tabuleiro[col][lin]=0;
		}
	}

	/*COMO USAR A ENGINE DE CRIAÇÃO
		O MAPA TEM 23 COLUNAS POR 16 LINHAS
		A marca na posição será no valor correspondente do cruzamento da linha e coluna
		por exemplo 
		Se na coluna 5 linha 5 o valor for 1 então será HABILITADO O BLOCO DE DESTRUIÇÃO na posicao correnpondente
		Trocando as posicoes das linhas abaixo altera-se a posição dos blocos 
		OBS: =0 DESABILITADO
		     =1 HABILITADO
		*/
		posicao_tabuleiro[10][ 9]=0; posicao_tabuleiro[11][ 9]=1; posicao_tabuleiro[12][ 9]=0; posicao_tabuleiro[13][ 9]=0;
		posicao_tabuleiro[10][10]=0; posicao_tabuleiro[11][10]=1; posicao_tabuleiro[12][10]=1; posicao_tabuleiro[13][10]=0;
		posicao_tabuleiro[10][11]=0; posicao_tabuleiro[11][11]=1; posicao_tabuleiro[12][11]=1; posicao_tabuleiro[13][11]=0;
		posicao_tabuleiro[10][12]=1; posicao_tabuleiro[11][12]=1; posicao_tabuleiro[12][12]=1; posicao_tabuleiro[13][12]=1;
		posicao_tabuleiro[10][13]=1; posicao_tabuleiro[11][13]=1; posicao_tabuleiro[12][13]=1; posicao_tabuleiro[13][13]=1;

//--------------------------------------------------------------------------------------

}

void cenario(void)
{
		C2D2P_RetanguloPintado(0, 0, 800, 400, 0,0,255);
		//montanhas
		C2D2P_ElipsePintada(0,400,500,70,100,0,0);
		C2D2P_ElipsePintada(700,400,500,90,70,0,0);
	
		C2D2P_RetanguloPintado(0, 400, 800, 600, 0,128,0);
	
		C2D2P_RetanguloPintado(0, 480, 800, 600, 0,0,0);

		//Sol
		C2D2P_ElipsePintada(90,90,100,100,255,246,94);
		
		// Algoritmo para desenhar uma linha reta, baseado em duas coordenadas no plano e uma cor em RGB
		C2D2P_Linha(0,434,800,434,128, 54, 0);// "------"

		C2D2P_RetanguloPintado(90,90,100,100,255,246,94);
		
		//if  (contaloop++ == 20) contaloop = 1;
		
		//if (contaloop == 1)
			
			nuvem_anda = nuvem_anda_inicial + velocidadedovento * 0.05; // posicao = posicaoinicial + velocidade * intervalodetempo
			if (nuvem_anda < -100)
			{
				nuvem_anda_inicial = 900;
			}else if (nuvem_anda > 900)
			{
				nuvem_anda_inicial = -100;
			}else
			{
				C2D2P_CirculoPintado(nuvem_anda, 250, 20, 255, 255, 255); // nuvens
				C2D2P_CirculoPintado(nuvem_anda+30, 250, 30, 255, 255, 255); // nuvens
				C2D2P_CirculoPintado(nuvem_anda+60, 250, 20, 255, 255, 255); // nuvens
				nuvem_anda_inicial = nuvem_anda;
			}
		
			nuvem_andab = nuvem_andab_inicial + (velocidadedovento) * 0.06; // posicao = posicaoinicial + velocidade * intervalodetempo
			if (nuvem_andab < -100)
			{
				nuvem_andab_inicial = 900;
			}else if (nuvem_andab > 900)
			{
				nuvem_andab_inicial = -100;
			}else
			{
				C2D2P_CirculoPintado(nuvem_andab, 200, 20, 255, 255, 255); // nuvens
				C2D2P_CirculoPintado(nuvem_andab+30, 200, 30, 255, 255, 255); // nuvens
				C2D2P_CirculoPintado(nuvem_andab+60, 200, 20, 255, 255, 255); // nuvens
				nuvem_andab_inicial = nuvem_andab;
			}

			col = 0;// coluna
			lin = 0;// linha
			for (col=0;col<24;col++){
				for (lin=0;lin<17;lin++){
					if (posicao_tabuleiro[col][lin]==1){// Gera BLOCO DESTRUTIVEL
						C2D2P_RetanguloPintado(col*32, lin*32, col*32+32, lin*32+32, 136,0,21);
						C2D2P_RetanguloPintado(col*32+2, lin*32+2, col*32+30, lin*32+30, 64,0,0);
						C2D2P_RetanguloPintado(col*32+4, lin*32+4, col*32+28, lin*32+28, 136,0,21);
					}
				}
			}	

}

//------------------------------------------------

				//sen& = CO/HIP = V0y/V0   // VALOR DO SENO DE ALFA
				//cos& = CA/HIP = V0x/HIP  // VALOR DO COSSENO DE ALFA

				//V0x = v0 * COS&    // velocidade HORIZONTAL
				//V0y = v0 * SEN&    // velocidade VERTICAL

				//------------------------------------------------
