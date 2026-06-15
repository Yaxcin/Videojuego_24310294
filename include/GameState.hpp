#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

// Maquina de estados del flujo de juego
enum class GameState {
    MENU,                 // Pantalla inicial
    CHARACTERS,           // Guia de personajes y bestiario
    TUTORIAL,             // Tutorial interactivo previo a la primera ronda
    ROUND_ACTIVE,         // Ola en progreso, torres atacando
    ROUND_PAUSE,          // Pausa entre olas, comprar/vender torres
    COLLECTING_COINS,     // Recolectar monedas de pinata "Bolo Bolo"
    VICTORY,              // Victoria (completar todas las olas)
    DEFEAT                // Derrota (0 vidas)
};

#endif
