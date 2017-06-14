/*
Morpion Multijoueur en Reseau Avec Bibliotheques SFML.

BENHARROUS YOUSSEF.
L3 Informatique.
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" //pour supprimer les warning deprecated
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <iostream>
#include <fstream>
#include <string>

using namespace sf;
using namespace std;

const int WIDTH = 600;
const int HEIGHT = 315;

IpAddress ip; //l'adresse ip avec laquelle on se connecte

unsigned short s_port = 50001; //port du serveur
unsigned short c_port = 50002; //port du client

UdpSocket socket;

bool serveur; //pour savoir si on serveur ou client
Vector2f position; // postionsur la grille de jeu
bool donnee_recu; // pour savoir si on a recu les donnee


// FONCTION RESPONSABLE DE LA RECEPTION DE DONNEE
void ReceptionDonnee()
{
	donnee_recu = false;

	unsigned short port;
	if(serveur) port = s_port;
	else port = c_port;

	Packet p;
	socket.receive(p,ip,port);

	Vector2f pos;
	p >> pos.x >> pos.y;

	position = pos;

	donnee_recu = true;
}


// FONCTION RESPONSABLE DE L'ENVOI DE DONNEE
void EnvoiDonnee()
{
	unsigned short port;
	if(serveur) port = c_port;
	else port = s_port;

	Packet p;
	p << position.x << position.y;

	socket.send(p,ip,port);
}


//FONCTION POUR VERIFIER QUI A GAGNE,elle retourne :0 si rien ne se passe,1 si le serveur gagne,2 si le client gagne,3 match nul.
int check(char Tabl[3][3])
{
	size_t occupee = 0;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(Tabl[i][j] != ' ')
				occupee++;
		}
	}
	if(occupee == 9)
		return 3;

	//cas numero: 1
	if(Tabl[0][0] != ' ' && Tabl[0][0] == Tabl[0][1] && Tabl[0][0] == Tabl[0][2])
	{
		if(Tabl[0][0] == 's') return 1;
		else return 2;
	}
	//cas numero: 2
	if(Tabl[1][0] != ' ' && Tabl[1][0] == Tabl[1][1] && Tabl[1][0] == Tabl[1][2])
	{
		if(Tabl[1][0] == 's') return 1;
		else return 2;
	}
	//cas numero: 3
	if(Tabl[2][0] != ' ' && Tabl[2][0] == Tabl[2][1] && Tabl[2][0] == Tabl[2][2])
	{
		if(Tabl[2][0] == 's') return 1;
		else return 2;
	}
	//cas numero: 4
	if(Tabl[0][0] != ' ' && Tabl[0][0] == Tabl[1][0] && Tabl[0][0] == Tabl[2][0])
	{
		if(Tabl[0][0] == 's') return 1;
		else return 2;
	}
	//cas numero: 5
	if(Tabl[0][1] != ' ' && Tabl[0][1] == Tabl[1][1] && Tabl[0][1] == Tabl[2][1])
	{
		if(Tabl[0][1] == 's') return 1;
		else return 2;
	}
	//cas numero: 6
	if(Tabl[0][2] != ' ' && Tabl[0][2] == Tabl[1][2] && Tabl[0][2] == Tabl[2][2])
	{
		if(Tabl[0][2] == 's') return 1;
		else return 2;
	}
	//cas numero: 7
	if(Tabl[0][0] != ' ' && Tabl[0][0] == Tabl[1][1] && Tabl[0][0] == Tabl[2][2])
	{
		if(Tabl[0][0] == 's') return 1;
		else return 2;
	}
	//cas numero: 8
	if(Tabl[2][0] != ' ' && Tabl[2][0] == Tabl[1][1] && Tabl[2][0] == Tabl[0][2])
	{
		if(Tabl[2][0] == 's') return 1;
		else return 2;
	}

	return 0;
}


//FONCTION PRINCIPALE
int main()
{
	RenderWindow window(VideoMode(WIDTH,HEIGHT),"Tic Tac Toe: Reseau (BENHARROUS Youssef)",Style::Close);

	cout<<"Votre adresse IP est: "<<IpAddress::getLocalAddress().toString()<<endl;


//J'AI AJOUTEE CETTE FONCTION SI ON VEUR CHARGER L'ADRESSE IP A PARTIR D4UN FICHIER A LA PLACE DE LA TAPER.
	fstream fichier("ip.txt",ios::in);
	if(fichier.is_open())
	{
		string str;
		fichier >> str;
		ip = str;
	}
	fichier.close();

	Font font;
	font.loadFromFile("font.ttf");

	Text fenetre("Choisir : Serveur ([S])/Client ([C])",font,30);
	fenetre.setPosition(WIDTH/2-fenetre.getGlobalBounds().width/2,(HEIGHT/2-fenetre.getGlobalBounds().height/2));

	while(true)
	{
		Event event;
		while(window.pollEvent(event));
		{
			if(event.type == Event::Closed) //evenement quand on ferme la fenetre
				return 0;
			if(event.type == Event::KeyReleased && event.key.code == Keyboard::S) //evenement quand on appuie sur la touche S
			{
				serveur = true;
				break;
			}
			if(event.type == Event::KeyReleased && event.key.code == Keyboard::C) //evenement quand on appuie sur la touche C
			{
				serveur = false;
				break;
			}
		}
		window.clear();
		window.draw(fenetre);
		window.display();
	}

	if(ip.toString() == "0.0.0.0") //si on charge pas le ficher avec l'adresse ip,bah on l'ecrit tout simplement
	{
		fenetre.setString("Donner adress IP : Client/Serveur \n");
		fenetre.setPosition(WIDTH/2-fenetre.getGlobalBounds().width/2,(HEIGHT/2-fenetre.getGlobalBounds().height/2)-30);

		Text text_ip("",font,25);
		string str_ip;

		bool b = false;
		while(!b)
		{
			Event event;
			while(window.pollEvent(event))
			{
				if(event.type == Event::Closed)
					return 0;
				if(event.type == Event::KeyReleased && event.key.code == Keyboard::Return)
				{
					ip = str_ip;
					b = true;
				}

				else if(event.type == Event::TextEntered)
				{
					if (event.text.unicode == 8)
					{
						if(str_ip.size()>0)
							str_ip.erase(str_ip.size() - 1, 1);
					}
					else if (event.text.unicode < 128 && event.text.unicode != 13)
						str_ip+= (char)event.text.unicode;

					text_ip.setString(str_ip);
					text_ip.setPosition(WIDTH/2-text_ip.getGlobalBounds().width/2,HEIGHT/2-text_ip.getGlobalBounds().height/2);
				}
			}

			window.clear();
			window.draw(fenetre);
			window.draw(text_ip);
			window.display();
		}

	}

	//test de connexion
	if(serveur) socket.bind(s_port);
	else socket.bind(c_port);

	if(serveur)
	{
		string tmp;
		Packet paquet;
		socket.receive(paquet,ip,s_port);

		paquet >> tmp;
		cout<<tmp<<endl;

		paquet.clear();

		tmp = "Vous etes Connecte au Serveur!\n";
		paquet << tmp;
		socket.send(paquet,ip,c_port);
	}
	else
	{
		string tmp = "Vous etes Connecte au Client\n";
		Packet paquet;
		paquet << tmp;
		socket.send(paquet,ip,s_port);

		paquet.clear();
		socket.receive(paquet,ip,c_port);
		paquet >> tmp;
		cout<<tmp;
	}

	char cas[3][3]; // les case dans la grille 's' pour serveur et 'c' pour client
	RectangleShape grille[3][3];
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			cas[i][j] = ' ';

			grille[i][j].setSize(Vector2f(60,60)); //grille de jeu de 60pixels
			grille[i][j].setPosition(60+i*65,60+j*65); // case de 5 pixels

			grille[i][j].setFillColor(Color(255,255,255));
			grille[i][j].setOutlineColor(Color(203,151,103));
			grille[i][j].setOutlineThickness(2.5f);
		}
	}


	Thread recep(ReceptionDonnee);
	Thread env(EnvoiDonnee);
	bool receiving = false;
	bool mon_tour = (serveur == false ? true : false); //si le mouvement actuel se joue de la part du serveur ou client

	if(mon_tour)
	{
		fenetre.setString("A vous de jouer.");
		fenetre.setColor(Color::Black);
	}
	else
	{
		fenetre.setString("Tour de l'adversaire.");
		fenetre.setColor(Color::Blue);
	}
	fenetre.setCharacterSize(25);
	fenetre.setPosition(WIDTH - 20 - fenetre.getGlobalBounds().width,10);

	bool exit = false;

	//boucle de jeu
	while(window.isOpen())
	{
		Event event;
		//reception des donnee sur l'adversaire
		if(!mon_tour)
		{
			if(!receiving)
			{
				receiving = true; //attente de reception de donnee
				recep.launch();
			}
			else if(donnee_recu)
			{
				receiving = false; // donnee recue

				//grille de jeu
				Vector2i p;
				p = Vector2i(position);
				cas[p.x][p.y] = (serveur ? 'k' : 's');
				grille[p.x][p.y].setFillColor(Color::Blue);

				mon_tour = true;
				fenetre.setString("A vous de jouer.");
				fenetre.setColor(Color::Black);
				fenetre.setPosition(WIDTH - 20 - fenetre.getGlobalBounds().width,10);
			}
		}

		while(window.pollEvent(event))
		{
			if(event.type == Event::Closed || exit)
			{
				if(exit)
					sleep(seconds(10));

				window.close();
				recep.terminate();
				env.terminate();
			}

			else if(mon_tour && event.type == Event::MouseButtonReleased)
			{
				for(int i=0;i<3;i++)
				{
					for(int j=0;j<3;j++)
					{
						if(grille[i][j].getGlobalBounds().contains(Vector2f(Mouse::getPosition(window))) && cas[i][j] == ' ')
						{
							position = Vector2f(i,j);
							cas[i][j] = (serveur ? 's' : 'k');
							grille[i][j].setFillColor(Color::Black);

							mon_tour = false;
							fenetre.setString("Tour de l'adversaire.");
							fenetre.setColor(Color::Blue);
							fenetre.setPosition(WIDTH - 20 - fenetre.getGlobalBounds().width,10);
							env.launch();
						}
					}
				}
			}
		}


//FONCTION DE TEST QUI ARRETE LA BOUCLE DE JEU SELON LES CAS (1-2-3)
		int test = check(cas);
		if(test != 0)
		{
			if(test == 1) fenetre.setString("Le joueur serveur gagne!!!");
			else if(test == 2) fenetre.setString("Le joueur client gagne!!!");
			else fenetre.setString("Match nul!!!");

			fenetre.setColor(Color::White);
			fenetre.setPosition(WIDTH - 20 - fenetre.getGlobalBounds().width,10);

			exit = true;
		}

		window.clear(Color(52,152,152));

		for(int i=0;i<3;i++)
			for(int j=0;j<3;j++)
				window.draw(grille[i][j]);
		window.draw(fenetre);

		window.display();
	}
	return 0;
}

//BENHARROUS YOUSSEF
