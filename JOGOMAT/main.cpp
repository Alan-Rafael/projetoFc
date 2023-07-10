/*Jogo de questões matemáticas simples envolvendo as 4 operações apenas com
 * inteiros, não considerar partes fracionárias, supondo crianças do 5. ano do
 * fundamental. Deve ser dado um tempo, digamos, 1 minuto, e neste tempo o
 * jogador deve resolver o maior número de questões que conseguir. No final
 * informa quantas acertou, os pontos (cada uma pode valer 10 pontos por
 * exemplo). Usar conceitos de programação concorrente, como threads ou
 * equivalente. No início deve ser pedido o nome do jogador e ao fim salvar em
 * arquivo o nome (ou login) e pontos. A cada jogada, exibir o TOP FIVE até o
 * momento.*/
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
using namespace std::chrono;
using Tempo = chrono::steady_clock::time_point;
using Periodo = chrono::duration<int, std::chrono::seconds::period>;

void contadorSegundos(int tempoMaximo, bool &fimDoTempo) {
  Tempo tempoInicial = steady_clock::now();
  while (true) {
    Tempo tempoAtual = steady_clock::now();
    Periodo duracao = duration_cast<seconds>(tempoAtual - tempoInicial);
    int segundosDecorridos = duracao.count();
    if (segundosDecorridos >= tempoMaximo) {
      fimDoTempo = true;
      break;
    }
    this_thread::sleep_for(seconds(1));
  }
}

struct tJogador {
  string nome;
  int pontos;
};

bool compararPontuacao(const tJogador &a, const tJogador &b) {
  return a.pontos > b.pontos;
}

int main() {
  char operador, continuar;
  int respostaUsuario, respostaCerta;
  int a, b, acertos, n = 0, maximoInteiro = 30;
  vector<tJogador> dadosExistentes;

  // criando arquivo
  fstream arquivoJogadores("jogadores.txt", ios::in | ios::out | ios::app);
  if (!arquivoJogadores.is_open()) {
    cout << "Erro ao abrir o arquivo de jogadores." << endl;
    return 1;
  }
  // Lê os jogadores já existentes no arquivo
  string nome;
  int pontos;
  while (arquivoJogadores >> nome >> pontos) {
    dadosExistentes.push_back({nome, pontos});
    n++;
  }
  arquivoJogadores.close();
  srand(time(0));
  cout << "\n\t\t                 Pontuação                     \n\n";
  cout << "\t\t_______________________________________________\n";
  cout << "\t\t| Acerto das contas de '+' ou '-' = 10 Pontos |\n";
  cout << "\t\t----------------------------------------------\n";
  cout << "\t\t| Acerto das contas de '/' ou 'x' = 15 Pontos |\n";
  cout << "\t\t ---------------------------------------------\n\n";

  cout << " Observaçoes   \n\n";
  cout << "- Você terá um tempo de 1 min, para responder o maximo de "
          "questoes.\n";
  cout << "- Para respoder, não considere as partes fracionarias.\n\n ";
  cout << "- O Tempo ira começar a contar quando você digitar seu nome.\n";
  cout << "\t\t\t\t  Aperte enter para continuar\t";
  getchar();

  // definindo o tempo de 1 minuto para o jogador.
  int tempoMaximo = 10;
  do {
    // Definir  variavel da funçao
    bool fimDoTempo = false;
    system("clear");
    cout << "\t Jogador digite o seu nome: ";
    getline(cin, nome);

    // reiniciado as variaveis para o proximo jogador
    dadosExistentes.push_back({nome, 0});
    acertos = 0;
    continuar = ' ';
    // criando thread para executar o contador de segundos
    thread t(contadorSegundos, tempoMaximo, ref(fimDoTempo));
    do {
      system("clear");
      // GERAR NUMEROS ALEATORIOS DE 1 A maximoInteiro*
      a = 1 + rand() % maximoInteiro;
      b = 1 + rand() % maximoInteiro;
      // GERAR NUMERO ALEATORIO DE 1 a 4 PARA O OPERADOR*
      operador = 1 + rand() % 4;

      switch (operador) {
      // CONFERIR O VALOR DO OPERADOR.
      case 1:
        operador = '+';
        respostaCerta = (a + b); // Atribruir a resposta certa para a variavel.
        break;
      case 2:
        operador = '-';
        respostaCerta = (a - b); // Atribruir a resposta certa para a variavel.
        break;
      case 3:
        operador = 'x';
        respostaCerta = (a * b); // Atribruir a resposta certa para a variavel.
        break;
      case 4:
        operador = '/';
        if (a < b) {
          a += 20;
        }
        respostaCerta = (a / b); // Atribruir a resposta certa para a variavel.
        break;
      }
      // mostrar a conta para o usuario e receber a resposta dele.
      cout << "\n\t\t" << a << " " << operador << " " << b << ": ";
      cin >> respostaUsuario;
      // CONFERIR A RESPOSTA SE ESTA CERTA OU ERRADA*
      if (respostaUsuario == respostaCerta) {
        acertos++;
        if (operador == '+' ||
            operador == '-') { // conferir se a conta é de mais ou de menos
          dadosExistentes[n].pontos += 10;
        } else if (operador == 'x' ||
                   operador ==
                       '/') { // conferir se a conta é de multi ou divisao
          dadosExistentes[n].pontos += 15;
        }
      }

    } while (!fimDoTempo);
    system("clear");
    cout << "\t\t\t\t TEMPO ESGOTADO.\n\n";
    // exibindo quantos pontos o usuario fez
    cout << "Você fez: " << dadosExistentes[n].pontos << " pontos. \n";
    cout << "Você acertou: " << acertos << " questoes\n";

    // Atualizar os dados existentes com os novos jogadores
    n++;
    sort(dadosExistentes.begin(), dadosExistentes.end(), compararPontuacao);

    // Manter apenas os 5 maiores pontuadores
    if (dadosExistentes.size() > 5) {
      dadosExistentes.resize(5);
    } else {
    }

    // Sobrescrever o arquivo com os dados atualizados
    ofstream arquivoJogadores("jogadores.txt", ios::out | ios::trunc);
    if (arquivoJogadores.is_open()) {
      // criando uma variavel jogador que representa os elementos
      for (const auto &jogador : dadosExistentes) {
        arquivoJogadores << jogador.nome << " " << jogador.pontos << endl;
      }
      cout << "Dados salvos no arquivo com sucesso." << endl;
    } else {
      cout << "Erro ao abrir o arquivo de jogadores." << endl;
    }
    arquivoJogadores.close();
    // predendo o usuario em um laço para responder
    do {
      cout << "\nDeseja Jogar novamente ? (s)-sim/(n)-não: ";
      cin >> continuar;
      getchar();
    } while (continuar != 'n' && continuar != 'N' && continuar != 'S' &&
             continuar != 's');

    fimDoTempo = false;
    // thread sincronizada com o programa
    t.join();

  } while (continuar != 'n' && continuar != 'N');

  cout << "\nFim do programa";
  return 0;
}