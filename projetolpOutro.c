#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
MUDANÇAS DO ORIGINAL
1. Listar Atividades
2. Listar Colaboradores
3. Buscar
4. Estatisticas Gerais
5. Ranking

As alterações feitas em atividades foram compactadas em uma única ação Editar Atividades
    -> Filtro de exibição, mudar nome, etapa, prioridade, descrição, colaborador e excluir
As alterações feitas em colaboradores foram compactadas em uma única ação Editar Colaboradores
    -> Exibir atividades e estatisticas, cadastrar novo usuario, cadastrar nova tarefa, editar nome, excluir
As demais seguem tendo ações próprias

Acrescentou-se: Indentação

*/
typedef struct {
    char nome[100];
    char descricao[500];
    int info[2]; //0: prioridade; 1: etapa
    //(1, 2 ou 3, alta, media, baixa); (1, 2 ou 3, a fazer, em andamento, concluido)
}itens;

typedef struct {
    char nome[100];
    char codigo[6]; //5 digitos
    int concluidas; 
    int lim; //total de tarefas de um colaborador
    itens tarefa[100];
}colaborador;

typedef struct{
    int user;
    int tarefa;
}info;

colaborador user[100]; 
int p=0; //Contador de colaboradores
int nivel=0;

//
//FUNCOES DE VERIFICACAO
int string_vazia(char string[]);

int posicao_codigo(char codigo[]);

//
//FUNCOES DE USO GERAL
int encerrar();

void minusculas(char string[]);

void remover_enter(char t[]);

void tirar_espacos(char string[]);

void exibir_menu();

void reorganizacao_por_status(int x);

void exibir_tipo(int x, int y);

void exibir_tarefas(int k);

void exibir_descricao(int x, int y);

//
//FUNCOES DE LEITURA DE DADOS NAO-ESPECIFICOS

int ler_string(char string[], int tam);

int ler_int(int a, int b);

//
//FUNCOES DE LEITURA DE DADOS ESPECIFICOS
int lerString_codigo(char codigo[]);

int lerString_tarefa(char tarefa[]);

int procurar_colaborador();
//
//SALVAR E CARREGAR DADOS
int carregar_dados(colaborador *c);

int salvar_dados(colaborador *c);

//
//FUNCIONALIDADES

void exibir_tarefasIntegra(int x, info pessoa[], int *contador);

void exibir_tarefasFiltro(int filtro, int tipo, info pessoa[], int *contador);

void editar_tarefa(int x, int y);

void espacos();

//
//
int main(){
    if(!carregar_dados(user)) return 0;

    exibir_menu();

    do{
        nivel=0;
        printf("=========================================================\n");
        printf("Escolha uma opcao do menu(6) ou digite 0 para finalizar.\n> ");
        int opcao = ler_int(0, 6);

        if(!opcao){
            printf("Programa finalizado.\n"); break;
        }
        else if(opcao==1){//listagem de atividades
            printf("\n> Listagem de atividades\n");
            do{
                nivel=1;
                info pessoa[p*100];
                int contador=0;

                printf("\n--> Exibir por:\n"); espacos();
                printf("1. Ordem de Prioridade\n"); espacos();
                printf("2. Ordem de Etapa\n"); espacos();
                printf("3. Aplicar Filtro\n"); espacos();
                printf("> ");
                int exibicao = ler_int(0, 3);

                //exibicao na tela
                if(!exibicao)break;
                else if(exibicao!=3)//sem filtro
                    exibir_tarefasIntegra(exibicao-1, pessoa, &contador);
                else{//com filtro
                    nivel++;  //(2)
                    espacos(nivel-1); printf("--> Filtrar por (1)Prioridade ou (2)Etapa: ");
                    int escolha = ler_int(0, 2);

                    if(!escolha)break;
                    else{
                        escolha--;
                        espacos();
                        if(!escolha) {
                            printf("Selecione a prioridade:\n"); espacos();
                            printf("1. Alta\n"); espacos();
                            printf("2. Media\n"); espacos();
                            printf("3. Baixa\n"); espacos();
                            printf("> "); 
                        }
                        else {
                            printf("Selecione a etapa:\n"); espacos();
                            printf("1. A fazer\n"); espacos();
                            printf("2. Em andamento\n"); espacos();
                            printf("3. Concluida\n"); espacos();
                            printf("> ");
                            }

                        int x = ler_int(0, 3);
                        if(!x)break;

                        printf("\n");
                        espacos(); 
                        printf("Exibindo todas as tarefas com filtro:\n"); exibir_tipo(escolha, x); printf("\n");
                        exibir_tarefasFiltro(x, escolha, pessoa, &contador);
                    }
                }
                
                //Edição de atividade. Vai se repetir para uma exibição especifica
                do{
                    espacos();
                    printf("--> Escolha uma tarefa para mais acoes.\n"); espacos();
                    printf("> ");
                    int t = ler_int(0, contador);
                    if(!t)break; t--;

                    editar_tarefa(pessoa[t].user, pessoa[t].tarefa);
                }while(1);
                nivel=1;
            }while(1);//escolha de exibicao
        }
        else if(opcao==2){//listagem de colaboradores
            do{
                nivel=1;
                printf("\n> Listagem de colaboradores\n");
                for(int i=0; i<p; i++){
                    printf("%d. %s (%s) - %d atividades pendentes\n", 
                        i+1, user[i].nome, user[i].codigo, user[i].lim-user[i].concluidas);
                }
                printf("\n--> Escolha uma acao:\n"); espacos();
                printf("1. Cadastrar novo colaborador\n"); espacos();
                printf("2. Selecionar colaborador\n"); espacos();
                printf("> ");

                nivel=2;
                int escolha = ler_int(0, 2);
                if(!escolha)break;
                else if(escolha==1) do{//cadastrar
                    if(p==100){
                        printf("O programa ja atingiu a capacidade maxima de usuarios cadastrados."
                        "Remova um usuario para adicionar outros.\n");
                        break;
                    }

                    char codigo[6], nome[100];
                    int s;
                    espacos(); printf("--> Codigo: ");
                    do{
                        while((s=lerString_codigo(codigo))==1);
                        if(s==2)break;

                        if((s=posicao_codigo(codigo))!=-1){
                            espacos();
                            printf("Este codigo ja esta sendo usado. Tente novamente.\n");
                            espacos();
                        }
                        else break;
                    }while(1);
                    if(s==2)break;

                    espacos(); printf("--> Nome: ");
                    while((s=ler_string(nome, 100))==1);
                    if(s==2)break;

                    printf("\n");
                    espacos(); printf("O novo usuario sera adicionado: %s (%s)\n", nome, codigo);
                    if(encerrar())break;

                    strcpy(user[p].nome, nome); strcpy(user[p].codigo, codigo);
                    user[p].concluidas=0; user[p].lim=0;
                    p++;
                    espacos(); printf("Usuario cadastrado com sucesso.\n\n");

                    espacos(); printf("Voce pode adicionar um novo colaborador.\n");
                    if(encerrar())break;
                    printf("\n");
                }while(1);
                else if(escolha==2) do{//selecionar
                    if(!p){
                        espacos();
                        printf("Nao existem colaboradores cadastrados.\n");
                        break;
                    }

                    int x;

                    printf("\n");
                    espacos(); printf("--> Selecione o usuario para editar (codigo): ");
                    while((x=procurar_colaborador(x))==-1);
                    if(x==-2)break;
                    
                    do{ //editar
                        nivel=3;
                        printf("\n");
                        espacos();
                        printf("Colaborador selecionado: %s (%s)\n", user[x].nome, user[x].codigo);
                        espacos();
                        printf("--> Escolha uma acao:\n"); espacos();
                        printf("1. Exibir tarefas\n"); espacos();
                        printf("2. Cadastrar tarefa\n"); espacos();
                        printf("3. Editar nome\n"); espacos();
                        printf("4. Remover usuario\n"); espacos();
                        printf("> ");
                        int escolha = ler_int(0, 4);

                        nivel=4;
                        if(!escolha)break;
                        else if(escolha==1){//exibir tarefas
                            exibir_tarefas(x);
                            if(user[x].lim==0)continue;
                            espacos(nivel-1);
                            printf("--> Escolha uma tarefa para realizar mais acoes.\n"); espacos();
                            printf("> ");
                            int item = ler_int(0, user[x].lim);
                            if(!item)continue;; item--;

                            nivel++;
                            editar_tarefa(x, item);
                        }
                        else if(escolha==2) do{//add tarefa
                            char nome[100], descricao[500];
                            int prioridade, s;

                            espacos();
                            printf("--> Nome da nova tarefa\n"); espacos();
                            printf("> ");
                            while((s=lerString_tarefa(nome))==1);
                            if(s==2)break;

                            espacos();
                            printf("--> Adicione uma descricao, ou deixe vazio para pular.\n"); espacos();
                            printf("> ");
                            while((s=ler_string(descricao, 500))==1);
                            
                            espacos();
                            printf("--> Prioridade (1-3): ");
                            prioridade = ler_int(0, 3);
                            if(!prioridade)break;

                            int tam = user[x].lim;
                            strcpy(user[x].tarefa[tam].nome, nome); 
                            strcpy(user[x].tarefa[tam].descricao, descricao);
                            user[x].tarefa[tam].info[0] = prioridade;
                            user[x].tarefa[tam].info[1] = 1; reorganizacao_por_status(x);
                            espacos();
                            printf("Tarefa cadastrada com sucesso.\n\n"); espacos();

                            printf("Voce pode adicionar outras tarefas.\n");
                            if(encerrar())break;
                        }while(1);
                        else if(escolha==3){//mudar nome
                            char novo[100];
                            int s;

                            espacos();
                            printf("--> Novo nome: ");
                            while((s=ler_string(novo, 100))==1);
                            if(s==2)continue;

                            espacos();
                            printf("A seguinte modificacao sera feita:\n"); espacos();
                            printf("%s -> %s\n", user[x].nome, novo);
                            if(encerrar())continue;

                            strcpy(user[x].nome, novo); espacos();
                            printf("Mudanca bem sucedida.\n");
                        }
                        else if(escolha==4){//remover
                            espacos();
                            printf("Esta acao nao pode ser desfeita. Todas as seguintes tarefas tambem serao apagadas.\n");
                            exibir_tarefas(x);

                            if(encerrar())break;

                            for(int i=x; i<p; i++)user[i]=user[i+1];
                            p--;

                            espacos();
                            printf("Remocao concluida.\n");
                            break;
                        }

                        nivel=2;
                        espacos();
                        printf("Voce pode realizar mais acoes com o colaborador selecionado.\n");
                        if(encerrar())break;
                    }while(1);
                    nivel=2;
                }while(1);
                nivel=1;
            }while(1);
        }
        else if(opcao==3){//busca
            printf("\n> Busca\n");
            do{
                char busca[100];
                int s;

                printf("\n--> Campo de Busca: ");
                while((s=ler_string(busca, 100))==1);
                if(s==2)break;
                minusculas(busca);

                do{
                    nivel=2;
                    printf("\n");
                    espacos();
                    printf("Exibindo resultados para a busca: '%s'\n\n", busca);
                    int tam=0;
                    info item[p*100];
                    for(int i=0; i<p; i++){
                        for(int j=0; j<user[i].lim; j++){

                            char copia[100];
                            strcpy(copia, user[i].tarefa[j].nome);
                            minusculas(copia);

                            if(strstr(copia, busca)!=NULL){
                                item[tam].user = i;
                                item[tam].tarefa = j;
                                tam++;

                                espacos();
                                printf("%d. %s (%s - %s)\n", tam, user[i].tarefa[j].nome, user[i].nome, user[i].codigo);
                            }
                        }
                    }
                
                    if(!tam){
                        printf("Nao foram encontrados resultados para esta busca;\n");
                        break;
                    }
                    printf("\n");
                    espacos();
                    printf("--> Escolha uma tarefa para exibir mais opcoes.\n    > ");
                    int x = ler_int(0, tam);
                    if(!x)break; x--;

                    nivel=3;
                    editar_tarefa(item[x].user, item[x].tarefa);
                }while(1);
            }while(1);
        }
        else if(opcao==4){//estatisticas gerais
            printf("\n> Estatisticas Gerais\n");
            int tarefas_total=0, status[3]={0}, prioridade[3]={0}, concluidas=0;
            float media1, media2;

            for(int i=0; i<p; i++){
                tarefas_total += user[i].lim;
                concluidas += user[i].concluidas;

                for(int j=0; j<user[i].lim; j++){

                    prioridade[user[i].tarefa[j].info[0]-1]++;
                    status[user[i].tarefa[j].info[1]-1]++;

                }
            }

            printf("\nTotal de colaboradores: %d\n"
                    "Total de atividades cadastradas: %d\n"
                    "Total de atividades pendentes no momento: %d\n",
                    p, tarefas_total, tarefas_total-concluidas);

            media1 = (float)tarefas_total/p; media2 = (float)concluidas/p;
            printf("\nMedia de atividades por colaborador: %.1f\n"
                    "Media de tarefas concluidas por colaborador: %.1f\n",
                    media1, media2);

            printf("\nAtividades com prioridade alta: %d\n"
                    "Atividades com prioridade media: %d\n"
                    "Atividades com prioridade baixa: %d\n",
                    prioridade[0], prioridade[1], prioridade[2]);

            printf("\nAtividades nao iniciadas: %d\n"
                    "Atividades em andamento: %d\n"
                    "Atividades concluidas %d\n",
                    status[0], status[1], status[2]);

            printf("\nFim da exibicao.\n");
        }
        else if(opcao==5){//ranking
            printf("\n+ RANKING +\n\n");
            int usados[5], u=0, ranking, maximo=5;
            if(p<5)maximo=p;

			//Impressao do ranking
            for(ranking=1; ranking<=maximo; ranking++){//para cada posicao do ranking
                int maior=0, posicao=0;

                for(int i=0; i<p; i++){//para cada pessoa
                    if(user[i].concluidas>maior){
                        int parar=0;
                        for(int j=0; j<u; j++){
                            if(i==usados[j]){parar=1; break;}
                        }
                        if(!parar){
                            maior=user[i].concluidas;
                            posicao = i;
                        }
                    }
                }
                printf("%do. %s (%d tarefas concluidas)\n", ranking, user[posicao].nome, user[posicao].concluidas);
                usados[u]=posicao; u++;
            }
            printf("\nFim da exibicao.\n");
        }
        else if(opcao==6)exibir_menu();

    }while(1);//geral
    salvar_dados(user);

    return 0;
}
//-------------------------------------------------------------------------------------------------------------------------
//
void exibir_tarefasIntegra(int x, info pessoa[], int *contador){//recebe 0(prioridade) ou 1(etapa)
    for(int i=1; i<=3; i++)//cada x
        for(int j=0; j<p; j++)//cada user
            for(int k=0; k<user[j].lim; k++)//cada tarefa
                if(user[j].tarefa[k].info[x]==i){

                    pessoa[*contador].user=j;
                    pessoa[*contador].tarefa=k;

                    espacos();
                    (*contador)++;
                    printf("%d. %s - %s (%s)\n", *contador, 
                        user[j].tarefa[k].nome, user[j].nome, user[j].codigo);
                    exibir_tipo(0, i); exibir_tipo(1, i);


                    printf("\n");
                }
}

void exibir_tarefasFiltro(int filtro, int tipo, info pessoa[], int *contador){
    for(int i=0; i<p; i++)//cada pessoa
        for(int j=0; j<user[i].lim; j++)//cada tarefa
            if(user[i].tarefa[j].info[tipo]==filtro){

                pessoa[*contador].user = i;
                pessoa[*contador].tarefa = j;

                (*contador)++;
                espacos();
                printf("%d. %s - %s (%s)\n", *contador, 
                    user[i].tarefa[j].nome, user[i].nome, user[i].codigo);
                exibir_tipo(0, filtro); exibir_tipo(1, filtro);
                printf("\n");
            }
}

void editar_tarefa(int x, int y){//usuario, tarefa
    itens *item = &user[x].tarefa[y]; nivel++;
    do{
        printf("\n"); espacos();
        printf("> %s\n", item->nome); espacos();
        printf("Descricao: ");
        if(string_vazia(item->descricao))printf("nao ha descricao.\n\n");
        else printf("%s\n\n", item->descricao);
        espacos();

        printf("Escolha uma acao:\n"); espacos();
        printf("1. Mudar nome\n"); espacos();
        printf("2. Mudar prioridade\n"); espacos();
        printf("3. Mudar etapa\n"); espacos();
        printf("4. Mudar colaborador responsavel\n"); espacos();
        printf("5. Mudar descricao\n"); espacos();
        printf("6. Excluir atividade\n"); espacos();
        printf("> ");
        int escolha = ler_int(0, 6);
        if(!escolha){nivel--; return;}

        nivel++;

        if(escolha==1){
            printf("\n");
            espacos();
            printf("Insira o novo nome.\n");espacos();
            printf("> ");
            char novo[100]; int s;

            while((s=lerString_tarefa(novo))==1);
            if(s==2){nivel--; continue;}
            
            espacos();
            printf("A seguinte mudanca sera feita:\n"); espacos();
            printf("%s -> %s\n", item->nome, novo);
            if(encerrar()){nivel--; continue;}

            strcpy(item->nome, novo); espacos();
            printf("Mudanca bem sucedida.\n");
        }
        else if(escolha==2){
            printf("\n");
            espacos();
            printf("Nova prioridade:\n"); espacos();
            printf("> ");

            int novo = ler_int(0, 3);
            if(!novo){nivel--; continue;}

            item->info[0] = novo;  espacos();
            printf("A tarefa agora tem: "); exibir_tipo(0, novo);
        }
        else if(escolha==3){
            if(item->info[1]==3){
                espacos();
                printf("Esta tarefa ja foi concluida.\n");
                nivel--; continue;
            }
            else{
                espacos();
                printf("A tarefa sera modificada de\n");
                exibir_tipo(1, item->info[1]); espacos();
                printf("Para\n");
                exibir_tipo(1, (item->info[1])+1);

                if(encerrar()){nivel--; continue;}

                (item->info[1])++; user[x].concluidas++;
                espacos();
                printf("Mudanca bem sucedida.\n");
            }
        }
        else if(escolha==4){
            printf("\n");
            espacos();
            printf("Insira o codigo do novo colaborador\n"); espacos();
            printf("> ");
            int outro;

            do{
                while((outro=procurar_colaborador())==-1);
                if(outro==-2)break;

                if(user[outro].lim==100){
                    espacos();
                    printf("Este usuario ja alcancou o maximo de atividades cadastradas. Escolha outro.\n");
                }
                else break;
            }while(1);
            if(outro==-2){nivel--; continue;}

            espacos();
            printf("A tarefa deixara de ser de %s e passara a ser de %s\n", 
                    user[x].nome, user[outro].nome);
            
            if(encerrar()){nivel--; continue;}

            //Troca
            user[outro].tarefa[user[outro].lim] = *item; 
            user[outro].lim++;

            for(int i=y; i<user[x].lim; i++)
                user[x].tarefa[i] = user[x].tarefa[i+1];
            user[x].lim--;

            if(item->info[1]==3){
                user[x].concluidas--;
                user[outro].concluidas++;
            }
            reorganizacao_por_status(x); reorganizacao_por_status(outro);

            espacos();
            printf("Mudanca bem sucedida.\n"); return;
        }
        else if(escolha==5){
            printf("\n");
            espacos();
            printf("Insira a nova descricao.\n"); espacos();
            printf("> ");
            char novo[500]; int s;

            while((s=ler_string(novo, 500))==1);
            if(s==2){nivel--; continue;}

            if(string_vazia(novo)){
                espacos();
                printf("A descricao será apagada.\n");
                if(encerrar()){nivel--; continue;}
            }
            strcpy(item->descricao, novo); espacos();
            printf("Mudanca bem sucedida.\n");
        }
        else if(escolha==6){
            printf("\n");
            espacos();
            printf("Esta acao nao pode ser desfeita.\n");
            if(encerrar()){nivel--; continue;}

            for(int i=y; i<user[x].lim; i++)
                user[x].tarefa[i] = user[x].tarefa[i+1];
            user[x].lim--;

            espacos();
            printf("Remocao bem sucedida.\n"); return;
        }

        nivel--;
        printf("\n"); espacos();
        printf("Voce pode realizar mais modificacoes.\n");
        if(encerrar())return;
    }while(1);
}

void espacos(){
    for(int i=0; i<nivel; i++)printf("  ");
}

//
//FUNCOES DE VERIFICACAO
int string_vazia(char string[]){//1: é vazia
	int i;
	for(i=0; i<=strlen(string); i++)if(string[i]!=' ' && string[i]!='\0')return 0;
	return 1;
}

int posicao_codigo(char codigo[]){//retorna a posicao do colaborador no array, ou -1 se nao existir
    int x = -1;
    int i = 0;
    for(i=0; i<p; i++){
        if(strcmp(user[i].codigo, codigo)==0){
            x = i; break;
        }
    }
    return x;
}

//
//FUNCOES DE USO GERAL
int encerrar(){ //return 1=parar processo atual; return 0=continuar
    espacos();
	printf("Continuar? (S/N): ");
	char c;
    scanf(" %c", &c); getchar();
	if(c=='n' || c=='N')return 1;
	return 0;
}

void minusculas(char string[]){ //transforma todas as letras da string passada no parametro em minusculas
	int i;
	for(i=0; string[i]!='\0'; i++){
		if(string[i]>='A' && string[i]<='Z')string[i]+='a'-'A';
	}
}

void remover_enter(char t[]){
    int x = strlen(t);
    if(t[x-1]=='\n' && x>0)t[x-1]='\0';
}

void tirar_espacos(char string[]){
    int meio=0;//deixa um espaço se a cadeia de espacos estiver no meio

    for(int i=0; string[i]!='\0'; i++){
        int a=i, espacos=0;
		
        if(string[i]!=' ')meio=1;//significa que a cadeia de espacos nao esta mais no inicio da string
		
        while(string[a]==' ' && string[a]!='\0'){
            espacos++; a++;
            if(string[a+1]=='\0')meio=0;//significa que a cadeia de espacos esta no fim da string
        }
        if(meio)espacos--;//quantidade de espacos que serao tirados
		
        if(espacos>0) {for(int j=i; string[j]!='\0'; j++)string[j]=string[j+espacos]; i--;}
    }
}

void exibir_menu(){
    printf("\nMENU\n");
    printf("1. Listar Atividades\n");
    printf("2. Listar Colaboradores\n");
    printf("3. Buscar\n");
    printf("4. Estatisticas Gerais\n");
    printf("5. Ranking\n");
    printf("6. Exibir menu\n");
    printf("0. Sair\n");
    printf("Obs: digite 0 ou deixe o campo vazio para cancelar qualquer acao em andamento.\n\n");
}

void reorganizacao_por_status(int x){//recebe a posicao do colaborador no array
    int i, limite=0, status;
    for(status=1; status<=3; status++)
        for(i=0; i<user[x].lim; i++)
            if(user[x].tarefa[i].info[1]==status){
                if(limite!=i){
                    itens t = user[x].tarefa[i];
                    user[x].tarefa[i] = user[x].tarefa[limite];
                    user[x].tarefa[limite] = t;
                    limite++;
                    i = limite;
                }
                else{
                    limite++;
                }
            }
}

void exibir_tipo(int x, int y){//x: tipo de dado(0p, 1e); y: numero do dado(1-3)
    espacos();
    if(x){
        printf("Etapa: ");
        switch(y){
            case 1: printf("A Fazer.\n"); break;
            case 2: printf("Em andamento.\n"); break;
            case 3: printf("Concluido.\n"); break;
        }
    }
    else{
        printf("Prioridade: ");
        switch(y){
            case 1: printf("Alta.\n"); break;
            case 2: printf("Media.\n"); break;
            case 3: printf("Baixa.\n"); break;
        }
    }

}

void exibir_tarefas(int k){//recebe a posicao da user no vetor e imprime todas as suas atividades
    int j;
    printf("\n");
    espacos();
    printf("Tarefas de %s (%s):\n", user[k].nome, user[k].codigo);

    if(user[k].lim==0){espacos(); printf("Este usuario nao possui atividades cadastradas.\n");}
    else for(j=0; j<user[k].lim; j++){
        espacos();
        printf("%d. %s\n", j+1, user[k].tarefa[j].nome);
        espacos();
        exibir_tipo(0, user[k].tarefa[j].info[0]);
        espacos();
        exibir_tipo(1, user[k].tarefa[j].info[1]);
        
        printf("\n");
    }
}

void exibir_descricao(int x, int y){
    int a; espacos();
    if((a=string_vazia(user[x].tarefa[y].descricao)))printf("Esta tarefa nao possui descricao.\n");
    else printf("%s\n", user[x].tarefa[y].descricao);
}

//
//FUNCOES DE LEITURA DE DADOS NAO-ESPECIFICOS
int ler_string(char string[], int tam){//substitui o fgets; retorna 1 para erro, 2 p/ string vazia
    //le string, remove o enter, verifica o tamanho e se eh vazio
	
	/*Exemplo de uso:
		while(ler_string(frase));
	o while se repete até ler_string se tornar "falsa" (0) 
	*/

    char teste[tam+100];
    int i;
    fgets(teste, tam+100, stdin); remover_enter(teste); tirar_espacos(teste);

    if(strlen(teste)>tam){
        espacos();
        printf("Estouro do limite de caracteres. Tente novamente.\n"); espacos(); return 1;
    }
    strcpy(string, teste);

    if(string_vazia(teste))return 2;

    return 0;
}

int ler_int(int a, int b){ //substitui o scanf para inteiros; n>=a && n<=b; retorna o valor n
    //le e verifica se a entrada do usuario eh mesmo um numero e se esta nas limitacoes especificadas

	/*Exemplo de uso:
		int x = ler_int(1, 10)
		x e um numero do intervalo [1, 10]
	*/

    do{
        char n[100];
        int i, num=0, erro=0, s;

        while((s=ler_string(n, 100))==1);
		
        if(s==2){espacos(); printf("Escolha um numero valido.\n"); 
            espacos();
            continue;
        }

        for(i=0; n[i]!='\0'; i++)
            if(n[i]<'0' || n[i]>'9'){
                espacos();
                printf("Insira um numero valido.\n"); 
                espacos();
                erro=1; 
                break;
            }
        if(erro)continue;

        for(i=0; n[i]!='\0'; i++){
            num *= 10;
            num += n[i] - '0';
        }

        if(num<a || num>b){espacos(); printf("Insira um numero valido.\n"); espacos(); continue;}

        return num;
    }while(1);

}

//
//FUNCOES DE LEITURA DE DADOS ESPECIFICOS
int lerString_codigo(char codigo[]){//retorna 2 para string vazia, 1 para erro

    //lê um codigo num formato valido (5 digitos, so numeros), sem verificar se ja existe

    int s;
    while((s = ler_string(codigo, 5))==1);
    if(s==2)return 2;

	//Codigo de 5 digitos
    if(strlen(codigo)<5){
        espacos();
        printf("O codigo deve conter 5 digitos. Tente novamente.\n"); espacos();
        return 1;
    }
	
    //Verifica se sao apenas numeros
    int i = 0;
    for(i=0; i<5; i++){
        if(codigo[i] < '0' || codigo[i] > '9'){
            espacos();
            printf("O codigo deve conter apenas numeros. Tente novamente.\n");
            return 1;
        }
    }
        
    return 0;
}

int lerString_tarefa(char tarefa[]){//retorna 2 para string vazia, 1 para erro
    int s, i, j;
    while((s=ler_string(tarefa, 100))==1);
    if(s==2)return 2;

	//Verifica se ja nao existe uma tarefa com este nome
    char busca[100]; 
    strcpy(busca, tarefa); minusculas(busca);
    for(i=0; i<p; i++){
        for(j=0; j<user[i].lim; j++){
            char copia[100];
            strcpy(copia, user[i].tarefa[j].nome);
            minusculas(copia);

            if(strcmp(copia, busca)==0){
                espacos();
                printf("Ja existe uma tarefa cadastrada com este nome. Tente novamente.\n");
                return 1;
            }
        }
    }
    return 0;
}

int procurar_colaborador(){//le o codigo e retorna a posicao do colaborador no array; -1: user nao encontrada; -2:string vazia
    int x = -1, s;
    char codigo[6];
    while((s=lerString_codigo(codigo))==1);
    if(s==2)return -2;

    x = posicao_codigo(codigo);
    if(x==-1){espacos(); printf("Usuario nao encontrado. Tente novamente.\n"); espacos();}
    return x;
}

//
//SALVAR E CARREGAR DADOS
int carregar_dados(colaborador *c){
    FILE *arquivo = fopen("dados_lp.bin", "rb");
    if(arquivo == NULL){
        printf("Ocorreu um erro ao carregar os arquivos.\n");
        return 0;
    }
    fread(&p, sizeof(int), 1, arquivo);
    fread(c, sizeof(colaborador), p, arquivo);
    
    fclose(arquivo);
    return 1;
}

int salvar_dados(colaborador *c){
    FILE *arquivo = fopen("dados_lp.bin", "wb");
    if(arquivo == NULL){
        printf("Ocorreu um erro ao salvar os arquivos.\n");
        return 0;
    }
    fwrite(&p, sizeof(int), 1, arquivo);
    fwrite(c, sizeof(colaborador), p, arquivo);
    
    fclose(arquivo);
    return 1;
}