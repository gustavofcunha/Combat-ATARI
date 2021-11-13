#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>


const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int BURACO_W=400;
const int BURACO_H=200;

const float THETA = M_PI/4;
const float RAIO_CAMPO_FORCA = 30;
const float VEL_TANQUES = 2.5;
const float PASSO_ANGULO = M_PI/90;

const float RAIO_TIRO = 6.5;
const float VEL_TIROS = 5.0;

typedef struct Ponto{
	float x;
	float y;
}Ponto;

typedef struct Tiro{
	Ponto centro;
	float raio;
	float velocidade;
	ALLEGRO_COLOR cor;
	float x_comp_freeze;
	float y_comp_freeze;
}Tiro;

typedef struct Tanque{
	Ponto centro;
	Ponto A, B, C;
	ALLEGRO_COLOR cor;
	float velocidade;
	float angulo;
	float x_comp;
	float y_comp;
	float vel_angular;
	Tiro tiro;
	int pontos;
	
}Tanque;

typedef struct Obstaculo{
	Ponto sup_esq;
	Ponto sup_dir;
	Ponto inf_dir;
	Ponto inf_esq;
}Obstaculo;

//coloca pontuacao e buraco negro na tela na tela 
void desenhaCenario(Tanque tanque1, Tanque tanque2, ALLEGRO_FONT *size_20, ALLEGRO_BITMAP *buraconegro,ALLEGRO_BITMAP *fundo){
	al_draw_bitmap(fundo, 0, 0, 0);
	al_draw_bitmap(buraconegro, SCREEN_W/2-BURACO_W/2, SCREEN_H/2-BURACO_H/2, 0);
	
	char pont1[20];
	sprintf(pont1, "Tanque 1: %d", tanque1.pontos);	
	al_draw_text(size_20, tanque1.cor, 5, 5, 0, pont1);

	char pont2[20];
	sprintf(pont2, "Tanque 2: %d", tanque2.pontos);	
	al_draw_text(size_20, tanque2.cor, SCREEN_W-200, 5, 0, pont2);

}

float distanciaPontos(Ponto a, Ponto b){
	float distq;
	distq=pow((b.x-a.x),2)+pow((b.y-a.y),2);
	float dist;
	dist=sqrt(distq);
	return dist;
}

int colisaoTanques(Tanque t1, Tanque t2){
	t1.centro.y= t1.centro.y+(t1.velocidade*t1.y_comp);
	t1.centro.x= t1.centro.x+(t1.velocidade*t1.x_comp);
		
	if((distanciaPontos(t1.centro, t2.centro))<(RAIO_CAMPO_FORCA*2))
		return 1;
	
	else
		return 0;
}

int colisaotirotanque(Tanque tanque1, Tanque tanque2){
	tanque1.tiro.centro.x-= tanque1.tiro.velocidade*tanque1.tiro.x_comp_freeze;
	tanque1.tiro.centro.y-= tanque1.tiro.velocidade*tanque1.tiro.y_comp_freeze;
	
	if(((distanciaPontos(tanque1.tiro.centro, tanque2.centro))<(RAIO_CAMPO_FORCA+RAIO_TIRO)) 
	/*&& (tanque1.tiro.velocidade>0)*/)
		return 1;
	
	else
		return 0;
		
		
}

int colisaotirotela(Tanque tanque1){
	
	if((tanque1.tiro.centro.x+RAIO_TIRO>SCREEN_W || tanque1.tiro.centro.x-RAIO_TIRO<0 || tanque1.tiro.centro.y+RAIO_TIRO>SCREEN_H || tanque1.tiro.centro.y-RAIO_TIRO<0) /*&& tanque1.tiro.velocidade>0*/){
		return 1;
	}

	
	else{
		return 0;
	}
	
}

float distFiguras(Tanque tanque, Obstaculo obstaculo){
	float distfig,ladoret,alturaret;
	Ponto p;
	ladoret=BURACO_W;
	alturaret=BURACO_H;

	if(tanque.centro.x>=obstaculo.sup_esq.x && tanque.centro.x<=obstaculo.inf_dir.x && tanque.centro.y<obstaculo.sup_esq.y){
		p.x=tanque.centro.x;
		p.y=obstaculo.sup_esq.y;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.x>=obstaculo.sup_esq.x && tanque.centro.x<=obstaculo.inf_dir.x && tanque.centro.y>obstaculo.sup_esq.y){
		p.x=tanque.centro.x;
		p.y=obstaculo.inf_dir.y;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.y>=obstaculo.sup_esq.y && tanque.centro.y<=obstaculo.inf_dir.y && tanque.centro.x>obstaculo.inf_dir.x){
		p.x=obstaculo.inf_dir.x;
		p.y=tanque.centro.y;
		distfig=distanciaPontos(p,tanque.centro);
	}

	if(tanque.centro.y>=obstaculo.sup_esq.y && tanque.centro.y<=obstaculo.inf_dir.y && tanque.centro.x<obstaculo.sup_esq.x){
		p.x=obstaculo.sup_esq.x;
		p.y=tanque.centro.y;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.x>obstaculo.inf_dir.x && tanque.centro.y<obstaculo.sup_esq.y){
		p.x= obstaculo.sup_esq.x + ladoret;
		p.y= obstaculo.sup_esq.y;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.x>obstaculo.inf_dir.x && tanque.centro.y>obstaculo.inf_dir.y){
		p=obstaculo.inf_dir;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.x<obstaculo.sup_esq.x && tanque.centro.y>obstaculo.inf_dir.y){
		p.x= obstaculo.sup_esq.x;
		p.y= obstaculo.inf_dir.y;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.x<obstaculo.sup_esq.x && tanque.centro.y<obstaculo.sup_esq.y){
		p=obstaculo.sup_esq;
		distfig=distanciaPontos(p,tanque.centro);
	}
	
	if(tanque.centro.x>obstaculo.sup_esq.x && tanque.centro.x<obstaculo.inf_dir.x && tanque.centro.y>obstaculo.sup_esq.y && tanque.centro.y<obstaculo.inf_dir.y){
		distfig=0;
	}
	
	return distfig;
}


int colisaoTanqueObstaculo(Tanque tanque, Obstaculo obstaculo) {
	//simulacao
	tanque.centro.y= tanque.centro.y+(tanque.velocidade*tanque.y_comp);
	tanque.centro.x= tanque.centro.x+(tanque.velocidade*tanque.x_comp);
	
	if(distFiguras(tanque,obstaculo)<=RAIO_CAMPO_FORCA)
		return 1;
    
	if(distFiguras(tanque,obstaculo)>RAIO_CAMPO_FORCA)
		return 0;

}

float distFiguras2(Tiro tiro, Obstaculo obstaculo){
	float distfig,ladoret,alturaret;
	Ponto p;
	ladoret=BURACO_W;
	alturaret=BURACO_H;

	if(tiro.centro.x>=obstaculo.sup_esq.x && tiro.centro.x<=obstaculo.inf_dir.x && tiro.centro.y<obstaculo.sup_esq.y){
		p.x=tiro.centro.x;
		p.y=obstaculo.sup_esq.y;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.x>=obstaculo.sup_esq.x && tiro.centro.x<=obstaculo.inf_dir.x && tiro.centro.y>obstaculo.sup_esq.y){
		p.x=tiro.centro.x;
		p.y=obstaculo.inf_dir.y;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.y>=obstaculo.sup_esq.y && tiro.centro.y<=obstaculo.inf_dir.y && tiro.centro.x>obstaculo.inf_dir.x){
		p.x=obstaculo.inf_dir.x;
		p.y=tiro.centro.y;
		distfig=distanciaPontos(p,tiro.centro);
	}

	if(tiro.centro.y>=obstaculo.sup_esq.y && tiro.centro.y<=obstaculo.inf_dir.y && tiro.centro.x<obstaculo.sup_esq.x){
		p.x=obstaculo.sup_esq.x;
		p.y=tiro.centro.y;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.x>obstaculo.inf_dir.x && tiro.centro.y<obstaculo.sup_esq.y){
		p.x= obstaculo.sup_esq.x + ladoret;
		p.y= obstaculo.sup_esq.y;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.x>obstaculo.inf_dir.x && tiro.centro.y>obstaculo.inf_dir.y){
		p=obstaculo.inf_dir;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.x<obstaculo.sup_esq.x && tiro.centro.y>obstaculo.inf_dir.y){
		p.x= obstaculo.sup_esq.x;
		p.y= obstaculo.inf_dir.y;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.x<obstaculo.sup_esq.x && tiro.centro.y<obstaculo.sup_esq.y){
		p=obstaculo.sup_esq;
		distfig=distanciaPontos(p,tiro.centro);
	}
	
	if(tiro.centro.x>obstaculo.sup_esq.x && tiro.centro.x<obstaculo.inf_dir.x && tiro.centro.y>obstaculo.sup_esq.y && tiro.centro.y<obstaculo.inf_dir.y){
		distfig=0;
	}
	
	return distfig;
}


int colisaoTiroObstaculo(Tanque tanque, Obstaculo obstaculo) {
	//simulacao
	tanque.tiro.centro.x-= tanque.tiro.velocidade*tanque.tiro.x_comp_freeze;
	tanque.tiro.centro.y-= tanque.tiro.velocidade*tanque.tiro.y_comp_freeze;
	
	if(distFiguras2(tanque.tiro,obstaculo)<=RAIO_TIRO)
		return 1;
    
	if(distFiguras2(tanque.tiro,obstaculo)>RAIO_TIRO)
		return 0;

}

int colisaoTelaTanquesY(Tanque tanque){
	//simulacao
	tanque.centro.y+=tanque.velocidade*tanque.y_comp;
	
	if(tanque.centro.y-RAIO_CAMPO_FORCA>=0 && tanque.centro.y+RAIO_CAMPO_FORCA<=SCREEN_H){
		return 0;
	}
	
	else{
		return 1;
	}
}

int colisaoTelaTanquesX(Tanque tanque){
	//simulacao
	tanque.centro.x+=tanque.velocidade*tanque.x_comp;
	
	if(tanque.centro.x-RAIO_CAMPO_FORCA>=0 && tanque.centro.x+RAIO_CAMPO_FORCA<=SCREEN_W){
		return 0;
	}
	
	else{
		return 1;
	}
}

//incializa tanque1
void initTanque1(Tanque *t){
	t->centro.x=160;
	t->centro.y=SCREEN_H/2-50;
	//t->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
	t->cor = al_map_rgb(255,0,255);
	
	t->A.x=0;
	t->A.y= -RAIO_CAMPO_FORCA;
	
	float alpha= M_PI/2 - THETA;
	float h = RAIO_CAMPO_FORCA * sin(alpha);
	float w = RAIO_CAMPO_FORCA * sin(THETA);

	
	t->B.x= -w;
	t->B.y= h;
	
	t->C.x= w;
	t->C.y= h;
	
	t->velocidade=0;
	t->angulo=M_PI/2;
	t->x_comp=cos(t->angulo);
	t->y_comp=sin(t->angulo);
	t->vel_angular=0;
	
	//incializa tiros (ponto A como referencia)
	t->tiro.centro.x= t->centro.x + t->A.x;
	t->tiro.centro.y= t->centro.y + t->A.y;
	t->tiro.cor = t->cor;
	t->tiro.velocidade = 0;
	
	//inicializa pontuacao com zero
	t->pontos=0;
	
}

//incializa tanque2
void initTanque2(Tanque *t){
	t->centro.x=SCREEN_W-160;
	t->centro.y=SCREEN_H/2+100;
	//t->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
	t->cor = al_map_rgb(253, 233, 16);
	
	t->A.x=0;
	t->A.y=-RAIO_CAMPO_FORCA;
	
	float alpha= M_PI/2 - THETA;
	float h = RAIO_CAMPO_FORCA * sin(alpha);
	float w = RAIO_CAMPO_FORCA * sin(THETA);

	
	t->B.x= -w;
	t->B.y= h;
	
	t->C.x= w;
	t->C.y= h;
	
	t->velocidade=0;
	t->angulo=M_PI/2;
	t->x_comp=cos(t->angulo);
	t->y_comp=sin(t->angulo);
	t->vel_angular=0;
	
	//incializa tiros (ponto A como referencia)
	t->tiro.centro.x= t->centro.x + t->A.x;
	t->tiro.centro.y= t->centro.y + t->A.y;
	t->tiro.cor = t->cor;
	t->tiro.velocidade = 0;
	
	//inicializa pontuacao com zero
	t->pontos=0;
	
}

//desenha tanques na tela
void desenhaTanque(Tanque t){
	
	al_draw_circle(t.centro.x, t.centro.y, RAIO_CAMPO_FORCA, t.cor, 1);
	
	al_draw_filled_triangle (t.A.x + t.centro.x, t.A.y + t.centro.y, 
	                         t.B.x + t.centro.x, t.B.y + t.centro.y, 
							 t.C.x + t.centro.x, t.C.y + t.centro.y, 
							 t.cor);
							 
	al_draw_filled_circle(t.tiro.centro.x, t.tiro.centro.y, RAIO_TIRO, t.tiro.cor);						 
}

void Rotate(Ponto *p, float angulo2){
	float x= p->x, y=p->y;
	
	p->x=(x*cos(angulo2))-(y*sin(angulo2));
	p->y=(y*cos(angulo2))+(x*sin(angulo2));
}

void rotacionaTanque(Tanque *t){
	if(t->vel_angular!=0){
		
		Rotate(&t->A, t->vel_angular);
		Rotate(&t->B, t->vel_angular);
		Rotate(&t->C, t->vel_angular);
		
		t->angulo +=  t->vel_angular;
		t->x_comp = cos(t->angulo);
		t->y_comp = sin(t->angulo);
	}

}

	
void atualizaTanque(Tanque *tanque1, Tanque tanque2, Obstaculo obstaculo){
	rotacionaTanque(tanque1);
	
	//movimento do tanque no eixo y (condicoes para impedir que saia da tela, impedir colisoes com outro tanque e impedir colisoes com obstaculo)
	if((!(colisaoTelaTanquesY((*tanque1)))) && (!(colisaoTanques((*tanque1), tanque2))) && (!colisaoTanqueObstaculo((*tanque1), obstaculo))){
		tanque1->centro.y+=tanque1->velocidade*tanque1->y_comp;
	}
	
	//movimento do tanque no eixo x (condicoes para impedir que saia da tela, impedir colisoes com outro tanque e impedir colisoes com obstaculo)
	if((!(colisaoTelaTanquesX((*tanque1)))) && (!(colisaoTanques((*tanque1), tanque2))) && (!colisaoTanqueObstaculo((*tanque1), obstaculo))){
		tanque1->centro.x+=tanque1->velocidade*tanque1->x_comp;
	}
	
	//movimento do tiro junto ao tanque (nao disparado)
	if(tanque1->tiro.velocidade==0){
		tanque1->tiro.centro.x= (tanque1->centro.x + tanque1->A.x)-RAIO_TIRO*tanque1->x_comp;
		tanque1->tiro.centro.y= (tanque1->centro.y + tanque1->A.y)-RAIO_TIRO*tanque1->y_comp;
	}
	
	//movimento do tiro quando disparado (nao muda de direcao junto ao tanque, mais)
	if(tanque1->tiro.velocidade>0){
		tanque1->tiro.centro.x-= tanque1->tiro.velocidade*tanque1->tiro.x_comp_freeze;
		tanque1->tiro.centro.y-= tanque1->tiro.velocidade*tanque1->tiro.y_comp_freeze;
	}
	
	//volta o tiro para o tanque, caso haja colisao com a tela, com outro tanque ou com o obstaculo
	if(colisaotirotela(*tanque1)|| colisaotirotanque((*tanque1), tanque2) || colisaoTiroObstaculo((*tanque1),obstaculo)){
		
		//sistema de pontuacao
		if(colisaotirotanque((*tanque1), tanque2)){
			if(tanque1->tiro.velocidade>0){
				tanque1->pontos++;
				printf("\nPontuacao: %d",tanque1->pontos);
			}
		}
		
		
		tanque1->tiro.velocidade=0;
		tanque1->tiro.centro.x= tanque1->centro.x + tanque1->A.x ;
		tanque1->tiro.centro.y= tanque1->centro.y + tanque1->A.y;
	}
}


//main =================================================================================================
int main(int argc, char **argv){
	int macro=1;
	while(macro){
		srand(time(NULL));
		ALLEGRO_DISPLAY *display = NULL;
		ALLEGRO_EVENT_QUEUE *event_queue = NULL;
		ALLEGRO_TIMER *timer = NULL;
	   
	   
	   
		//----------------------- rotinas de inicializacao ---------------------------------------
		
		//inicializa o Allegro
		if(!al_init()) {
			fprintf(stderr, "failed to initialize allegro!\n");
			return -1;
		}
		
		//inicializa o módulo de primitivas do Allegro
		if(!al_init_primitives_addon()){
			fprintf(stderr, "failed to initialize primitives!\n");
			return -1;
		}	
		
		//inicializa o modulo que permite carregar imagens no jogo
		if(!al_init_image_addon()){
			fprintf(stderr, "failed to initialize image module!\n");
			return -1;
		}
	   
		//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
		timer = al_create_timer(1.0 / FPS);
		if(!timer) {
			fprintf(stderr, "failed to create timer!\n");
			return -1;
		}
	 
		//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
		display = al_create_display(SCREEN_W, SCREEN_H);
		if(!display) {
			fprintf(stderr, "failed to create display!\n");
			al_destroy_timer(timer);
			return -1;
		}

		//instala o teclado
		if(!al_install_keyboard()) {
			fprintf(stderr, "failed to install keyboard!\n");
			return -1;
		}
		

		//inicializa o modulo allegro que carrega as fontes
		al_init_font_addon();

		//inicializa o modulo allegro que entende arquivos tff de fontes
		if(!al_init_ttf_addon()) {
			fprintf(stderr, "failed to load tff font module!\n");
			return -1;
		}
		
		
		//carrega fontes e define tamanhos
		ALLEGRO_FONT *size_15 = al_load_font("atari-abandoned.ttf", 15, 1);   
		if(size_15 == NULL) {
			fprintf(stderr, "font file does not exist or cannot be accessed!\n");
		}
		
		ALLEGRO_FONT *size_20 = al_load_font("atari-abandoned.ttf", 20, 1);   
		if(size_20 == NULL) {
			fprintf(stderr, "font file does not exist or cannot be accessed!\n");
		}
		
		ALLEGRO_FONT *size_32 = al_load_font("atari-abandoned.ttf", 32, 1);   
		if(size_32 == NULL) {
			fprintf(stderr, "font file does not exist or cannot be accessed!\n");
		}
		
		ALLEGRO_FONT *size_40 = al_load_font("atari-abandoned.ttf", 40, 1);   
		if(size_40 == NULL) {
			fprintf(stderr, "font file does not exist or cannot be accessed!\n");
		}
		
		ALLEGRO_FONT *size_70 = al_load_font("atari-abandoned.ttf", 70, 1);   
		if(size_70 == NULL) {
			fprintf(stderr, "font file does not exist or cannot be accessed!\n");
		}

		//cria a fila de eventos
		event_queue = al_create_event_queue();
		if(!event_queue) {
			fprintf(stderr, "failed to create event_queue!\n");
			al_destroy_display(display);
			return -1;
		}
	   


		//registra na fila os eventos de tela (ex: clicar no X na janela)
		al_register_event_source(event_queue, al_get_display_event_source(display));
		//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
		al_register_event_source(event_queue, al_get_timer_event_source(timer));
		//registra na fila os eventos de teclado (ex: pressionar uma tecla)
		al_register_event_source(event_queue, al_get_keyboard_event_source());
		
	
		//declara e carrega imagens
		ALLEGRO_BITMAP *fundo = al_load_bitmap("universe.jpg");
		ALLEGRO_BITMAP *buraconegro = al_load_bitmap("buraconegro3.png");

		//cria e incializa pontos do obstaculo
		Obstaculo obstaculo;
		
		obstaculo.sup_esq.x=SCREEN_W/2-200;
		obstaculo.sup_esq.y=SCREEN_H/2-100;
		
		obstaculo.sup_dir.x=obstaculo.sup_esq.x+BURACO_W;
		obstaculo.sup_dir.y=obstaculo.sup_esq.y;
		
		obstaculo.inf_esq.x=obstaculo.sup_esq.x;
		obstaculo.inf_esq.y=obstaculo.sup_esq.y+BURACO_H;
		
		obstaculo.inf_dir.x=obstaculo.sup_dir.x;
		obstaculo.inf_dir.y=obstaculo.inf_esq.y;
	
		//----------------------- fim das rotinas de inicializacao ---------------------------------------
	
	
		//inicia o temporizador
		al_start_timer(timer);
		
		int playing = 1;
		
		//tela de inicio ---------------------------------------------------------------------------
		while(playing==1) {

			
			ALLEGRO_EVENT ev2;
			//espera por um evento e o armazena na variavel de evento ev
			al_wait_for_event(event_queue, &ev2);

			//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
			if(ev2.type == ALLEGRO_EVENT_TIMER) {
				al_draw_bitmap(fundo, 0, 0, 0);

				
				al_draw_text(size_70, al_map_rgb(255, 255, 255), SCREEN_W/2-210, SCREEN_H/2-120, 0, "Combat");
				al_draw_text(size_40, al_map_rgb(30, 144, 255), SCREEN_W/2-100, SCREEN_H/2, 0, "atari");
				
				al_draw_text(size_32, al_map_rgb(255, 219, 88), SCREEN_W/2-380, SCREEN_H/2+150, 0, "pressione              para jogar");
				
				al_draw_text(size_32, al_map_rgb(rand()%256, rand()%256, rand()%256), SCREEN_W/2-80, SCREEN_H/2+150, 0, "enter");
				al_flip_display();
			}
		
			//se o tipo de evento for o fechamento da tela (clique no x da janela)
			else if(ev2.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				playing=0;
			}
			
			//se o tipo de evento for um pressionar de uma tecla
			else if(ev2.type == ALLEGRO_EVENT_KEY_DOWN) {
				//imprime qual tecla foi apertada
				printf("\ncodigo tecla apertada: %d", ev2.keyboard.keycode);
				
				switch(ev2.keyboard.keycode){
					case ALLEGRO_KEY_ENTER:
						playing=2;
					break;
				}
			}
		
		}//fim do while da tela de inicio------------------------------------------------------------------------
		
		
		//cria e inicializa os tanques
		Tanque tanque1,tanque2;
		initTanque1(&tanque1);
		initTanque2(&tanque2);
		
		
		int vencedor=0;
		int consumido=0;
		
		//tela do jogo-----------------------------------------------------------------------------------------
		while(playing==2) {
			ALLEGRO_EVENT ev;
			//espera por um evento e o armazena na variavel de evento ev
			al_wait_for_event(event_queue, &ev);

			//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
			if(ev.type == ALLEGRO_EVENT_TIMER) {
				desenhaCenario(tanque1, tanque2, size_20, buraconegro, fundo);
				
				//atualiza, testa e desenha tanque 1
				atualizaTanque(&tanque1, tanque2, obstaculo);
				if(tanque1.pontos>=5){
					vencedor=1;
					playing=0;
				}
				
				if(colisaoTanqueObstaculo(tanque1,obstaculo)){
					vencedor=2;
					consumido=1;
					playing=0;
				}
				desenhaTanque(tanque1);
				
				
				//atualiza, testa e desenha tanque 2
				atualizaTanque(&tanque2, tanque1, obstaculo);
				if(tanque2.pontos>=5){
					vencedor=2;
					playing=0;
				}
				
				if(colisaoTanqueObstaculo(tanque2,obstaculo)){
					vencedor=1;
					consumido=2;
					playing=0;
				}
				desenhaTanque(tanque2);
				

				//atualiza a tela (quando houver algo para mostrar)
				al_flip_display();
				
				if(al_get_timer_count(timer)%(int)FPS == 0)
					printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
			}
			
			
			//se o tipo de evento for o fechamento da tela (clique no x da janela)
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				playing = 0;
			}

			//se o tipo de evento for um pressionar de uma tecla
			else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				//imprime qual tecla foi apertada
				printf("\ncodigo tecla apertada: %d", ev.keyboard.keycode);
				
				switch(ev.keyboard.keycode){
					
					//comandos tanque 1
					case ALLEGRO_KEY_W:
						tanque1.velocidade-=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_S:
						tanque1.velocidade+=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_D:
						tanque1.vel_angular+=PASSO_ANGULO;
					break;
					
					
					case ALLEGRO_KEY_A:
						tanque1.vel_angular-=PASSO_ANGULO;
					break;
					
					case ALLEGRO_KEY_Q:
						if(tanque1.tiro.velocidade==0){
							tanque1.tiro.x_comp_freeze=tanque1.x_comp;
							tanque1.tiro.y_comp_freeze=tanque1.y_comp;
							tanque1.tiro.velocidade= VEL_TIROS;
						}
					break;
					
					
					
					//comandos tanque 2
					case ALLEGRO_KEY_UP:
						tanque2.velocidade-=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_DOWN:
						tanque2.velocidade+=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_RIGHT:
						tanque2.vel_angular+=PASSO_ANGULO;
					break;
					
					
					case ALLEGRO_KEY_LEFT:
						tanque2.vel_angular-=PASSO_ANGULO;
					break;
					
					case ALLEGRO_KEY_ENTER:
						if(tanque2.tiro.velocidade==0){
							tanque2.tiro.x_comp_freeze=tanque2.x_comp;
							tanque2.tiro.y_comp_freeze=tanque2.y_comp;
							tanque2.tiro.velocidade= VEL_TIROS;
						}
					break;
				}
			}
			
			//se o tipo de evento for soltar uma tecla
			else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
				//imprime qual tecla foi
				printf("\ncodigo tecla soltada: %d", ev.keyboard.keycode);
				
				switch(ev.keyboard.keycode){
					
					//comandos tanque 1
					case ALLEGRO_KEY_W:
						tanque1.velocidade+=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_S:
						tanque1.velocidade-=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_D:
						tanque1.vel_angular-=PASSO_ANGULO;
					break;
					
					
					case ALLEGRO_KEY_A:
						tanque1.vel_angular+=PASSO_ANGULO;
					break;
					
					
					//comandos tanque 2
					case ALLEGRO_KEY_UP:
						tanque2.velocidade+=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_DOWN:
						tanque2.velocidade-=VEL_TANQUES;
					break;
					
					
					case ALLEGRO_KEY_RIGHT:
						tanque2.vel_angular-=PASSO_ANGULO;
					break;
					
					
					case ALLEGRO_KEY_LEFT:
						tanque2.vel_angular+=PASSO_ANGULO;
					break;
				}
			}

		} //fim do while da tela de jogo-----------------------------------------------------------------------------------
		 
		//leitura do que esta no historico para alteracao posterior
		FILE *arq;
		arq=fopen("historico.txt","r");
		int h1=0,h2=0;
		fscanf(arq, "%d %d", &h1, &h2);
		fclose(arq);
		
		
		//alteracao do historico
		if(vencedor>0){
			arq=fopen("historico.txt","w");
			if(vencedor==1){
				fprintf(arq, "%d %d", h1+1,h2);
			}
			
			if(vencedor==2){
				fprintf(arq, "%d %d", h1,h2+1);	
			}		
			fclose(arq);
		}
		
	
		 
		 //tela de fim de jogo----------------------------------------------------------------------------------------
		while(!(playing)){
			ALLEGRO_EVENT ev3;
				//espera por um evento e o armazena na variavel de evento ev
				al_wait_for_event(event_queue, &ev3);
			
			//strings para impressoes na tela (funcao fprintf)
			char vencedor_ptr[20];
			char placar[5];
			char historico1[20];
			char historico2[20];
			
			
			if(ev3.type == ALLEGRO_EVENT_TIMER) {
				al_draw_bitmap(fundo, 0, 0, 0);
				
				//desenho ilustrativo dos tanques na tela final
				desenhaTanque(tanque1);
				desenhaTanque(tanque2);
				
				if(vencedor>0){
					sprintf(vencedor_ptr, "Vitoria do Tanque %d!", vencedor);	
					al_draw_text(size_40, al_map_rgb(30, 144, 255), SCREEN_W/3-200, SCREEN_H/2-115, 0, vencedor_ptr);
					
						
					//impressao de mensagem caso algum tanque seja consumido pelo buraco negro	
					if(consumido==1){
					al_draw_text(size_20, al_map_rgb(255, 219, 88), SCREEN_W/3-250, SCREEN_H/2-200, 0, "O Tanque 1 foi consumido pelo Buraco Negro...");
					}
				
					if(consumido==2){
					al_draw_text(size_20, al_map_rgb(255, 219, 88), SCREEN_W/3-250, SCREEN_H/2-200, 0, "O Tanque 2 foi consumido pelo Buraco Negro...");
					}	
					
				}
				
				//leitura do historico para exibicao
				arq=fopen("historico.txt","r");
				int h11,h22;
				fscanf(arq, "%d %d", &h11, &h22);
				fclose(arq);
				
				sprintf(placar, "Placar desta partida: %d x %d", tanque1.pontos, tanque2.pontos);	
				al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/3-225, SCREEN_H/2, 0, placar);
				
				al_draw_text(size_20, al_map_rgb(255, 255, 255), SCREEN_W/3-40, SCREEN_H/2+105, 0, "Historico de partidas:");
				
				sprintf(historico1, "Tanque 1: %d vitorias", h11);	
				al_draw_text(size_20, tanque1.cor, SCREEN_W/3-20, SCREEN_H/2+140, 0, historico1);
				
				sprintf(historico2, "Tanque 2: %d vitorias", h22);	
				al_draw_text(size_20, tanque2.cor, SCREEN_W/3-20, SCREEN_H/2+180, 0, historico2);
				
				al_draw_text(size_15, al_map_rgb(rand()%256,rand()%256,rand()%256), 5, SCREEN_H-20, 0, "Pressione enter para jogar novamente");
				al_draw_text(size_15, al_map_rgb(255, 0, 0), SCREEN_W-300, SCREEN_H-20, 0, "Pressione x para sair do jogo");
				
				al_flip_display();
			}
			
	
			
			else if(ev3.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				playing=11;
				macro=0;
			}
		
			
			else if(ev3.type == ALLEGRO_EVENT_KEY_DOWN) {
				//imprime qual tecla foi apertada
				printf("\ncodigo tecla apertada: %d", ev3.keyboard.keycode);
				switch(ev3.keyboard.keycode){
					case ALLEGRO_KEY_ENTER:
						playing=1;
					break;
					
					case ALLEGRO_KEY_X:
						playing=11;
						macro=0;
					break;
				}
			}	
		}//fim do while da tela de fim de jogo --------------------------------------------------------------	
	
		//al_rest(1);
		//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
		al_destroy_bitmap(fundo);
		al_destroy_bitmap(buraconegro);
		al_destroy_timer(timer);
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
	}
	return 0;
	
}