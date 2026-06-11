# Tower Defense Mexicano

Tower Defense inspirado en Bloons, pero con una tematica mexicana completa.
Basado en la estructura de proyecto de **EjemplosJuego** del profesor.

## Tema del Juego
- **Enemigos**: Pinatas (Engrudo, Arcilla, Revelacion, Fruta, Hipnotizadora)
- **Torres**: Personajes mexicanos (Nino con Palo, Viejo con Machete, Taquero, Abuelita, Don de Cohetes, Organillero, Raspadero)
- **Monedas**: Dulces de chocolate de la pinata "Bolo Bolo"

## Requisitos
- **Compilador**: MinGW64 (via MSYS2)
- **SFML 3.x**: Instalado en MSYS2
- **make**: Parte de mingw64-toolchain
- **PowerShell 5+** o bash

## Compilacion

### Opcion 1: Makefile
```bash
make all
```

### Opcion 2: Compilar y ejecutar
```bash
make run
```

### Opcion 3: Recompilacion limpia
```bash
make rebuild
```

## Ejecucion
```powershell
.\bin\TowerDefenseMexicano.exe
```

## Controles
- **ESC**: Cerrar juego
- **SPACE**: Iniciar oleada
- **Click izquierdo en panel**: Seleccionar torre
- **Click izquierdo en mapa**: Colocar torre o mover torre seleccionada
- **Click derecho**: Cancelar seleccion
- **M**: +100 dinero (debug)
- **L**: -1 vida (debug)
- **E**: Crear pinata Engrudo (debug)

## Estructura del Proyecto

```text
.
|-- include/
|   |-- GameWindow.hpp
|   |-- GameState.hpp
|   |-- Entity.hpp
|   |-- Enemies/
|   |-- Towers/
|-- src/
|   |-- main.cpp
|   |-- Game.cpp
|   |-- Entity.cpp
|   |-- Enemies/
|   |-- Towers/
|-- assets/
|   |-- textures/
|   |-- fonts/
|-- bin/
|-- obj/
|-- makefile
|-- README.md
```

## Comandos make utiles

```bash
make all       # Compilar proyecto
make run       # Compilar y ejecutar
make clean     # Limpiar compilacion
make rebuild   # Limpiar y recompilar
make help      # Mostrar ayuda
```

## Estado del Proyecto

### Fase 1: Infraestructura base
- [x] Configuracion SFML 3.x
- [x] Bucle principal a 60 FPS
- [x] Maquina de estados
- [x] Estructura compatible con EjemplosJuego

### Fase 2: Sistema de pinatas
- [x] Pinatas con camino y movimiento
- [x] Tipos visuales y estadisticas base
- [ ] Habilidades especiales de cada tipo

### Fase 3: Sistema de torres
- [x] Torre base
- [x] Personajes principales colocables
- [x] Targeting basico y efectos visuales simples
- [ ] Proyectiles/sprites animados

### Fase 4: Flujo de olas y economia
- [x] Oleadas basicas
- [x] Sistema de dinero y recompensas
- [x] Compra y movimiento entre rondas
- [ ] Mecanica "Bolo Bolo"

### Fase 5: UI y pulido
- [x] HUD en pantalla
- [x] Costos en panel lateral
- [ ] Sonidos y musica
- [ ] Menus completos

## Notas Tecnicas
- **C++**: Estandar C++17
- **SFML 3.x**: Adaptado a nueva API (std::optional para eventos)
- **Estructura**: Basada en EjemplosJuego del repositorio del profesor

## Recursos Utiles
- Repositorio de ejemplos: `C:\Users\higar\OneDrive\Escritorio\EjemplosJuego`
- Documentacion SFML 3: https://www.sfml-dev.org/documentation/3.0/
- MinGW64 via MSYS2: https://www.msys2.org/

---

**Version**: 0.2.0
**Ultima actualizacion**: 2026-06-11
