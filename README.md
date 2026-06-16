# Pinaton: Tower Defense Mexicano

## Descripcion del Proyecto

Pinaton es un juego de tower defense inspirado en la estructura clasica de Bloons, pero con una tematica mexicana. En lugar de globos, el jugador defiende el camino de distintas piñatas con habilidades especiales, usando personajes mexicanos como torres.

El juego incluye menu principal, tutorial jugable, sistema de oleadas, musica, efectos de sonido, pantalla de opciones, pausa, bestiario, animaciones, proyectiles y condiciones de victoria/derrota.

## Objetivo del Juego

El objetivo es sobrevivir 15 rondas evitando que las piñatas lleguen al final del camino. Para defenderte debes colocar personajes alrededor del mapa, ganar dulces al destruir piñatas y mejorar tu defensa entre rondas.

Si completas las 15 rondas, ganas la partida. Si las piñatas reducen tus vidas a 0, pierdes.

## Controles

- Mouse izquierdo: seleccionar personajes, colocarlos, moverlos entre rondas y usar botones del menu.
- Mouse derecho: cancelar la seleccion actual.
- SPACE: avanzar pantallas de preparacion e iniciar oleadas.
- ENTER: avanzar pasos del tutorial cuando se indique.
- TAB: cambiar velocidad de juego entre x1 y x2 durante la partida.
- ESC: abrir el menu de pausa durante la partida.
- Backspace: volver desde pantallas secundarias como opciones o personajes.

## Mecanicas

- Dulces: moneda del juego. Se obtienen al destruir piñatas y se usan para comprar personajes.
- Oleadas: cada ronda genera una combinacion distinta de piñatas.
- Preparacion: entre rondas puedes colocar o mover personajes. Durante una oleada no se pueden modificar defensas.
- Limites de personajes: cada tipo de personaje tiene un limite para mantener el balance.
- Tutorial: antes de iniciar una partida se puede jugar un tutorial guiado para aprender las mecanicas principales.
- Velocidad: durante las oleadas se puede acelerar el juego a x2.

### Personajes

- Niño con Palo: personaje barato de dano directo.
- Viejo con Machete: fuerte contra piñatas resistentes como la de arcilla.
- Taquero: lanza limones como proyectiles rapidos.
- Abuelita: ataca a la piñata mas avanzada del mapa y puede deshipnotizar personajes.
- Don de los Cohetes: lanza cohetes con dano en area.
- Organillero: aumenta la velocidad de ataque de personajes cercanos y protege contra hipnosis.
- Raspadero: lanza hielo y ralentiza piñatas.

### Piñatas

- Engrudo: piñata basica y rapida.
- Arcilla: piñata resistente que funciona como tanque.
- Revelacion: al morir genera dos piñatas bebe, una rosa y una azul.
- Fruta: al romperse ralentiza temporalmente a personajes cercanos.
- Hipnotizadora: provoca que personajes dentro de su efecto fallen ataques y reduce temporalmente el alcance del Organillero.

## Caracteristicas

- 15 rondas balanceadas.
- Mapa personalizado con camino de tierra.
- Menu principal con botones funcionales.
- Pantalla de personajes y bestiario.
- Tutorial jugable.
- Musica de menu, partida, victoria y derrota.
- Efectos de sonido para ataques, impactos y eventos.
- Animaciones de ataque y proyectiles para los personajes.
- Sistema de pausa, reinicio, victoria y derrota.
- Opciones de volumen para musica y efectos.

## Equipo

- Yael Higareda Velazquez - Desarrollo, diseno, implementacion, balance y pruebas.

## Tecnologias

- Lenguaje: C++17.
- Libreria grafica y multimedia: SFML 3.x.
- Compilador: MinGW64 mediante MSYS2.
- Automatizacion local: makefile.
- Control de versiones: Git y GitHub.
- Herramientas de apoyo: Visual Studio Code, Procreate, OBS Studio y herramientas de generacion/edicion de assets.

## Creditos

- Inspiracion de jugabilidad: juegos de tower defense tipo Bloons.
- Tematica visual: fiestas mexicanas, piñatas, dulces y personajes populares.
- Libreria multimedia: SFML.
- Assets visuales, musica y efectos: recursos creados y editados para este proyecto con apoyo de herramientas digitales e IA generativa.

## Compilacion y Ejecucion

Para compilar:

```bash
make all
```

Para ejecutar:

```bash
make run
```

Tambien se puede abrir directamente el ejecutable:

```powershell
.\bin\TowerDefenseMexicano.exe
```

## Estructura para CETUS

```text
.
|-- .github/
|   |-- workflows/
|-- video/
|   |-- demo.mp4
|-- gallery/
|   |-- cover.png
|-- screenshots/
|   |-- screenshot1.png
|   |-- screenshot2.png
|   |-- screenshot3.png
|-- bin/
|   |-- TowerDefenseMexicano.exe
|-- assets/
|-- include/
|-- src/
|-- README.md
|-- makefile
|-- .gitignore
```

## Notas de Entrega

- El video de gameplay debe guardarse como `video/demo.mp4`.
- La portada debe guardarse como `gallery/cover.png` con tamano 720x1080.
- Las capturas deben guardarse en `screenshots/` como PNG, minimo 3 imagenes.
- El ejecutable principal debe estar en `bin/`.
